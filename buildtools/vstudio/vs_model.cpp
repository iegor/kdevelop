/*
*kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;
*
*  C/C++ Implementation: vs_model
*
* Description: Contains items for explorer and their methods
*
*
* Author: iegor <rmtdev@gmail.com>, (C) 2013
*
* Copyright: See COPYING file that comes with this distribution
*/
#include <string.h>
#include <qtextstream.h>
#include <kdebug.h>

#include "vs_model.h"

namespace VStudio {
  //===========================================================================
  // Visual Studio entity methods
  //===========================================================================
  VSEntity::VSEntity(e_VSEntityType typ, const QString &nm)
  : name(nm), type(typ) {
    uuid = QUuid::createUuid();
  }

  VSEntity::VSEntity(e_VSEntityType typ, const QString &nm, const QUuid &uid)
  : name(nm) , type(typ) {
    uuid = uid;
  }

  VSEntity::~VSEntity() {
  }

  //===========================================================================
  // Visual studio solution methods
  //===========================================================================
  VSSolution::VSSolution(const QString &nm, const QString &path)
  : VSEntity(vs_solution, nm)
  , path_rlt(path) {
  }

  VSSolution::VSSolution(const QString &nm, const QUuid &uid, const QString &path)
  : VSEntity(vs_solution, nm, uid)
  , path_rlt(path) {
  }

  VSSolution::~VSSolution() {
#ifdef USE_BOOST
    for(proj_v_i it=projects.begin(); it!=projects.end(); ++it) {
      if((*it) != NULL) {
        delete (*it); (*it)=NULL;
      }
    }
#else
#endif
  }

  void VSSolution::insertProj(VSProject *prj) {
#ifdef USE_BOOST
    projects.push_back(prj);
#else
#endif
    prj->setParent(this);
  }

  //===========================================================================
  // Visual studio project methods
  //===========================================================================
  VSProject::VSProject(const QString &nm, const QString &path)
  : VSEntity(vs_project, nm)
  , path_rlt(path) {
  }

  VSProject::VSProject(const QString &nm, const QUuid &uid, const QString &path)
  : VSEntity(vs_project, nm, uid)
  , path_rlt(path) {
  }

  VSProject::~VSProject() {
    parent = 0;
  }

  void VSProject::setParent(VSSolution *pnt) {
    parent = pnt;
  }
};
