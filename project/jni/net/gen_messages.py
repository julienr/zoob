#!/usr/bin/python
import sys

#A message definition has two attributes. A name and a set of fields. 
#Message that are only used as component for other messages (and are therefore NEVER sent on their
#own) must set their 'component' attribute to true. No message identifier will be generated for those

messagesDef=[
  {'name':'BytesArray','fields':{'bytes':('array','char')},'component':True},
  {'name':'Vector','fields':{'x':'float','y':'float'},'component':True},
  {'name':'RocketInfo','fields':{'rocketID':'uint16_t','position':'Vector','velocity':'Vector', 'speed':'float'},'component':True},
  {'name':'BombInfo','fields':{'bombID':'uint16_t','position':'Vector','timeleft':'float'},'component':True},
  {'name':'TankInfo','fields':
    {'tankID':'uint16_t',
     'position':'Vector',
     'velocity':'Vector',
     'livesLeft':'uint16_t',
     'rocketInfos':('array','RocketInfo'),
     'bombInfos':('array','BombInfo')},'component':True},
  {'name':'PlayerCommands','fields':
    {'tankID':'uint16_t',
      'fire':'bool',
      'mine':'bool',
      'shield':'bool',
      'fireDir':'Vector',
     'moveDir':'Vector'}},
  {'name':'Hello','fields':
    {'nickname':'BytesArray'}},
  {'name':'Version','fields':
    {'version':'uint16_t'}},
  {'name':'Welcome','fields':
    {'level':'BytesArray',
     'serverState':'uint8_t',
     'playerID':'uint16_t'}},
  {'name':'Kicked','fields':
    {'reason':'BytesArray'}},
  {'name':'WantSpawn','fields':
    {}},
  {'name':'Spawn','fields':
    {'position':'Vector',
     'tankID':'uint16_t'}},
  {'name':'Explosion','fields':
    {'position':'Vector',
     'boom':'bool',
     'destroyedEntities':('array','uint16_t')},
     'component':True},
  {'name':'GameState','fields':
    {'state':'uint8_t',
     'stateSecLeft':'float',
     'tankInfos':('array','TankInfo'),
     'explosions':('array','Explosion')}},
]

#messagesDef = [
#   {'name':'BytesArray','fields':{'array':('char','bytes')},'component':True},
#   {'name':'Test', 'fields':
#     {'array':('float','floats'),
#      'String':'testStr'}},
#]

# DO NOT TOUCH ANY FURTHER

def writeUnserialize(out, name, type, numTabs):
  for i in range(numTabs):
    out.write(' ')
  out.write('success &= ')
  if type == 'bool':
    out.write('readBool(len, data, offset, msg.%s); ERR_CHK\n'%name)
  elif type == 'uint8_t':
    out.write('readUint8_t(len, data, offset, msg.%s); ERR_CHK\n'%name)
  elif type == 'uint16_t':
    out.write('readUint16_t(len, data, offset, msg.%s); ERR_CHK\n'%name)
  elif type == 'float':
    out.write('readFloat(len, data, offset, msg.%s); ERR_CHK\n'%name)
  elif type == 'char':
    out.write('readChar(len, data, offset, msg.%s); ERR_CHK\n'%name)
  else:
    out.write('%s::unpack(len, data, offset, msg.%s); ERR_CHK\n'%(type, name))

def writeSerialize(out, name, type, numTabs):
  for i in range(numTabs):
    out.write(' ')
  out.write('success &= ')
  if type == 'bool':
    out.write('writeBool(len, data, offset, msg.%s); ERR_CHK\n'%name)
  elif type == 'uint8_t':
    out.write('writeUint8_t(len, data, offset, msg.%s); ERR_CHK\n'%name)
  elif type == 'uint16_t':
    out.write('writeUint16_t(len, data, offset, msg.%s); ERR_CHK\n'%name)
  elif type == 'float':
    out.write('writeFloat(len, data, offset, msg.%s); ERR_CHK\n'%name)
  elif type == 'char':
    out.write('writeChar(len, data, offset, msg.%s); ERR_CHK\n'%name)
  else:
    out.write('%s::pack(len, data, offset, msg.%s); ERR_CHK\n'%(type, name))

def writeSerializedSize(out, name, type, numTabs):
  for i in range(numTabs):
    out.write(' ')
  out.write('size += ')
  if type == 'bool':
    out.write('sizeof(char);\n')
  elif type == 'uint8_t':
    out.write('sizeof(uint8_t);\n')
  elif type == 'uint16_t':
    out.write('sizeof(uint16_t);\n')
  elif type == 'float':
    out.write('sizeof(uint32_t);\n')
  elif type == 'char':
    out.write('sizeof(char);\n')
  else:
    out.write('%s::packedSize(msg.%s);\n'%(type, name))



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

