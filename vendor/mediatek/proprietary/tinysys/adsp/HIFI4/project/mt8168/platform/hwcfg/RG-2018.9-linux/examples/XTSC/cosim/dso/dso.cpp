// Customer ID=13943; Build=0x75f5e; Copyright (c) 2009-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

#if !defined(PEEK_CONFIG_MEMORY) || !defined(POKE_CONFIG_MEMORY)
#error The C++ macros PEEK_CONFIG_MEMORY and POKE_CONFIG_MEMORY must be defined in order to compile this file
#endif

#define NCSC_INCLUDE_TASK_CALLS
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cstring>
#include <iomanip>
#include <map>
#include "svdpi.h"


using namespace std;



typedef unsigned char u8;
typedef unsigned int  u32;



#undef LOCATION
#undef LOG
#undef QUOTE
#undef _QUOTE
#undef EXTERN


#define LOCATION __FILE__ << ":" << __LINE__

#if 0
#define LOG(msg)  do { cout << LOCATION << ": " << msg << dec << endl; } while (false)
#else
#define LOG(msg)  do { ; } while (false)
#endif

#define _QUOTE(a) #a
#define QUOTE(arg) _QUOTE(arg)


#if defined(cadence) || defined(synopsys)
#define EXTERN extern "C"
#else
#define EXTERN extern 
#endif


EXTERN int PEEK_CONFIG_MEMORY(u32 address,       u32 *data);
EXTERN int POKE_CONFIG_MEMORY(u32 address, const u32  data);



static map<string, svScope> scope_map;



extern "C" void peek(u32 address8, u32 size8, u8 *buffer, const char *dso_cookie, u32 port) {
  if (!dso_cookie || !dso_cookie[0]) {
    ostringstream oss;
    oss << LOCATION << " peek() requires a non-null, non-empty dso_cookie";
    throw runtime_error(oss.str());
  }

  u8 *buf = buffer;
  svScope scope         (NULL);
  string  scope_name    (dso_cookie);

  map<string, svScope>::const_iterator is = scope_map.find(scope_name);
  if (is != scope_map.end()) {
    scope = is->second;
  }
  else {
    scope = svGetScopeFromName(scope_name.c_str());
    if (scope == NULL) {
      ostringstream oss;
      oss << LOCATION << " peek() svGetScopeFromName() failed for dso_cookie \"" << dso_cookie << "\"";
      throw runtime_error(oss.str());
    }
    scope_map[scope_name] = scope;
  }
  svSetScope(scope);

  u32 header_bytes  = ((address8 & 0x3) ? min(size8, (4 - (address8 & 0x3))) : ((size8 < 4) ? size8 : 0));
  u32 trailer_bytes = (size8 - header_bytes) & 0x3;
  u32 body_bytes    = size8 - header_bytes - trailer_bytes;

  u32 data;
  u32 addr8 = address8;

  if (header_bytes) {
    u32 offset = addr8 & 0x3;
    addr8 = addr8 & static_cast<u32>(-4);  // align addr8 to word32 boundary
    if (PEEK_CONFIG_MEMORY(addr8, &data) != 0) {
      ostringstream oss;
      oss << LOCATION << " " << QUOTE(PEEK_CONFIG_MEMORY) << "() for header bytes failed.  address8=0x" << hex << address8
          << " size8=0x" << size8;
      throw runtime_error(oss.str());
    }
    u8 *p_d = reinterpret_cast<u8*>(&data);
    u8 *p_byte_array = reinterpret_cast<u8*>(buffer);
    u32 num_bytes = header_bytes;
    p_d += offset;
    memcpy(p_byte_array, p_d, num_bytes);
    buffer += header_bytes;
    addr8 += 4;
  }

  while (body_bytes) {
    if (PEEK_CONFIG_MEMORY(addr8, &data) != 0) {
      ostringstream oss;
      oss << LOCATION << " " << QUOTE(PEEK_CONFIG_MEMORY) << "() for body bytes failed.  address8=0x" << hex << address8
          << " size8=0x" << size8;
      throw runtime_error(oss.str());
    }
    u8 *p_d = reinterpret_cast<u8*>(&data);
    u8 *p_byte_array = reinterpret_cast<u8*>(buffer);
    memcpy(p_byte_array, p_d, 4);
    buffer += 4;
    addr8 += 4;
    body_bytes -= 4;
  }

  if (trailer_bytes) {
    if (PEEK_CONFIG_MEMORY(addr8, &data) != 0) {
      ostringstream oss;
      oss << LOCATION << " " << QUOTE(PEEK_CONFIG_MEMORY) << "() for trailer bytes failed.  address8=0x" << hex << address8
          << " size8=0x" << size8;
      throw runtime_error(oss.str());
    }
    u8 *p_d    = reinterpret_cast<u8*>(&data);
    u8 *p_byte_array = reinterpret_cast<u8*>(buffer);
    u32 num_bytes = trailer_bytes;
    memcpy(p_byte_array, p_d, num_bytes);
    buffer += trailer_bytes;
    addr8 += 4;
  }

  ostringstream oss;
  for (u32 i=0; i<size8; ++i) {
    oss << hex << setfill('0') << setw(2) << (u32)buf[i] << " ";
  }
  LOG("peek cookie=" << (dso_cookie ? dso_cookie : "NULL") << " address=0x" << hex << setfill('0') << setw(8) << address8 <<
      " size=" << dec << size8 << " data: " << oss.str());
}



