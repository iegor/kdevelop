/*  C/C++ Implementation: vs_model
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

/* Qt */
#include <qfile.h>
#include <qdom.h>
#include <qguardedptr.h>

/* KDE */
#include <kurl.h>

/* VStudio */
#include "vs_common.h"

class QFile;

//BEGIN //VStudio namespace
namespace VStudio {
  //===========================================================================
  //BEGIN VS basic entities
  class VSRefcountable {
    public:
      VSRefcountable();
      virtual ~VSRefcountable();

      bool acquire(const vse_p parent);
      vsr_r release(const vse_p parent);
      vsinline uint refs() const vsinline_attrib { return rfc; }
      vsinline bool isfree() const vsinline_attrib { return static_cast<bool>(rfc==0); }
      vsinline const pv_vse_cr parents() const vsinline_attrib { return pnts; }

    protected:
      //TODO: ATTENTION !
      // This must be done via boost's or Qt's guarded ptr.
      uint rfc;
#ifdef USE_BOOST
      pv_vse pnts; // Parents
#else
#endif
  };

  class VSNameable {
    public:
      VSNameable(const QString &name);
      virtual ~VSNameable();

      virtual const QString &getName() const;
      virtual void setName(const QString &name);

    protected:
      QString name;
  };

  class VSIndexable {
    public:
      VSIndexable(const QUuid& uid);
      virtual ~VSIndexable();

      vsinline const QUuid& getUID() const vsinline_attrib { return uid; }
      vsinline void setUID(const QUuid& u) vsinline_attrib { uid=u; }

    protected:
      QUuid uid;
  };

  class VSFSStored {
    public:
      enum e_state {
        IS_REACHABLE = 0x00000001, /*! File can be reached for read|write */
        IS_IN_SYNC = 0x00000002, /*! File is syncronized with its fs representation */
        IS_LOADED_OK = 0x00000003, /*! File is found and parsed successfully */
      };
    public:
      VSFSStored();
      virtual ~VSFSStored();

      //KURL& getURL() { return url; }
      vsinline const KURL& getURL() const vsinline_attrib { return url; }
      //KURL& getURL();

      /** \fn VSFSStored::setPath(const QString& abp, bool ttr)
       * \brief Sets the path (or part of it) for entity
       * @param abp path or URL to the fs resource
       * @param ttr <b>try to reach</b> flag, if \a true then fresh path is tested for
       * accessibility.
       */
      vsinline void setPath(const QString& abs_path, bool tryToReach=true) {
        url = KURL::fromPathOrURL(abs_path);
        if(tryToReach) { __try_reach(); }
      }

      QString getExt(bool ignore_dot=false) const;
      void setExt(const QString &extension);

      vsinline bool isInSync() const vsinline_attrib { return check_bit(fsflg, IS_IN_SYNC); }
      vsinline bool isReachable() const vsinline_attrib { return check_bit(fsflg, IS_REACHABLE); }

      /** \fn VSFSStored::isLoaded()
       * \brief Checks FS based VSEntity for positive load status
       * <b>e.g.</b>
       * VSSolution - Tells if solution loading procedure succeded
       * VSProject - Tells if project loading and parsing procedure succeded
       * @return \b true if file was found and parsed successfully upon read
       */
      vsinline bool isLoaded() const vsinline_attrib { return check_bit(fsflg, IS_LOADED_OK); }

      virtual bool write(bool synchronize=true) = 0;
      virtual bool write(QTextStream &stream, bool synchronize=true) = 0;
      virtual bool read(bool synchronize=true) = 0;
      virtual bool read(QTextStream &stream, bool synchronize=true) = 0;

    protected:
      vsinline void __try_reach() vsinline_attrib {
#ifdef DEBUG
    // if(url.isValid()) { kddbg << QString("URL: \"%1\" is valid.\n").arg(url.url()); }
    // if(url.isLocalFile()) { kddbg << QString("URL: \"%1\" is local.\n").arg(url.url()); }
#endif
        if(url.isValid() && url.isLocalFile()) {
          QFile fl;
          if(fl.exists(url.pathOrURL())) { set_bit(fsflg, IS_REACHABLE); return; }
        }
        clear_bit(fsflg, IS_REACHABLE);
      }

