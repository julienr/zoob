#!/usr/bin/python
import sys

#messagesDef=[
#  {'name':'BytesArray','fields':{'array':('char','bytes')}},
#  {'name':'Vector','fields':{'float':'x','float':'y'}},
#  {'name':'Damage','fields':{'uint16_t':'entityID','uint16_t':'damages'}},
#  {'name':'RocketInfo','fields':{'uint16_t':'rocketID','Vector':'position','Vector':'velocity'}},
#  {'name':'MineInfo','fields':{'uint16_t':'mineID','Vector':'position'}},
#  {'name':'PlayerInfo','fields':
#    {'uint16_t':'playerID',
#     'Vector':'position',
#     'Vector':'velocity',
#     'array':('RocketInfo','rocketInfos'),
#     'array':('MineInfo','mineInfos')}},
#  {'name':'Hello','fields':
#    {'String':'nickname'}},
#  {'name':'Welcome','fields':
#    {'BytesArray':'level',
#      'ServerState':'serverState',
#      'uint16_t':'playerID'}},
#  {'name':'Kicked','fields':
#    {'String':'reason'}},
#  {'name':'Join','fields':
#    {}},
#  {'name':'Joined','fields':
#    {}},
#  {'name':'NotJoined','fields':
#    {}},
#  {'name':'Spawn','fields':
#    {'Vector':'position'}},
#  {'name':'Explosion','fields':
#    {'Vector':'position',
#     'uint16_t':'exploderID',
#     'bool':'destroyExploder',
#     'array':('Damage','damages')}},
#  {'name':'GameState','fields':
#    {'array':('PlayerInfo','playerInfos')}},
#  {'name':'StateChange','fields':
#    {'uint8_t':'newState',
#     'uint8_t':'stateDuration'}}
#]

messagesDef = [
   {'name':'BytesArray','fields':{'array':('char','bytes')}},
   {'name':'Test', 'fields':
     {'array':('float','floats'),
      'String':'testStr'}},
]

# DO NOT TOUCH ANY FURTHER

def writeUnserialize(out, name, type, numTabs):
  for i in range(numTabs):
    out.write(' ')
  out.write('loaded |= ')
  if type == 'bool':
    out.write('readBool(len, data, offset, %s);\n'%name)
  elif type == 'uint16_t':
    out.write('readUint16_t(len, data, offset, %s);\n'%name)
  elif type == 'float':
    out.write('readFloat(len, data, offset, %s);\n'%name)
  elif type == 'char':
    out.write('readChar(len, data, offset, %s);\n'%name)
  else:
    out.write('%s._unserialize(len, data, offset);\n'%name)

def writeSerialize(out, name, type, numTabs):
  for i in range(numTabs):
    out.write(' ')
  if type == 'bool':
    out.write('writeBool(data, offset, %s);\n'%name)
  elif type == 'uint16_t':
    out.write('writeUint16_t(data, offset, %s);\n'%name)
  elif type == 'float':
    out.write('writeFloat(data, offset, %s);\n'%name)
  elif type == 'char':
    out.write('writeChar(data, offset, %s);\n'%name)
  else:
    out.write('%s._serialize(data, offset);\n'%name)

def writeSerializedSize(out, name, type, numTabs):
  for i in range(numTabs):
    out.write(' ')
  out.write('size += ')
  if type == 'bool':
    out.write('sizeof(char);\n')
  elif type == 'uint16_t':
    out.write('sizeof(uint16_t);\n')
  elif type == 'float':
    out.write('sizeof(uint32_t);\n')
  elif type == 'char':
    out.write('sizeof(char);\n')
  else:
    out.write('%s.serializedSize();\n'%name)



class Field:
  def __init__(self, type, name):
    self.type = type
    self.name = name
  def writeDecl(self, out):
    out.write('  %s %s;\n'%(self.type, self.name))

  def writeUnserialize(self, out):
    writeUnserialize(out, self.name, self.type, 2)

  def writeSerialize(self, out):
    writeSerialize(out, self.name, self.type, 2)

  def writeSerializedSize(self, out):
    writeSerializedSize(out, self.name, self.type, 2)

  def writeDestructor(self, out):
    pass

  def writeConstructorDefault(self, out):
    pass

