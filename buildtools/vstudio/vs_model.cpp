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
#include <kaction.h>

#include "vs_model.h"
#include "vs_part.h"
#include "vs_explorer.h"

namespace VStudio {
  //BEGIN // Basic entity types

  //===========================================================================
  // Visual Studio entity methods
  //===========================================================================
  VSPart *VSEntity::sys_part = 0;
  VSEntity::VSEntity(e_VSEntityType typ, const QString &nm)
  : name(nm)
  , type(typ)
  , refs(0) {
    uuid = QUuid::createUuid();
  }

  VSEntity::VSEntity(e_VSEntityType typ, const QString &nm, const QUuid &uid)
  : name(nm)
  , type(typ)
  , refs(0) {
    uuid = uid;
  }

  VSEntity::~VSEntity() {
    if(refs > 0) kddbg << "Error! deleting referenced entity!\n";
    kddbg << "Freeing " << type2String(getType()) << ": " << getName() << endl;
  }

  bool VSEntity::release() {
    if((--refs) == 0) return true;
    return false;
  }

  void VSEntity::acquire() {
    ++refs;
#ifdef DEBUG
    kddbg << type2String(getType()) << " \"" << getName() << "\" refcount = "
        << refs << endl;
#endif
  }

  void VSEntity::setPart(VSPart *part) {
    if(part==0) {
      kddbg << "Warning!!! Setting sys part to 0\n";
    } else  {
    //if(sys_part==0) {
      kddbg << "Setting sys part\n";
    }
      sys_part = part;
    //}
  }

  void VSEntity::setName(const QString &nm) {
    name = nm;
  }

  void VSEntity::setParent(vse_p pnt) {
    kddbg << type2String(getType()) << " \"" << getName() << "\" is parented by "
        << type2String(pnt->getType()) << " \"" << pnt->getName() << "\"\n";
    acquire();
  }

  void VSEntity::insert(vse_p pnt) {
    kddbg << type2String(getType()) << "'s ::insert method is not implemented. "
        << "when inserting \"" << pnt->getName() << "\" "
        << type2String(pnt->getType()) << endl;
  }

  bool VSEntity::createUI(uivse_p /*pnt*/) { return true; }
  //===========================================================================
  // Visual studio solution methods
  //===========================================================================
  VSSolution::VSSolution(const QString &nm, const QString &path)
  : VSEntity(vs_solution, nm)
  , path_rlt(path)
  , uisln(0) {
  }

  VSSolution::VSSolution(const QString &nm, const QUuid &uid, const QString &path)
  : VSEntity(vs_solution, nm, uid)
  , path_rlt(path)
  , uisln(0) {
  }

  VSSolution::~VSSolution() {
    // Delete UI representation
    if(uisln!=0) { delete uisln; uisln=0; }
    // Delete all filters
#ifdef USE_BOOST
    for(vf_ci it=filters.begin(); it!=filters.end(); ++it) {
#else
      //TODO: Implement this
#endif
      if((*it) != 0) {
        if((*it)->release()) { delete (*it); /*(*it)=0;*/ }
      } else {
        kddbg << "Error! Solution's filters list is corrupted\n";
      }
    }

    // Delete all projects
#ifdef USE_BOOST
    for(vp_ci it=projects.begin(); it!=projects.end(); ++it) {
#else
      //TODO: Implement this
#endif
      if((*it) != 0) {
        if((*it)->release()) { delete (*it); /*(*it)=0;*/ }
      } else {
        kddbg << "Error! Solution's projects list is corrupted\n";
      }
    }

    // Delete all meta-dependencies
#ifdef USE_BOOST
    for(vsmd_ci mdci=mdeps.begin(); mdci!=mdeps.end(); ++mdci) {
#else
#error "VStudio: Boost support is no enabled"
    //TODO: Implement later
#endif
      if((*mdci) != 0) {
         delete (*mdci);
         // (*mdci) = 0;
      } else {
        kddbg << "Error! Solution's meta-dependencies tree is corrupted.\n";
      }
    }
  }

  void VSSolution::insert(vse_p item) {
    switch(item->getType()) {
      case vs_project:
#ifdef USE_BOOST
        projects.push_back(static_cast<vsp_p>(item));
#else
        //TODO: Implement this
#endif
        break;
      case vs_filter:
#ifdef USE_BOOST
        filters.push_back(static_cast<vsf_p>(item));
#else
        //TODO: Implement this
#endif
        break;
      default:
        kddbg << "Error! Can't insert unknown node of type \"" <<
            type2String(item->getType()) << "\" into solution\n";
       return;
    }
    item->setParent(this); //NOTE: acquires item
  }

