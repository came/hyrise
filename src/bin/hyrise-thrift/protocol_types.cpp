/**
 * Autogenerated by Thrift Compiler (0.9.1)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "protocol_types.h"

#include <algorithm>



const char* Query::ascii_fingerprint = "72EF5B703EDB4CFCAA2C8933873614A4";
const uint8_t Query::binary_fingerprint[16] = {0x72,0xEF,0x5B,0x70,0x3E,0xDB,0x4C,0xFC,0xAA,0x2C,0x89,0x33,0x87,0x36,0x14,0xA4};

uint32_t Query::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;

  bool isset_query = false;

  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->query);
          isset_query = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_I64) {
          xfer += iprot->readI64(this->session_context);
          this->__isset.session_context = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_BOOL) {
          xfer += iprot->readBool(this->autocommit);
          this->__isset.autocommit = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 4:
        if (ftype == ::apache::thrift::protocol::T_I64) {
          xfer += iprot->readI64(this->limit);
          this->__isset.limit = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 5:
        if (ftype == ::apache::thrift::protocol::T_I64) {
          xfer += iprot->readI64(this->offset);
          this->__isset.offset = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  if (!isset_query)
    throw TProtocolException(TProtocolException::INVALID_DATA);
  return xfer;
}

uint32_t Query::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("Query");

  xfer += oprot->writeFieldBegin("query", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeString(this->query);
  xfer += oprot->writeFieldEnd();

  if (this->__isset.session_context) {
    xfer += oprot->writeFieldBegin("session_context", ::apache::thrift::protocol::T_I64, 2);
    xfer += oprot->writeI64(this->session_context);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.autocommit) {
    xfer += oprot->writeFieldBegin("autocommit", ::apache::thrift::protocol::T_BOOL, 3);
    xfer += oprot->writeBool(this->autocommit);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.limit) {
    xfer += oprot->writeFieldBegin("limit", ::apache::thrift::protocol::T_I64, 4);
    xfer += oprot->writeI64(this->limit);
    xfer += oprot->writeFieldEnd();
  }
  if (this->__isset.offset) {
    xfer += oprot->writeFieldBegin("offset", ::apache::thrift::protocol::T_I64, 5);
    xfer += oprot->writeI64(this->offset);
    xfer += oprot->writeFieldEnd();
  }
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(Query &a, Query &b) {
  using ::std::swap;
  swap(a.query, b.query);
  swap(a.session_context, b.session_context);
  swap(a.autocommit, b.autocommit);
  swap(a.limit, b.limit);
  swap(a.offset, b.offset);
  swap(a.__isset, b.__isset);
}

const char* Result::ascii_fingerprint = "EFB929595D312AC8F305D5A794CFEDA1";
const uint8_t Result::binary_fingerprint[16] = {0xEF,0xB9,0x29,0x59,0x5D,0x31,0x2A,0xC8,0xF3,0x05,0xD5,0xA7,0x94,0xCF,0xED,0xA1};

uint32_t Result::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;

  bool isset_result = false;

  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->result);
          isset_result = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  if (!isset_result)
    throw TProtocolException(TProtocolException::INVALID_DATA);
  return xfer;
}

uint32_t Result::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("Result");

  xfer += oprot->writeFieldBegin("result", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeString(this->result);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(Result &a, Result &b) {
  using ::std::swap;
  swap(a.result, b.result);
}