      /** \fn VSFSStored::__synced()
       * \brief Sets bitmask so that entity is considered to be in syncronized state
       * @p sync whether entity should be considered <a>syncronized/unsynchronized</a>
       */
      vsinline void __synced(bool sync=true) vsinline_attrib {
        if(sync) { set_bit(fsflg, IS_IN_SYNC); }
        else { clear_bit(fsflg, IS_IN_SYNC); }
      }

      /** \fn VSFSStored::__loaded()
       * \brief Sets bitmask so that entity is considered to be in loaded_ok state
       * @p load whether entity should be considered <a>loaded_ok or not</a>
       */
      vsinline void __loaded(bool load=true) vsinline_attrib {
        if(load) { set_bit(fsflg, IS_LOADED_OK); }
        else { clear_bit(fsflg, IS_LOADED_OK); }
      }

    protected:
      KURL url;
      uint fsflg;
  };

  class VSEntity {
    public:
      enum e_state {
        IS_ACTIVE = 0x00000001,
        IS_CONFIGURED = 0x00000002,
      };
    public:
      VSEntity(e_VSEntityType typ);
      virtual ~VSEntity();

      vsinline e_VSEntityType getType() const vsinline_attrib { return type; }
      static void setPart(VSPart* part);
      vsinline VSPart* part() const vsinline_attrib { return sys_part; }
      vsinline bool isConfigured() const vsinline_attrib { return check_bit(enflg, IS_CONFIGURED); }
      vsinline bool isActive() const vsinline_attrib { return check_bit(enflg, IS_ACTIVE); }

      virtual void insert(vse_p item);

      virtual void setParent(vse_p parent) = 0;
      virtual vse_p getParent() const = 0;

    protected:
      e_VSEntityType type;
      //uivse_p uient;  // UI entity
      uint enflg;

    private:
      static VSPart *sys_part; // System part
  };

  class VSSolution : public VSEntity,
  public VSNameable,
  public VSFSStored {
    public:
      predeclare_vs_typ(VSMetaDependency, vsmd);
      class VSMetaDependency {
        public:
          VSMetaDependency(const QUuid &uuid) {
            uid = uuid;
          }
          ~VSMetaDependency() {
          }

          void addDependency(const QUuid &uuid) {
#ifdef USE_BOOST
            deps.push_back(uuid);
#else
#error "VStudio: Boost support is no enabled"
#endif
          }

          void syncToPrj(vsp_p prj);
          void syncFromPrj(vsp_p prj);

        public:
          QUuid uid;
#ifdef USE_BOOST
          boost::container::vector<QUuid> deps;
#else
          //TODO: Implement this
#error "VStudio: Boost support is no enabled"
#endif
      };

    public:
      VSSolution(const QString &name);
      virtual ~VSSolution();

    // VS Entity interface methods:
      virtual void insert(vse_p item);
      virtual void setParent(vse_p parent);
      virtual vse_p getParent() const;

    // VS FSStored interface methods:
      virtual bool write(bool synchronize=true);
      virtual bool write(QTextStream &stream, bool synchronize=true);
      virtual bool read(bool synchronize=true);
      virtual bool read(QTextStream &stream, bool synchronize=true);

    // VS Solution methods:
      vsp_p getByUID(const QUuid &uid) const;
      vsinline uivss_p getUI() const vsinline_attrib { return uisln; }
      void setUI(uivss_p uisln);
      vsf_p getFltByUID(const QUuid &uid) const;
      vsmd_p metaDependency(const QUuid &uid);
      bool updateDependencies();
      bool createCfg(const vcfg_p parent_config, const vcfgcr_r pc);
      bool selectCfg(const vcfg_p parent_config);
      vsbb_p getBB(const QString &config) const;
      vsbb_p getBB(vcfg_cp parent_config) const;
      vcfg_cp currentCfg() const;

      void setActive(bool active=true);
      bool setActivePrj(vsp_p project);
      bool setActivePrj(const QString &int_name);
      vsinline vsp_p getActivePrj() const vsinline_attrib { return active_prj; }
      vsp_p getProject(const QString &int_name) const;
      vsinline bool isDetached() const vsinline_attrib { return active_bb == 0; }
      vsinline const pv_vsp_cr projs() const vsinline_attrib { return projects; }
      vsinline const pv_vsf_cr filts() const vsinline_attrib { return filters; }

      /** \fn VSSolution::bbs()
       * \brief Get Build-Boxes for this solution for non-modifying purposes
       * @return const ref to vector with Build-Boxes
       */
      vsinline pv_vsbb_cr bbs() const vsinline_attrib { return bboxes; }

    private:
      bool __read_parse_shdr(QTextIStream &stream, QString &name, QString &param);
      bool __read_parse_uid(QTextIStream &stream, QChar &control_chr, QUuid &uid);

    private:
      uivss_p uisln;  // UI representation TODO: remove in favor of VSEntity::uient
      vsbb_p active_bb; // Active buildbox selected for this solution
      vsp_p active_prj; // Active project for this solution
      pv_vsp projects;
      pv_vsf filters;
      pv_vsbb bboxes;  // build boxes, used to buil|clean, etc entire solution

      /*! Meta-dependencies tree
       * Used to contain raw dependencies for projects
       * Not filtered and allowed to have a cyclic dependencies
       */
      pv_vsmd mdeps;
      e_VSSlnVersion version;
      int fmt_version; // Format version (something VS internal for .sln)
  };