  void VSSolution::setParent(vse_p pnt) {
    VSEntity::setParent(pnt);
  }

  bool VSSolution::setRelativePath(const QString &p) {
    if(p.isEmpty()) {
      kddbg << "Can't set empty path" << endl;
      return false;
    }
    path_rlt = p;
    return true;
  }

  vse_p VSSolution::getByUID(const QUuid &uid) const {
#ifdef USE_BOOST
    for(vp_ci it=projects.begin(); it!=projects.end(); ++it) {
#else
      //TODO: Implement this
#endif
      if((*it)!=0) {
        if((*it)->uidGet() == uid) { return (*it); }
      } else {
        kddbg << "Error!!! corrupted projects list in solution: \"" << name << "\"\n";
        return 0;
      }
    }
    return 0;
  }

  bool VSSolution::createUI() {
    if(uisln==0) {
      uisln = part()->explorerWidget()->addSolutionNode(this);
      if(uisln==0) { kddbg << "failed to add sln UI node" << endl; return false; }
      kddbg << "<<< Sln: " << name << endl;
    }
    return true;
  }

  bool VSSolution::dumpLayout(QTextOStream &s) {
    // Dump version data
    s << "Microsoft Visual Studio Solution File, Format Version 10.00\n";
    s << "# Visual Studio 2008\n";

    // Save project layout data
    for(vp_i it=projects.begin(); it!=projects.end(); ++it) {
      if((*it) == 0) {
        kddbg << "Error!!! corrupted projects list in solution: \"" << name << "\"\n";
        return false;
      }
      (*it)->dumpLayout(s);
    }
    //BEGIN // Save solution sections data
    //END //Save solution sections data
    return true;
  }

  vsf_p VSSolution::getFltByUID(const QUuid &uid) const {
#ifdef USE_BOOST
    for(vf_ci it=filters.begin(); it!=filters.end(); ++it) {
#else
      //TODO: Implement this
#endif
      if((*it)!=0) {
        if((*it)->uidGet() == uid) { return (*it); }
      } else {
        kddbg << "Error!!! corrupted filters list in solution: \"" << name << "\"\n";
        return 0;
      }
    }
    return 0;
  }

  void VSSolution::forEachProj(entityFunctor fctr) {
#ifdef USE_BOOST
    for(vp_ci it=projects.begin(); it!=projects.end(); ++it) {
#else
      //TODO: Implement this
#endif
      if(!fctr(*it)) {
        kddbg << "forEachProject: project \"" << (*it)->getName() << "\" failed in functor\n";
        return;
      }
    }
  }

  void VSSolution::forEachFilter(entityFunctor fctr) {
#ifdef USE_BOOST
    for(vf_ci it=filters.begin(); it!=filters.end(); ++it) {
#else
      //TODO: Implement this
#endif
      if(!fctr(*it)) {
        kddbg << "forEachFilter: filter \"" << (*it)->getName() << "\" failed in functor\n";
        return;
      }
    }
  }

  bool VSSolution::populateUI() {
    // Solution item
    if(!createUI()) {
      kddbg << "Error! Failed to create UI representation for sln \""
          << name << "\"\n";
      return false;
    }
    // Insert leve filters, if any
#ifdef USE_BOOST
    for(vf_ci it=filters.begin(); it!=filters.end(); ++it) {
#else
      //TODO: Implement this
#endif
      if(!(*it)->createUI()) return false;
      if(!(*it)->populateUI()) return false;
    }
#ifdef USE_BOOST
    for(vp_ci it=projects.begin(); it!=projects.end(); ++it) {
#else
#endif
      if(!(*it)->createUI(uisln)) return false;
      if(!(*it)->populateUI()) return false;
    }
    return true;
  }

