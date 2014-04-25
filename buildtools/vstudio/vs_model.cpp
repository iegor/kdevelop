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

  bool VSSolution::setRelativePath(const QString &p) {
    if(p.isEmpty()) {
      kddbg << "Can't set empty path" << endl;
      return false;
    }
    path_rlt = p;
    return true;
  }

  vsp_p VSSolution::getByUID(const QUuid &uid) const {
#ifdef USE_BOOST
    for(vp_ci it=projects.begin(); it!=projects.end(); ++it) {
      if((*it) == 0) {
        kddbg << "Error!!! corrupted projects list in solution: \"" << name << "\"\n";
        return 0;
      }
      if((*it)->uidGet() == uid) {
        return (*it);
      }
    }
#else
#endif
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

  vsf_p VSSolution::getFltByUID(const QUuid &uid) const {
#ifdef USE_BOOST
    for(vf_ci it=filters.begin(); it!=filters.end(); ++it) {
      if((*it) == 0) {
        kddbg << "Error!!! corrupted filters list in solution: \"" << name << "\"\n";
        return 0;
      }
      if((*it)->uidGet() == uid) {
        return (*it);
      }
    }
#else
#endif
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

  bool VSProject::setRelativePath(const QString &p) {
    if(p.isEmpty()) {
      kddbg << "Can't set empty path" << endl;
      return false;
    }
    path_rlt = p;
    return true;
  }

  vsp_p VSProject::getByUID(const QUuid &uid) const {
#ifdef USE_BOOST
    for(vp_ci it=deps.begin(); it!=deps.end(); ++it) {
      if((*it) == 0) {
        kddbg << "Error!!! corrupted depencies list in project: \"" << name << "\"\n";
        return 0;
      }
      if((*it)->uidGet() == uid) {
        return (*it);
      }
    }
#else
#endif
    return 0;
  }

  vsp_p VSProject::getReqByUid(const QUuid &uid) const {
#ifdef USE_BOOST
    for(vp_ci it=reqs.begin(); it!=reqs.end(); ++it) {
      if((*it) == 0) {
        kddbg << "Error!!! corrupted requirements list in project: \"" << name << "\"\n";
        return 0;
      }
      if((*it)->uidGet() == uid) {
        return (*it);
      }
    }
#else
#endif
  }

  /*
  //===========================================================================
  // Visual studio filter methods
  //===========================================================================
  VSFilter::VSFilter(const QString &name, vse_p pnt)
  : VSEntity(vs_filter, nm)
  , parent(pnt) {
  }

  VSFilter::~VSFilter() {
  }

  QString VSFilter::getRelativePath() const {
    return ""; //TODO: make it return path relative to entity it's in
  }

  bool VSFilter::setRelativePath(const QString &pth) {
    //TODO: make it change parents, based upon type of it's parent (maybe, don't quite know yet)
    // so that it became not necessary to store relative path in instance and this "property"
    // became procedural
    return false;
  }

  vse_p VSFilter::getByUID(const QUuid &uid) const {
#ifdef USE_BOOST
    for(ve_ci it=chld.begin(); it!=chld.end(); ++it) {
      if((*it) == 0) {
        kddbg << "Error!!! corrupted children list in filter: \"" << name << "\"\n";
        return 0;
      }
      if((*it)->uidGet() == uid) {
        return (*it);
      }
    }
#else
#endif
  }
  */

  //===========================================================================
  // Visual studio file methods
  //===========================================================================
  VSFile::VSFile(const QString &nm, vsp_p pnt)
  : VSEntity(vs_file, nm)
  , parent(pnt) {
#ifdef USE_BOOST
    pnts.push_back(parent);
#else
#endif
  }

  VSFile::~VSFile() {
#ifdef USE_BOOST
#else
#endif
  }

  QString VSFile::getRelativePath() const {
    //TODO: make it return a path relative to project it's in
    return "";
  }

  bool VSFile::setRelativePath(const QString &path) {
    //TODO: make it change parent according to relative path from input
    //so that it moved between parent projects (copied or moved)
    //and that also makes storing path not necessary, because nature of
    //it will be procedural
    return false;
  }

  vsp_p VSFile::getByUID(const QUuid &uid) const {
#ifdef USE_BOOST
    for(vp_ci it=pnts.begin(); it!=pnts.end(); ++it) {
      if((*it) == 0) {
        kddbg << "Error!!! corrupted parents list for file: \"" << name << "\"\n";
        return 0;
      }
      if((*it)->uidGet() == uid) {
        return (*it);
      }
    }
#else
#endif
  }
};
