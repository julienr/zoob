#!/usr/bin/python
import sys

#A message definition has two attributes. A name and a set of fields. 
#Message that are only used as component for other messages (and are therefore NEVER sent on their
#own) must set their 'component' attribute to true. No message identifier will be generated for those

messagesDef=[
  {'name':'BytesArray','fields':{'bytes':('array','char')},'component':True},
  {'name':'Vector','fields':{'x':'float','y':'float'},'component':True},
  {'name':'Damage','fields':{'entityID':'uint16_t','damages':'uint16_t'},'component':True},
  {'name':'RocketInfo','fields':{'rocketID':'uint16_t','position':'Vector','velocity':'Vector'},'component':True},
  {'name':'MineInfo','fields':{'mineID':'uint16_t','position':'Vector'},'component':True},
  {'name':'PlayerInfo','fields':
    {'playerID':'uint16_t',
     'position':'Vector',
     'velocity':'Vector',
     'rocketInfos':('array','RocketInfo'),
     'mineInfos':('array','MineInfo')},'component':True},
  {'name':'Hello','fields':
    {'nickname':'String'}},
  {'name':'Welcome','fields':
    {'level':'BytesArray',
     'serverState':'uint8_t',
     'playerID':'uint16_t'}},
  {'name':'Kicked','fields':
    {'reason':'String'}},
  {'name':'Join','fields':
    {}},
  {'name':'Joined','fields':
    {}},
  {'name':'NotJoined','fields':
    {}},
  {'name':'Spawn','fields':
    {'position':'Vector'}},
  {'name':'Explosion','fields':
    {'position':'Vector',
     'exploderID':'uint16_t',
     'destroyExploder':'bool',
     'damages':('array','Damage')}},
  {'name':'GameState','fields':
    {'playerInfos':('array','PlayerInfo')}},
  {'name':'StateChange','fields':
    {'newState':'uint8_t',
     'stateDuration':'uint8_t'}}
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
  out.write('loaded &= ')
  if type == 'bool':
    out.write('readBool(len, data, offset, %s); ERR_CHK\n'%name)
  elif type == 'uint8_t':
    out.write('readUint8_t(len, data, offset, %s); ERR_CHK\n'%name)
  elif type == 'uint16_t':
    out.write('readUint16_t(len, data, offset, %s); ERR_CHK\n'%name)
  elif type == 'float':
    out.write('readFloat(len, data, offset, %s); ERR_CHK\n'%name)
  elif type == 'char':
    out.write('readChar(len, data, offset, %s); ERR_CHK\n'%name)
  else:
    out.write('%s.unserialize(len, data, offset); ERR_CHK\n'%name)

def writeSerialize(out, name, type, numTabs):
  for i in range(numTabs):
    out.write(' ')
  if type == 'bool':
    out.write('writeBool(data, offset, %s);\n'%name)
  elif type == 'uint8_t':
    out.write('writeUint8_t(data, offset, %s);\n'%name)
  elif type == 'uint16_t':
    out.write('writeUint16_t(data, offset, %s);\n'%name)
  elif type == 'float':
    out.write('writeFloat(data, offset, %s);\n'%name)
  elif type == 'char':
    out.write('writeChar(data, offset, %s);\n'%name)
  else:
    out.write('%s.serialize(data, offset);\n'%name)

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
    out.write('  loaded &= readUint16_t(len, data, offset, %s); ERR_CHK\n'%self.counterName)
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

  def writeDecl(self, out):
    out.write('struct %s : public Message {\n'%self.name)
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

    # methods
    out.write('  virtual ~%s ();\n'%self.name)
    out.write('\n  //methods\n')
    out.write('  MessageIdentifier getIdentifier() const;\n')
    out.write('  size_t serializedSize() const;\n')
    out.write('  void serialize (char* data, size_t& offset) const;\n')
    out.write('  bool unserialize (size_t len, const char* data, size_t& offset);\n')
    out.write('\n  //loading flags\n')

    # forbid copy constructor and assignment
    out.write('private:\n')
    out.write('  %s (const %s& o) {}\n'%(self.name, self.name))
    out.write('  %s& operator = (const %s& o) { return *this; }\n'%(self.name, self.name))
    out.write('};\n\n')

    #Really ugly hack to have a String struct that simply inherits BytesArray
    if self.name == 'BytesArray':
      out.write('struct String : public BytesArray {};\n')

  def _writeUnserialize(self, out):
    out.write('bool %s::unserialize (size_t len, const char* data, size_t& offset) {\n'%self.name)
    out.write('  bool loaded=true;\n')
    for f in self.fields:
      f.writeUnserialize(out)
    out.write('  error:\n')
    out.write('  return loaded;\n')
    out.write('}\n\n')

  def _writeSerialize(self, out):
    out.write('void %s::serialize (char* data, size_t& offset) const {\n'%self.name)
    for f in self.fields:
      f.writeSerialize(out)
    out.write('}\n\n')

  def _writeSerializedSize(self, out):
    out.write('size_t %s::serializedSize () const {\n'%self.name)
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

  def writeGetIdentifier(self, out):
    out.write('MessageIdentifier %s::getIdentifier() const {\n'%self.name)
    if self.component:
      out.write('  return MESSAGE_IDENTIFIER_COMPONENT;\n')
    else:
      out.write('  return %s;\n'%self.name.upper())
    out.write('}\n\n')


  def writeNewFunc(self, out):
    out.write('Message* _new%s () {\n'%self.name)
    out.write('  return new %s();\n'%self.name)
    out.write('}\n\n')

  def writeImpl(self, out):
    self.writeGetIdentifier(out)
    self._writeUnserialize(out)
    self._writeSerialize(out)
    self._writeSerializedSize(out)
    self.writeDestructor(out)
    if not self.component:
      self.writeNewFunc(out)

decodeMessage = """
Message* decodeMessage (size_t dataLen, const char* data, size_t& offset) {
  uint8_t msgID = data[offset++];
  if (msgID >= MESSAGE_IDENTIFIER_COUNT) {
    LOGE("Unknown msgID : %u", msgID);
    return NULL;
  }
  Message* msg = msgFuncs[msgID]();
  if (!msg->unserialize(dataLen, data, offset)) {
    LOGE("Error unserializing message with ID %u", msgID);
    delete msg;
    return NULL;
  }
  return msg;
}

void encodeMessage (const Message* msg, char* data, size_t& offset) {
  MessageIdentifier id = msg->getIdentifier();
  if (id >= MESSAGE_IDENTIFIER_COUNT) {
    LOGE("Error : trying to encode unhandled message id : %u", id);
    return;
  }
  uint8_t msgID = (uint8_t)id;
  writeUint8_t(data, offset, msgID);
  msg->serialize(data, offset);
}

"""

def writeIndirectionTable(out, messages):
  out.write('typedef Message* (*newMsgFunc) ();\n')
  out.write('newMsgFunc msgFuncs[MESSAGE_IDENTIFIER_COUNT] = {\n')
  firstLevelMessages = [m for m in messages if not m.component]
  firstLevelMessages.sort(key=lambda x: x.identifier)
  for message in messages:
    if not message.component:
      out.write('_new%s,\n'%message.name)
  out.write('};\n\n')

  out.write('')
  out.write(decodeMessage) 


declStaticCode = """

struct Message {
  virtual MessageIdentifier getIdentifier() const = 0;
  //unserialize the given data of given len. Returns true on success, false if the unserialization failed
  //serialize this message into the given structure. The given data MUST have a size of at least serializedSize
  virtual void serialize (char* data, size_t& offset) const = 0;
  virtual bool unserialize (size_t len, const char* data, size_t& offset) = 0;

  //size of this structure when serialized
  virtual size_t serializedSize () const = 0;
};


//This function will decode the message contained in the block of data of size dataLen
//starting at the given offset. The offset will be modified so it point to the byte just after the last read.
//This can return NULL on failure (if data doesn't contain a valid message for example)
//On success, returns a newly allocated Message. The caller is responsible for deallocation
Message* decodeMessage (size_t dataLen, const char* data, size_t& offset);

//Encode the given message in the given datablock. Data MUST be large enough to contain serializedSize()+1 bytes.
void encodeMessage (const Message* msg, char* data, size_t& offset);

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

#define CHECK_LOAD_ERROR(size) if (offset+size > len) { \\
    LOGE("Error in readUint16_t, offset(%lu) > len(%lu)", (offset+size), len); \\
    return false; \\
  } 

static inline bool readBool (size_t len, const char* data, size_t& offset, bool& var) {
  CHECK_LOAD_ERROR(sizeof(char))
  var = data[offset] != 0;
  offset++;
  return true;
}

static inline bool readUint8_t (size_t len, const char* data, size_t& offset, uint8_t& var) {
  CHECK_LOAD_ERROR(sizeof(uint8_t))
  var = *(uint8_t*)&data[offset];
  offset += sizeof(uint8_t);
  return true;
}

static inline bool readChar (size_t len, const char* data, size_t& offset, char& var) {
  CHECK_LOAD_ERROR(sizeof(char))
  var = data[offset];
  offset += sizeof(char);
  return true;
}

static inline bool readUint16_t (size_t len, const char* data, size_t& offset, uint16_t& var) {
  CHECK_LOAD_ERROR(sizeof(uint16_t))
  var = *(uint16_t*)&data[offset];
  offset += sizeof(uint16_t);
  if (!is_big_endian())
    endian_swap(var);
  return true;
}

static inline bool readFloat (size_t len, const char* data, size_t& offset, float& var) {
  CHECK_LOAD_ERROR(sizeof(uint32_t))
  uint32_t tmp = *(uint32_t*)&data[offset];
  if (!is_big_endian())
    endian_swap(tmp);
  var = tmp/1000.0f;
  offset+=sizeof(uint32_t);
  return true;
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

#define ERR_CHK if(!loaded) { LOGE("Deserialization error at %s:%u", __FILE__, __LINE__); goto error; }

"""

import os
def main():
  messages = []
  for message in messagesDef:
    messages.append(Message(message))

  #HEADER
  out = open("msg.h",'w')
  out.write("#ifndef _MESSAGES_H\n#define _MESSAGES_H\n#include \"def.h\"\n\n")
  out.write("namespace zoobmsg {\n")
  #First, output message identifier enum
  out.write("enum MessageIdentifier {\n")
  idCount = 0
  for message in messages:
    if not message.component:
      out.write("  %s=%i,\n"%(message.name.upper(), idCount))
      message.identifier = idCount
      idCount += 1
  out.write("  MESSAGE_IDENTIFIER_COUNT=%i,\n"%idCount)
  out.write("  MESSAGE_IDENTIFIER_COMPONENT=%i\n"%(idCount+1))
  out.write("};\n")
  out.write(declStaticCode)
  for message in messages:
    message.writeDecl(out)
  out.write("} //zoobmsg\n")
  out.write("#endif\n")
  out.close()

  #CPP
  out = open("msg.cpp",'w')
  out.write('#include "msg.h"\n')
  out.write('namespace zoobmsg {\n')
  out.write(implStaticCode)
  for message in messages:
    message.writeImpl(out)
  writeIndirectionTable(out, messages)
  out.write('} //zoobmsg\n')
  out.close()

#  print '-- checking msg.h syntax'
#  os.system('cat msg.h | gcc -x c++ -fsyntax-only -')
#  print '\n-- checking msg.cpp syntax'
#  os.system('cat msg.cpp | gcc -x c++ -fsyntax-only -')

if __name__ == '__main__':
  main()