  VSSolution::vsmd_p VSSolution::metaDependency(const QUuid &uuid) {
    vsmd_p md = 0;
#ifdef USE_BOOST
    for(vsmd_i mdci=mdeps.begin(); mdci!=mdeps.end(); ++mdci) {
#else
#error "VStudio: Boost support is no enabled"
    //TODO: Implement later
#endif
      if((*mdci)->uid == uuid) {
        md = (*mdci);
        break;
      }
    }

    if(md == 0) {
      md = new VSMetaDependency(uuid);
      if(md != 0) {
        mdeps.push_back(md);
      } else {
        kddbg << "Error! can't allocate meta-dependency\n";
      }
    }

    return md;
  }

  bool VSSolution::updateDependencies() {
#ifdef USE_BOOST
    for(vsmd_ci mdci=mdeps.begin(); mdci!=mdeps.end(); ++mdci) {
#else
#error "VStudio: Boost support is no enabled"
    //TODO: Implement later
#endif
      vsp_p p=(vsp_p)getByUID((*mdci)->uid);
      if(p != 0) {
        (*mdci)->syncToPrj(p);
      } else {
        kddbg << "Error! Can't find project by uid.\n";
        return false;
      }
    }
    return true;
  }

  void VSSolution::VSMetaDependency::syncToPrj(vsp_p prj) {
#ifdef USE_BOOST
    for(boost::container::vector<QUuid>::const_iterator uidc=deps.begin(); uidc!=deps.end(); ++uidc) {
#else
#error "VStudio: Boost support is no enabled"
    //TODO: Implement later
#endif
      prj->addRequirement((*uidc));
    }
  }

  void VSSolution::VSMetaDependency::syncFromPrj(vsp_p prj) {
#ifdef USE_BOOST
    deps.clear();
#else
#error "VStudio: Boost support is no enabled"
    //TODO: Implement later
#endif
    //TODO: Not sure if this method is needed at all
    // For not I'll leave it just cleaning the meta-deps tree
  }

  //===========================================================================
  // Visual studio project methods
  //===========================================================================
  VSProject::VSProject(e_VSPrjLangType ltype, const QString &nm, const QString &path)
  : VSEntity(vs_project, nm)
  , lang(ltype)
  , path_rlt(path)
  , sln(0)
  , uiprj(0) {
  }

  VSProject::VSProject(e_VSPrjLangType ltype, const QString &nm, const QUuid &uid, const QString &path)
  : VSEntity(vs_project, nm, uid)
  , lang(ltype)
  , path_rlt(path)
  , sln(0)
  , uiprj(0) {
  }

  VSProject::~VSProject() {
    // Delete UI representation
    if(uiprj!=0) { delete uiprj; uiprj=0; }
    // Delete all filters
#ifdef USE_BOOST
    for(vf_ci it=filters.begin(); it!=filters.end(); ++it) {
#else
      //TODO: Implement this
#endif
      if((*it)!=0) {
        if((*it)->release()) { delete (*it); /*(*it)=0;*/ }
      } else {
        kddbg << "Error! Project's filters list corrupted.\n";
        //TODO: In future, maybe throw and exception here
      }
    }

#ifdef USE_BOOST
    for(vfl_ci it=files.begin(); it!=files.end(); ++it) {
#else
      //TODO: Implement this
#endif
      if((*it)!=0) {
        if((*it)->release()) { delete (*it); /*(*it)=0;*/ }
      } else {
        kddbg << "Error! Project's files list is corrupted.\n";
      }
    }
    sln = 0;
  }

  void VSProject::insert(vse_p item) {
    switch(item->getType()) {
      case vs_filter:
#ifdef USE_BOOST
        filters.push_back((vsf_p)item);
#else
        //TODO: Implement this
#endif
        break;
      case vs_file:
#ifdef USE_BOOST
        files.push_back((vsfl_p)item);
#else
        //TODO: Implement this
#endif
        break;
      default:
        kddbg << "Error! Can't insert unknown node of type \"" <<
            type2String(item->getType()) << "\" into project\n";
        return;
    }
    item->setParent(this); //NOTE: acquires item
  }

  void VSProject::setParent(vse_p pnt) {
    if(sln==0) {
      sln = static_cast<vss_p>(pnt);
#ifdef USE_BOOST
      pnts.push_back(sln);
#else
      //TODO: Implement this
#endif
      VSEntity::setParent(pnt); //NOTE: increases refcount
    }
  }

  bool VSProject::setRelativePath(const QString &p) {
    if(p.isEmpty()) {
      kddbg << "Can't set empty path" << endl;
      return false;
    }
    path_rlt = p;
    return true;
  }

