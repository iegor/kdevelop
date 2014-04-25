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
//#include <string.h>

#include "vs_common.h"

namespace VStudio {
  /*
  UUID::UUID() {
    memset(&m1, 0, 32);
  }
  */

  bool readGUID(QTextStream &s, QUuid &uid) {
    // if(!s) {
    //   kddbg << "Error! unusable stream supplied" << endl;
    //   return false; }
#ifndef QT_NO_QUUID_STRING
    ushort raw[37]={0};
    s.readRawBytes((char*)raw, 72);
    QString guid((const QChar*)raw, 36);
    uid = QUuid(guid);
    /*
    QRegExp rx("[A-F0-9]{8}-[A-F0-9]{4}-[A-F0-9]{4}-[A-F0-9]{4}-[A-F0-9]{12}");
    int irx = rx.search(token);
    QString prjguid = token.mid(irx, 36);
    QUuid prj_uid(prjguid);
    */
#else
    char d4[8];
    s >> iuid.data1 >> iuid.data2 >> iuid.data3;
    s >> d4[0] >> d4[1] >> d4[2] >> d4[3] >> d4[4] >> d4[5] >> d4[6] >> d4[7];
    memcpy(iuid.data4, d4, sizeof(char)*8);
#endif
    return true;
  }

  QString guid2String(const QUuid &uid) {
#ifndef QT_NO_QUUID_STRING
    return uid.toString();
#else
#endif
  }

  e_VSEntityType uid2VSType(const QUuid &uid) const {
    if(uid == uid_vs8project) return vs_project;
    else if(uid == uid_vs8filter) return vs_filter;
  }
};