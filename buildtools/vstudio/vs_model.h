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
      uint refs() const;
      bool isfree() const;
      const pv_VSEntity& parents() const;

    protected:
      //TODO: ATTENTION !
      // This must be done via boost's or Qt's guarded ptr.
      uint rfc;
#ifdef USE_BOOST
      pv_VSEntity pnts; // Parents
#else
#endif
  };

  class VSNameable {
    public:
      VSNameable(const QString& name);
      virtual ~VSNameable();

      virtual const QString& getName() const;
      virtual void setName(const QString& name);

    protected:
      QString name;
  };

  class VSIndexable {
    public:
      VSIndexable(const QUuid& uid);
      virtual ~VSIndexable();

      const QUuid& getUID() const;
      void setUID(const QUuid& uid);

    protected:
      QUuid uid;
  };

  class VSFSStored {
    public:
      VSFSStored();
      virtual ~VSFSStored();

      const QString& getRelPath() const;
      void setRelPath(const QString& rel_path);
      const QString& getAbsPath() const;
      void setAbsPath(const QString& abs_path);

    protected:
      QString path_rlt;
      QString path_abs;
  };

  class VSEntity {
    public:
      VSEntity(e_VSEntityType typ);
      virtual ~VSEntity();

      e_VSEntityType getType() const { return type; }
      static void setPart(VSPart* part);
      inline VSPart* part() const { return sys_part; }

      virtual void insert(vse_p item);
      virtual bool createUI(uivse_p parent_ui);

      virtual vse_p getByUID(const QUuid &uid) const = 0;
      virtual uivse_p getUI() const = 0;
      virtual void setParent(vse_p parent) = 0;
      virtual vse_p getParent() const = 0;

    protected:
      e_VSEntityType type;
      uivse_p uient;  // UI entity

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
      VSSolution(const QString &name, const QString &path_rlt);
      virtual ~VSSolution();

    // VS Entity interface methods:
      virtual void insert(vse_p item);
      virtual bool createUI();
      /**
       * @param uid
       * @return project ptr
       */
      virtual vse_p getByUID(const QUuid &uid) const;
      virtual uivse_p getUI() const { return (uivse_p)uisln; }
      virtual vse_p getParent() const;
      virtual void setParent(vse_p parent);

    // VS Solution methods:
      bool dumpLayout(QTextStream &layout);
      vsf_p getFltByUID(const QUuid &uid) const;
      void forEachProj(entityFunctor functor);
      void forEachFilter(entityFunctor functor);
      bool populateUI(); // This will populate/update UI items tree
      vsmd_p metaDependency(const QUuid &uid);
      bool updateDependencies();

      /*!
       * Will create a new config using \a parent_config as it's parent.
       * - will not set new config as \a selected.
       * - will check internally so that configs wouldn't doublicate.
       * - if any config will have the same parent as the new one being created, it will be disabled for action and
       *   new one will be enabled instead.
       * @param parent_config configuration to use as \b parent
       * @param pc is a pointer to \a VSConfig::VSConfigCreate struct containing params for config creation
       * @return \b true if all work is done perfectly well
       */
      bool createCfg(const vcfg_p parent_config, const vcfgcr_r pc);
      bool selectCfg(const vcfg_p parent_config);
      vsbb_p getBB(const QString &config) const;
      vsbb_p getBB(const vcfg_p parent_config) const;

      void setActive(bool active=true);
      bool isActive() const;
      bool setActivePrj(vsp_p project);
      bool setActivePrj(const QString &int_name);
      vsp_p getActivePrj() const;
      vsp_p getProject(const QString &int_name) const;

    private:
      uivss_p uisln;  // UI representation
      vsbb_p active_bb; // Active buildbox selected for this solution
      vsp_p active_prj; // Active project for this solution
#ifdef USE_BOOST
      pv_VSProject projects;
      pv_VSFilter filters;
      pv_VSBuildBox bboxes;  // build boxes, used to buil|clean, etc entire solution
#else
#error "VStudio: Boost support is no enabled"
#endif
      /*! Meta-dependencies tree
       * Used to contain raw dependencies for projects
       * Not filtered and allowed to have a cyclic dependencies
      */
#ifdef USE_BOOST
      pv_VSMetaDependency mdeps;
#else
#error "VStudio: Boost support is no enabled"
#endif
      bool active;
  };

  class VSProject : public VSEntity,
  public VSRefcountable,
  public VSNameable,
  public VSIndexable,
  public VSFSStored {
    public:
      VSProject(e_VSPrjLangType ltype, const QString &name, const QUuid &uid, const QString &path_rlt);
      virtual ~VSProject();

    // VS Entity methods:
      virtual void insert(vse_p item);
      virtual bool createUI(uivse_p parent_ui);
      virtual void setParent(vse_p parent);
      /**
       * @param uid uid of lookup file
       * @return file in project
       */
      virtual vse_p getByUID(const QUuid &uid) const;
      virtual uivse_p getUI() const { return (uivse_p)uiprj; }
      virtual vse_p getParent() const;

    // VS Project methods:
      bool dumpLayout(QTextStream &layout);
      vsp_p getReqByUID(const QUuid &uid) const;
      vsp_p getDepByUID(const QUuid &uid) const;
      vsf_p getFltByUID(const QUuid &uid) const;
      bool addDependency(vsp_p dep);
      bool addDependency(const QUuid &uid);
      bool addRequirement(vsp_p req);
      bool addRequirement(const QUuid &uid);
      bool populateUI();
      void setLanguage(e_VSPrjLangType lang);
      e_VSPrjLangType getLang() { return lang; }
      void setActive(bool active=true);
      bool isActive() const;

      // Configuraion works
      bool createCfg(const vcfg_cp parent_config, const vcfgcr_r cr);
      bool selectCfg(const vcfg_cp parent_config);

    private:
      vsbb_p getBB(const vcfg_cp parent_config) const;

    private:
      e_VSPrjLangType lang; // Project choosen language
      vss_p sln;  // Parent solution
      uivsp_p uiprj; // UI representation
      vsbb_p active_bb; // Active buildbox
#ifdef USE_BOOST
      pv_VSProject deps; // Projects dependant on this one
      pv_VSProject reqs; // Projects required to build this one, i.e. dependencies
      pv_VSFilter filters;
      pv_VSFile files;
      pv_VSBuildBox bboxes;
#else
#endif
      bool active;
  };

  class VSFilter : public VSEntity,
  public VSNameable,
  public VSIndexable {
    public:
      VSFilter(const QString &name, const QUuid &uid);
      virtual ~VSFilter();

    // VS Entity interface methods:
      virtual void insert(vse_p item);
      virtual bool createUI();
      virtual void setParent(vse_p parent); //NOTE: Inserts this filter into parent's filters
      // virtual QString getRelativePath() const;
      // virtual bool setRelativePath(const QString &path);
      virtual vse_p getByUID(const QUuid &uid) const;
      virtual uivse_p getUI() const { return (uivse_p)uiflt; }
      virtual vse_p getParent() const;

    // VS Filter methods:
      bool dumpLayout(QTextStream &layout);
      e_VSEntityType getParentType() const { return parent->getType(); }
      bool getParentUID(QUuid* uid) const;
      bool populateUI();

    private:
      vse_p parent; // Parent solution|filter|project
      uivsf_p uiflt; // UI representation
#ifdef USE_BOOST
      pv_VSEntity chld; // Children
#else
#endif
  };

  /** VS File model representation
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
  public VSIndexable,
  public VSFSStored {
    public:
      VSFile(const QString &name, const QUuid &uid, vsp_p parent);
      virtual ~VSFile();

    // VS Entity interface methods:
      virtual bool createUI(uivse_p ui_parent);
      virtual void setParent(vsp_p parent_prj);
      // virtual QString getRelativePath() const;
      // virtual bool setRelativePath(const QString &path);
      virtual vsp_p getByUID(const QUuid &uid) const;
      virtual uivse_p getUI() const { return (uivse_p)uifl; }
      virtual vse_p getParent() const;

    // VS File methods:

    private:
      vsp_p parent; // Current parent project
      uivsfl_p uifl; // UI representation
  };
  //END VS basic entities
  //===========================================================================

  //===========================================================================
  //BEGIN VS derived entities
  class VSProject_c : public VSProject {
    public:
      VSProject_c(const QString &name, const QUuid &uid, const QString &path_rlt);
      virtual ~VSProject_c();

    //VSProject intergace methods:
    //VSProject_c methods:
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
      virtual vse_p getByUID(const QUuid &uid) const = 0;
      virtual uivse_p getUI() const = 0;
      virtual void setParent(vse_p parent) = 0;
      virtual vse_p getParent() const = 0;

    // VS Tool interface methods:

    protected:
      e_VSBuildTool vstl;
  };

  class VSToolCompilerMSVC : public VSTool {
    public:
      VSToolCompilerMSVC();
      virtual ~VSToolCompilerMSVC();

    // VS Entity interface methods:
      virtual vse_p getByUID(const QUuid &uid) const;
      virtual uivse_p getUI() const;
      virtual void setParent(vse_p parent);
      virtual vse_p getParent() const;

    private:
  };

  class VSToolLinkerMSVC : public VSTool {
    public:
      VSToolLinkerMSVC();
      virtual ~VSToolLinkerMSVC();

    // VS Entity interface methods:
      virtual vse_p getByUID(const QUuid &uid) const;
      virtual uivse_p getUI() const;
      virtual void setParent(vse_p parent);
      virtual vse_p getParent() const;

    private:
  };

  class VSToolCompilerMSMidl : public VSTool {
    public:
      VSToolCompilerMSMidl();
      virtual ~VSToolCompilerMSMidl();

    // VS Entity interface methods:
      virtual vse_p getByUID(const QUuid &uid) const;
      virtual uivse_p getUI() const;
      virtual void setParent(vse_p parent);
      virtual vse_p getParent() const;

    private:
  };

  class VSPlatform : public VSEntity {
    public:
      VSPlatform(e_VSPlatform platform);
      virtual ~VSPlatform();

    // VS Entity interface methods:
      virtual vse_p getByUID(const QUuid &uid) const = 0;
      virtual uivse_p getUI() const = 0;
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
      virtual vse_p getByUID(const QUuid &uid) const;
      virtual uivse_p getUI() const;
      virtual void setParent(vse_p parent);
      virtual vse_p getParent() const;
  };

  class VSPlWin64 : public VSPlatform {
    public:
      VSPlWin64();
      virtual ~VSPlWin64();

    // VS Entity interface methods:
      virtual vse_p getByUID(const QUuid &uid) const;
      virtual uivse_p getUI() const;
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

  class VSConfig : public VSEntity
  , public VSNameable {
    public:
      VSConfig(const QString &name, e_VSPlatform vspl);
      VSConfig(const QString &name, const QString &vspl);
      VSConfig(const QString &config);
      virtual ~VSConfig();

    // VS Entity interface methods:
      virtual vse_p getByUID(const QUuid &uid) const;
      virtual uivse_p getUI() const;
      virtual void setParent(vse_p parent);
      virtual vse_p getParent() const;

    // VS Config interface methods:
      e_VSPlatform platform() const { return vspl.platform(); }
      const QString toString() const;
      bool operator == (const vcfg_cr config) const;
      bool operator == (const vcfgcr_r cr) const;

    private:
      const VSPlatform &vspl;
  };

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
      virtual vse_p getByUID(const QUuid &uid) const;
      virtual uivse_p getUI() const;
      virtual void setParent(vse_p parent);
      virtual vse_p getParent() const;

    // VS BuildBox interface methods:
      bool isEnabled() const;
      void setEnabled(bool enabled=true);
      const vcfg_cp parentConfig() const;
      void setParentCfg(const vcfg_cp config);
      const vcfg_cr config() const;
      bool belongs(const vcfg_cp parent_cfg) const;

    private:
      const VSConfig cfg;
      vcfg_cp pcfg; // Parent (higher order) configuration
      bool enabled;
  };

  //END VS build entities
  //===========================================================================

  vss_p getParentSln(vse_p entity);
};
//END // VStudio namespace
#endif /*__KDEVPART_VSTUDIOPART_SOLUTION_H__ */