  vse_p VSProject::getByUID(const QUuid &uid) const {
#ifdef USE_BOOST
    for(vfl_ci it=files.begin(); it!=files.end(); ++it) {
#else
      //TODO: Implement this
#endif
      if((*it)!=0) {
        if((*it)->uidGet() == uid) { return (*it); }
      } else {
        kddbg << "Error!!! corrupted files list in project: \"" << name << "\"\n";
        return 0;
      }
    }
    return 0;
  }

  bool VSProject::createUI(uivse_p uipnt) {
    if(uiprj==0) {
      uiprj = part()->explorerWidget()->addProjectNode(uipnt, this);
      if(uiprj==0) { kddbg << "failed to add prj UI node" << endl; return false; }
      kddbg << "Prj: " << name << " in " << uipnt->getName() << endl;
    }
    return true;
  }

  bool VSProject::dumpLayout(QTextOStream &s) {
    // Write project header
    s << "Project(\"";
    switch(lang) {
      case vs_prjlang_c: {
        s << guid2String(uid_vs9project_c);
        break; }
      case vs_prjlang_cs: {
        s << guid2String(uid_vs9project_cs);
      }
      default:
        kddbg << "Error! project language \"" << prjLangType2String(lang)
            << "\" is not handled\n";
        return false;
    }
    s << "\" = \"" << getName() << "\", \"" << getRelativePath() << "\", \"";
    s << guid2String(uidGet()) << "\"\n";

    // Write project requirements
#ifdef USE_BOOST
    if(!reqs.empty()) {
      s << "\tProjectSection(" << VSPART_PRJSECTION_DEPENDENCIES << ") = postProject\n";
      for(vp_ci it=reqs.begin(); it!=reqs.end(); ++it) {
#else
        //TODO: Implement this
#endif
        s << "\t\t" << guid2String((*it)->uidGet()) << " = " << guid2String((*it)->uidGet()) << endl;
      }
      s << "\tEndProjectSection\n";
    }
    s << "EndProject\n";
    return true;
  }

  vsp_p VSProject::getReqByUID(const QUuid &uid) const {
#ifdef USE_BOOST
    for(vp_ci it=reqs.begin(); it!=reqs.end(); ++it) {
#else
      //TODO: Implement this
#endif
      if((*it)!=0) {
        if((*it)->uidGet() == uid) { return (*it); }
      } else {
        kddbg << "Error!!! corrupted dependencies list in project: \"" << name << "\"\n";
        return 0;
      }
    }
    return 0;
  }

  vsp_p VSProject::getDepByUID(const QUuid &uid) const {
#ifdef USE_BOOST
    for(vp_ci it=deps.begin(); it!=deps.end(); ++it) {
#else
      //TODO: Implement this
#endif
      if((*it)!=0) {
        if((*it)->uidGet() == uid) { return (*it); }
      } else {
        kddbg << "Error!!! corrupted requirements list in project: \"" << name << "\"\n";
        return 0;
      }
    }
    return 0;
  }

  vsf_p VSProject::getFltByUID(const QUuid &uid) const {
#ifdef USE_BOOST
    for(vf_ci it=filters.begin(); it!=filters.end(); ++it) {
#else
      //TODO: Implement this
#endif
      if((*it)!=0) {
        if((*it)->uidGet() == uid) { return (*it); }
      } else {
        kddbg << "Error!!! corrupted filters list in solution: \"" << name << "\"\n";
        return 0;
      }
    }
    return 0;
  }

  bool VSProject::addDependency(vsp_p dp) {
    // Scan for circularity
#ifdef USE_BOOST
    for(vp_ci it=reqs.begin(); it!=reqs.end(); ++it) {
#else
      //TODO: Implement this
#endif
      if((*it)->uidGet() == dp->uidGet()) {
        kddbg << "Error! Circularity in requirement chain found. "
            << "|" << (*it)->getName() << "|" << getName() << "|"
            << dp->getName() << "|\n";
        return false;
      }
    }

    // Scan for duplicates
#ifdef USE_BOOST
    for(vp_ci it=deps.begin(); it!=deps.end(); ++it) {
#else
      //TODO: Implement this
#endif
      if((*it)->uidGet() == dp->uidGet()) {
        kddbg << "Warning! Can't add \"" << dp->getName() << "\" as dependency to \""
            << getName() << "\". Duplicated dependencies aren't allowed.\n";
        return false;
      }
    }

#ifdef USE_BOOST
    deps.push_back(dp);
#else
    //TODO: Implement this
#endif
    return true;
  }