  class VSProject : public VSEntity,
  public VSRefcountable,
  public VSNameable,
  public VSIndexable,
  public VSFSStored {
    public:
      VSProject(e_VSPrjLangType ltype, const QString &name, const QUuid &uid);
      virtual ~VSProject();

    // VS Entity methods:
      virtual void insert(vse_p item);
      virtual void setParent(vse_p parent);
      virtual vse_p getParent() const { return sln; }

    // VS FSStored interface methods:
      virtual bool write(bool synchronize=true) = 0;
      virtual bool write(QTextStream &stream, bool synchronize=true) = 0;
      virtual bool read(bool synchronize=true) = 0;
      virtual bool read(QTextStream &stream, bool synchronize=true) = 0;

    // VS Project methods:
      vsinline uivsp_p getUI() const vsinline_attrib { return uiprj; }
      void setUI(uivsp_p uiprj);

      bool dumpLayout(QTextStream &stream);

      vsp_p getReqByUID(const QUuid &uid) const;
      vsp_p getDepByUID(const QUuid &uid) const;
      vsf_p getFltByUID(const QUuid &uid) const;
      vsf_p getFilter(const QString &name) const;

      bool addDependency(vsp_p dep);
      bool addDependency(const QUuid &uid);
      bool addRequirement(vsp_p req);
      bool addRequirement(const QUuid &uid);

      bool populateUI(uivse_p parent_ui);

      void setLanguage(e_VSPrjLangType lang);
      vsinline e_VSPrjLangType getLang() const vsinline_attrib { return lang; }

      void setActive(bool active=true);
      vsinline bool isActive() const vsinline_attrib { return check_bit(enflg, IS_ACTIVE); }

      bool createCfg(const vcfg_cp parent_config, const vcfgcr_r cr);
      bool selectCfg(const vcfg_cp parent_config);
      vsinline vcfg_cp currentCfg() const vsinline_attrib;
      vsbb_p getBB(const QString &config) const;
      vsbb_p getBB(const vcfg_cp parent_config) const;

      /** \fn VSProject::bbs()
       * \brief Get Build-Boxes for this project for non-modifying purposes
       * @return const ref to vector with Build-Boxes
       */
      vsinline pv_vsbb_cr bbs() const vsinline_attrib { return bboxes; }
      /** \fn VSProject::filts()
       * \brief Get constant reference to filters list of this project
       * @return constant reference to filters list of this project
       */
      vsinline pv_vsf_cr filts() const vsinline_attrib { return filters; }
      /** \fn VSProject::fls()
       * \brief Get constant reference to files list of this project
       * @return constant reference to list of files in this project
       */
      vsinline pv_vsfl_cr fls() const vsinline_attrib { return files; }

      bool build();

      vsinline bool isDetached() const vsinline_attrib {
        if(bboxes.size() > 0) {
          if(active_bb != 0) { return false; }
        }
        return true;
      }

      vsinline const KURL& getOutDir() const vsinline_attrib { return outdir; }
      void setOutDir(const KURL &outdir);
      vsinline const KURL& getIntDir() const vsinline_attrib { return intdir; }
      void setIntDir(const KURL &intdir);

    protected:
      e_VSPrjLangType lang; // Project choosen language
      vss_p sln;  // Parent solution
      uivsp_p uiprj; // UI representation
      vsbb_p active_bb; // Active buildbox
      pv_vsp deps; // Projects dependant on this one
      pv_vsp reqs; // Projects required to build this one, i.e. dependencies
      pv_vsf filters;
      pv_vsfl files;
      pv_vsbb bboxes;
      e_VSPrjVersion version;
      QDomDocument doc;
      KURL outdir;
      KURL intdir;
  };

