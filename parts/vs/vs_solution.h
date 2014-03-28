//kate: space-indent on; tab-width 4; indent-width 4; indent-mode cstyle; encoding UTF-8;
// C++ Interface: vs_solution
//
// Description:
//
//
// Author: iegor <rmtdev@gmail.com>, (C) 2013
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef __KDEVPART_VSTUDIOPART_SOLUTION_H__
#define __KDEVPART_VSTUDIOPART_SOLUTION_H__

#include <qfile.h>

#define VSSOLUTION_VERSION 10

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
            };
        };
    };
};

/**
 * Visual Studio solution.
 */
class VSSolution {
public:
    VSSolution(QString name, QString path);
    ~VSSolution();

    bool Parse();
private: // Data
    QFile fl;
private: // Utils
    bool _write_file();
}; /* VSSolution */

#endif /*__KDEVPART_VSTUDIOPART_SOLUTION_H__ */