  bool VSProject::addDependency(const QUuid &uid) {
    vsp_p dp_p = (vsp_p)sln->getByUID(uid);
    if(dp_p != 0) {
      return addDependency(dp_p);
    } else {
      kddbg << "Error! can't find project by uid.\n";
      return false;
    }
  }

  bool VSProject::addRequirement(vsp_p rq) {
    //Scan for circularity
#ifdef USE_BOOST
    for(vp_ci it=deps.begin(); it!=deps.end(); ++it) {
#else
      //TODO: Implement this
#endif
      if((*it)->uidGet() == rq->uidGet()) {
        kddbg << "Error! Circularity in dependency chain found. "
            << "|" << rq->getName() << "|" << getName() << "|"
            << (*it)->getName() << "|\n";
        return false;
      }
    }

    // Scan for duplicates
#ifdef USE_BOOST
    for(vp_ci it=reqs.begin(); it!=reqs.end(); ++it) {
#else
      //TODO: Implement this
#endif
      if((*it)->uidGet() == rq->uidGet()) {
        kddbg << "Warning! Can't add \"" << rq->getName() << "\" as requirement to \""
            << getName() << "\". Duplicated requirements aren't allowed.\n";
        return false;
      }
    }

#ifdef USE_BOOST
    reqs.push_back(rq);
#else
    //TODO: Implement this
#endif
    return true;
  }

  bool VSProject::addRequirement(const QUuid &uid) {
    vsp_p rq_p = (vsp_p)sln->getByUID(uid);
    if(rq_p != 0) {
      return addRequirement(rq_p);
    } else {
      kddbg << "Error! Can't find project by uid. \n";
      return false;
    }
  }

  bool VSProject::populateUI() {
#ifdef USE_BOOST
    if(!filters.empty()) {
      for(vf_ci it=filters.begin(); it!=filters.end(); ++it) {
#else
#endif
        if(!(*it)->createUI()) return false;
        if(!(*it)->populateUI()) return false;
      }
    } else {
#ifdef USE_BOOST
      for(vfl_ci it=files.begin(); it!=files.end(); ++it) {
#else
#endif
        if(!(*it)->createUI((uivse_p)uiprj)) return false;
      }
    }
    return true;
  }

  void VSProject::setLanguage(e_VSPrjLangType lng) {
    if(lng==vs_prjlang_unknown) {
      kddbg << "Warning!!! Tried to set unknown project language in \""
          << name << "\" project \n";
      return;
    }
    lang = lng;
  }

  //===========================================================================
  // Visual studio filter methods
  //===========================================================================
  VSFilter::VSFilter(const QString &nm)
  : VSEntity(vs_filter, nm)
  , parent(0)
  , uiflt(0) {
  }

  VSFilter::VSFilter(const QString &nm, const QUuid &uid)
  : VSEntity(vs_filter, nm, uid)
  , parent(0)
  , uiflt(0) {
  }

  VSFilter::~VSFilter() {
    if(uiflt!=0) { delete uiflt; uiflt=0; }
  }

  void VSFilter::insert(vse_p item) {
    switch(item->getType()) {
      case vs_file:
      case vs_project:
#ifdef USE_BOOST
        chld.push_back(item);
#else
        //TODO: Implement this
#endif
        //NOTE: We are not setting this filter as parent of
        // projects and files. Those already have a proper parent,
        // solution or project.
        break;
      case vs_filter:
#ifdef USE_BOOST
        chld.push_back(item);
#else
        //TODO: Implement this
#endif
        item->setParent(this);
        break;
      default:
        kddbg << "Error! Can't insert unknown node of type \"" <<
            type2String(item->getType()) << "\" into filter\n";
        return;
    }
  }