#An array field actually encompasses two fields :
#If T is the type of the elements in the array and N the name of the array, it will have
#first a counter then the actual array
#uint16_t numN
#T* N;
class ArrayField(Field):
  def __init__(self, type, name):
    self.type = name[0]
    self.name = name[1]
    self.counterName = "num%s%s"%(self.name[0].upper(), self.name[1:])

  def writeDecl(self, out):
    out.write('  uint16_t %s;\n'%self.counterName)
    out.write('  %s* %s;\n'%(self.type, self.name))

  def writeUnserialize(self, out):
    out.write('  loaded |= readUint16_t(len, data, offset, %s);\n'%self.counterName)
    out.write('  delete [] %s; //avoid memory leak\n'%self.name)
    out.write('  %s = new %s[%s];\n'%(self.name, self.type, self.counterName))
    out.write('  for (uint16_t i=0; i<%s; i++) {\n'%self.counterName)
    writeUnserialize(out, '%s[i]'%self.name, self.type, 4)
    out.write('  }\n')

  def writeSerialize(self, out):
    out.write('  writeUint16_t(data, offset, %s);\n'%self.counterName)
    out.write('  for (uint16_t i=0; i<%s; i++) {\n'%self.counterName)
    writeSerialize(out, '%s[i]'%self.name, self.type, 4)
    out.write('  }\n')

  def writeSerializedSize(self, out):
    out.write('  size += sizeof(uint16_t);\n')
    out.write('  for (uint16_t i=0; i<%s; i++) {\n'%self.counterName)
    writeSerializedSize(out, '%s[i]'%self.name, self.type, 4)
    out.write('  }\n')

  def writeDestructor(self, out):
    out.write('  delete [] %s;\n'%self.name);

  def writeConstructorDefault(self, out):
    out.write('%s(NULL),%s(0)'%(self.name,self.counterName))

class Message:
  def __init__(self, dict):
    self.name = dict['name']
    self.fields = []
    fields = dict['fields']
    for type,name in fields.items():
      if type == 'array':
        self.fields.append(ArrayField(type, name))
      else:
        self.fields.append(Field(type, name))

  def writeDecl(self, out):
    out.write('struct %s : public Message {\n'%self.name)
    for f in self.fields:
      f.writeDecl(out)

    # constructor
    out.write('\n')
    out.write('  %s () : '%self.name)
    for f in self.fields:
      f.writeConstructorDefault(out)
    out.write('{}\n')

    out.write('  virtual ~%s ();\n'%self.name)
    out.write('\n  //methods\n')
    out.write('  size_t serializedSize();\n')
    out.write('  void _serialize (char* data, size_t& offset);\n')
    out.write('  bool _unserialize (size_t len, const char* data, size_t& offset);\n')
    out.write('\n  //loading flags\n')

    # forbid copy constructor and assignment
    out.write('private:\n')
    out.write('  %s (const %s& o) {}\n'%(self.name, self.name))
    out.write('  %s& operator = (const %s& o) {}\n'%(self.name, self.name))
    out.write('};\n\n')

    #Really ugly hack to have a String struct that simply inherits BytesArray
    if self.name == 'BytesArray':
      out.write('struct String : public BytesArray {};\n')

  def _writeUnserialize(self, out):
    out.write('bool %s::_unserialize (size_t len, const char* data, size_t& offset) {\n'%self.name)
    out.write('  bool loaded=true;\n')
    for f in self.fields:
      f.writeUnserialize(out)
    out.write('  return loaded;\n')
    out.write('}\n\n')

  def _writeSerialize(self, out):
    out.write('void %s::_serialize (char* data, size_t& offset) {\n'%self.name)
    for f in self.fields:
      f.writeSerialize(out)
    out.write('}\n\n')

  def _writeSerializedSize(self, out):
    out.write('size_t %s::serializedSize () {\n'%self.name)
    out.write('  size_t size = 0;\n')
    for f in self.fields:
      f.writeSerializedSize(out)
    out.write('  return size;\n')
    out.write('}\n\n')

  def writeDestructor(self, out):
    out.write('%s::~%s () {\n'%(self.name, self.name))
    for f in self.fields:
      f.writeDestructor(out)
    out.write('}\n\n')

  def writeImpl(self, out):
    self._writeUnserialize(out)
    self._writeSerialize(out)
    self._writeSerializedSize(out)
    self.writeDestructor(out)

declStaticCode = """
struct Message {
  //unserialize the given data of given len. Returns true on success, false if the unserialization failed
  bool unserialize (size_t len, const char* data);
  void serialize (char* data);

  //size of this structure when serialized
  virtual size_t serializedSize () = 0;
  //serialize this message into the given structure. The given data MUST have a size of at least serializedSize
  protected:
  virtual void _serialize (char* data, size_t& offset) = 0;
  virtual bool _unserialize (size_t len, const char* data, size_t& offset) = 0;
};

"""