  class VSFilter : public VSEntity,
  public VSNameable,
  public VSIndexable {
    public:
      VSFilter(const QString &name, const QUuid &uid);
      virtual ~VSFilter();

    // VS Entity interface methods:
      virtual void insert(vse_p item);
      virtual void setParent(vse_p parent); //NOTE: Inserts this filter into parent's filters
      virtual vse_p getParent() const;

    // VS Filter methods:
      vse_p getByUID(const QUuid &uid) const;

      vsinline uivsf_p getUI() const vsinline_attrib { return uiflt; }
      void setUI(uivsf_p uiflt);

      bool dumpLayout(QTextStream &layout);

      e_VSEntityType getParentType() const { return parent->getType(); }
      bool getParentUID(QUuid* uid) const;

      /** \fn VSFilter::children()
       * \brief Get constant reference to children list of this filter
       * @return constant reference to children list of this filter
       */
      vsinline pv_vse_cr children() const vsinline_attrib { return chld; }

    private:
      vse_p parent; // Parent solution|filter|project
      uivsf_p uiflt; // UI representation
      pv_vse chld; // Children
  };

  /** \class VStudio::VSFile
   * \brief VS file model representation
   * Used to hold almost all (if not all) logic for VS file.
   * NOTE:
   * Since file is a refcountable entity, list of parent projects
   * used to store every parent project that uses current file
   * to detect usage of sources, to maybe then optimize the
   * archtecture of projects is in VSRefcountable::pnts
  */
  class VSFile : public VSEntity,
  public VSRefcountable,
  public VSNameable,
  //public VSIndexable,
  public VSFSStored {
    public:
      VSFile(const QString &name/*, const QUuid &uid*/, vsp_p project);
      virtual ~VSFile();

    // VS Entity interface methods:
      virtual void setParent(vse_p parent_prj);
      virtual vse_p getParent() const;

    // VS FSStored interface methods:
      virtual bool write(bool synchronize=true);
      virtual bool write(QTextStream &stream, bool synchronize=true);
      virtual bool read(bool synchronize=true);
      virtual bool read(QTextStream &stream, bool synchronize=true);

    // VS File methods:
      vsp_p getByUID(const QUuid &uid) const;
      vsinline uivsfl_p getUI() const vsinline_attrib { return uifl; }
      void setUI(uivsfl_p uifl);
      bool read(QDomElement dom, bool synchronize=true);

      vsinline void setDom(const QDomElement& el) vsinline_attrib { dom = el; }
      vsinline vsp_p getProject() const vsinline_attrib { return project; }

      /** \fn VSProject::bbs()
       * \brief Get Build-Boxes for this project for non-modifying purposes
       * @return const ref to vector with Build-Boxes
       */
      vsinline pv_vsbb_cr bbs() const vsinline_attrib { return bboxes; }

    private:
      vsp_p project; // Current parent project
      vse_p parent; // Parent that can be project or filter
      uivsfl_p uifl; // UI representation
      QDomElement dom; // VS Dom representation of a file
      vsbb_p active_bb; // Active build-box, represents active configuration for this file
      pv_vsbb bboxes;
  };
  //END VS basic entities
  //===========================================================================

  //===========================================================================
  //BEGIN VS derived entities
  class VSProject_c : public VSProject {
    public:
      VSProject_c(const QString &name, const QUuid &uid);
      virtual ~VSProject_c();

    //VS FSStored intefrace methods:
      virtual bool write(bool synchronize=true);
      virtual bool write(QTextStream &stream, bool synchronize=true);
      virtual bool read(bool synchronize=true);
      virtual bool read(QTextStream &stream, bool synchronize=true);
    //VSProject intergace methods:
    //VSProject_c methods:

    private:
      bool __read_unit(QDomElement unit, vse_p parent);
  };

  class VSProject_cs : public VSProject {
    public:
      VSProject_cs(const QString &name, const QUuid &uid);
      virtual ~VSProject_cs();

    // VS FSStored interface methods:
      virtual bool write(bool synchronize=true);
      virtual bool write(QTextStream &stream, bool synchronize=true);
      virtual bool read(bool synchronize=true);
      virtual bool read(QTextStream &stream, bool synchronize=true);
  };
  //END VS derived entities
  //===========================================================================

