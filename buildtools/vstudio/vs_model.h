/*
*kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;
*
*  C/C++ Implementation: vs_model
*
* Description:
* <_rlt> prefix means relative path
*
* NOTE: multiple parents stored for entities
* That is done mainly to track an entity from it's internals in all parent instances
* used in current session
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

    virtual void setName(const QString &name);

    e_VSEntityType getType() const {
      return type;
    }

    virtual QString getRelativePath() const = 0;
    virtual bool setRelativePath(const QString &path) = 0;
    virtual vse_p getByUID(const QUuid &uid) const = 0;
  protected:
    QString name;
    QUuid uuid;
    e_VSEntityType type;
  };

  class VSSolution : public VSEntity {
    public:
      VSSolution(const QString &name, const QString &path_rlt);
      VSSolution(const QString &name, const QUuid &uid, const QString &path_rlt);
      virtual ~VSSolution();

    // VS Entity interface methods:
      virtual QString getRelativePath() const { return path_rlt; }
      virtual bool setRelativePath(const QString &path);
      virtual vsp_p getByUID(const QUuid &uid) const; // This will return project ptr, to reuse code

    // VS Solution methods:
      void insertProj(vsp_p prj);
      bool dumpProjectsLayout(QString &layout);
      vsf_p getFltByUID(const QUuid &uid) const;
    private:
      QString path_rlt;
#ifdef USE_BOOST
      boost::container::vector<vsp_p> projects;
      boost::container::vector<vsf_p> filters;
#else
#endif
  };

  class VSProject : public VSEntity {
    public:
      VSProject(const QString &name, const QString &path_rlt);
      VSProject(const QString &name, const QUuid &uid, const QString &path_rlt);
      virtual ~VSProject();

      // VS Entity methods:
      virtual QString getRelativePath() const { return path_rlt; }
      virtual bool setRelativePath(const QString &path);
      virtual vsp_p getByUID(const QUuid &uid) const; // This will get dependency to reuse code

      // VS Project methods:
      void setParent(vss_p parent);
      vsp_p getReqByUid(const QUuid &uid) const;
    private:
      QString path_rlt;
#ifdef USE_BOOST
      boost::container::vector<vss_p> pnts; // Parent solutions
      boost::container::vector<vsp_p> deps; // Projects dependant on this one
      boost::container::vector<vsp_p> reqs; // Projects required to build this one, i.e. dependencies
#else
#endif
  };

  class VSFilter : public VSEntity {
  public:
    VSFilter(const QString &name, vse_p parent);
    virtual ~VSFilter();

    // VS Entity interface methods:
    virtual QString getRelativePath() const;
    virtual bool setRelativePath(const QString &path);
    virtual vse_p getByUID(const QUuid &uid) const;

    // VS Filter methods:
    e_VSEntityType getParentType() {
      return parent->getType();
    }
  private:
#ifdef USE_BOOST
    boost::container::vector<vse_p> chld; // Children
#else
#endif
    vse_p parent;
  };

  class VSFile : public VSEntity {
  public:
    VSFile(const QString &name, vsp_p parent);
    virtual ~VSFile();

    // VS Entity interface methods:
    virtual QString getRelativePath() const;
    virtual bool setRelativePath(const QString &path);
    /*!
    * NOTE: that this will return a pointer to project, one of
    * possible many parents.
    */
    virtual vse_p getByUID(const QUuid &uid) const;
  private:
    vsp_p parent; // Current parent project
    /**
    * List of parent projects
    * used to store every parent project that uses current file
     * to detect usage of sources, to maybe then optimize the
    * archtecture of projects
    */
#ifdef USE_BOOST
    boost::container::vector<vsp_p> pnts;
#else
#endif
  };
};
//END // VStudio namespace
#endif /*__KDEVPART_VSTUDIOPART_SOLUTION_H__ */