implStaticCode = """
#include "msg.h"

static inline void endian_swap(uint16_t& x) {
  x = (x >> 8) |
      (x << 8);
}

static inline void endian_swap(uint32_t& x) {
  x = (x >> 24) |
     ((x << 8) & 0x00FF0000) |
     ((x >> 8) & 0x0000FF00) |
      (x << 24);
}

static inline void endian_swap(uint64_t& x) {
  x = (x >> 56) |
     ((x << 40) & 0x00FF000000000000) |
     ((x << 24) & 0x0000FF0000000000) |
      ((x << 8) & 0x000000FF00000000) |
      ((x >> 8) & 0x00000000FF000000) |
     ((x >> 24) & 0x0000000000FF0000) |
     ((x >> 40) & 0x000000000000FF00) |
      (x << 56);
}

static inline int is_big_endian() {
    union {
        uint32_t i;
        char c[4];
    } bint = {0x01020304};

    return bint.c[0] == 1; 
}

#define CHECK_LOAD_ERROR(size) if (offset+size > len) { \\
    LOGE("Error in readUint16_t, offset(%i) > len(%i)", (offset+size), len); \\
    return false; \\
  } 

static inline bool readBool (size_t len, const char* data, size_t& offset, bool& var) {
  CHECK_LOAD_ERROR(sizeof(char))
  var = data[offset] != 0;
  offset++;
}

static inline bool readUint8_t (size_t len, const char* data, size_t& offset, uint8_t& var) {
  CHECK_LOAD_ERROR(sizeof(uint8_t))
  var = *(uint8_t*)&data[offset];
  offset += sizeof(uint8_t);
}

static inline bool readChar (size_t len, const char* data, size_t& offset, char& var) {
  CHECK_LOAD_ERROR(sizeof(char))
  var = data[offset];
  offset += sizeof(char);
}

static inline bool readUint16_t (size_t len, const char* data, size_t& offset, uint16_t& var) {
  CHECK_LOAD_ERROR(sizeof(uint16_t))
  var = *(uint16_t*)&data[offset];
  offset += sizeof(uint16_t);
  if (!is_big_endian())
    endian_swap(var);
}

static inline bool readFloat (size_t len, const char* data, size_t& offset, float& var) {
  CHECK_LOAD_ERROR(sizeof(uint32_t))
  uint32_t tmp = *(uint32_t*)&data[offset];
  if (!is_big_endian())
    endian_swap(tmp);
  var = tmp/1000.0f;
  offset+=sizeof(uint32_t);
}

static inline void writeBool (char* data, size_t& offset, bool var) {
  data[offset++] = var?(char)1:(char)0;
}

static inline void writeUint8_t (char* data, size_t& offset, uint8_t var) {
  data[offset++] = *(char*)&var;
}

static inline void writeChar (char* data, size_t& offset, char var) {
  data[offset++] = var;
}

static inline void writeUint16_t (char* data, size_t& offset, uint16_t var) {
  if (!is_big_endian()) {
    uint16_t tmp = var;
    endian_swap(tmp);
    *(uint16_t*)&data[offset] = tmp;
  } else {
    *(uint16_t*)&data[offset] = var;
  }
  offset += sizeof(uint16_t);
}

static inline void writeFloat (char* data, size_t& offset, float var) {
  uint32_t tmp = var*1000;
  if (!is_big_endian())
    endian_swap(tmp);
  *(uint32_t*)&data[offset] = tmp;
  offset += sizeof(uint32_t);
}

bool Message::unserialize (size_t len, const char* data) {
  size_t offset = 0;
  return _unserialize(len, data, offset);
}

void Message::serialize (char* data) {
  size_t offset = 0;
  _serialize(data, offset);
}

"""

import os
def main():
  messages = []
  for message in messagesDef:
    messages.append(Message(message))

  out = open("msg.h",'w')
  out.write("#ifndef _MESSAGES_H\n#define _MESSAGES_H\n#include \"def.h\"\n\n")
  out.write(declStaticCode)
  for message in messages:
    message.writeDecl(out)
  out.write("#endif\n")
  out.close()

  out = open("msg.cpp",'w')
  out.write(implStaticCode)
  for message in messages:
    message.writeImpl(out)
  out.close()

  print '-- checking msg.h syntax'
  os.system('cat msg.h | gcc -x c++ -fsyntax-only -')
  print '\n-- checking msg.cpp syntax'
  os.system('cat msg.cpp | gcc -x c++ -fsyntax-only -')

if __name__ == '__main__':
  main()

