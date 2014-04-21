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

// #include <qfile.h>
#include <qguardedptr.h>

#include "vs_common.h"

#ifdef USE_BOOST
#include <boost/container/vector.hpp>
#endif

//BEGIN //VStudio namespace
namespace VStudio {
  class VSProject;
  class VSSolution;

  class VSEntity {
  public:
    VSEntity(e_VSEntityType typ, const QString &name);
    VSEntity(e_VSEntityType typ, const QString &name, const QUuid &uid);
    virtual ~VSEntity();

    void uidSet(const QUuid &uid) {
      uuid = uid;
    }

    QUuid uidGet() const {
      return uuid;
    }

    QString getName() const {
      return name;
    }

    e_VSEntityType getType() const {
      return type;
    }

  protected:
    QString name;
    QUuid uuid;
    e_VSEntityType type;
  };

  class VSProject : public VSEntity {
    public:
      VSProject(const QString &name, const QString &path_rlt);
      VSProject(const QString &name, const QUuid &uid, const QString &path_rlt);
      virtual ~VSProject();

      void setParent(VSSolution* parent);
      QString getRelativePath() const { return path_rlt; }
    private:
      QString path_rlt;
      VSSolution *parent;
  };

  class VSSolution : public VSEntity {
#ifdef USE_BOOST
    typedef boost::container::vector<VSProject*>::const_iterator proj_v_ci;
    typedef boost::container::vector<VSProject*>::iterator proj_v_i;
#else
#endif
  public:
    VSSolution(const QString &name, const QString &path_rlt);
    VSSolution(const QString &name, const QUuid &uid, const QString &path_rlt);
    virtual ~VSSolution();

    void insertProj(VSProject* prj);
    QString getRelativePath() const { return path_rlt; }
    bool dumpProjectsLayout(QString &layout);
  private:
    QString path_rlt;
#ifdef USE_BOOST
    boost::container::vector<VSProject*> projects;
#else
#endif
  };
};
//END // VStudio namespace
#endif /*__KDEVPART_VSTUDIOPART_SOLUTION_H__ */
