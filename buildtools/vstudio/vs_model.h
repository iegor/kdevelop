/*
*kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;
*
*  C/C++ Implementation: vs_model
*
* Description:
* <_rlt> prefix means relative path
*
* Author: iegor <rmtdev@gmail.com>, (C) 2013
*
* Copyright: See COPYING file that comes with this distribution
*/
#ifndef __KDEVPART_VSTUDIOPART_SOLUTION_H__
#define __KDEVPART_VSTUDIOPART_SOLUTION_H__

#include <qfile.h>
#include <qguardedptr.h>

#include "vs_common.h"

//BEGIN //VStudio namespace
namespace VStudio {
  class VSEntity {
  public:
    enum _vs_ent_type {
      vs_solution = 0,
      vs_project,
      vs_filter,
      vs_file,
    };
  public:
    VSEntity(_vs_ent_type typ, const QString &name);
    VSEntity(_vs_ent_type typ, const QString &name, const QUuid &uid);
    virtual ~VSEntity();

    QString name;
    QUuid uuid;
    _vs_ent_type type;
  };

  class VSSolution : public VSEntity {
  public:
    VSSolution(const QString &name, const QString &path_rlt);
    VSSolution(const QString &name, const QUuid &uid, const QString &path_rlt);
    virtual ~VSSolution();
  private:
    QString path_rlt;
  };

  class VSProject : public VSEntity {
  public:
    VSProject(const QString &name, const QString &path_rlt);
    VSProject(const QString &name, const QUuid &uid, const QString &path_rlt);
    virtual ~VSProject();
  private:
    QString path_rlt;
  };
};
//END // VStudio namespace
#endif /*__KDEVPART_VSTUDIOPART_SOLUTION_H__ */