extern "C" void poke(u32 address8, u32 size8, const u8 *buffer, const char *dso_cookie, u32 port) {
  if (!dso_cookie || !dso_cookie[0]) {
    ostringstream oss;
    oss << LOCATION << " peek() requires a non-null, non-empty dso_cookie";
    throw runtime_error(oss.str());
  }

  ostringstream oss;
  for (u32 i=0; i<size8; ++i) {
    oss << hex << setfill('0') << setw(2) << (u32)buffer[i] << " ";
  }
  LOG("poke cookie=" << (dso_cookie ? dso_cookie : "NULL") << " address=0x" << hex << setfill('0') << setw(8) << address8 <<
      " data: " << oss.str());

  svScope scope         (NULL);
  string  scope_name    (dso_cookie);

  map<string, svScope>::const_iterator is = scope_map.find(scope_name);
  if (is != scope_map.end()) {
    scope = is->second;
  }
  else {
    scope = svGetScopeFromName(scope_name.c_str());
    if (scope == NULL) {
      ostringstream oss;
      oss << LOCATION << " poke() svGetScopeFromName() failed for dso_cookie \"" << dso_cookie << "\"";
      throw runtime_error(oss.str());
    }
    scope_map[scope_name] = scope;
  }
  svSetScope(scope);


  u32 header_bytes  = ((address8 & 0x3) ? min(size8, (4 - (address8 & 0x3))) : ((size8 < 4) ? size8 : 0));
  u32 trailer_bytes = (size8 - header_bytes) & 0x3;
  u32 body_bytes    = size8 - header_bytes - trailer_bytes;

  u32 data;
  u32 addr8 = address8;

  if (header_bytes) {
    u32 offset = addr8 & 0x3;
    addr8 = addr8 & static_cast<u32>(-4);  // align addr8 to word32 boundary
    if (PEEK_CONFIG_MEMORY(addr8, &data) != 0) {
      ostringstream oss;
      oss << LOCATION << " " << QUOTE(PEEK_CONFIG_MEMORY) << "() for header bytes failed.  address8=0x" << hex << address8
          << " size8=0x" << size8;
      throw runtime_error(oss.str());
    }
    u8 *p_d = reinterpret_cast<u8*>(&data);
    const u8 *p_byte_array = reinterpret_cast<const u8*>(buffer);
    u32 num_bytes = header_bytes;
    p_d += offset;
    memcpy(p_d, p_byte_array, num_bytes);
    if (POKE_CONFIG_MEMORY(addr8, data) != 0) {
      ostringstream oss;
      oss << LOCATION << " " << QUOTE(POKE_CONFIG_MEMORY) << "() for header bytes failed.  address8=0x" << hex << address8
          << " size8=0x" << size8;
      throw runtime_error(oss.str());
    }
    buffer += header_bytes;
    addr8 += 4;
  }

  while (body_bytes) {
    u8 *p_d = reinterpret_cast<u8*>(&data);
    const u8 *p_byte_array = reinterpret_cast<const u8*>(buffer);
    memcpy(p_d, p_byte_array, 4);
    if (POKE_CONFIG_MEMORY(addr8, data) != 0) {
      ostringstream oss;
      oss << LOCATION << " " << QUOTE(POKE_CONFIG_MEMORY) << "() for body bytes failed.  address8=0x" << hex << address8
          << " size8=0x" << size8;
      throw runtime_error(oss.str());
    }
    buffer += 4;
    addr8 += 4;
    body_bytes -= 4;
  }

  if (trailer_bytes) {
    if (PEEK_CONFIG_MEMORY(addr8, &data) != 0) {
      ostringstream oss;
      oss << LOCATION << " " << QUOTE(PEEK_CONFIG_MEMORY) << "() for trailer bytes failed.  address8=0x" << hex << address8
          << " size8=0x" << size8;
      throw runtime_error(oss.str());
    }
    u8 *p_d = reinterpret_cast<u8*>(&data);
    const u8 *p_byte_array = reinterpret_cast<const u8*>(buffer);
    u32 num_bytes = trailer_bytes;
    memcpy(p_d, p_byte_array, num_bytes);
    if (POKE_CONFIG_MEMORY(addr8, data) != 0) {
      ostringstream oss;
      oss << LOCATION << " " << QUOTE(POKE_CONFIG_MEMORY) << "() for trailer bytes failed.  address8=0x" << hex << address8
          << " size8=0x" << size8;
      throw runtime_error(oss.str());
    }
    buffer += trailer_bytes;
    addr8 += 4;
  }

}