  void VSFilter::setParent(vse_p pnt) {
    switch(pnt->getType()) {
      case vs_solution:
      case vs_project:
      case vs_filter:
        parent = pnt;
        break;
      default:
        kddbg << "Error! Can't set \"" << type2String(pnt->getType())
            << "\" as a parent of \"" << type2String(getType()) << "\"\n";
        return;
    }
    VSEntity::setParent(pnt); //NOTE: increases refcount
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
#else
      //TODO: Implement this
#endif
      if((*it)!=0) {
        if((*it)->uidGet() == uid) { return (*it); }
      } else {
        kddbg << "Error!!! corrupted children list in filter: \"" << name << "\"\n";
        return 0;
      }
    }
    return 0;
  }

  bool VSFilter::createUI() {
    if(uiflt==0) {
      uiflt = part()->explorerWidget()->addFilterNode(parent->getUI(), this);
      if(uiflt==0) { kddbg << "failed to add filter UI node" << endl; return false; }
      kddbg << "Flt: " << name << " in " << parent->getName() << endl;
    }
    return true;
  }

  bool VSFilter::populateUI() {
#ifdef USE_BOOST
    for(ve_ci it=chld.begin(); it!=chld.end(); ++it) {
#else
      //TODO: Implement this
#endif
      switch((*it)->getType()) {
        case vs_project: {
          vsp_p prj=(vsp_p)(*it);
          if(!prj->createUI((uivse_p)uiflt)) return false;
          if(!prj->populateUI()) return false;
          break; }
        case vs_file: {
          vsfl_p fl=(vsfl_p)(*it);
          if(!fl->createUI((uivse_p)uiflt)) return false;
          break; }
        default:
          break;
      }
    }
    return true;
  }

  //===========================================================================
  // Visual studio file methods
  //===========================================================================
  VSFile::VSFile(const QString &nm, vsp_p pnt)
  : VSEntity(vs_file, nm)
  , uifl(0) {
    setParent(pnt);
  }

  VSFile::VSFile(const QString &nm, const QUuid &uid, vsp_p pnt)
  : VSEntity(vs_file, nm, uid)
  , uifl(0) {
    setParent(pnt);
  }

  VSFile::~VSFile() {
    if(uifl!=0) { delete uifl; uifl=0; }
  }

  void VSFile::setParent(vsp_p pnt) {
    switch(pnt->getType()) {
      case vs_project: {
        parent = pnt;
        break; }
      default:
        kddbg << "Error! Can't set \"" << type2String(pnt->getType())
            << "\" as a parent of \"" << type2String(getType()) << "\"\n";
        return;
    }
#ifdef USE_BOOST
    pnts.push_back(parent);
#else
    //TODO: Implement this
#endif
    VSEntity::setParent(pnt); //NOTE: increases refcount
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
#else
      //TODO: Implement this
#endif
      if((*it) == 0) {
        kddbg << "Error!!! corrupted parents list for file: \"" << name << "\"\n";
        return 0;
      }
      if((*it)->uidGet() == uid) {
        return (*it);
      }
    }
    return 0;
  }

  bool VSFile::createUI(uivse_p pnt) {
    if(uifl==0) {
      uifl = part()->explorerWidget()->addFileNode(pnt, this);
      if(uifl==0) { kddbg << "failed to add file UI node" << endl; return false; }
      kddbg << "File: " << name << " in " << pnt->getName() << endl;
    }
    return true;
  }
  //END // Basic entity types

  //BEGIN // Inherited entity types

  //===========================================================================
  // Visual studio file methods
  //===========================================================================
  VSProject_c::VSProject_c(const QString &nm, const QString &path)
  : VSProject(vs_prjlang_c, nm, path) {
  }

  VSProject_c::VSProject_c(const QString &nm, const QUuid &uid, const QString &path)
  : VSProject(vs_prjlang_c, nm, uid, path) {
  }

  VSProject_c::~VSProject_c() {
  }
  //END // Inherited entity types

  //BEGIN VS build entities
  //===========================================================================
  // Visual studio build tool methods
  //===========================================================================
  VSTool::VSTool(e_VSBuildTool t)
  : VSEntity(vs_tool, tool2String(t))
  , vstl(t) {
  }

  VSTool::~VSTool() {
  }

  void VSTool::setName(const QString &/*n*/) {
#ifdef DEBUG
    kddbg << "Error! It is impossible to change the name of a build tool.\n"
        << "\tPlease remove call to this method from your code.\n";
#endif
  }

  bool VSTool::setRelativePath(const QString &/*s*/) {
#ifdef DEBUG
    kddbg << "Error! It is not possible to change a path to build tool.\n"
        << "\tPlease remove call to this method from your code.\n";
#endif
    return false;
  }

  vse_p VSTool::getByUID(const QUuid &/*uid*/) const {
#ifdef DEBUG
    kddbg << "Error! There is nothing that can be returned from buil tool by uid.\n"
        << "\tPlease remove call to this method from your code.\n";
#endif
    return 0;
  }

  //===========================================================================
  // Visual studio "MSVC compiler" build tool methods
  //===========================================================================
  VSToolCompilerMSVC::VSToolCompilerMSVC()
  : VSTool(vstl_compiler) {
  }

  VSToolCompilerMSVC::~VSToolCompilerMSVC() {
  }

  QString VSToolCompilerMSVC::getRelativePath() const {
    return "VStudio/MSVC/CompilerC";
  }

  uivse_p VSToolCompilerMSVC::getUI() const {
    //TODO: Implement UI for MSVC compiler build tool
    return 0;
  }

  //===========================================================================
  // Visual studio "MSVC linker" build tool methods
  //===========================================================================
  VSToolLinkerMSVC::VSToolLinkerMSVC()
  : VSTool(vstl_linker) {
  }

  VSToolLinkerMSVC::~VSToolLinkerMSVC() {
  }

  QString VSToolLinkerMSVC::getRelativePath() const {
    return "VStudio/MSVC/LinkerC";
  }

  uivse_p VSToolLinkerMSVC::getUI() const {
    //TODO: Implement UI for MSVC linker build tool
    return 0;
  }

  //===========================================================================
  // Visual studio "MSVC linker" build tool methods
  //===========================================================================
  VSToolCompilerMSMidl::VSToolCompilerMSMidl()
  : VSTool(vstl_midl) {
  }

  VSToolCompilerMSMidl::~VSToolCompilerMSMidl() {
  }

  QString VSToolCompilerMSMidl::getRelativePath() const {
    return "VStudio/MSVC/MidlC";
  }

  uivse_p VSToolCompilerMSMidl::getUI() const {
    //TODO: Implement UI for MS midl compiler build tool
    return 0;
  }

  //===========================================================================
  // VS WIN32 Platform methods
  //===========================================================================
  class VSPlWin32 : public VSPlatform {
    public:
      VSPlWin32()
      : VSPlatform(vspl_win32) {
      }
      virtual ~VSPlWin32() {
      }
  };
  static const VSPlWin32 g_Win32Platform;

  //===========================================================================
  // VS WIN64 Platform methods
  //===========================================================================
  class VSPlWin64 : public VSPlatform {
    public:
      VSPlWin64()
      : VSPlatform(vspl_win64) {
      }
      virtual ~VSPlWin64() {
      }
  };
  static const VSPlWin64 g_Win64Platform;

  //===========================================================================
  // VS Platform methods
  //===========================================================================
  VSPlatform::VSPlatform(e_VSPlatform p)
  : vspl(p) {
  }

  VSPlatform::~VSPlatform() {
  }

  const VSPlatform* VSPlatform::getVSPlatform(e_VSPlatform p) {
    switch(p) {
      case vspl_win32: return static_cast<const VSPlatform*>(&g_Win32Platform);
      case vspl_win64: return static_cast<const VSPlatform*>(&g_Win64Platform);
      default: {
#ifdef DEBUG
        kddbg << "Error!!! Unsupported platform \"" << platform2String(p)
            << "\" is requested, aborting\n";
#endif
        return 0;
      }
    }
  }

  //===========================================================================
  // VS Configuration methods
  //===========================================================================
  VSConfig::VSConfig(const QString &n, e_VSPlatform p)
  : VSEntity(vs_config, n)
  , vspl(*VSPlatform::getVSPlatform(p)) {
  }

  VSConfig::~VSConfig() {
  }

  QString VSConfig::getRelativePath() const {
    return "VStudio/Configuration";
  }

  bool VSConfig::setRelativePath(const QString &/*s*/) {
#ifdef DEBUG
    kddbg << "Error! It is impossible to set the relative path for configuration.\n"
        << "\tPlease remove call to this method from your code.\n";
#endif
    return false;
  }

  vse_p VSConfig::getByUID(const QUuid &/*uid*/) const {
#ifdef DEBUG
    kddbg << "Error! It is impossible to get anything by UID from configuration.\n"
        << "\tPlease remove call to this method from your code.\n";
#endif
    return 0;
  }

  uivse_p VSConfig::getUI() const {
    //TODO: Implement UI for VS configuration
    return 0;
  }
  //END VS build entities
};