  //===========================================================================
  //BEGIN VS build entities
  class VSTool : public VSEntity {
    public:
      VSTool(e_VSBuildTool vstl);
      virtual ~VSTool();

    // VS Entity interface methods:
      virtual void setParent(vse_p parent) = 0;
      virtual vse_p getParent() const = 0;

    // VS Tool interface methods:

    protected:
      e_VSBuildTool vstl;
      QString ext; // Supported file extensions like [*.c;*.cpp] devided by ';'
  };

  /** Settings
   * Used to setup buildtool or configuration, etc
   * There are several types of settings for tools
   * - value ( \b e.g. \a OutputName,OutputDir,TargetMachineArch )
   * - value list ( \b e.g. \a AdditionalIncludeDirectories,Predefines,Libraries )
   *
   * With different kinds of values:
   * - Boolean ( \b e.g. \a MinimalRebuild )
   * - Index ( \b e.g. \a UsePrecompiledHeaders,WarningLevel,Optimization )
   *    Now, those are interesting...
   *    Thing is that kind of settings will be stored logically like an array, or list of integers
   *    Each integer will mean something on pre-build stage where buildtools will be setup.
   *
   * Each build tool will have it's own set of settings, they will belong to this type of tool only.
   *
   */

  class VSSetting {
    public:
      VSSetting() {
      }
      virtual ~VSSetting() {
      }

      /*! Getting value for build command
       * @return will return something like /Gb or -lfoo.lib or -I /usr/include/
       */
      virtual const QString& cmd() const = 0;
      /*! Getting value for storing or UI purposes
       * @return will return something like 1 or WINDOWS;DEBUG;WIN32 or /usr/include;/my/inc
       */
      virtual const QString& cfg() const = 0;
  };

  /*! \class VSSetting_preprocdefs
   * \brief Contains a preprocessor pre-defines for project|file
   */
  class VSPreprocDefs : public VSSetting {
    public:
      VSPreprocDefs();

      virtual const QString& cmd() const;
      virtual const QString& cfg() const;

    public:
  };

  class VSToolCompiler : public VSTool {
    public:
      VSToolCompiler();
      virtual ~VSToolCompiler();

    // VS Entity interface methods:
      virtual void setParent(vse_p parent);
      virtual vse_p getParent() const;

    private:
      // ppdfs; // preprocessor definitions
  };

  class VSToolLinker : public VSTool {
    public:
      VSToolLinker();
      virtual ~VSToolLinker();

    // VS Entity interface methods:
      virtual void setParent(vse_p parent);
      virtual vse_p getParent() const;

    private:
  };

  class VSToolMidl : public VSTool {
    public:
      VSToolMidl();
      virtual ~VSToolMidl();

    // VS Entity interface methods:
      virtual void setParent(vse_p parent);
      virtual vse_p getParent() const;

    private:
  };

  class VSPlatform : public VSEntity {
    public:
      VSPlatform(e_VSPlatform platform);
      virtual ~VSPlatform();

    // VS Entity interface methods:
      virtual void setParent(vse_p parent) = 0;
      virtual vse_p getParent() const = 0;

    // VS Platform interface methods:
      e_VSPlatform platform() const { return vspl; }
      static const VSPlatform* getVSPlatform(e_VSPlatform vspl);

      //TODO: methods that configure tools like linker or midl upon action

    protected:
      e_VSPlatform vspl;
  };

  class VSPlWin32 : public VSPlatform {
    public:
      VSPlWin32();
      virtual ~VSPlWin32();

    // VS Entity interface methods:
      virtual void setParent(vse_p parent);
      virtual vse_p getParent() const;
  };

  class VSPlWin64 : public VSPlatform {
    public:
      VSPlWin64();
      virtual ~VSPlWin64();

    // VS Entity interface methods:
      virtual void setParent(vse_p parent);
      virtual vse_p getParent() const;
  };

  class VSPlMixed : public VSPlatform {
    public:
      VSPlMixed();
      virtual ~VSPlMixed();

    // VS Entity interface methods:
      virtual void setParent(vse_p parent);
      virtual vse_p getParent() const;
  };

  class VSPlAnyCPU : public VSPlatform {
    public:
      VSPlAnyCPU();
      virtual ~VSPlAnyCPU();