#An array field actually encompasses two fields :
#If T is the type of the elements in the array and N the name of the array, it will have
#first a counter then the actual array
#uint16_t numN
#T* N;
class ArrayField(Field):
  def __init__(self, type, name):
    self.type = type[1]
    self.name = name
    self.counterName = "num%s%s"%(self.name[0].upper(), self.name[1:])

  def writeDecl(self, out):
    out.write('  uint16_t %s;\n'%self.counterName)
    out.write('  %s* %s;\n'%(self.type, self.name))

  def writeUnserialize(self, out):
    out.write('  success &= readUint16_t(len, data, offset, msg.%s); ERR_CHK\n'%self.counterName)
    out.write('  delete [] msg.%s; //avoid memory leak\n'%self.name)
    out.write('  msg.%s = new %s[msg.%s];\n'%(self.name, self.type, self.counterName))
    out.write('  for (uint16_t i=0; i<msg.%s; i++) {\n'%self.counterName)
    writeUnserialize(out, '%s[i]'%self.name, self.type, 4)
    out.write('  }\n')

  def writeSerialize(self, out):
    out.write('  success &= writeUint16_t(len, data, offset, msg.%s);\n'%self.counterName)
    out.write('  for (uint16_t i=0; i<msg.%s; i++) {\n'%self.counterName)
    writeSerialize(out, '%s[i]'%self.name, self.type, 4)
    out.write('  }\n')

  def writeSerializedSize(self, out):
    out.write('  size += sizeof(uint16_t);\n')
    out.write('  for (uint16_t i=0; i<msg.%s; i++) {\n'%self.counterName)
    writeSerializedSize(out, '%s[i]'%self.name, self.type, 4)
    out.write('  }\n')

  def writeDestructor(self, out):
    out.write('  delete [] %s;\n'%self.name);

  def constructorDefault(self):
    return '%s(0),%s(NULL)'%(self.counterName,self.name)


class Message:
  def __init__(self, dict):
    self.name = dict['name']
    self.component = dict.get('component', False)
    self.fields = []
    fields = dict['fields']
    for name,t in fields.items():
      if type(t) is tuple and t[0] == 'array':
        self.fields.append(ArrayField(t, name))
      else:
        self.fields.append(Field(t, name))

  def writeDecl(self, out, count):
    out.write('struct %s {\n'%self.name)
    for f in self.fields:
      f.writeDecl(out)

    # constructor
    out.write('\n')
    out.write('  %s ()'%self.name)
    fieldsToInitialize = [f for f in self.fields if "constructorDefault" in dir(f)]
    if (len(fieldsToInitialize) > 0):
      defaults = []
      for f in fieldsToInitialize:
        defaults.append(f.constructorDefault())
      out.write(':%s'%(','.join(defaults)))
    out.write('{}\n')

    #type
    if not self.component:
      out.write('  static const uint8_t messageID = %i;\n'%count)
    # methods
    out.write('  virtual ~%s ();\n'%self.name)
    out.write('\n  //methods\n')
    out.write('  static size_t packedSize (const %s& msg);\n'%self.name)
    out.write('  static bool unpack (size_t len, const uint8_t* data, size_t& offset, %s& msg);\n'%self.name)
    out.write('  static bool pack (size_t len, uint8_t* data, size_t& offset, const %s& msg);\n'%self.name)

    # forbid copy constructor and assignment
    out.write('private:\n')
    out.write('  %s (const %s& UNUSED(o)) {}\n'%(self.name, self.name))
    out.write('  %s& operator = (const %s& UNUSED(o)) { return *this; }\n'%(self.name, self.name))
    out.write('};\n\n')

  def _writeUnserialize(self, out):
    out.write('bool %s::unpack (size_t len, const uint8_t* data, size_t& offset, %s& msg) {\n'%(self.name, self.name))
    out.write('  bool success=true;\n')
    for f in self.fields:
      f.writeUnserialize(out)
    out.write('  error:\n')
    out.write('  return success;\n')
    out.write('}\n\n')

  def _writeSerialize(self, out):
    out.write('bool %s::pack (size_t len, uint8_t* data, size_t& offset, const %s& msg) {\n'%(self.name, self.name))
    out.write('  bool success=true;\n')
    for f in self.fields:
      f.writeSerialize(out)
    out.write('  error:\n')
    out.write('  return success;\n')
    out.write('}\n\n')

  def _writeSerializedSize(self, out):
    out.write('size_t %s::packedSize (const %s& msg) {\n'%(self.name, self.name))
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

"""

implStaticCode = """
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

#define CHECK_BUF_SIZE(size) if (offset+size > len) { \\
    LOGE("Buffer overflow at %s:%i, offset(%lu) > len(%lu)", __FILE__, __LINE__, (offset+size), len); \\
    return false; \\
  } 

static inline bool readBool (size_t len, const uint8_t* data, size_t& offset, bool& var) {
  CHECK_BUF_SIZE(sizeof(char))
  var = data[offset] != 0;
  offset++;
  return true;
}

