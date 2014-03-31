/*
*kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;
*
*  C/C++ Implementation: vs_common
*
* Description:
*
*
* Author: iegor <rmtdev@gmail.com>, (C) 2014
*
* Copyright: See COPYING file that comes with this distribution
*/
#ifndef __KDEVPART_VSTUDIOPART_COMMON_H__
#define __KDEVPART_VSTUDIOPART_COMMON_H__

#include <quuid.h>

// Debug messaging
#define kddbg kdDebug() << "[-- VS PART --]: "

#define VSPART_SOLUTION_ITEM "vs_solution"
#define VSSOLUTION_VERSION 10

//BEGIN //VStudio namespace
namespace VStudio {
  /**
  * Unique identifier class helper
  * used mostly in VS entities
  */
  /*
  class UUID {
  public:
    UUID();
  private:
    union {
      unsigned char m1[32];
      struct {
        unsigned int f1;
        unsigned short s1;
        unsigned short s2;
        unsigned short s3;
        union {
          unsigned char m2[12];
          struct {
            unsigned int f2;
            unsigned short s4;
          };
          struct {
            unsigned short sx1;
            unsigned short sx2;
            unsigned short sx3;
          };
        };
      };
    };
  };
  */
};
//END // VStudio namespace
#endif