    // VS Entity interface methods:
      virtual void setParent(vse_p parent);
      virtual vse_p getParent() const;
  };

  /*!
   * Used to send into vs entities that can act.
   * @param name configuration name
   * @param platform configuration platform
   * @param sync_subs \b true if we must add coresponding config into each sub-node
   * <a>\b e.g. into projects within solution upon adding new config</a>
   */
  class VSConfigCreate {
    public:
      VSConfigCreate()
      : name(QString::null)
      , platform(vspl_unknown)
      , sync_subs(false) {
      }

      VSConfigCreate(const VSConfigCreate& cr)
      : sync_subs(cr.sync_subs) {
        name = cr.name;
        platform = cr.platform;
      }

      const QString string() const {
        return QString(name).append("|%1").arg(platform2String(platform));
      }

      QString name;
      e_VSPlatform platform;
      bool sync_subs;
  };

  class VSConfig : public VSEntity,
  public VSNameable {
    public:
      VSConfig(const QString &name, e_VSPlatform vspl);
      VSConfig(const QString &name, const QString &vspl);
      VSConfig(const QString &config);
      virtual ~VSConfig();

    // VS Entity interface methods:
      virtual void setParent(vse_p /*parent*/) {}
      virtual vse_p getParent() const { return 0; }

    // VS Config interface methods:
      e_VSPlatform platform() const { return vspl.platform(); }
      QString toString() const;
      QString getPlatform() const;
      bool operator == (const vcfg_cr config) const;
      bool operator == (const vcfgcr_r cr) const;

    private:
      const VSPlatform &vspl;
  };

  /*! \class VSBuildBox
   * \brief Contains configurations and tools necessary for \a vs_entity to be built|cleaned|re-builded, etc
   *
   * - Connects configurations of entity's parent to self.
   * - Tunes and setup build tools to work with them
   *
   * TODO: Think about merge concept for build box.
   *   When doing some action ( e.g. \a build ) there will be moments when for example files will have their own
   *   settings customized by user, those settings must be taken into account.
   *   Kind of \a "merging" settings from different buildboxes will be a good solution to that topic.
   */

  class VSBuildBox : public VSEntity {
    public:
      class VSToolUnit {
        public:
          VSToolUnit();
          ~VSToolUnit();

          int weight; // Will determine order in the action, it is a "weight" of tool
          vstl_p tool;
      };
    public:
      /*!
       * Will construct the build box
       * @param name - name for config that will signify this bb when acting
       * @param platform - platform part of \a VSConfig
       * NOTE: That parent config can be changed, but not the config of bb
       */
      VSBuildBox(const QString &name, e_VSPlatform platform);
      virtual ~VSBuildBox();

    // VS Entity interface methods:
      virtual void setParent(vse_p parent);
      virtual vse_p getParent() const;

    // VS BuildBox interface methods:
      vsinline bool isEnabled() const vsinline_attrib { return enabled; }
      vsinline void setEnabled(bool e=true) vsinline_attrib { enabled = e; }
      vsinline vcfg_cp parentConfig() const vsinline_attrib { return pcfg; }
      void setParentCfg(const vcfg_cp config);
      vsinline const vcfg_cr config() const vsinline_attrib { return cfg; }
      bool belongs(const vcfg_cp parent_cfg) const;
      void setDom(const QDomElement &element) { dom = element; }

    private:
      const VSConfig cfg;
      vcfg_cp pcfg; // Parent (higher order) configuration
      bool enabled;
      QDomElement dom;
  };

  class VSPrjCBuildBox : public VSBuildBox {
    public:
      VSPrjCBuildBox(const QString& name, e_VSPlatform platform);
      virtual ~VSPrjCBuildBox();

    private:
      pv_vstl vtls; // Tools
  };

  //END VS build entities
  //===========================================================================

  vsinline vcfg_cp VSProject::currentCfg() const {
    if(active_bb != 0) { return &active_bb->config(); }
    return 0;
  }

  /**
   * TODO: Think about compiler-collection object, kind of box where compile, linker, midl, etc are
   *   that will contain paths to tool-executables and maybe some additional info
   *   It will be used within project when build is happening, to provide info to
   *   setup a commond for tool along with buildbox
   */

  vss_p getParentSln(vse_p entity);
};
//END // VStudio namespace
#endif /* __KDEVPART_VSTUDIOPART_SOLUTION_H__ */
