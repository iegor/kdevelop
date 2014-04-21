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

  void VSEntity::setName(const QString &nm) {
    name = nm;
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

  bool VSSolution::dumpProjectsLayout(QString &layout) {
    QString ln;
    //BEGIN // Save project layout data
    for(proj_v_ci it=projects.begin(); it!=projects.end(); ++it) {
      if((*it) == 0) {
        kddbg << "Error!!! Project data either missing or corrupted." << endl;
        return false;
      }
      VSProject &prj = (*(*it));
      /* Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "testing_stuf", "testing_stuf.vcproj", "{4B448DC1-8FF4-41AC-8734-A655187A84D7}" */
      ln.append("Project(\"").append(guid2String(uidGet()));
      ln.append("\") = \"").append(prj.getName()).append("\", \"").append(prj.getRelativePath()).append("\", \"");
      ln.append(guid2String(prj.uidGet())).append("\"").append("\n");
      //BEGIN // Save project section data
      //END // Save project section data
    }
    //END // Save project layout data
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
