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
#include <qregexp.h>

#include <kdebug.h>
#include <kaction.h>

#include "vs_model.h"
#include "vs_part.h"
#include "vs_explorer.h"

namespace VStudio {
  static const VSPlWin32 g_Win32Platform;
  static const VSPlWin64 g_Win64Platform;
  static const VSPlMixed g_MixedPlatform;
  static const VSPlAnyCPU g_AnyCPUPlatform;

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
      BOOSTVEC_PUSHBACK(pnts, p);
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
      vse_ci it=pnts.begin();
      BOOSTVEC_OFOR(it, pnts) { if((*it) == p) { break; } }
#ifdef USE_BOOST
      if(it!=pnts.end()) {
        pnts.erase(it);
#else
#error "VStudio: Boost support is no enabled" //TODO: Implement this
#endif
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

  /*inline*/ const pv_vse_cr VSRefcountable::parents() const {
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
  , path_abs(QString::null)
  , fsflg(0) {
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
    __try_reach();
  }

  /*inline*/ bool VSFSStored::isInSync() const { return check_bit(fsflg, IS_IN_SYNC); }
  /*inline*/ bool VSFSStored::isReachable() const { return check_bit(fsflg, IS_REACHABLE); }
  /*inline*/ bool VSFSStored::isLoaded() const { return check_bit(fsflg, IS_LOADED_OK); }

  /*inline*/ void VSFSStored::__try_reach() {
    if(!path_abs.isEmpty()) {
      QFile fl;
      if(fl.exists(path_abs)) { set_bit(fsflg, IS_REACHABLE); return; }
    }
    clear_bit(fsflg, IS_REACHABLE);
  }

  //===========================================================================
  // Visual Studio entity methods
  //===========================================================================
  VSPart *VSEntity::sys_part = 0;
  VSEntity::VSEntity(e_VSEntityType typ)
  : type(typ)
  // , uient(0)
  , enflg(0) {
  }

  VSEntity::~VSEntity() {
#ifdef DEBUG
    // kddbg << "Freeing entity: " << type2String(type) << endl;
#endif
  }

  /*inline*/ e_VSEntityType VSEntity::getType() const { return type; }

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

  /*inline*/ VSPart* VSEntity::part() const { return sys_part; }
  /*inline*/ bool VSEntity::isConfigured() const { return check_bit(enflg, IS_CONFIGURED); }
  /*inline*/ bool VSEntity::isActive() const { return check_bit(enflg, IS_ACTIVE); }

  void VSEntity::insert(vse_p /*pnt*/) {
    // kddbg << type2String(getType()) << "'s ::insert method is not implemented. "
    //     << "when inserting \"" << pnt->getName() << "\" "
    //     << type2String(pnt->getType()) << endl;
  }

  bool VSEntity::createUI(uivse_p /*pnt*/) { return true; }

  //===========================================================================
  // Visual studio solution methods
  //===========================================================================
  VSSolution::VSSolution(const QString &nm)
  : VSEntity(vs_solution)
  , VSNameable(nm)
  , VSFSStored()
  , uisln(0)
  , active_bb(0)
  , active_prj(0)
  , version(vssln_ver_unknown)
  , fmt_version(0) {
  }

  VSSolution::~VSSolution() {
    active_prj = 0;
    // Delete UI representation
    if(uisln != 0) { delete uisln; uisln=0; }

    // Delete all configurations
    // setConfiguration(QString::null);
    BOOSTVEC_FOR(vsbb_ci, it, bboxes) {
      vsbb_p bb(*it);
      if(bb != 0) { delete bb; }
    }

    // Delete all filters
    BOOSTVEC_FOR(vsf_ci, it, filters) {
      if((*it) != 0) { delete (*it); /*(*it)=0;*/ }
      else {
        kddbg << g_err_list_corrupted.arg(VSPART_FILTER).arg("VSSolution::~VSSolution");
      }
    }

    // Delete all projects
    BOOSTVEC_FOR(vsp_ci, it, projects) {
      vsp_p prj(*it);
      if(prj != 0) {
        if(prj->release(static_cast<vse_p>(this)).isfree()) { delete prj; /*(*it)=0;*/ }
      } else {
        kddbg << g_err_list_corrupted.arg(VSPART_PROJECT).arg("VSSolution::~VSSolution");
      }
    }

    // Delete all meta-dependencies
    BOOSTVEC_FOR(vsmd_ci, mdci, mdeps) {
      if((*mdci) != 0) { delete (*mdci); /*(*mdci) = 0;*/ }
      else {
        kddbg << g_err_list_corrupted.arg("Meta-dependency").arg("VSSolution::~VSSolution");
      }
    }
  }

  void VSSolution::insert(vse_p item) {
    switch(item->getType()) {
      case vs_project: {
        vsp_p p = static_cast<vsp_p>(item);
        BOOSTVEC_PUSHBACK(projects, p);
        p->setParent(static_cast<vse_p>(this)); //NOTE: acquires project
        break; }
      case vs_filter: {
        vsf_p f = static_cast<vsf_p>(item);
        BOOSTVEC_PUSHBACK(filters, f);
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

  bool VSSolution::write(bool sync/*=true*/) {
    /* Check that all necessary for saving points are checked
        NOTE: Allow save op when we have all projects at least at CONFIGURED state */
    BOOSTVEC_FOR(vsp_ci, it, projects) {
      vsp_p prj(*it);
      if(prj != 0) {
        if(prj->isConfigured()) {
          // Save modified project files
          if(!prj->isInSync()) {
            if(!prj->write(true)) {
              kddbg << QString(VSPART_ERROR"Can't save modified project [%1].\n").arg(prj->getName());
              return false;
            }
          }
        }
        else {
          kddbg << QString(VSPART_ERROR"Can't save [%1], contained project [%2] is not fully loaded.\n").
              arg(name).arg(prj->getName());
          return false;
        }
      }
      else {
        kddbg << g_err_list_corrupted.arg(VSPART_PROJECT).arg(QString("VSSolution[%1]::write").arg(name));
        return false;
      }
    }

    QString path = path_abs.append(".test");
    QString str;
    QFile sln_f(path);
      // if(!sln_f.exists(abspath)) { kddbg << "solution: " << abspath << " will be created from scratch" << endl; }
    sln_f.setName(path);
    if(!sln_f.open(IO_WriteOnly|IO_Raw)) { kddbg << VSPART_ERROR"Can't open \"" << path << "\"\n"; return false; }
    if(!sln_f.isWritable()) { kddbg << VSPART_ERROR"Path \"" << path << "\" is not writable\n"; return false; }

    QTextStream s(&sln_f);
    kddbg << "[" << name << "]========================================: Begin save\n";

    if(!write(s, sync)) {
      kddbg << VSPART_ERROR"Can't save: \"" << path << "\"\n";
      sln_f.flush();
      sln_f.close();
      return false;
    }

    sln_f.flush();
    sln_f.close();

    kddbg << "[" << name << "]========================================: Saved\n";
    return true;
  }

  bool VSSolution::write(QTextStream &s, bool sync/*=true*/) {
    // Dump version data
    s << QString("Microsoft Visual Studio Solution File, Format Version %1.00\n").arg(fmt_version);

    // Write VS version for .sln file
    switch(version) {
      case vssln_ver9: { s << "# Visual Studio 2008\n"; break; }
      case vssln_ver8: { break; }
      case vssln_ver7: { break; }
      default: {
        kddbg << VSPART_ERROR"Unsupported version of VS file: " << slnVer2String(version) << endl;
        return false;
      }
    }

    // Save filters layout
    BOOSTVEC_FOR(vsf_ci, it, filters) {
      if((*it) != 0) {
        (*it)->dumpLayout(s);
      }
      else {
        kddbg << g_err_list_corrupted.arg("filters").arg("VSSolution::dumpLayout");
        return false;
      }
    }

    // Save projects layout
    BOOSTVEC_FOR(vsp_ci, it, projects) {
      if((*it) != 0) {
        (*it)->dumpLayout(s);
      }
      else {
        kddbg << g_err_list_corrupted.arg("projects").arg("VSSolution::dumpLayout");
        return false;
      }
    }

    //BEGIN // Save solution sections data
    s << "Global" << endl;
    const QString section_header("\tGlobalSection(%1) = %2\n");
    const QString section_footer("\tEndGlobalSection\n");

    // Save solution's configurations
    s << section_header.arg(VSPART_SLNSECTION_SCFG_PLATFORMS).arg("preSolution");
    BOOSTVEC_FOR(vsbb_ci, it, bboxes) {
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
    BOOSTVEC_FOR(vsp_ci, it, projects) {
      vsp_p prj(*it);
      if(prj != 0) {
        BOOSTVEC_FOR(vsbb_ci, itbb, prj->bbs()) {
          vsbb_p bb(*itbb);
          if(bb != 0) {
            if(bb->parentConfig() == 0) {
              kddbg << QString(VSPART_ERROR"[%1]:[%2] config is parentless.\n").arg(prj->getName()).
                  arg(bb->config().toString());
              continue;
            }
            else {
#ifdef DEBUG
              kddbg << QString("Write [%1]:[%2] config.\n").arg(prj->getName()).arg(bb->config().toString());
#endif
              s << "\t\t" << guid2String(prj->getUID()) << "." << bb->parentConfig()->toString() << ".ActiveCfg = "
                  << bb->config().toString() << endl;
              if(bb->isEnabled()) {
                s << "\t\t" << guid2String(prj->getUID()) << "." << bb->parentConfig()->toString() << ".Build.0 = "
                    << bb->config().toString() << endl;
              }
            }
          }
          else { kddbg << g_err_list_corrupted.arg(VSPART_BUILDBOX).arg("VSSolution::write"); return false; }
        }
      }
      else { kddbg << g_err_list_corrupted.arg(VSPART_PROJECT).arg("VSSolution::write"); return false; }
    }
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
    if(sync) { set_bit(fsflg, IS_IN_SYNC); } else { clear_bit(fsflg, IS_IN_SYNC); }
    return true;
  }

  bool VSSolution::read(bool /*sync=true*/) {
    //Check paths:
    if(path_rlt.isEmpty()) { kddbg << g_err_emptypath.arg("Relative").arg("VSSolution::read"); return false; }
    if(path_abs.isEmpty()) { kddbg << g_err_emptypath.arg("Absolute").arg("VSSolution::read"); return false; }

    QFile fl;

    if(!fl.exists(path_abs)) { kddbg << VSPART_ERROR"File \"" << path_abs << "\" not found.\n"; return false; }
    fl.setName(path_abs);
    if(!fl.open(IO_ReadWrite|IO_Raw)) { kddbg << "can't open solution file" << endl; return false; }
    if(!fl.isReadable()) { kddbg << "is not readable" << endl; return false; }
    if(!fl.isWritable()) { kddbg << "is not writable" << endl; return false; }
    if(!fl.isReadWrite()) { kddbg << "can't read and write" << endl; return false; }

    QTextStream stream(&fl);

    if(!read(stream, true)) {
      kddbg << VSPART_ERROR"Failed to parse sln: " << name << " file \"" << path_abs << "\"\n";
      return false;
    }

    fl.close();
    return true;
  }

  bool VSSolution::read(QTextStream &str, bool sync/*=true*/) {
    QString ln;
    bool nestedprjs_section_loaded = false;
    vsp_p prj_active = 0; // Active project to collect dependencies from project section
    kddbg << "[" << name << "]========================================: Begin parse\n";
    kddbg << "PATH: " << path_abs << endl;

    //BEGIN // Read the .sln header and version info
    QString header, guard, ver;
    //int sln_ver = 0;
    //int sln_ver_expected = 0;

    // Read the header of a file
    guard = str.readLine();
    kddbg << "HEADER: " << guard << endl;

    //TODO: I forgot what was about this "one chance save" empty line detection code
    if(guard.isEmpty()) {
      kddbg << VSPART_ERROR"Empty line at begin of file detected.\n";
      guard = str.readLine();
    }

    // Detect version of a VS by file
    header = str.readLine();
    kddbg << "VERSION STRING: " << header << endl;

    if(header.isEmpty() || header[0].latin1() != '#') {
      kddbg << VSPART_ERROR"File \"" << path_abs << "\" is not a .sln file.\n";
      return false;
    }

    if(header.compare(QString("# Visual Studio 2008")) == 0) { //NOTE: Pretty simple, maybe there is a neater way
      version = vssln_ver9;
    } else if(header.compare(QString("# Visual Studio 2005")) == 0) {
      version = vssln_ver8;
    } else {
      version = vssln_ver7;
    }

    if(version == vssln_ver_unknown) {
      kddbg << VSPART_ERROR"Failed to read version from \"" << path_abs << "\".\n";
      return false;
    }
    kddbg << "VS VERSION: " << slnVer2String(version) << endl;

    //Format version detection
    //TODO: Still don't know what is the proper use for this
    //NOTE: It says "Format version", suppose it's intenal for .sln
    //TODO: Work a simplier solution which will eliminat need of QString ver
    ver = guard.right(5);
    ver.remove(ver.find('.'), 3);
    bool b_ok = false;
    fmt_version = ver.toInt(&b_ok, 10);
    if(!b_ok) { kddbg << "Error! Can't convert version string to int.\n"; return false; }
    kddbg << "VS_FMT_VER: " << fmt_version << endl;
    //END // Read the .sln header and version info

    while(!str.atEnd()) {
      ln = str.readLine();
      //BEGIN // Read project info
      if(0 == ln.left(7).compare(QString("Project"))) { // kddbg << " >>>>> Solution item section\n";
        while(0 != ln.left(10).compare(QString("EndProject"))) { // kddbg << "Line: " << ln << endl;

          //BEGIN // Read project section info, dependencies
          if(ln.find(QRegExp("ProjectSection"), 0) >= 0) { // kddbg << "Project section found\n";
            /** Tells us what elements and settings should sln entity (project|filter) have
             * e.g. ProjectSection(ProjectDependencies) = postProject
            */
            QTextIStream s(&ln);
            QChar ch(0);
            QString psname;  // Section name
            QString pssetting; // Section setting

            if(!__read_parse_shdr(s, psname, pssetting)) {
              kddbg << VSPART_ERROR"Can't parse section header\n";
              return false;
            }
            ln = str.readLine();

            // Read section data
            e_VSPrjSection stype = string2PrjSectionType(psname);
            switch(stype) {
              //BEGIN // ProjectDependencies section
              case prjs_dependencies: {
                while(ln.find(QRegExp("EndProjectSection"), 0) < 0) {
                  QTextIStream si(&ln);
                  ch = 0;
                  QUuid uuid1, uuid2, *uid=&uuid1;

                  while(!si.atEnd()) {
                    switch(ch.latin1()) {
                      case '{': {
                        if(!__read_parse_uid(si, ch, *uid)) return false;
                        uid=&uuid2;
                        si >> ch;
                        break; }
                      default: { si >> ch; break; }
                    }
                  }
                  // kddbg << "\t\t\t" << psname << ": " << uuid1.toString() << " = " << uuid2.toString() << endl;
                  // prj_active->addDependency((vsp_p)sln->getByUID(uuid2));
                  vsmd_p mdp = metaDependency(prj_active->getUID());
                  if(mdp != 0) {
                    mdp->addDependency(uuid2);
                  }
                  ln = str.readLine();
                }
                break; }
              //END // ProjectDependencies section
              //BEGIN // SolutionItems section
                case prjs_slnitems: {
                  while(ln.find(QRegExp("EndProjectSection"), 0) < 0) { kddbg << psname << ": " << ln << endl;
                    ln = str.readLine();
                  }
                  break; }
              //END // SolutionItems section
              default: { kddbg << "Error! Unknown section type: " << psname << endl; return false; }
            }
          }
          //END // Read project section info, dependencies
          //BEGIN // Read and analyze solution unit data
          else {
            // Read solution unit data
            QTextIStream is(&ln);
            bool typeuid_found = false;
            bool prjuid_found = false;
            bool prjname_found = false;
            bool prjrltpath_found = false;
            QUuid puid; //Internal and project UIDs
            e_VSEntityType typ;
            e_VSPrjLangType ltyp;
            QString prjname, prjpath_rlt;
            QChar ch(0);
#ifdef DEBUG
            //char latin1ch = 0x00;
#endif
            /** Analyze project string to get project parameters:
             * Project info inside sln looks like this.
             * Project("TYPE_GUID") = "internal name" , "rel path", "PRJ_GUID"
             */
            while(!is.atEnd()) {
              switch(ch.latin1()) {
                case '"': {
                  if(!typeuid_found) {
                    QUuid tuid;
                    is >> ch;
                    if(!__read_parse_uid(is, ch, tuid)) return false;
                    typeuid_found = true;
                    is >> ch >> ch >> ch >> ch;
                    typ = uid2VSType(tuid);
                    ltyp = uid2PrjLangType(tuid);
                  }
                  else if(typeuid_found && !prjname_found) {
                    is >> ch;
                    do {
                      prjname.append(ch);
                      is >> ch;
                    } while(ch.latin1() != '"');
                    prjname_found = true;
                    is >> ch >> ch;
                  }
                  else if(prjname_found && !prjrltpath_found) {
                    is >> ch;
                    do {
                      prjpath_rlt.append(ch);
                      is >> ch;
                    } while(ch.latin1() != '"');
                    prjrltpath_found = true;
                    is >> ch >> ch;
                  }
                  else if(!prjuid_found) {
                    is >> ch;
                    if(!__read_parse_uid(is, ch, puid)) return false;
                    prjuid_found = true;
                    is >> ch;
                  }
                  break; }
                default: {
                  is >> ch;
#ifdef DEBUG
                  //latin1ch = ch.latin1();
#endif
                  break; }
              }
            }

            NormalizeSlashes(path_rlt);

            switch(typ) {
              case vs_project: {
                /* kddbg << "Project [" << prjLangType2String(ltyp) << "] " << puid.toString() << " \"" << prjname
                << "\" under: \"" << prjpath_rlt << "\"\n"; */
                switch(ltyp) {
                  case vs_prjlang_c: {
                    // Create and add model representation
                    //NOTE: Set most recent "active" project ptr
                    prj_active = new VSProject_c(prjname, puid);
                    if(prj_active != 0) {
                      prj_active->setRelPath(prjpath_rlt);
                      prj_active->setAbsPath(RebasePath_Win(path_abs, prjpath_rlt));
#ifdef DEBUG
                      kddbg << "Project [c/c++] abs path: " << prj_active->getAbsPath() << endl;
#endif
                      insert(static_cast<vsp_p>(prj_active));
                      // Read project .vcproj file
                      if(!prj_active->read(true)) {
                        kddbg << VSPART_ERROR"Can't read project file \"" << prj_active->getAbsPath() << "\"\n";
                      }
                    }
                    else {
                      kddbg << g_err_notenoughmem.arg(VSPART_PROJECT).arg("VSSolution::read");
                      return false;
                    }
                    break; }
                  case vs_prjlang_cs: {
                    prj_active = new VSProject_cs(prjname, puid);
                    if(prj_active != 0) {
                      prj_active->setRelPath(prjpath_rlt);
                      prj_active->setAbsPath(RebasePath_Win(path_abs, prjpath_rlt));
#ifdef DEBUG
                      kddbg << "Project [c#] abs path: " << prj_active->getAbsPath() << endl;
#endif
                      insert(static_cast<vsp_p>(prj_active));

                      //TODO: Reading of C# project
                    }
                    break; }
                  default:
                    kddbg << "Error! " << type2String(typ) << " \"" << prjname << "\": language ["
                        << prjLangType2String(ltyp) << "] support is not implemented\n";
                    ln = str.readLine();
                    continue; //NOTE: Just skip this unknown project
                }
                break; }
              case vs_filter: {
#ifdef DEBUG
                /* kddbg << "Filter " << puid.toString() << " \"" << prjname << "\" under: \"" << prjpath_rlt << "\"\n";
                */
#endif
                // Create and add model representation
                vsf_p flt = new VSFilter(prjname, puid);
                if(flt != 0) {
                  insert(static_cast<vse_p>(flt));
                }
                else {
                  kddbg << g_err_notenoughmem.arg(VSPART_FILTER).arg("VSSolution::read");
                  return false;
                }
                break; }
              default: { kddbg << g_err_unsupportedtyp.arg(type2String(typ)).arg("VSSolution::read"); return false; }
            }
          }
          //END // Read and analyze solution unit data
          ln = str.readLine();
        }
      }
      //END // Read project info
      //BEGIN // Read global solution sections
      else if(0 == ln.compare("Global")) { kddbg << "Entering global settings section.\n";
        while(ln.find(QRegExp("EndGlobal"), 0) < 0) { kddbg << "SECTION: " << ln << endl;
          if(ln.find(QRegExp("GlobalSection"), 0) >= 0) { // kddbg << "(G)Line: " << ln << endl;
            QTextIStream si(&ln);
            QString sname; // Section name
            QString sparam; // Section parameter

            if(!__read_parse_shdr(si, sname, sparam)) { kddbg << "Error! Can't parse section header\n"; return false; }
            ln = str.readLine();

            // Read section data
            e_VSSlnSection stype = string2SlnSectionType(sname);
            switch(stype) {
              //BEGIN // SolutionConfigurationPlatforms
              case slns_sln_cfgplatforms: {
                // ln = str.readLine();
                while(ln.find(QRegExp("EndGlobalSection"), 0) < 0) { // kddbg << "SEC: " << sname << ": " << ln << endl;
                  QString conf_name;
                  QString c_internal_name;
                  QRegExp rx("^\t\t(.+\|\w+)\ \=\ (.+\|\w+)$");
                  if(-1 != rx.search(ln)) {
                    conf_name = rx.cap(1);
                    c_internal_name = rx.cap(2);
                  }
#ifdef DEBUG
                  kddbg << "Configuration: " << conf_name << " presented as: " << c_internal_name << endl;
#endif
                  // Create and add configuration
                  VSConfigCreate cr;
                  cr.name = conf_name.left(conf_name.find('|'));
                  cr.platform = string2Platform(conf_name.mid(conf_name.find('|')+1));

                  //TODO: Consider enabling VSSolution::createCfg to handle 0 as non-lethal case,
                  //  for setting some tmp parent config that will be replaces upon loading of project
                  if(!createCfg(0, cr)) {
                    kddbg << VSPART_ERROR"Config: " << conf_name << " can't be added.\n";
                    return false;
                  }
                  ln = str.readLine();
                }
                break; }
              //END // SolutionConfigurationPlatforms
              //BEGIN // ProjectConfigurationPlaftorms
                case slns_prj_cfgplatforms: {
                  while(ln.find(QRegExp("EndGlobalSection"), 0) < 0) { // kddbg << sname << ": " << ln << endl;
                    /**
                      * There is two parts that are responsible for enabling project
                      * to be built in the solution
                      * - Active configuration string
                      *  Tells what project configuration will be used for build.
                      * - Build "marker"
                      *  Tells if the project is enabled to be build under
                      *  specified solution configuration
                      *
                      * ActiveCfg:
                      *   GUID.sln_config_name.ActiveCfg = prj_config_name
                      * Build "marker":
                      *   GUID.sln_config_name.Build.0 = prj_config_name
                      *
                      * Build marker can be present or not
                      * if not, that means that this project is not enabled in the
                      * selected configuration
                    */
                    QUuid uid;
                    ln.remove(0, 2);
                    // ln = ln.stripWhiteSpace();
#ifdef DEBUG
                    // kddbg << "LN: \"" << ln << "\"\n";
#endif
                    // Split: stage 1
                    QStringList cfg_line = QStringList::split(QString(" = "), ln);
#ifdef DEBUG
                    // kddbg << "cfg_line[0]: " << cfg_line[0] << endl;
                    // kddbg << "cfg_line[1]: " << cfg_line[1] << endl;
#endif
                    QString box_cfg = cfg_line[1];  // Remember configuration for project's bbox

                    // Split: stage 2
                    QStringList cfg_init = QStringList::split(QString("."), cfg_line[0]);
                    QTextIStream si(&cfg_init[0]);
                    QChar ch(0);
                    si >> ch; // should contain '{'
#ifdef DEBUG
                    // kddbg << "cfg_init[0]: \"" << cfg_init[0] << "\"\n";
                    // kddbg << "cfg_init[1]: \"" << cfg_init[1] << "\"\n";
                    // kddbg << "cfg_init[2]: \"" << cfg_init[2] << "\"\n";
#endif
                    if(!__read_parse_uid(si, ch, uid)) {
                      kddbg << g_err_guidparse.arg(cfg_init[0]).arg("VSSolution::read");
                      return false;
                    }
#ifdef DEBUG
                    // kddbg << "GUID: " << guid2String(uid) << endl;
#endif
                    // Get project by UID
                    vsp_p prj = static_cast<vsp_p>(getByUID(uid));
                    if(prj != 0) {
                      if(static_cast<VSFSStored*>(prj)->isLoaded()) {
                        vsbb_p pbb = getBB(cfg_init[1]);
                        if(pbb != 0) {
                            // Get bb from project by config
                            vsbb_p prjbb = prj->getBB(box_cfg);
                            if(prjbb != 0) {
                              if(cfg_init[2].compare("ActiveCfg") == 0) {
                                prjbb->setParentCfg(&pbb->config());
                                kddbg << "[" << prj->getName() << "]:[" << prjbb->config().toString()
                                    << "], parent is [" << pbb->config().toString() << "].\n";
                              }
                              else if(cfg_init[2].compare("Build") == 0) {
                                prjbb->setEnabled();
                                kddbg << "[" << prj->getName() << "]:[" << prjbb->config().toString()
                                    << "] is enabled to build under it's parent [" << pbb->config().toString() << "].\n";
                              }
                            }
                            else {
                              kddbg << g_err_ent_notfound.arg(VSPART_BUILDBOX).arg(box_cfg).arg("VSSolution::read");
                              //return false;
                            }
                        }
                        else {
                          kddbg << g_err_ent_notfound.arg(VSPART_BUILDBOX).arg(cfg_init[1]).arg("VSSolution::read");
                          return false;
                        }
                      }
                    }
                    else {
                      kddbg << g_err_ent_notfound.arg(VSPART_PROJECT).arg(cfg_init[0]).arg("VSSolution::read");
                      // return false;
                    }

                    ln = str.readLine();
                  }

                  // Mark this solution as CONFIGURED
                  { vsp_ci it = projects.begin();
                    BOOSTVEC_OFOR(it, projects) {
                      vsp_p prj(*it);
                      if(prj != 0) {
                        if(!prj->isConfigured()) { break; }
                      }
                    }
                    if(it == projects.end()) {
                      set_bit(enflg, VSEntity::IS_CONFIGURED);
                    }
                  }
                  break; }
              //END // ProjectConfigurationPlaftorms
              //BEGIN // SolutionProperties
                  case slns_sln_properties: {
                    while(ln.find(QRegExp("EndGlobalSection"), 0) < 0) { // kddbg << sname << ": " << ln << endl;
                      ln = str.readLine();
                    }
                    break; }
              //END // SolutionProperties
              //BEGIN // NestedProjects
                    case slns_nested_prjs: {
                      while(ln.find(QRegExp("EndGlobalSection"), 0) < 0) { // kddbg << sname << ": " << ln << endl;
                        QTextIStream si(&ln);
                        QChar ch(0);
                        QUuid uid1, uid2, *uid=&uid1;

                        while(!si.atEnd()) {
                          switch(ch.latin1()) {
                            case '{': {
                              if(!__read_parse_uid(si, ch, *uid)) return false;
                              uid=&uid2;
                              si >> ch;
                              break; }
                            default: { si >> ch; break; }
                          }
                        }

                        //Get filer and project model representation

                        // Get entity
                        vse_p ent = getByUID(uid1);
                        if(ent == 0) {
                          //NOTE: Try once more, in case entity is a filter
                          ent = getFltByUID(uid1);
                          if(ent == 0) {
                            kddbg << VSPART_WARNING"Failed to obtain entity for " << uid1.toString() << endl;
                            ln = str.readLine();
                            continue;
                          }
                        }

                        // Get container
                        vsf_p cnt = getFltByUID(uid2);
                        if(cnt == 0) {
                          kddbg << VSPART_WARNING"Failed to obtain container for " << uid2.toString() << endl;
                          ln = str.readLine();
                          continue;
                        }
#ifdef DEBUG
                        /*
                        kddbg << type2String(cnt->getType()) << " \"" << cnt->getName() << "\" <<< "
                            << type2String(ent->getType()) << " \"" << ent->getName() << "\"\n";
                        */
#endif
                        cnt->insert(ent); // Put entity into container
                        ln = str.readLine();
                      }
                      nestedprjs_section_loaded = true;
                      break; }
              //END // NestedProjects
              default: { kddbg << VSPART_WARNING"Unknown section type: " << sname << endl; /*return false;*/ }
            }
            // ln = str.readLine();
          }
          ln = str.readLine();
        }
      }
      //END // Read global solution sections
    }

    /* NOTE: Some solutions will be without NESTED section
    * that is basically means that all projects are inserted into a solution node
    * i.e. without any filters involved
    */
    if(!nestedprjs_section_loaded) {
      if(!filters.empty()) {
        kddbg << VSPART_ERROR"No nesting was not done, but there are filters in [" << name << "] sln.\n";
          //return false;
      }
      BOOSTVEC_FOR(vsp_ci, it, projects) {
        if((*it) != 0) {
            //insert((*it));
        }
      }
    }

    kddbg << "[" << name << "]========================================: Parsed" << endl;
    set_bit(enflg, IS_LOADED_OK);
    if(sync) { set_bit(fsflg, IS_IN_SYNC); } else { clear_bit(fsflg, IS_IN_SYNC); }
    return true;
  }

  /** \fn VSSolution::getByUID
   * \brief Gets project withi this solution by it's UID
   * @param uid uid of the project
   * @return project ptr
   */
  vsp_p VSSolution::getByUID(const QUuid &uid) const {
    BOOSTVEC_FOR(vsp_ci, it, projects) {
      vsp_p prj(*it);
      if(prj != 0) {
        if(prj->getUID() == uid) { return prj; }
      }
      else { kddbg << g_err_list_corrupted.arg(VSPART_PROJECT).arg("VSSolution::getByUID"); return 0; }
    }
    kddbg << g_err_ent_notfound.arg(VSPART_PROJECT).arg(guid2String(uid)).arg(name);
    return 0;
  }

  /*inline*/ uivss_p VSSolution::getUI() const { return uisln; }

  bool VSSolution::createUI(uivse_p /*pnt*/) {
    if(uisln == 0) {
      uisln = part()->explorerWidget()->addSolutionNode(this);
      if(uisln == 0) { kddbg << "failed to add sln UI node" << endl; return false; }
#ifdef DEBUG
      // kddbg << QString("UI for [%1]:[%2] created\n").arg(type2String(type)).arg(name);
#endif
    }
    return true;
  }

  bool VSSolution::dumpLayout(QTextStream &/*s*/) {
    return false;
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
    if(!createUI(0)) { kddbg << VSPART_ERROR"Can't create UI item for sln \"" << name << "\"\n"; return false; }

    // Insert filters, if any
    BOOSTVEC_FOR(vsf_ci, it, filters) {
      vsf_p flt(*it);
      if(flt != 0) {
        if(!flt->populateUI(uisln)) { return false; }
      }
    }

    // Create UI items for all projects that are not in filters
    BOOSTVEC_FOR(vsp_ci, it, projects) {
      vsp_p prj(*it);
      if(prj != 0) {
        if(!prj->createUI(uisln)) { return false; }
        if(!prj->populateUI()) { return false; }
      }
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

  /** \fn VSProject::createCfg(const vcfg_cp parent_config, const vcfgcr_r cr)
   * \brief Will create a new config using \a parent_config as it's parent.
   * - will not set new config as \a selected.
   * - will check internally so that configs wouldn't doublicate.
   * - if any config will have the same parent as the new one being created, it will be disabled for action and
   *   new one will be enabled instead.
   * @param parent_config configuration to use as \b parent
   * @param cr is a pointer to \a VSConfig::VSConfigCreate struct containing params for config creation
   * @return \b true if all work is done perfectly well
   */
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
      bb->setParentCfg(p);
      bb->setEnabled(true);

      // Create configs for nested projects if necessary
      if(pc.sync_subs) {
        BOOSTVEC_FOR(vsp_ci, it, projects) {
          vsp_p prj = static_cast<vsp_p>(*it);
          if(prj != 0) {
            if(static_cast<vsfs_p>(prj)->isLoaded()) {
              pc.sync_subs = false;
              if(!prj->createCfg(&bb->config(), pc)) {
                kddbg << VSPART_ERROR"Can't create config: " << pc.string() << " for project: "
                      << prj->getName() << ".\n";
                delete bb;
                return false;
              }
            }
            else { kddbg << g_err_prjload.arg(prj->getName()).arg("VSSolution::createCfg"); }
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
      kddbg << g_err_notenoughmem.arg(VSPART_BUILDBOX).arg("VSSolution::createCfg");
      return false;
    }
    return false;
  }

  /*! VSSolution::selectCfg
   * \brief Selects config from its own list of build-boxes based on KDevelop project's parent config
   *
   * receives pointer to parent config, searches the corresponding config in own list of build-boxes
   * <b>if bbox is found</b> for parent config, then switches it's config and also switches and updates
   * configs for all projects within solution.
   * <b>if bbox is not found</b> returns false
   * @param p pointer to parent configuration within KDevelop project
   * @return \b true upon success, \b false if something is seriously wrong
   */
  bool VSSolution::selectCfg(const vcfg_p p) {
    if(p != 0) {
      active_bb = getBB(p); // Get bb that is for this configuration

      if(active_bb != 0) {
#ifdef DEBUG
        kddbg << g_msg_configapply.arg(active_bb->config().toString()).arg(name).arg("VSSolution::selectCfg");
#endif

        // Update active configurations for all projects
        BOOSTVEC_FOR(vsp_ci, it, projects) {
          vsp_p prj(*it);
          if(prj != 0) {
            if(static_cast<vsfs_p>(prj)->isLoaded()) {
              if(!prj->selectCfg(&active_bb->config())) {
#ifdef DEBUG
                kddbg << VSPART_ERROR"Can't set config for project: \"" << prj->getName() << "\" in \""
                    << getName() << "\", in {VSSolution::selectCfg}\n";
#endif
              }
            }
#ifdef DEBUG
            else { kddbg << g_err_prjload.arg(prj->getName()).arg("VSSolution::selectCfg"); }
#endif
          }
          else {
            kddbg << g_err_list_corrupted.arg(VSPART_PROJECT).arg("VSSolution::selectCfg");
            return false;
          }
        }

        // Update UI in VSExplorer
        if(uisln != 0) {
          /*NOTE: TODO: This is temporary, untill I figure out how to design and implement a descent
            GUI items for VSExplorer */
          uisln->setState("normal");
        }
      }
      else {
#ifdef DEBUG
        /* kddbg << g_err_ent_notfound.arg(VSPART_BUILDBOX).arg(p->toString()).arg(QString(
                                        "VSSolution[%1]::selectCfg").arg(name)); */
#endif
      }
      return true;
    }
    else { kddbg << g_err_nullptr.arg("VSSolution::selectCfg"); return false; }
    return false;
  }

  void VSSolution::setActive(bool a) {
    if(a) { set_bit(enflg, VSEntity::IS_ACTIVE); }
    else { clear_bit(enflg, VSEntity::IS_ACTIVE); }
  }

  bool VSSolution::setActivePrj(vsp_p p) {
    if(p != 0) {
      if(active_prj != 0) {
        active_prj->setActive(false);
      }
      active_prj = p;
      active_prj->setActive();
      return true;
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

  /*inline*/ vsp_p VSSolution::getActivePrj() const { return active_prj; }

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

  /*inline*/ bool VSSolution::isDetached() const { return active_bb == 0; }
  /*inline*/ const pv_vsp_cr VSSolution::projs() const { return projects; }

  vsbb_p VSSolution::getBB(const QString &c) const {
    if(c != QString::null) {
      BOOSTVEC_FOR(vsbb_ci, it, bboxes) {
        if((*it) != 0) {
          if((*it)->config().toString() == c) { return (*it); }
        }
      }
      kddbg << g_err_ent_notfound.arg(VSPART_BUILDBOX).arg(c).arg(QString("VSSolution[%1]::getBB").arg(name));
      return 0;
    }
#ifdef DEBUG
    kddbg << VSPART_ERROR"Wrong param in {VSSolution[" << name << "]::getBB}.\n";
#endif
    return 0;
  }

  vsbb_p VSSolution::getBB(const vcfg_p parent_cfg) const {
    // Print all bboxes for this solution
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
          return 0;
        }
      }
      kddbg << g_err_ent_notfound.arg(VSPART_BUILDBOX).arg(parent_cfg->toString()).
          arg( "VSSolution[%1]::getBB").arg(name);
      return 0;
    }
    kddbg << g_err_nullptr.arg("VSSolution::getBB");
    return 0;
  }

  /*inline*/ vcfg_cp VSSolution::currentCfg() const {
    if(active_bb != 0) {
      return &active_bb->config();
    }
    else {
      return 0;
    }
  }

  void VSSolution::VSMetaDependency::syncToPrj(vsp_p prj) {
    BOOSTVEC_FOR(boost::container::vector<QUuid>::const_iterator, uidc, deps) {
      prj->addRequirement((*uidc));
    }
  }

  void VSSolution::VSMetaDependency::syncFromPrj(vsp_p /*prj*/) {
#ifdef USE_BOOST
    deps.clear();
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement later
#endif
    //TODO: Not sure if this method is needed at all
    // For now I'll leave it just cleaning the meta-deps tree
  }

  bool VSSolution::__read_parse_shdr(QTextIStream &s, QString &nm, QString &prm) {
    QChar c(0);
#ifdef DEBUG
    char latin1c(0);
#endif
    while(!s.atEnd()) {
      switch(c.latin1()) {
        case '(': {
          s >> c;
          do {
            nm.append(c);
            s >> c;
          } while(c.latin1() != ')');
          break; }
          case '=': {
            s >> prm;
            break; }
        default:
          s >> c;
#ifdef DEBUG
          latin1c = c.latin1();
#endif
          break;
      }
    }
    // kddbg << "| Section: " << nm << " set: " << prm << " |\n";
    return true;
  }

  bool VSSolution::__read_parse_uid(QTextIStream &s, QChar &ch, QUuid &uid) {
    if(ch.latin1() != '{') {
      kddbg << VSPART_ERROR"Can't get GUID, incorrect uid string format, expect {XXXXX...XXXX} format" << endl;
      return false;
    }
    if(!readGUID(s, uid)) {
      kddbg << g_err_guidparse.arg("{...}").arg("VSSolution::__read_parse_uid");
      return false;
    }
    s >> ch;
    return true;
  }

  //===========================================================================
  // Visual studio project methods
  //===========================================================================
  VSProject::VSProject(e_VSPrjLangType ltype, const QString &nm, const QUuid &uid)
  : VSEntity(vs_project)
  , VSRefcountable()
  , VSNameable(nm)
  , VSIndexable(uid)
  , VSFSStored()
  , lang(ltype)
  , sln(0)
  , uiprj(0)
  , active_bb(0)
  , version(vsprj_ver_unknown) {
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
        BOOSTVEC_PUSHBACK(filters, static_cast<vsf_p>(item));
        break; }
      case vs_file: {
        vsfl_p f = static_cast<vsfl_p>(item);
        BOOSTVEC_PUSHBACK(files, f);
        // f->setParent(static_cast<vse_p>(this)); //NOTE: acquires item
        break; }
      default: {
        kddbg << g_wrn_unsupportedtyp.arg(type2String(item->getType())).arg("VSProject::insert");
        return; }
    }
    item->setParent(static_cast<vse_p>(this));
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
      case vs_filter: // NOTE: We don't make filter our parent
      default: {
        kddbg << g_wrn_unsupportedtyp.arg(type2String(pnt->getType())).arg("VSProject::insert");
        break; }
    }
  }

  vse_p VSProject::getParent() const {
    return sln;
  }

  /** \fn VSProject::getByUID
   * \brief Gets file by it's UID
   * @param uid uid of file
   * @return ptr to file in project
   */
  /* vsfl_p VSProject::getByUID(const QUuid &uid) const {
    return 0;
  } */

  /*inline*/ uivsp_p VSProject::getUI() const { return uiprj; }

  bool VSProject::createUI(uivse_p p) {
    if(uiprj==0) {
      uiprj = part()->explorerWidget()->addProjectNode(p, this);
      if(uiprj==0) { kddbg << "failed to add prj UI node" << endl; return false; }
#ifdef DEBUG
      // kddbg << QString("UI for [%1]:[%2] created in [%3][%4]\n").arg(type2String(type)).arg(name).
      //     arg(type2String(p->getType())).arg(p->text(0));
#endif
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

  bool VSProject::dumpConfigLayout(QTextStream &/*s*/) {
    return false;
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

  vsf_p VSProject::getFilter(const QString &nm) const {
    BOOSTVEC_FOR(vsf_ci, it, filters) {
      if((*it) != 0) {
        if((*it)->getName() == nm) { return (*it); }
      }
      else {
        kddbg << g_err_list_corrupted.arg(VSPART_FILTER).arg(QString("in {VSProject[%1]::getFilter}.\n").arg(name));
        return 0;
      }
    }
#ifdef DEBUG
    kddbg << g_err_ent_notfound.arg(VSPART_FILTER).arg(nm).arg(QString("in {VSProject[%1]::getFilter}.\n").arg(name));
#endif
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

  /** \fn VSProject::populateUI
   * \brief This will populate/update UI items tree
   */
  bool VSProject::populateUI() {
    BOOSTVEC_FOR(vsf_ci, it, filters) {
      if(!(*it)->populateUI(static_cast<uivse_p>(uiprj))) { return false; }
    }

    BOOSTVEC_FOR(vsfl_ci, it, files) {
      if(!(*it)->createUI(static_cast<uivse_p>(uiprj))) { return false; }
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
    if(a) { set_bit(enflg, IS_ACTIVE); }
    else { clear_bit(enflg, IS_ACTIVE); }
  }

  /** \fn VSProject::createCfg(const vcfg_cp parent_config, const vcfgcr_r cr)
   * \brief Will create a new config using \a parent_config as it's parent.
   * - will not set new config as \a selected.
   * - will check internally so that configs wouldn't doublicate.
   * - if any config will have the same parent as the new one being created, it will be disabled for action and
   *   new one will be enabled instead.
   * @param parent_config configuration to use as \b parent
   * @param cr is a pointer to \a VSConfig::VSConfigCreate struct containing params for config creation
   * @return \b true if all work is done perfectly well
   */
  bool VSProject::createCfg(const vcfg_cp p, const vcfgcr_r cr) {
    // Check configs list for duplications and ambiguities
    BOOSTVEC_FOR(vsbb_ci, it, bboxes) {
      vsbb_p pbb = static_cast<vsbb_p>(*it);
      if(pbb != 0) {
        if(pbb->config() == cr) {
          kddbg << VSPART_ERROR"Config [" << pbb->config().toString() << "] already exists in: " << name << " prj.\n";
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
    return false;
  }

  bool VSProject::selectCfg(const vcfg_cp p) {
    if(p != 0) {
      vsbb_p bb = getBB(p);
      if(bb != 0) {
        active_bb = bb;
#ifdef DEBUG
        kddbg << g_msg_configapply.arg(active_bb->config().toString()).arg(name).arg("VSProject::selectCfg");
#endif
        // Update UI in VSExplorer
        if(uiprj != 0) {
          uiprj->slotRefreshText();
        }
      }
      else {
        //TODO: Here "error" message for "Problems" view should be generated
        kddbg << QString("No bbox for config [%1]\n").arg(p->toString());
      }
    }
    else { kddbg << g_err_nullptr.arg("VSProject::selectCfg"); return false; }
    return false;
  }

  bool VSProject::build() {
    /** NOTE: This method will make VSProject a pure virtual class.
     *
     * The build process:
     * - Solution is setting up a tree of projects based on dependencies between projects.
     * - The forms something like a list of build levels, going from level to level, solution will trigger a level-set
     *   of projects to be build.
     * - Project will go through it's files and based on priorities of tools within build-box project will process
     *     it's files.
     *     NOTE: That rings an idea that every tool should have a list of file extensions that are supported,
     *       to help project to form a sequence of build.
     *     Based on build-box and information about tool-executables project will form commands and execute them
     *     NOTE: That will be based on project's build-box settings, if they allow multi-threaded build.
     *     Project will take each file and will form a command taking info from build-box and other sources,
     *     then upon successful command generation it will be executed and report about that will be formed
     *     according to settings.
     *
     * Some stages for build command:
     * - Prepare
     * - Report
     * - Execute
     * - Report result
     */

      /// Will return list of tools that are arranged by their "order" in the build box
      // virtual const pv_VSTool& tools() const = 0;
      /// Prepares build|clear, etc command for a file
      // virtual bool prepare_cmd(vsfl_p file) = 0;
      /// Executes build|clear command for a file
      // virtual bool execute_cmd(vsfl_p file) = 0;
    return false;
  }

  /*inline*/ bool VSProject::isDetached() const {
    if(bboxes.size() > 0) {
      if(active_bb != 0) {
        return false;
      }
    }
    return true;
  }

  vsbb_p VSProject::getBB(const QString &c) const {
    if(c != QString::null) {
      BOOSTVEC_FOR(vsbb_ci, it, bboxes) {
        if((*it) != 0) {
          if((*it)->config().toString() == c) { return (*it); }
        }
      }
      kddbg << g_err_ent_notfound.arg(VSPART_BUILDBOX).arg(c).arg("VSProject[%1]::getBB").arg(name);
      return 0;
    }
#ifdef DEBUG
    kddbg << VSPART_ERROR"Wrong param in {VSProject::getBB}.\n";
#endif
    return 0;
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

  /*inline*/ vcfg_cp VSProject::currentCfg() const {
    if(active_bb != 0) {
      return &active_bb->config();
    }
    else {
      return 0;
    }
  }

  /** \fn VSProject::bbs()
   * \brief Get Build-Boxes for this project for non-modifying purposes
   * @return const ref to vector with Build-Boxes
   */
  /*inline*/ pv_vsbb_cr VSProject::bbs() const { return bboxes; }

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
      case vs_project: {
        BOOSTVEC_PUSHBACK(chld, item);

        //NOTE: We are not setting this filter as parent of
        // projects and files. Those already have a proper parent,
        // solution or project.
        break; }
      case vs_filter: {
        BOOSTVEC_PUSHBACK(chld, item);
        item->setParent(static_cast<vse_p>(this));
        break; }
      default: {
        kddbg << g_wrn_unsupportedtyp.arg(type2String(item->getType())).arg("VSFilter::insert");
      }
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

  /** \fn VSFilter::getByUID(const QUuid &uid)
   * \brief Retrieves project|filter contained in this filter
   * @p uid uid of filter|project that is being searched for
   * @return vse_p of filter|project
   */
  vse_p VSFilter::getByUID(const QUuid &uid) const {
    vse_ci it=chld.begin();
    BOOSTVEC_OFOR(it, chld) {
      vse_p ent(*it);
      if(ent != 0) {
        switch(ent->getType()) {
          case vs_project: {
            if(static_cast<vsp_p>(ent)->getUID() == uid) { break; }
            break; }
          case vs_filter: {
            if(static_cast<vsf_p>(ent)->getUID() == uid) { break; }
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
    else { kddbg << "Child " << guid2String(uid) << " not found.\n"; }
    return 0;
  }

  /*inline*/ uivsf_p VSFilter::getUI() const { return uiflt; }

  bool VSFilter::createUI(uivse_p pnt) {
    if(uiflt == 0) {
      uiflt = part()->explorerWidget()->addFilterNode(pnt, this);
      if(uiflt == 0) { kddbg << "failed to add filter UI node" << endl; return false; }
#ifdef DEBUG
      // kddbg << QString("UI for [%1]:[%2] created in [%3][%4]\n").arg(type2String(type)).arg(name).
      //     arg(type2String(pnt->getType())).arg(pnt->text(0));
#endif
    }
    return true;
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

  bool VSFilter::populateUI(uivse_p pnt) {
    if(pnt != 0) {
      if(!createUI(pnt)) { kddbg << VSPART_ERROR"Can't create UI item for flt \"" << name << "\"\n"; return false; }
      BOOSTVEC_FOR(vse_ci, it, chld) {
        if((*it) != 0) {
          switch((*it)->getType()) {
            case vs_project: {
              vsp_p prj=static_cast<vsp_p>(*it);
              if(!prj->createUI(static_cast<uivse_p>(uiflt))) { return false; }
              if(!prj->populateUI()) { return false; }
              break; }
            case vs_file: {
              vsfl_p fl=static_cast<vsfl_p>(*it);
              if(!fl->createUI(static_cast<uivse_p>(uiflt))) { return false; }
              break; }
            case vs_filter: {
              vsf_p flt = static_cast<vsf_p>(*it);
              if(!flt->populateUI(static_cast<uivse_p>(uiflt))) { return false; }
              break; }
            default: {
              break; }
          }
        }
        else {
          kddbg << g_err_list_corrupted.arg("vs_entity").arg("VSFilter::populateUI");
          return false;
        }
      }
    }
    return true;
  }

  //===========================================================================
  // Visual studio file methods
  //===========================================================================
  VSFile::VSFile(const QString &nm/*, const QUuid &uid*/, vsp_p p)
  : VSEntity(vs_file)
  , VSRefcountable()
  , VSNameable(nm)
//  , VSIndexable(uid)
  , VSFSStored()
  , project(p)
  , parent(0)
  , uifl(0)
  , active_bb(0) {
  }

  VSFile::~VSFile() {
    // if(uifl!=0) { delete uifl; uifl=0; }
  }

  void VSFile::setParent(vse_p pnt) {
    //TODO: Think about how should parent variable behave, since there
    // is setting projects and solutions into active states and many
    // other things that will require "parent" variable
    if(parent == 0) {
      switch(pnt->getType()) {
        case vs_project: {
          parent = pnt;
          /* NOTE: DO NOT UPDATE PROJECT VAR ! Since it is set in [ctr]
            project = static_cast<vsp_p>(parent);
          */
          acquire(pnt);

          /* Since setting parent should happen in only some cases that grant absolute paths for file,
              test FS file accessibility and modes.
            NOTE: This might be not very good place to it, but for now I can't imagine better place.
            TODO: In future search|create a better workflow for this
          */
          __try_reach();
          break; }
        case vs_filter: {
          parent = pnt;
          break; }
        default: {
          kddbg << VSPART_ERROR"Can't set \"" << type2String(pnt->getType()) << "\" as a parent of \""
              << type2String(getType()) << "\"\n";
        }
      }
    }
  }

  /*inline*/ vse_p VSFile::getParent() const { return parent; }

  bool VSFile::write(bool sync/*=true*/) {
    if(sync) { set_bit(fsflg, IS_IN_SYNC); } else { clear_bit(fsflg, IS_IN_SYNC); }
    return true;
  }

  bool VSFile::write(QTextStream &/*s*/, bool sync/*=true*/) {
    if(sync) { set_bit(fsflg, IS_IN_SYNC); } else { clear_bit(fsflg, IS_IN_SYNC); }
    return true;
  }

  bool VSFile::read(bool sync/*=true*/) {
    if(!dom.isNull()) {
      //QString fl_name("<not loaded>");
      QString fl_path_r = dom.attribute("RelativePath");
      NormalizeSlashes(fl_path_r);
      setRelPath(fl_path_r);

      // Retrieve name from relative path
      setName(path_rlt.mid(path_rlt.findRev(g_slash)+1));
#ifdef DEBUG
      // kddbg << "Filename condidate: " << name << endl;
#endif

      // Retrieve absolute path
      setAbsPath(RebasePath_Win(project->getAbsPath(), path_rlt));  //NOTE: This tests inside if file is reachable
#ifdef DEBUG
      // kddbg << "PATH: " << path_abs << endl;
#endif

      //NOTE: This can be the place where the problem with case sensitive names is detected upon loading
      if(!isReachable()) {
        kddbg << QString("File [%1] is not reachable.\n").arg(name);
        return true;
      }

      // Check for file configurations
      //TODO: Make something like initBBFromDom()
      QDomElement it_cfg_e = dom.firstChild().toElement();
      while(!it_cfg_e.isNull()) {
        kddbg << QString("config [%2] for [%1].\n").arg(name).arg(it_cfg_e.attribute("Name"));
        it_cfg_e = it_cfg_e.nextSibling().toElement();
      }

      set_bit(fsflg, IS_LOADED_OK);
      if(sync) { set_bit(fsflg, IS_IN_SYNC); } else { clear_bit(fsflg, IS_IN_SYNC); }
      return true;
    }
    else {
      kddbg << QString(VSPART_ERROR"Can't read file [%1], dom is 0.\n").arg(name);
      return false;
    }
  }

  bool VSFile::read(QTextStream &/*s*/, bool /*sync=true*/) {
    /* if(FALSE == dom.setContent(s.read())) {
      kddbg << VSPART_ERROR"Can't set content for DOM element in file: " << name << endl;
      return false;
    }

    if(!dom.isNull()) { return read(sync); }
    else {
      kddbg << QString(VSPART_ERROR"Can't read file [%1], dom is 0.\n").arg(name);
      return false;
    } */

    return false;
  }

  bool VSFile::read(QDomElement el, bool sync/*=true*/) {
    if(!el.isNull()) {
      dom = el; //NOTE: copy DOM element for modification|storage|saving
      return read(sync);
    }
    else {
      kddbg << QString(VSPART_ERROR"Can't read file [%1], dom is 0.\n").arg(name);
      return false;
    }
  }

  /*inline*/ void VSFile::setDom(QDomElement el) { dom = el; }
  /*inline*/ vsp_p VSFile::getProject() const { return project; }

  /** \fn VSFile::getByUID(const QUuid &uid)
   * \brief Retrieves parent project of this file by it's UID
   * @p uid QUuid of parent project that is being looked for
   * @return vsp_p to parent project, or 0 no parent found
   */
  vsp_p VSFile::getByUID(const QUuid &uid) const {
    vse_ci it=pnts.begin();
    BOOSTVEC_OFOR(it, pnts) {
      vse_p ent(*it);
      if(ent != 0) {
        if(ent->getType() == vs_project) {
          if(static_cast<vsp_p>(ent)->getUID() == uid) { break; }
        }
      } else { kddbg << g_err_list_corrupted.arg("VSEntity").arg("VSFile::getByUID"); return 0; }
    }
#ifdef USE_BOOST
    if(it!=pnts.end()) { return static_cast<vsp_p>(*it); }
#else
#error "VStudio: Boost support is not enabled" //TODO: Implement this
#endif
    else {
      kddbg << QString("Can't find project [%1] in {%2}.\n").arg(guid2String(uid)).
          arg(QString("VSFile[%1]::getByUID").arg(name));
    }
    return 0;
  }

  /*inline*/ uivsfl_p VSFile::getUI() const { return uifl; }

  bool VSFile::createUI(uivse_p pnt) {
    if(uifl==0) {
      uifl = part()->explorerWidget()->addFileNode(pnt, this);
      if(uifl==0) { kddbg << "failed to add file UI node" << endl; return false; }
#ifdef DEBUG
      // kddbg << QString("UI for [%1]:[%2] created in [%3][%4]\n").arg(type2String(type)).arg(name).
      //    arg(type2String(pnt->getType())).arg(pnt->text(0));
#endif
    }
    return true;
  }
  //END // Basic entity types

  //BEGIN // Inherited entity types

  //===========================================================================
  // VStudio C/C++ project methods
  //===========================================================================
  VSProject_c::VSProject_c(const QString &nm, const QUuid &uid)
  : VSProject(vs_prjlang_c, nm, uid) {
  }

  VSProject_c::~VSProject_c() {
  }

  bool VSProject_c::write(bool sync/*=true*/) {
    if(sync) { set_bit(fsflg, IS_IN_SYNC); } else { clear_bit(fsflg, IS_IN_SYNC); }
    return true;
  }

  bool VSProject_c::write(QTextStream &/*stream*/, bool sync/*=true*/) {
    if(sync) { set_bit(fsflg, IS_IN_SYNC); } else { clear_bit(fsflg, IS_IN_SYNC); }
    return true;
  }

  bool VSProject_c::read(bool /*sync=true*/) {
    //Check paths:
    if(path_rlt.isEmpty()) { kddbg << g_err_emptypath.arg("Relative").arg("VSProject::read"); return false; }
    if(path_abs.isEmpty()) { kddbg << g_err_emptypath.arg("Absolute").arg("VSProject::read"); return false; }

    QFile fl;

    if(!fl.exists(path_abs)) { kddbg << VSPART_ERROR"File \"" << path_abs << "\" not found.\n"; return false; }
    fl.setName(path_abs);
    if(!fl.open(IO_ReadWrite|IO_Raw)) { kddbg << "can't open project file" << endl; return false; }
    if(!fl.isReadable()) { kddbg << g_err_fileread.arg(path_abs).arg("VSProject::read"); return false; }
    if(!fl.isWritable()) { kddbg << g_err_filewrite.arg(path_abs).arg("VSProject::read"); return false; }
    if(!fl.isReadWrite()) { kddbg << "can't read and write" << endl; return false; }

    QTextStream stream(&fl);

    if(!read(stream, true)) {
      kddbg << VSPART_ERROR"Failed to parse prj: " << name << " file \"" << path_abs << "\"\n";
      return false;
    }

    fl.close();
    return true;
  }

  bool VSProject_c::read(QTextStream &stream, bool sync/*=true*/) {
    if(FALSE == doc.setContent(stream.read())) {
      kddbg << VSPART_ERROR"Can't set content for DOM element in project: " << name << endl;
      return false;
    }

    // Acquire all necessary elements
    QDomElement prj_e = doc.documentElement();
    QDomElement cfg_e = prj_e.namedItem(VSPRJ_DOM_CONFIGS).toElement();
    QDomElement fil_e = prj_e.namedItem(VSPRJ_DOM_FILES).toElement();

    // Check all elements
    if(prj_e.isNull()) {
      kddbg << g_err_domelemnotpresent.arg("Project root").arg(QString("in {VSProject[%1]::read}.\n").arg(name));
      return false;
    }
    if(cfg_e.isNull()) {
      kddbg << g_err_domelemnotpresent.arg(VSPRJ_DOM_CONFIGS).arg(QString("in {VSProject[%1]::read}.\n").arg(name));
      return false;
    }
    if(fil_e.isNull()) {
      kddbg << g_err_domelemnotpresent.arg(VSPRJ_DOM_FILES).arg(QString("in {VSProject[%1]::read}.\n").arg(name));
      return false;
    }

    //BEGIN: Read project configurations
#ifdef DEBUG
    kddbg << "VSPROJ: [" << name << "] Reading configs.\n";
    // QDomElement q = doc.documentElement();
    // QDomNode n = q.firstChild();
    // while(!n.isNull()) {
    //   QDomElement e = n.toElement();
    //   kddbg << "Tag: " << e.tagName() << endl;
    //   n = n.nextSibling();
    // }
#endif
    // DomUtil::elementByPath(doc, VSPART_DOM_PROJECT"/"VSPART_DOM_PROJECT_CONFIGS);
    QDomElement it_e = cfg_e.firstChild().toElement();
    if(it_e.isNull()) {
      kddbg << g_err_domelemnotpresent.arg("it_e:configuration").arg(QString("in {VSProject[%1]::read}.\n").arg(name));
      return false;
    }

    while(!it_e.isNull()) {
#ifdef DEBUG
      kddbg << "Parsing: " << it_e.tagName() << QString(" in {VSProject[%1]::read}.\n").arg(name);
#endif
      if(it_e.tagName() == "Configuration") {
        QString cfg_name = it_e.attribute("Name");
        QString outdir = it_e.attribute("OutputDirectory");

        VSConfigCreate cr;
        cr.name = cfg_name.left(cfg_name.find('|'));
        cr.platform = string2Platform(cfg_name.mid(cfg_name.find('|')+1));

        if(!createCfg(0, cr)) {
          kddbg << VSPART_ERROR"Can't create config: " << name << " for: " << name << endl;
          return false;
        }
#ifdef DEBUG
        else { kddbg << g_msg_configapply.arg(cfg_name).arg("VSProject[%1]::read").arg(name); }
#endif
      }
      else {
        kddbg << VSPART_WARNING"Wrong tag: " << it_e.tagName() << endl;
      }
      it_e = it_e.nextSibling().toElement();
    }
    //END: Read project configurations

    /* NOTE: upon reaching this point project is considered as configured
    To make sure that configurations will be parented and upon saving there will be no crash because of that. */
    if(bboxes.size() > 0) { set_bit(enflg, IS_CONFIGURED); }

    //BEGIN: Read project files
    /* it_e = fil_e.firstChild().toElement();
    if(it_e.isNull()) {
    kddbg << g_err_domelemnotpresent.arg("it_e = Files").arg(QString(" in {VSProject[%1]::read}.\n").arg(name));
    return false;
  } */

    if(!__read_unit(fil_e, static_cast<vse_p>(this))) {
      kddbg << QString("Failed to read Files in {%1}").arg(QString(" in {VSProject[%1]::read}.\n").arg(name));
      return false;
    }

    /* while(!it_e.isNull()) {
      // "Unfiltered" file
    if(it_e.tagName() == "File") {
#ifdef DEBUG
    kddbg << "Parsing: " << QString("[File]:[%1]").arg(it_e.attribute("RelativePath"))
    << QString(" in {VSProject[%1]::read}.\n").arg(name);
#endif
    vsfl_p fl = new vsfl("unknown", this);
    if(fl != 0) {
    if(fl->read(it_e, true)) {
    insert(fl); //NOTE: Insert into project, so that file->prj variable was initialized
#ifdef DEBUG
    kddbg << QString("File [%1] inserted into project [%2].\n").arg(fl->getName()).arg(name);
#endif
  }
    else { kddbg << QString(VSPART_ERROR"Failed to read file \"%1\".\n").arg(it_e.attribute("RelativePath")); }
  }
    else {
    kddbg << g_err_notenoughmem.arg(QString(VSPART_FILE)).arg(QString("in {VSProject[%1]::read}").arg(name));
    return false;
  }
  }
      // "Filtered" file
    else if(it_e.tagName() == "Filter") {
    if(!__read_filter(it_e)) {
    kddbg << VSPART_ERROR"Failed to read filter [ " << it_e.attribute("Name") << " ] contents.\n";
    return false;
  }
  }
    it_e = it_e.nextSibling().toElement();
  } */
    //END: Read project files

    set_bit(enflg, IS_LOADED_OK);
    if(sync) { set_bit(fsflg, IS_IN_SYNC); } else { clear_bit(fsflg, IS_IN_SYNC); }
    return true;
  }

  bool VSProject_c::__read_unit(QDomElement el, vse_p pnt) {
    if(pnt != 0) {
      if(!el.isNull()) {
        // Read unit's children
        QDomElement chld = el.firstChild().toElement();
        while(!chld.isNull()) {
          if(chld.tagName().compare("File") == 0) {
#ifdef DEBUG
            QString report;
            report.append(QString("project [%1] file \"%2\" <found>").arg(name).arg(chld.attribute("RelativePath")));
#endif
            vsfl_p fl = new vsfl("this file is not yet loaded", this);
            if(fl != 0) {
              if(fl->read(chld, true)) {
#ifdef DEBUG
                report.append(" <read>");
#endif
              }
              else { kddbg << QString(VSPART_ERROR"Can't read file \"%1\".\n").arg(chld.attribute("RelativePath")); }

              // Insert file into project, even if it is not loaded or reachable
              switch(pnt->getType()) {
                case vs_project: {
                  insert(fl);
                  break; }
                  case vs_filter: {
                    insert(fl);
                    static_cast<vsf_p>(pnt)->insert(fl); // Add file to filter
                    break; }
                    default: { break; }
              }
#ifdef DEBUG
              report.append(" <inserted>\n");
              kddbg << report;
#endif
            }
            else {
              kddbg << g_err_notenoughmem.arg(QString(VSPART_FILE)).arg(QString("in {VSProject[%1]::__read_file}").
                  arg(name));
              return false;
            }
          }
          else if(chld.tagName().compare("Filter") == 0) {
#ifdef DEBUG
            switch(pnt->getType()) {
              case vs_project: {
                kddbg << QString(">>> P:[%1] filter [%2]\n").
                    arg(static_cast<vsp_p>(pnt)->getName()).
                    arg(chld.attribute("Name"));
                break; }
                case vs_filter: {
                  kddbg << QString(">>> F:[%1] filter [%2]\n").
                      arg(static_cast<vsf_p>(pnt)->getName()).
                      arg(chld.attribute("Name"));
                  break; }
                  default: {
                    kddbg << ">>> unknown parent for filter\n";
                    break; }
            }
#endif
            // Create VS filter
            QUuid flt_uid;
            if(readGUID(chld.attribute("UniqueIdentifier"), flt_uid)) {
              vsf_p filter = new vsf(chld.attribute("Name"), flt_uid);

              if(filter != 0) {
                pnt->insert(filter);
                // Read filter items
                if(!__read_unit(chld, static_cast<vse_p>(filter))) {
                  kddbg << VSPART_ERROR"Failed to read filter [ " << chld.attribute("Name") << " ] contents.\n";
                }
                //TODO: make filters to store and work with filtered file extensions
#ifdef DEBUG
                kddbg << QString("<<< project [%1] filter [%2] <inserted>\n").arg(name).arg(filter->getName());
#endif
              }
              else {
                kddbg << g_err_notenoughmem.arg(QString("["VSPART_FILTER"]:[%1]").arg(chld.attribute("Name"))).
                    arg(QString("in {VSProject[%1]::__read_unit}").arg(name));
                return false;
              }
            }
            else {
              kddbg << g_err_guidparse.arg(chld.attribute("UniqueIdentifier")).
                  arg(QString("in {VSProject[%1]::__read_unit}").arg(name));
            }
          }
          chld = chld.nextSibling().toElement();
        }
        return true;
      }
    }
    return false;
  }

  //===========================================================================
  // VStudio C# project methods
  //===========================================================================
  VSProject_cs::VSProject_cs(const QString &nm, const QUuid &uid)
  : VSProject(vs_prjlang_cs, nm, uid) {
  }

  VSProject_cs::~VSProject_cs() {
  }

  bool VSProject_cs::write(bool sync/*=true*/) {
    return false;
  }

  bool VSProject_cs::write(QTextStream &/*s*/, bool sync/*=true*/) {
    return false;
  }

  bool VSProject_cs::read(bool sync/*=true*/) {
    return false;
  }

  bool VSProject_cs::read(QTextStream &/*s*/, bool sync/*=true*/) {
    return false;
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
  VSToolCompiler::VSToolCompiler()
  : VSTool(vstl_compiler) {
  }

  VSToolCompiler::~VSToolCompiler() {
  }

  void VSToolCompiler::setParent(vse_p /*parent*/) {
  }

  vse_p VSToolCompiler::getParent() const {
    return 0;
  }

  //===========================================================================
  // Visual studio "MSVC linker" build tool methods
  //===========================================================================
  VSToolLinker::VSToolLinker()
  : VSTool(vstl_linker) {
  }

  VSToolLinker::~VSToolLinker() {
  }

  void VSToolLinker::setParent(vse_p /*parent*/) {
  }

  vse_p VSToolLinker::getParent() const {
    return 0;
  }

  //===========================================================================
  // Visual studio "MSVC linker" build tool methods
  //===========================================================================
  VSToolMidl::VSToolMidl()
  : VSTool(vstl_midl) {
  }

  VSToolMidl::~VSToolMidl() {
  }

  void VSToolMidl::setParent(vse_p /*parent*/) {
  }

  vse_p VSToolMidl::getParent() const {
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
      case vspl_win32: { return static_cast<vspl_cp>(&g_Win32Platform); }
      case vspl_win64: { return static_cast<vspl_cp>(&g_Win64Platform); }
      case vspl_mixed: { return static_cast<vspl_cp>(&g_MixedPlatform); }
      case vspl_anycpu: { return static_cast<vspl_cp>(&g_AnyCPUPlatform); }
      default: {
        kddbg << "Error!!! Unsupported platform \"" << platform2String(p) << "\" is requested, aborting\n";
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

  void VSPlWin64::setParent(vse_p /*parent*/) {
  }

  vse_p VSPlWin64::getParent() const {
    return 0;
  }

  //===========================================================================
  // VS Platform "Mixed Platforms" methods
  //===========================================================================
  VSPlMixed::VSPlMixed()
  :VSPlatform(vspl_mixed) {
  }

  VSPlMixed::~VSPlMixed() {
  }

  void VSPlMixed::setParent(vse_p /*parent*/) {
  }

  vse_p VSPlMixed::getParent() const {
    return 0;
  }

  //===========================================================================
  // VS Platform "Any CPU" methods
  //===========================================================================
  VSPlAnyCPU::VSPlAnyCPU()
  : VSPlatform(vspl_anycpu) {
  }

  VSPlAnyCPU::~VSPlAnyCPU() {
  }

  void VSPlAnyCPU::setParent(vse_p /*parent*/) {
  }

  vse_p VSPlAnyCPU::getParent() const {
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

  QString VSConfig::toString() const {
    return QString("%1|%2").arg(name).arg(platform2String(vspl.platform()));
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

  void VSBuildBox::setParent(const vse_p /*p*/) {
    kddbg << VSPART_ERROR"method {VSBuildBox::setParent} is undefined, and should not be used.\n";
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

  /*inline*/ vcfg_cp VSBuildBox::parentConfig() const {
    return pcfg;
  }

  void VSBuildBox::setParentCfg(const vcfg_cp c) {
    pcfg = c;
    if(pcfg != 0) {
#ifdef DEBUG
      kddbg << "Config: " << pcfg->toString() << " is parent to: " << cfg.toString() << endl;
#endif
    }
    else {
#ifdef DEBUG
      kddbg << VSPART_WARNING"parent config is 0, in {VSBuildBox::setParentCfg}.\n";
#endif
      enabled = false;
    }
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
          sln = static_cast<vss_p>(static_cast<vsfl_p>(e)->getProject()->getParent());
          break; }
        default: {
          kddbg << g_err_unsupportedtyp.arg(type2String(e->getType()));
          break; }
      }
    }
    return sln;
  }
};
