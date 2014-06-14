/*
*kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;
* NOTE: 120 symbols is the limit for text line. it marked with '#' in this line                                        #
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
  static const VSPlWin32 g_Win32Platform;
  static const VSPlWin64 g_Win64Platform;

  //BEGIN // Basic entity types

  //===========================================================================
  // Visual Studio refcountable methods
  //===========================================================================
  VSRefcountable::VSRefcountable()
  : rfc(0) {
  }

  VSRefcountable::~VSRefcountable() {
    if(rfc > 0) kddbg << VSPART_ERR_REFCOUNT_NONZERODELETE;
  }

  bool VSRefcountable::acquire(const vse_p p) {
    if(p != 0) {
#ifdef DEBUG
      if(static_cast<uint>(rfc+1) == 0) {
        kddbg << VSPART_ERR_REFCOUNT_WARPAROUND;
        return false;
      }
#endif
#ifdef USE_BOOST
      pnts.push_back(p);
#else
#error "VStudio: Boost support is no enabled" //TODO: Implement this
#endif
      ++rfc;
      return true;
    }
#ifdef DEBUG
    else { kddbg << "Can't acquire with parent = 0.\n"; }
    //kddbg << type2String(getType()) << " \"" << getName() << "\" refcount = "
    //    << refs << endl;
#endif
    return false;
  }

  vsr_r VSRefcountable::release(const vse_p p) {
    if(p != 0) {
#ifdef DEBUG
      if(static_cast<uint>(rfc-1) == UINT_MAX-1) {
        kddbg << VSPART_ERR_REFCOUNT_WARPAROUND;
        return *this;
      }
#endif
#ifdef USE_BOOST
      vse_ci it=pnts.begin();
      for(; it!=pnts.end(); ++it) {
#else
#error "VStudio: Boost support is no enabled" //TODO: Implement this
#endif
        if((*it) == p) { break; }
      }
#ifdef USE_BOOST
      if(it!=pnts.end()) {
#else
#error "VStudio: Boost support is no enabled" //TODO: Implement this
#endif
        pnts.erase(it);
      } else {
        kddbg << VSPART_ERROR"Can't release with false parent.\n";
        return *this;
      }
      --rfc;
      return *this;
    }
#ifdef DEBUG
    else { kddbg << "Can't release with parent = 0.\n"; }
#endif
    return *this;
  }

  /*inline*/ uint VSRefcountable::refs() const {
    return rfc;
  }

  /*inline*/ bool VSRefcountable::isfree() const {
    return static_cast<bool>(rfc==0);
  }

  /*inline*/ const pv_VSEntity& VSRefcountable::parents() const {
    return pnts;
  }

  //===========================================================================
  // Visual Studio nameable methods
  //===========================================================================
  VSNameable::VSNameable(const QString& n)
  : name(n) {
  }

  VSNameable::~VSNameable() {
  }

  /*inline*/ const QString& VSNameable::getName() const {
    return name;
  }

  /*inline*/ void VSNameable::setName(const QString& n) {
    name = n;
  }

  //===========================================================================
  // Visual Studio indexable methods
  //===========================================================================
  VSIndexable::VSIndexable(const QUuid& u)
  :uid(u) {
  }

  VSIndexable::~VSIndexable() {
  }

  /*inline*/ const QUuid& VSIndexable::getUID() const {
    return uid;
  }

  /*inline*/ void VSIndexable::setUID(const QUuid& u) {
    uid=u;
  }

  //===========================================================================
  // Visual Studio FS Stored methods
  //===========================================================================
  VSFSStored::VSFSStored()
  : path_rlt(QString::null)
  , path_abs(QString::null) {
  }

  VSFSStored::~VSFSStored() {
  }

  /*inline*/ const QString& VSFSStored::getRelPath() const {
    return path_rlt;
  }

  /*inline*/ void VSFSStored::setRelPath(const QString& rlp) {
    path_rlt = rlp;
  }

  /*inline*/ const QString& VSFSStored::getAbsPath() const {
    return path_abs;
  }

  /*inline*/ void VSFSStored::setAbsPath(const QString& abp) {
    path_abs = abp;
  }

  //===========================================================================
  // Visual Studio entity methods
  //===========================================================================
  VSPart *VSEntity::sys_part = 0;
  VSEntity::VSEntity(e_VSEntityType typ)
  : type(typ) {
  }

  VSEntity::~VSEntity() {
#ifdef DEBUG
    kddbg << "Freeing entity: " << type2String(type) << endl;
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

  void VSEntity::insert(vse_p /*pnt*/) {
    // kddbg << type2String(getType()) << "'s ::insert method is not implemented. "
    //     << "when inserting \"" << pnt->getName() << "\" "
    //     << type2String(pnt->getType()) << endl;
  }

  bool VSEntity::createUI(uivse_p /*pnt*/) { return true; }

  //===========================================================================
  // Visual studio solution methods
  //===========================================================================
  VSSolution::VSSolution(const QString &nm, const QString &path)
  : VSEntity(vs_solution)
  , VSNameable(nm)
  , VSFSStored()
  , uisln(0)
  , active_bb(0)
  , active_prj(0)
  , active(false) {
    setRelPath(path);
  }

  VSSolution::~VSSolution() {
    active_prj = 0;
    // Delete UI representation
    if(uisln != 0) { delete uisln; uisln=0; }

    // Delete all configurations
    // setConfiguration(QString::null);
#ifdef USE_BOOST
    // for(vcfg_ci it=cfgs.begin(); it!=cfgs.end(); ++it) {
#else
#error "VStudio: Boost support is no enabled" //TODO: Implement this
#endif
    // }

    // Delete all filters
#ifdef USE_BOOST
    for(vsf_ci it=filters.begin(); it!=filters.end(); ++it) {
#else
#error "VStudio: Boost support is no enabled" //TODO: Implement this
#endif
      if((*it) != 0) {
        delete (*it); /*(*it)=0;*/
      } else {
        kddbg << g_err_list_corrupted.arg(VSPART_FILTER).arg("VSSolution::~VSSolution");
      }
    }

    // Delete all projects
#ifdef USE_BOOST
    for(vsp_ci it=projects.begin(); it!=projects.end(); ++it) {
#else
#error "VStudio: Boost support is no enabled" //TODO: Implement this
#endif
      if((*it) != 0) {
        if((*it)->release(static_cast<vse_p>(this)).isfree()) { delete (*it); /*(*it)=0;*/ }
      } else {
        kddbg << g_err_list_corrupted.arg(VSPART_PROJECT).arg("VSSolution::~VSSolution");
      }
    }

    // Delete all meta-dependencies
#ifdef USE_BOOST
    for(vsmd_ci mdci=mdeps.begin(); mdci!=mdeps.end(); ++mdci) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement later
#endif
      if((*mdci) != 0) {
         delete (*mdci);
         // (*mdci) = 0;
      } else {
        kddbg << g_err_list_corrupted.arg("Meta-dependency").arg("VSSolution::~VSSolution");
      }
    }
  }

  void VSSolution::insert(vse_p item) {
    switch(item->getType()) {
      case vs_project: {
        vsp_p p = static_cast<vsp_p>(item);
#ifdef USE_BOOST
        projects.push_back(p);
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement later
#endif
        p->setParent(static_cast<vse_p>(this)); //NOTE: acquires project
        break; }
      case vs_filter: {
        vsf_p f = static_cast<vsf_p>(item);
#ifdef USE_BOOST
        filters.push_back(f);
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement later
#endif
        f->setParent(static_cast<vse_p>(this)); //NOTE: acquires project
        break; }
      default: {
        kddbg << g_wrn_unsupportedtyp.arg(type2String(item->getType())).arg("VSSolution::insert");
        return; }
    }
  }

  void VSSolution::setParent(vse_p /*pnt*/) {
        /* kddbg << type2String(getType()) << " \"" << getName() << "\" is parented by "
    << type2String(pnt->getType()) << " \"" << pnt->getName() << "\"\n";
        */
  }

  /*inline*/ vse_p VSSolution::getParent() const {
    return 0;
  }

  vse_p VSSolution::getByUID(const QUuid &uid) const {
#ifdef USE_BOOST
    vsp_ci it=projects.begin();
    for(; it!=projects.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement later
#endif
      if((*it) != 0) {
        if((*it)->getUID() == uid) { break; }
      } else {
        kddbg << g_err_list_corrupted.arg(VSPART_PROJECT).arg("VSSolution::getByUID");
        return 0;
      }
    }
#ifdef USE_BOOST
    if(it!=projects.end()) { return (*it); }
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement later
#endif
    else {
         kddbg << g_err_ent_notfound.arg(VSPART_PROJECT).arg(guid2String(uid)).arg(name);
      return 0;
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

  bool VSSolution::dumpLayout(QTextStream &s) {
    // Dump version data
    s << "Microsoft Visual Studio Solution File, Format Version 10.00\n";
    s << "# Visual Studio 2008\n";

    // Save projects layout
#ifdef USE_BOOST
    for(vsp_ci it=projects.begin(); it!=projects.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement later
#endif
      if((*it) == 0) {
         kddbg << g_err_list_corrupted.arg("projects").arg("VSSolution::dumpLayout");
        return false;
      }
      (*it)->dumpLayout(s);
    }

    // Save filters layout
#ifdef USE_BOOST
    for(vsf_ci it=filters.begin(); it!=filters.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement later
#endif
      if((*it) == 0) {
        kddbg << g_err_list_corrupted.arg("filters").arg("VSSolution::dumpLayout");
        return false;
      }
      (*it)->dumpLayout(s);
    }

    //BEGIN // Save solution sections data
    s << "Global" << endl;
    const QString section_header("\tGlobalSection(%1) = %2\n");
    const QString section_footer("\tEndGlobalSection\n");

    // Save solution's configurations
    s << section_header.arg(VSPART_SLNSECTION_SCFG_PLATFORMS).arg("preSolution");
#ifdef USE_BOOST
    for(vsbb_ci it=bboxes.begin(); it!=bboxes.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement later
#endif
      vsbb_p bb = static_cast<vsbb_p>(*it);
      if(bb != 0) {
        s << "\t\t" << bb->config().toString() << " = " << bb->config().toString() << endl;
      } else {
        kddbg << g_err_list_corrupted.arg(VSPART_BUILDBOX).arg("VSSolution::dumpLayout");
      }
    }
    s << section_footer;

    // Save solution's project configuration platforms
    s << section_header.arg(VSPART_SLNSECTION_PCFG_PLATFORMS).arg("postSolution");
    s << section_footer;

    // Save solution's properties
    s << section_header.arg(VSPART_SLNSECTION_SPROPS).arg("preSolution");
    s << "\t\tHideSolutionNode = FALSE\n"; //TODO: Work on solution's properties
    s << section_footer;

    // Save solution's nesting info
    s << section_header.arg(VSPART_SLNSECTION_NESTEDPRJ).arg("preSolution");
    s << section_footer;

    s << "EndGlobal" << endl;
    //END //Save solution sections data
    return true;
  }

  vsf_p VSSolution::getFltByUID(const QUuid &uid) const {
#ifdef USE_BOOST
    vsf_ci it=filters.begin();
    for(; it!=filters.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement later
#endif
      if((*it)!=0) {
        if((*it)->getUID() == uid) { break; }
      } else {
        kddbg << g_err_list_corrupted.arg(VSPART_FILTER).arg("VSSolution::getFltByUID");
        return 0;
      }
    }
#ifdef USE_BOOST
    if(it!=filters.end()) { return (*it); }
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement later
#endif
    else {
         kddbg << g_err_ent_notfound.arg(VSPART_FILTER).arg(guid2String(uid)).arg(name);
    }
    return 0;
  }

  void VSSolution::forEachProj(entityFunctor fctr) {
#ifdef USE_BOOST
    for(vsp_ci it=projects.begin(); it!=projects.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
      if(!fctr(*it)) {
        kddbg << "forEachProject: project \"" << (*it)->getName() << "\" failed in functor\n";
        return;
      }
    }
  }

  void VSSolution::forEachFilter(entityFunctor fctr) {
#ifdef USE_BOOST
    for(vsf_ci it=filters.begin(); it!=filters.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
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
    for(vsf_ci it=filters.begin(); it!=filters.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
      if(!(*it)->createUI()) return false;
      if(!(*it)->populateUI()) return false;
    }
#ifdef USE_BOOST
    for(vsp_ci it=projects.begin(); it!=projects.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
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
#error "VStudio: Boost support is no enabled" //TODO: Implement later
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
#error "VStudio: Boost support is no enabled" //TODO: Implement later
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

  bool VSSolution::createCfg(const vcfg_p p, const vcfgcr_r pc) {
    // Check if we already have a build box with this config as parent
    BOOSTVEC_FOR(vsbb_ci, it, bboxes) {
      vsbb_p pbb = static_cast<vsbb_p>(*it);
      if(pbb != 0) {
        if(pbb->config() == pc) {
          kddbg << VSPART_ERROR"Config [" << pbb->config().toString() << "] already exists in: " << name << "sln.\n";
          return false;
        }
        //NOTE: We can't have two configs with same parent either, this will lead to ambiguity.
        if(p != 0) {
          if(pbb->belongs(p)) {
            if(pbb->isEnabled()) {
              // Substitute this config with new one that will be created
              pbb->setEnabled(false);
              continue;
            }
          }
        }
      }
    }

    // Create new build box and make all necessary preparations
    vsbb_p bb = new VSBuildBox(pc.name, pc.platform);
    if(bb != 0) {
      bb->setParent(p);
      bb->setEnabled(true);

      // Create configs for nested projects if necessary
      if(pc.sync_subs) {
        BOOSTVEC_FOR(vsp_ci, it, projects) {
          vsp_p prj = static_cast<vsp_p>(*it);
          if(prj != 0) {
            pc.sync_subs = false;
            if(!prj->createCfg(&bb->config(), pc)) {
              kddbg << VSPART_ERROR"Can't create config: " << pc.string() << " for project: "
                    << prj->getName() << ".\n";
              delete bb;
              return false;
            }
          } else {
            kddbg << g_err_list_corrupted.arg(VSPART_PROJECT).arg("VSSolution::createCfg");
            delete bb;
            return false;
          }
        }
      }
      BOOSTVEC_PUSHBACK(bboxes, bb);
#ifdef DEBUG
      kddbg << "Config created: " << bb->config().toString() << " within: " << getName() << " sln.\n";
#endif
      return true;
    } else {
      kddbg << g_err_notenoughmem.arg(VSPART_BUILDBOX).arg("VSSolution::selectCfg");
      return false;
    }
    return false;
  }

  bool VSSolution::selectCfg(const vcfg_p p) {
    vsbb_p bb = 0;
    if(p != 0) {
      bb = getBB(p); // Get bb that is for this configuration

      if(bb != 0) {
        active_bb = bb;
#ifdef DEBUG
        kddbg << g_msg_configapply.arg(active_bb->config().toString()).arg("VSSolution::selectCfg");
#endif

        // Update active configurations for all projects
        BOOSTVEC_FOR(vsp_ci, it, projects) {
          vsp_p prj(*it);
          if(prj != 0) {
            if(!prj->selectCfg(&bb->config())) {
#ifdef DEBUG
              kddbg << VSPART_ERROR"Can't set config for project: \"" << prj->getName() << "\" in \""
                  << getName() << "\", in {VSSolution::setCfg}\n";
#endif
            }
          }
          else {
            kddbg << g_err_list_corrupted.arg(VSPART_PROJECT).arg("VSSolution::selectCfg");
          }
        }

        // Update UI in VSExplorer
        if(uisln != 0) {
          //NOTE: TODO: This is temporary, untill I figure out how to design and implement a descent
          //  GUI items for VSExplorer
          uisln->setText(0, QString(name).append(" [%1]").arg(active_bb->config().toString()));
        }
        return true;
      }
      else {
        kddbg << g_err_ent_notfound.arg(VSPART_BUILDBOX).arg(p->toString()).arg("VSSolution::selectCfg");
        return false;
      }
    }
    else { kddbg << g_err_nullptr.arg("VSSolution::selectCfg"); return false; }
    return false;
  }

  void VSSolution::setActive(bool a) {
    active = a;
  }

  /*inline*/ bool VSSolution::isActive() const {
    return active;
  }

  bool VSSolution::setActivePrj(vsp_p p) {
    if(p != 0) {
      if(active_prj != 0) {
        active_prj->setActive(false);
      }
      active_prj = p;
      active_prj->setActive();
    }
    return false;
  }

  bool VSSolution::setActivePrj(const QString &n) {
    if(n != QString::null) {
      vsp_p prj = getProject(n);
      if(prj != 0) {
        kddbg << "Setting active prj: " << prj->getName() << " cval: "
            << n << endl;
        return setActivePrj(prj);
      }
    }
    return false;
  }

  /*inline*/ vsp_p VSSolution::getActivePrj() const {
    return active_prj;
  }

  /*inline*/ vsp_p VSSolution::getProject(const QString &n) const {
    if(n != QString::null) {
#ifdef USE_BOOST
      vsp_ci it=projects.begin();
      for(; it!=projects.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement later
#endif
        if((*it) != 0) {
          if((*it)->getName() == n) { break; }
        } else {
          kddbg << g_err_list_corrupted.arg(VSPART_PROJECT).arg(name);
          return 0;
        }
      }

#ifdef USE_BOOST
      if(it!=projects.end()) { return (*it); }
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement later
#endif
      else {
        kddbg << g_err_ent_notfound.arg(VSPART_PROJECT).arg(n).arg(name);
        return 0;
      }
    }
    return 0;
  }

  vsbb_p VSSolution::getBB(const QString &c) const {
    if(c != QString::null) {
      BOOSTVEC_FOR(vsbb_ci, it, bboxes) {
        if((*it) != 0) {
          if((*it)->config().toString() == c) { return (*it); }
        }
      }
      kddbg << g_err_ent_notfound.arg(VSPART_BUILDBOX).arg(c).arg("VSSolution::getBB");
      return 0;
    }
#ifdef DEBUG
    kddbg << VSPART_ERROR"Wrong param in {VSSolution::getBB}.\n";
#endif
    return 0;
  }

  vsbb_p VSSolution::getBB(const vcfg_p parent_cfg) const {
#ifdef DEBUG
    BOOSTVEC_FOR(vsbb_ci, it, bboxes) {
      if((*it) != 0) {
        kddbg << "BBOX: {" << "pcfg: " << ( (*it)->parentConfig() ? (*it)->parentConfig()->toString() : "0" )
            << " cfg: " << (*it)->config().toString() << " }.\n";
      }
    }
#endif
    if(parent_cfg != 0) {
      BOOSTVEC_FOR(vsbb_ci, it, bboxes) {
        if((*it) != 0) {
          if((*it)->parentConfig() != 0) {
            if((*it)->belongs(parent_cfg)) { return (*it); }
          }
          else { kddbg << VSPART_ERROR"Parent cfg is 0 in: " << (*it)->config().toString() << endl; return 0; }
        }
        else {
          kddbg << g_err_list_corrupted.arg(VSPART_BUILDBOX).arg("VSSolution::getBB");
        }
      }
      kddbg << g_err_ent_notfound.arg(VSPART_BUILDBOX).arg(parent_cfg->toString()).arg("VSSolution::getBB");
      return 0;
    }
    kddbg << g_err_nullptr.arg("VSSolution::getBB");
    return 0;
  }

  void VSSolution::VSMetaDependency::syncToPrj(vsp_p prj) {
    BOOSTVEC_FOR(boost::container::vector<QUuid>::const_iterator, uidc, deps) {
      prj->addRequirement((*uidc));
    }
  }

  void VSSolution::VSMetaDependency::syncFromPrj(vsp_p prj) {
#ifdef USE_BOOST
    deps.clear();
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement later
#endif
    //TODO: Not sure if this method is needed at all
    // For now I'll leave it just cleaning the meta-deps tree
  }

  //===========================================================================
  // Visual studio project methods
  //===========================================================================
  VSProject::VSProject(e_VSPrjLangType ltype, const QString &nm, const QUuid &uid, const QString &path)
  : VSEntity(vs_project)
  , VSRefcountable()
  , VSNameable(nm)
  , VSIndexable(uid)
  , VSFSStored()
  , lang(ltype)
  , sln(0)
  , uiprj(0)
  , active_bb(0)
  , active(false) {
    setRelPath(path);
  }

  VSProject::~VSProject() {
    // Delete UI representation
    // if(uiprj!=0) { delete uiprj; uiprj=0; }
    // Delete all filters
#ifdef USE_BOOST
    for(vsf_ci it=filters.begin(); it!=filters.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
      if((*it)!=0) {
        delete (*it); /*(*it)=0;*/
      } else {
        kddbg << g_err_list_corrupted.arg(VSPART_FILTER).arg("VSProject::~VSProject");
        //TODO: In future, maybe throw and exception here
      }
    }

    // Free all files
#ifdef USE_BOOST
    for(vsfl_ci it=files.begin(); it!=files.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
      if((*it)!=0) {
        if((*it)->release(static_cast<vse_p>(this)).isfree()) { delete (*it); /*(*it)=0;*/ }
      } else {
        kddbg << g_err_list_corrupted.arg(VSPART_FILE).arg("VSProject::~VSProject");
      }
    }
    sln = 0;
  }

  void VSProject::insert(vse_p item) {
    switch(item->getType()) {
      case vs_filter: {
#ifdef USE_BOOST
        filters.push_back(static_cast<vsf_p>(item));
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
        break; }
      case vs_file: {
        vsfl_p f = static_cast<vsfl_p>(item);
#ifdef USE_BOOST
        files.push_back(f);
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
        f->setParent(this); //NOTE: acquires item
        break; }
      default: {
        kddbg << g_wrn_unsupportedtyp.arg(type2String(item->getType())).arg("VSProject::insert");
        return; }
    }
  }

  void VSProject::setParent(vse_p pnt) {
    switch(pnt->getType()) {
      case vs_solution: {
        if(sln == 0) {
          sln = static_cast<vss_p>(pnt);
        } else {
          kddbg << "Project: \"" << name << "\" is used in more than one solution.\n";
        }
        acquire(pnt); //NOTE: increases refcount
        break; }
      case vs_filter: {
        // NOTE: We don't make filter our parent
        break; }
      default: {
        kddbg << g_wrn_unsupportedtyp.arg(type2String(pnt->getType())).arg("VSProject::insert");
        break; }
    }
  }

  vse_p VSProject::getParent() const {
    return sln;
  }

  vse_p VSProject::getByUID(const QUuid &uid) const {
#ifdef USE_BOOST
    vsfl_ci it=files.begin();
    for(; it!=files.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
      if((*it)!=0) {
        if((*it)->getUID() == uid) { break; }
      } else {
        kddbg << g_err_list_corrupted.arg(VSPART_FILE).arg("");
        return 0;
      }
    }
#ifdef USE_BOOST
    if(it!=files.end()) { return (*it); }
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
    else {
      kddbg << "File " << guid2String(uid) << " not found.\n";
      return 0;
    }
    return 0;
  }

  bool VSProject::createUI(uivse_p uipnt) {
    if(uiprj==0) {
      uiprj = part()->explorerWidget()->addProjectNode(uipnt, this);
      if(uiprj==0) { kddbg << "failed to add prj UI node" << endl; return false; }
      // kddbg << "Prj: " << name << " in " << uipnt->getName() << endl;
    }
    return true;
  }

  bool VSProject::dumpLayout(QTextStream &s) {
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
    s << "\") = \"" << getName() << "\", \"" << getRelPath() << "\", \"";
    s << guid2String(getUID()) << "\"\n";

    // Write project requirements
#ifdef USE_BOOST
    if(!reqs.empty()) {
      s << "\tProjectSection(" << VSPART_PRJSECTION_DEPENDENCIES << ") = postProject\n";
      for(vsp_ci it=reqs.begin(); it!=reqs.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
        s << "\t\t" << guid2String((*it)->getUID()) << " = " << guid2String((*it)->getUID()) << endl;
      }
      s << "\tEndProjectSection\n";
    }
    s << "EndProject\n";
    return true;
  }

  vsp_p VSProject::getReqByUID(const QUuid &uid) const {
#ifdef USE_BOOST
    vsp_ci it=reqs.begin();
    for(; it!=reqs.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
      if((*it)!=0) {
        if((*it)->getUID() == uid) { break; }
      } else {
        kddbg << g_err_list_corrupted.arg(VSPART_PROJECT).arg("VSProject::getReqByUID");
        return 0;
      }
    }
#ifdef USE_BOOST
    if(it!=reqs.end()) { return (*it); }
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
    else {
      kddbg << "Requisite prj: \"" << guid2String(uid) << "\" not found.\n";
      return 0;
    }
    return 0;
  }

  vsp_p VSProject::getDepByUID(const QUuid &uid) const {
#ifdef USE_BOOST
    vsp_ci it=deps.begin();
    for(; it!=deps.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
      if((*it)!=0) {
        if((*it)->getUID() == uid) { break; }
      } else {
        kddbg << g_err_list_corrupted.arg(VSPART_PROJECT).arg("VSProject::getDepByUID");
        return 0;
      }
    }
#ifdef USE_BOOST
    if(it!=deps.end()) { return (*it); }
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
    else {
      kddbg << "Dependency prj: \"" << guid2String(uid) << "\" not found.\n";
      return 0;
    }
    return 0;
  }

  vsf_p VSProject::getFltByUID(const QUuid &uid) const {
#ifdef USE_BOOST
    vsf_ci it=filters.begin();
    for(; it!=filters.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
      if((*it)!=0) {
        if((*it)->getUID() == uid) { break; }
      } else {
        kddbg << g_err_list_corrupted.arg(VSPART_FILTER).arg("VSProject::getFltByUID");
        return 0;
      }
    }
#ifdef USE_BOOST
    if(it!=filters.end()) { return (*it); }
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
    else {
      kddbg << "Filter: \"" << guid2String(uid) << "\" not found.\n";
      return 0;
    }
    return 0;
  }

  bool VSProject::addDependency(vsp_p dp) {
    if(dp != 0) {
      if(dp->getType() == vs_project) {
        // Scan for duplicates
#ifdef USE_BOOST
        vsp_ci it=deps.begin();
        for(; it!=deps.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
          if((*it)->getUID() == dp->getUID()) { break; }
        }
#ifdef USE_BOOST
        if(it!=deps.end()) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
          kddbg << "Warning! Can't add \"" << dp->getName() << "\" as dependency to \""
              << getName() << "\". Duplicated dependencies aren't allowed.\n";
          return false;
        }

        // Scan for circularity
#ifdef USE_BOOST
        it=reqs.begin();
        for(; it!=reqs.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
          if((*it)->getUID() == dp->getUID()) { break; }
        }
#ifdef USE_BOOST
        if(it!=reqs.end()) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
          kddbg << "Error! Circularity in requirement chain found. "
              << "|" << (*it)->getName() << "|" << getName() << "|"
              << dp->getName() << "|\n";
          return false;
        }

#ifdef USE_BOOST
        deps.push_back(dp);
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
        return true;
      }
    }
    return false;
  }

  bool VSProject::addDependency(const QUuid &uid) {
    return addDependency(static_cast<vsp_p>(sln->getByUID(uid)));
  }

  bool VSProject::addRequirement(vsp_p rq) {
    if(rq != 0) {
      if(rq->getType() == vs_project) {
        // Scan for duplicates
#ifdef USE_BOOST
        vsp_ci it=reqs.begin();
        for(; it!=reqs.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
          if((*it)->getUID() == rq->getUID()) { break; }
        }
#ifdef USE_BOOST
        if(it!=reqs.end()) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
          kddbg << "Warning! Can't add \"" << rq->getName() << "\" as requirement to \""
              << getName() << "\". Duplicated requirements aren't allowed.\n";
          return false;
        }

        //Scan for circularity
#ifdef USE_BOOST
        it=deps.begin();
        for(; it!=deps.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
          if((*it)->getUID() == rq->getUID()) { break; }
        }
#ifdef USE_BOOST
        if(it!=deps.end()) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
          kddbg << "Error! Circularity in dependency chain found. "
              << "|" << rq->getName() << "|" << getName() << "|"
              << (*it)->getName() << "|\n";
          return false;
        }

#ifdef USE_BOOST
        reqs.push_back(rq);
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
        return true;
      }
    }
    return false;
  }

  bool VSProject::addRequirement(const QUuid &uid) {
    return addRequirement(static_cast<vsp_p>(sln->getByUID(uid)));
  }

  bool VSProject::populateUI() {
#ifdef USE_BOOST
    if(!filters.empty()) {
      for(vsf_ci it=filters.begin(); it!=filters.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
        if(!(*it)->createUI()) return false;
        if(!(*it)->populateUI()) return false;
      }
    } else {
#ifdef USE_BOOST
      for(vsfl_ci it=files.begin(); it!=files.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
        if(!(*it)->createUI((uivse_p)uiprj)) return false;
      }
    }
    return true;
  }

  void VSProject::setLanguage(e_VSPrjLangType lng) {
    switch(lng) {
      case vs_prjlang_c: {
        lang = lng;
        break; }
      case vs_prjlang_cs: {
        lang = lng;
        break; }
      default: {
        kddbg << "Warning!!! Tried to set unknown project language in \""
            << name << "\" project \n";
        break; }
    }
  }

  void VSProject::setActive(bool a) {
    active = a;
  }

  /*inline*/ bool VSProject::isActive() const {
    return active;
  }

  bool VSProject::createCfg(const vcfg_cp p, const vcfgcr_r cr) {
    if(p != 0) {
      // Check configs list for duplications and ambiguities
      BOOSTVEC_FOR(vsbb_ci, it, bboxes) {
        vsbb_p pbb = static_cast<vsbb_p>(*it);
        if(pbb != 0) {
          if(pbb->config() == cr) {
            kddbg << VSPART_ERROR"Config [" << pbb->config().toString() << "] already exists in: " << name << " prj.\n";
            return false;
          }
          //NOTE: We can't have two configs with same parent either, this will lead to ambiguity.
          else if(pbb->belongs(p)) {
            if(pbb->isEnabled()) {
              // Substitute this config with new one that will be created
              pbb->setEnabled(false);
              continue;
            }
          }
        } else {
          kddbg << g_err_list_corrupted.arg(VSPART_BUILDBOX).arg("VSProject::createCfg");
          return false;
        }
      }

      // Create new buildbox and make all necessary preparations
      vsbb_p bb = new VSBuildBox(cr.name, cr.platform);

      if(bb != 0) {
        bb->setParentCfg(p);
        bb->setEnabled(true);
        BOOSTVEC_PUSHBACK(bboxes, bb);
#ifdef DEBUG
        kddbg << "Config created: " << bb->config().toString() << " within: " << getName() << ".\n";
#endif
        return true;
      } else {
        kddbg << g_err_notenoughmem.arg("vsbb_p bb").arg("VSProject::createCfg");
        return false;
      }
    }
    else { kddbg << g_err_nullptr.arg("VSProject::createCfg"); return false; }
    return false;
  }

  bool VSProject::selectCfg(const vcfg_cp p) {
    if(p != 0) {
      vsbb_p bb = getBB(p);
      if(bb != 0) {
        active_bb = bb;
#ifdef DEBUG
        kddbg << g_msg_configapply.arg(active_bb->config().toString()).arg("VSProject::selectCfg");
#endif
      }
    }
    else { kddbg << g_err_nullptr.arg("VSProject::selectCfg"); return false; }
    return false;
  }

  vsbb_p VSProject::getBB(const vcfg_cp parent_cfg) const {
    if(parent_cfg != 0) {
      BOOSTVEC_FOR(vsbb_ci, it, bboxes) {
        if((*it) != 0) {
          if((*it)->belongs(parent_cfg)) { return (*it); }
        }
        else { kddbg << g_err_list_corrupted.arg(VSPART_BUILDBOX).arg("VSProject::getBB"); return 0; }
      }
#ifdef DEBUG
      kddbg << g_err_ent_notfound.arg(VSPART_BUILDBOX).arg(parent_cfg->toString()).arg("VSProject::getBB");
#endif
      return 0;
    }
    else { kddbg << g_err_nullptr.arg("VSProject::getBB"); return 0; }
    return 0;
  }

  //===========================================================================
  // Visual studio filter methods
  //===========================================================================
  VSFilter::VSFilter(const QString &nm, const QUuid &uid)
  : VSEntity(vs_filter)
  , VSNameable(nm)
  , VSIndexable(uid)
  , parent(0)
  , uiflt(0) {
  }

  VSFilter::~VSFilter() {
    // Delete UI representation
    // if(uiflt!=0) { delete uiflt; uiflt=0; }
  }

  void VSFilter::insert(vse_p item) {
    switch(item->getType()) {
      case vs_file:
      case vs_project:
#ifdef USE_BOOST
        chld.push_back(item);
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
        //NOTE: We are not setting this filter as parent of
        // projects and files. Those already have a proper parent,
        // solution or project.
        break;
      case vs_filter:
#ifdef USE_BOOST
        chld.push_back(item);
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
        item->setParent(this);
        break;
      default:
        kddbg << g_wrn_unsupportedtyp.arg(type2String(item->getType())).arg("VSFilter::insert");
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
        kddbg << g_wrn_unsupportedtyp.arg(type2String(pnt->getType())).arg("VSFilter::setParent");
        return;
    }
  }

  /*inline*/ vse_p VSFilter::getParent() const {
    return parent;
  }

  // QString VSFilter::getRelativePath() const {
  //   return ""; //TODO: make it return path relative to entity it's in
  // }

  // bool VSFilter::setRelativePath(const QString &pth) {
  //   //TODO: make it change parents, based upon type of it's parent (maybe, don't quite know yet)
  //   // so that it became not necessary to store relative path in instance and this "property"
  //   // became procedural
  //   return false;
  // }

  vse_p VSFilter::getByUID(const QUuid &uid) const {
#ifdef USE_BOOST
    vse_ci it=chld.begin();
    for(; it!=chld.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
      if((*it) != 0) {
        switch((*it)->getType()) {
          case vs_project: {
            if(static_cast<vsp_p>(*it)->getUID() == uid) { break; }
            break; }
          case vs_filter: {
            if(static_cast<vsf_p>(*it)->getUID() == uid) { break; }
            break; }
          case vs_file: {
            if(static_cast<vsfl_p>(*it)->getUID() == uid) { break; }
            break; }
          default: {
            break; }
        }
      } else {
        kddbg << g_err_list_corrupted.arg("VSEntity").arg("VSFilter::getByUID");
        return 0;
      }
    }
#ifdef USE_BOOST
    if(it!=chld.end()) { return (*it); }
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
    else {
      kddbg << "Child " << guid2String(uid) << " not found.\n";
      return 0;
    }
    return 0;
  }

  bool VSFilter::createUI() {
    if(uiflt==0) {
      if(parent != 0) {
        uiflt = part()->explorerWidget()->addFilterNode(parent->getUI(), this);
        if(uiflt==0) { kddbg << "failed to add filter UI node" << endl; return false; }
        return true;
      } else {
        kddbg << "Fiter \"" << name << "\" has no parent.\n";
        return false;
      }
      // kddbg << "Flt: " << name << " in " << parent->getName() << endl;
    }
    return false;
  }

  bool VSFilter::dumpLayout(QTextStream &s) {
    s << "Project(\"" << guid2String(uid_vs9filter) << "\") = \""
        << getName() << "\", \"" << getName() << "\", \""
        << guid2String(getUID()) << "\"\n" << "EndProject\n";
    return true;
  }

  bool VSFilter::getParentUID(QUuid* uid) const {
    if(parent != 0) {
      switch(parent->getType()) {
        case vs_solution: {
          if(uid != 0) {
            *uid = 0;
          }
          return false;
          break; }
        case vs_project: {
          if(uid != 0) {
            *uid = static_cast<vsp_p>(parent)->getUID();
            return true;
          }
          break; }
        case vs_filter: {
          if(uid != 0) {
            *uid = static_cast<vsf_p>(parent)->getUID();
            return true;
          }
          break; }
        default: {
          kddbg << g_err_unsupportedtyp.arg(parent->getType()).arg("VSFilter::getParentUID");
          break; }
      }
    }
    return false;
  }

  bool VSFilter::populateUI() {
#ifdef USE_BOOST
    for(vse_ci it=chld.begin(); it!=chld.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
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
  VSFile::VSFile(const QString &nm, const QUuid &uid, vsp_p pnt)
  : VSEntity(vs_file)
  , VSRefcountable()
  , VSNameable(nm)
  , VSIndexable(uid)
  , VSFSStored()
  , uifl(0) {
    setParent(pnt);
  }

  VSFile::~VSFile() {
    if(uifl!=0) { delete uifl; uifl=0; }
  }

  void VSFile::setParent(vsp_p pnt) {
    switch(pnt->getType()) {
      case vs_project: {
        //TODO: Think about how should parent variable behave, since there
        // is setting projects and solutions into active states and many
        // other things that will require "parent" variable
        if(parent == 0) {
          parent = pnt;
        }
        acquire(static_cast<vse_p>(pnt));
        break; }
      default: {
        kddbg << "Error! Can't set \"" << type2String(pnt->getType())
            << "\" as a parent of \"" << type2String(getType()) << "\"\n";
        return; }
    }
  }

  /*inline*/ vse_p VSFile::getParent() const {
    return parent;
  }

  // QString VSFile::getRelativePath() const {
  //   //TODO: make it return a path relative to project it's in
  //   return "";
  // }

  // bool VSFile::setRelativePath(const QString &path) {
  //   //TODO: make it change parent according to relative path from input
  //   //so that it moved between parent projects (copied or moved)
  //   //and that also makes storing path not necessary, because nature of
  //   //it will be procedural
  //   return false;
  // }

  vsp_p VSFile::getByUID(const QUuid &uid) const {
#ifdef USE_BOOST
    vse_ci it=pnts.begin();
    for(; it!=pnts.end(); ++it) {
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
      if((*it) != 0) {
        if((*it)->getType() == vs_project) {
          if(static_cast<vsp_p>(*it)->getUID() == uid) { break; }
        }
      } else {
        kddbg << g_err_list_corrupted.arg("VSEntity").arg("VSFile::getByUID");
        return 0;
      }
    }
#ifdef USE_BOOST
    if(it!=pnts.end()) { return static_cast<vsp_p>(*it); }
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
    else {
      kddbg << "Can't find parent: " << guid2String(uid) << "\n";
      return 0;
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
  : VSEntity(vs_tool)
  , vstl(t) {
  }

  VSTool::~VSTool() {
  }

  //===========================================================================
  // Visual studio "MSVC compiler" build tool methods
  //===========================================================================
  VSToolCompilerMSVC::VSToolCompilerMSVC()
  : VSTool(vstl_compiler) {
  }

  VSToolCompilerMSVC::~VSToolCompilerMSVC() {
  }

  vse_p VSToolCompilerMSVC::getByUID(const QUuid &/*uid*/) const {
    return 0;
  }

  uivse_p VSToolCompilerMSVC::getUI() const {
    //TODO: Implement UI for MSVC compiler build tool
    return 0;
  }

  void VSToolCompilerMSVC::setParent(vse_p /*parent*/) {
  }

  vse_p VSToolCompilerMSVC::getParent() const {
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

  vse_p VSToolLinkerMSVC::getByUID(const QUuid &/*uid*/) const {
    return 0;
  }

  uivse_p VSToolLinkerMSVC::getUI() const {
    //TODO: Implement UI for MSVC linker build tool
    return 0;
  }

  void VSToolLinkerMSVC::setParent(vse_p /*parent*/) {
  }

  vse_p VSToolLinkerMSVC::getParent() const {
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

  vse_p VSToolCompilerMSMidl::getByUID(const QUuid &/*uid*/) const {
    return 0;
  }

  uivse_p VSToolCompilerMSMidl::getUI() const {
    //TODO: Implement UI for MS midl compiler build tool
    return 0;
  }

  void VSToolCompilerMSMidl::setParent(vse_p /*parent*/) {
  }

  vse_p VSToolCompilerMSMidl::getParent() const {
    return 0;
  }

  //===========================================================================
  // VS Platform methods
  //===========================================================================
  VSPlatform::VSPlatform(e_VSPlatform p)
  : VSEntity(vs_platform)
  , vspl(p) {
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
  // VS WIN32 Platform methods
  //===========================================================================
  VSPlWin32::VSPlWin32()
  : VSPlatform(vspl_win32) {
  }

  VSPlWin32::~VSPlWin32() {
  }

  vse_p VSPlWin32::getByUID(const QUuid &/*uid*/) const {
    return 0;
  }

  uivse_p VSPlWin32::getUI() const {
    //TODO: Implement UI for Win32 Platform build tool
    return 0;
  }

  void VSPlWin32::setParent(vse_p /*parent*/) {
  }

  vse_p VSPlWin32::getParent() const {
    return 0;
  }

  //===========================================================================
  // VS WIN64 Platform methods
  //===========================================================================
  VSPlWin64::VSPlWin64()
  : VSPlatform(vspl_win64) {
  }

  VSPlWin64::~VSPlWin64() {
  }

  vse_p VSPlWin64::getByUID(const QUuid &/*uid*/) const {
    return 0;
  }

  uivse_p VSPlWin64::getUI() const {
    //TODO: Implement UI for x64 Platform build tool
    return 0;
  }

  void VSPlWin64::setParent(vse_p /*parent*/) {
  }

  vse_p VSPlWin64::getParent() const {
    return 0;
  }

  //===========================================================================
  // VS Configuration methods
  //===========================================================================
  VSConfig::VSConfig(const QString &n, e_VSPlatform p)
  : VSEntity(vs_config)
  , VSNameable(n)
  , vspl(*VSPlatform::getVSPlatform(p)) {
  }

  VSConfig::VSConfig(const QString &n, const QString &p)
  : VSEntity(vs_config)
  , VSNameable(n)
  , vspl(*VSPlatform::getVSPlatform(string2Platform(p))) {
#ifdef DEBUG
    kddbg << "VSConfig: " << n << " [" << p << "]\n";
#endif
  }

  VSConfig::VSConfig(const QString &config)
  : VSEntity(vs_config)
  , VSNameable(config.left(config.find('|')))
  , vspl(*VSPlatform::getVSPlatform(string2Platform(config.right(config.find('|', -1))))) {
#ifdef DEBUG
    kddbg << "VSConfig: (" << config << ") " << config.left(config.find('|')) <<
        " [" << config.right(config.find('|', -1)) << "]\n";
#endif
  }

  VSConfig::~VSConfig() {
  }

  vse_p VSConfig::getByUID(const QUuid &/*uid*/) const {
#ifdef DEBUG
    kddbg << "Error! It is impossible to get anything by UID from configuration.\n"
        << "\tPlease remove call to this method from your code.\n";
#endif
    return 0;
  }

  /*inline*/ uivse_p VSConfig::getUI() const {
    //TODO: Implement UI for VS configuration
    return 0;
  }

  void VSConfig::setParent(vse_p /*parent*/) {
  }

  /*inline*/ vse_p VSConfig::getParent() const {
    return 0;
  }

  bool VSConfig::operator == (const vcfg_cr c) const {
    return ((name == c.getName()) && (vspl.platform() == c.platform()));
  }

  bool VSConfig::operator == (const vcfgcr_r cr) const {
    return ((name == cr.name) && (vspl.platform() == cr.platform));
  }

  const QString VSConfig::toString() const {
    return QString(name).append("|%1").arg(platform2String(vspl.platform()));
  }

  //===========================================================================
  // VS BuildBox::VSToolUnit methods
  //===========================================================================
  VSBuildBox::VSToolUnit::VSToolUnit()
  : weight(0)
  , tool(0) {
  }

  VSBuildBox::VSToolUnit::~VSToolUnit() {
    if(tool != 0) { delete tool; tool = 0; }
    else { kddbg << VSPART_ERROR"Tool was deleted prior the destruction.\n"; }
  }

  //===========================================================================
  // VS BuildBox methods
  //===========================================================================
  VSBuildBox::VSBuildBox(const QString& n, e_VSPlatform p)
  : VSEntity(vs_buildbox)
  , cfg(n,p)
  , pcfg(0)
  , enabled(false) {
  }

  VSBuildBox::~VSBuildBox() {
    pcfg = 0;
  }

  vse_p VSBuildBox::getByUID(const QUuid &/*uid*/) const {
    return 0;
  }

  uivse_p VSBuildBox::getUI() const {
    return 0; //TODO: Think about implementing UI for buildbox
  }

  void VSBuildBox::setParent(const vse_p /*p*/) {
    kddbg << VSPART_ERROR"VSBuildBox::setParent is undefined.\n";
  }

  vse_p VSBuildBox::getParent() const {
    return 0;
  }

  /*inline*/ bool VSBuildBox::isEnabled() const {
    return enabled;
  }

  /*inline*/ void VSBuildBox::setEnabled(bool e/*=true*/) {
    enabled = e;
  }

  /*inline*/ const vcfg_cp VSBuildBox::parentConfig() const {
    return pcfg;
  }

  void VSBuildBox::setParentCfg(const vcfg_cp c) {
    pcfg = c;
#ifdef DEBUG
    if(pcfg != 0) {
      kddbg << "Config: " << pcfg->toString() << " is parent to: " << cfg.toString() << endl;
    } else { kddbg << VSPART_ERROR"parent config is still 0.\n"; }
#endif
  }

  /*inline*/ const vcfg_cr VSBuildBox::config() const {
    return cfg;
  }

  bool VSBuildBox::belongs(const vcfg_cp parent_cfg) const {
    if(parent_cfg == pcfg) { return true; }
    return false;
  }

  //END VS build entities

  vss_p getParentSln(vse_p e) {
    vss_p sln = 0;
    if(e != 0) {
      switch(e->getType()) {
        case vs_project: {
          sln = static_cast<vss_p>(e->getParent());
          break; }
        case vs_filter: {
          switch(e->getParent()->getType()) {
            case vs_solution: {
              sln = static_cast<vss_p>(e->getParent());
              break; }
            case vs_project: {
              sln = static_cast<vss_p>(e->getParent()->getParent());
              break; }
            default: {
              break; }
          }
          break; }
        case vs_file: {
          switch(e->getParent()->getType()) {
            case vs_project: {
              sln = static_cast<vss_p>(e->getParent());
              break; }
            case vs_filter: {
              sln = static_cast<vss_p>(e->getParent()->getParent());
              break; }
            default: {
              break; }
          }
          break; }
        default: {
          kddbg << g_err_unsupportedtyp.arg(type2String(e->getType()));
          break; }
      }
    }
    return sln;
  }
};