static inline bool readUint8_t (size_t len, const uint8_t* data, size_t& offset, uint8_t& var) {
  CHECK_BUF_SIZE(sizeof(uint8_t))
  var = *(uint8_t*)&data[offset];
  offset += sizeof(uint8_t);
  return true;
}

static inline bool readChar (size_t len, const uint8_t* data, size_t& offset, char& var) {
  CHECK_BUF_SIZE(sizeof(char))
  var = data[offset];
  offset += sizeof(char);
  return true;
}

static inline bool readUint16_t (size_t len, const uint8_t* data, size_t& offset, uint16_t& var) {
  CHECK_BUF_SIZE(sizeof(uint16_t))
  var = *(uint16_t*)&data[offset];
  offset += sizeof(uint16_t);
  if (!is_big_endian())
    endian_swap(var);
  return true;
}

static inline bool readFloat (size_t len, const uint8_t* data, size_t& offset, float& var) {
  CHECK_BUF_SIZE(sizeof(uint32_t))
  uint32_t tmp = *(uint32_t*)&data[offset];
  if (!is_big_endian())
    endian_swap(tmp);
  var = ((int32_t)tmp)/1000.0f;
  offset+=sizeof(uint32_t);
  return true;
}

static inline bool writeBool (size_t len, uint8_t* data, size_t& offset, bool var) {
  CHECK_BUF_SIZE(1);
  data[offset++] = var?(char)1:(char)0;
  return true;
}

static inline bool writeUint8_t (size_t len, uint8_t* data, size_t& offset, uint8_t var) {
  CHECK_BUF_SIZE(1);
  data[offset++] = *(char*)&var;
  return true;
}

static inline bool writeChar (size_t len, uint8_t* data, size_t& offset, char var) {
  CHECK_BUF_SIZE(1);
  data[offset++] = var;
  return true;
}

static inline bool writeUint16_t (size_t len, uint8_t* data, size_t& offset, uint16_t var) {
  CHECK_BUF_SIZE(1);
  if (!is_big_endian()) {
    uint16_t tmp = var;
    endian_swap(tmp);
    *(uint16_t*)&data[offset] = tmp;
  } else {
    *(uint16_t*)&data[offset] = var;
  }
  offset += sizeof(uint16_t);
  return true;
}

static inline bool writeFloat (size_t len, uint8_t* data, size_t& offset, float var) {
  CHECK_BUF_SIZE(1);
  uint32_t tmp = (uint32_t)(var*(int32_t)1000);
  if (!is_big_endian())
    endian_swap(tmp);
  *(uint32_t*)&data[offset] = tmp;
  offset += sizeof(uint32_t);
  return true;
}

#define ERR_CHK if(!success) { LOGE("Error at %s:%u", __FILE__, __LINE__); goto error; }

"""

import os
def main():
  messages = []
  for message in messagesDef:
    messages.append(Message(message))

  #HEADER
  out = open("Messages.h",'w')
  out.write("#ifndef _MESSAGES_H\n#define _MESSAGES_H\n#include \"def.h\"\n\n")
  out.write("namespace zoobmsg {\n")
  #First, output message identifier enum
#  out.write("enum MessageIdentifier {\n")
#  idCount = 0
#  for message in messages:
#    if not message.component:
#      out.write("  %s=%i,\n"%(message.name.upper(), idCount))
#      message.identifier = idCount
#      idCount += 1
#  out.write("  MESSAGE_IDENTIFIER_COUNT=%i,\n"%idCount)
#  out.write("  MESSAGE_IDENTIFIER_COMPONENT=%i\n"%(idCount+1))
#  out.write("};\n")
  out.write(declStaticCode)

  idCount = 0
  for message in messages:
    message.writeDecl(out, idCount)
    if not message.component:
      idCount += 1

  out.write("const uint8_t numMessages = %i;\n"%idCount)
  out.write("} //zoobmsg\n")
  out.write("#endif\n")
  out.close()

  #CPP
  out = open("Messages.cpp",'w')
  out.write('#include "Messages.h"\n')
#  out.write('#pragma GCC diagnostic push\n')
  out.write('#pragma GCC diagnostic ignored "-Wunused-parameter"\n')
  out.write('#pragma GCC diagnostic ignored "-Wunused-label"\n')
  out.write('namespace zoobmsg {\n')
  out.write(implStaticCode)
  for message in messages:
    message.writeImpl(out)
  out.write('} //zoobmsg\n')
#  out.write('#pragma GCC diagnostic pop\n')
  out.close()

#  print '-- checking msg.h syntax'
#  os.system('cat msg.h | gcc -x c++ -fsyntax-only -')
#  print '\n-- checking msg.cpp syntax'
#  os.system('cat msg.cpp | gcc -x c++ -fsyntax-only -')

if __name__ == '__main__':
  main()

