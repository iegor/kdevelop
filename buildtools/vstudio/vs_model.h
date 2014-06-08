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
  class VSEntity {
    public:
      VSEntity(e_VSEntityType typ, const QString &name);
      VSEntity(e_VSEntityType typ, const QString &name, const QUuid &uid);
      virtual ~VSEntity();

      void uidSet(const QUuid &uid) { uuid = uid; }
      QUuid uidGet() const { return uuid; }
      e_VSEntityType getType() const { return type; }
      bool release();
      void acquire();

      static void setPart(VSPart* part);
      inline VSPart* part() const { return sys_part; }

      virtual void setName(const QString &name);
      virtual const QString& getName() const;
      virtual void setParent(vse_p parent);
      virtual void insert(vse_p item);
      virtual bool createUI(uivse_p parent_ui);

      virtual QString getRelativePath() const = 0;
      virtual bool setRelativePath(const QString &path) = 0;
      virtual vse_p getByUID(const QUuid &uid) const = 0;
      virtual uivse_p getUI() const = 0;
      virtual vse_p getParent() const = 0;
    protected:
      QString name;
      QUuid uuid;
      e_VSEntityType type;

      //TODO: ATTENTION !
      // This must be done via boost's or Qt's guarded ptr.
      int refs;

      // UI data
      uivse_p uient;  // UI entity
    private:
      static VSPart *sys_part; // System part
  };

  class VSSolution : public VSEntity {
    public:
      predeclare_vs_typ(VSMetaDependency, vsmd_p, vsmd_ci, vsmd_i);
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
      VSSolution(const QString &name, const QUuid &uid, const QString &path_rlt);
      virtual ~VSSolution();

    // VS Entity interface methods:
      virtual void insert(vse_p item);
      virtual bool createUI();
      virtual void setParent(vse_p parent);
      virtual QString getRelativePath() const { return path_rlt; }
      virtual bool setRelativePath(const QString &path);
      virtual vse_p getByUID(const QUuid &uid) const; // This will return project ptr, to reuse code
      virtual uivse_p getUI() const { return (uivse_p)uisln; }
      virtual vse_p getParent() const;

    // VS Solution methods:
      bool dumpLayout(QTextOStream &layout);
      vsf_p getFltByUID(const QUuid &uid) const;
      void forEachProj(entityFunctor functor);
      void forEachFilter(entityFunctor functor);
      bool populateUI(); // This will populate/update UI items tree
      vsmd_p metaDependency(const QUuid &uid);
      bool updateDependencies();
      bool addConfiguration(const QString &config);
      bool addConfiguration(const QString &name, const QString &platform);
      bool addConfiguration(const QString &name, e_VSPlatform platform);
      bool setConfiguration(const QString &config);
      bool setConfiguration(const QString &name, const QString &platform);
      bool setConfiguration(const QString &name, e_VSPlatform platform);
      const pv_VSConfig* vcfg() const;
      void setActive(bool active=true);
      bool isActive() const;
      bool setActivePrj(vsp_p project);
      bool setActivePrj(const QString &int_name);
      vsp_p getActivePrj() const;
      vsp_p getProject(const QString &int_name) const;
    private:
      QString path_rlt;
      uivss_p uisln;  // UI representation
      vcfg_p config; // Active configuration selected for this solution
      vsp_p actprj; // Active project for this solution
#ifdef USE_BOOST
      pv_VSProject projects;
      pv_VSFilter filters;
      pv_VSConfig cfgs;  // Configurations
#else
#error "VStudio: Boost support is no enabled"
#endif
      /** Meta-dependencies tree
       * Used to contain raw dependencies for projects
       * Not filtered and allowed to have a cyclic dependencies
      */
#ifdef USE_BOOST
      boost::container::vector<vsmd_p> mdeps;
#else
#error "VStudio: Boost support is no enabled"
#endif
      bool active;
  };

  class VSProject : public VSEntity {
    public:
      VSProject(e_VSPrjLangType ltype, const QString &name, const QString &path_rlt);
      VSProject(e_VSPrjLangType ltype, const QString &name, const QUuid &uid, const QString &path_rlt);
      virtual ~VSProject();

    // VS Entity methods:
      virtual void insert(vse_p item);
      virtual bool createUI(uivse_p parent_ui);
      virtual void setParent(vse_p parent);
      virtual QString getRelativePath() const { return path_rlt; }
      virtual bool setRelativePath(const QString &path);
      virtual vse_p getByUID(const QUuid &uid) const; // This will get file in project
      virtual uivse_p getUI() const { return (uivse_p)uiprj; }
      virtual vse_p getParent() const;

    // VS Project methods:
      bool dumpLayout(QTextOStream &layout);
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

    private:
      e_VSPrjLangType lang; // Project choosen language
      QString path_rlt;
      vss_p sln;  // Parent solution
      uivsp_p uiprj; // UI representation
#ifdef USE_BOOST
      boost::container::vector<vss_p> pnts; // Parent solutions
      boost::container::vector<vsp_p> deps; // Projects dependant on this one
      boost::container::vector<vsp_p> reqs; // Projects required to build this one, i.e. dependencies
      boost::container::vector<vsf_p> filters;
      boost::container::vector<vsfl_p> files;
#else
#endif
      bool active;
  };

  class VSFilter : public VSEntity {
    public:
      VSFilter(const QString &name);
      VSFilter(const QString &name, const QUuid &uid);
      virtual ~VSFilter();

    // VS Entity interface methods:
      virtual void insert(vse_p item);
      virtual bool createUI();
      virtual void setParent(vse_p parent); //NOTE: Inserts this filter into parent's filters
      virtual QString getRelativePath() const;
      virtual bool setRelativePath(const QString &path);
      virtual vse_p getByUID(const QUuid &uid) const;
      virtual uivse_p getUI() const { return (uivse_p)uiflt; }
      virtual vse_p getParent() const;

    // VS Filter methods:
      e_VSEntityType getParentType() {
        return parent->getType();
      }

      QUuid getParentUID() {
        return parent->uidGet();
      }

      bool populateUI();
    private:
      vse_p parent; // Parent filter or project
      uivsf_p uiflt; // UI representation
#ifdef USE_BOOST
      boost::container::vector<vse_p> chld; // Children
#else
#endif
  };

  class VSFile : public VSEntity {
    public:
      VSFile(const QString &name, vsp_p parent);
      VSFile(const QString &name, const QUuid &uid, vsp_p parent);
      virtual ~VSFile();

    // VS Entity interface methods:
      virtual bool createUI(uivse_p ui_parent);
      virtual void setParent(vsp_p parent_prj);
      virtual QString getRelativePath() const;
      virtual bool setRelativePath(const QString &path);
      virtual vsp_p getByUID(const QUuid &uid) const;
      virtual uivse_p getUI() const { return (uivse_p)uifl; }
      virtual vse_p getParent() const;

    // VS File methods:
    private:
      vsp_p parent; // Current parent project
      uivsfl_p uifl; // UI representation
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
  //END VS basic entities
  //===========================================================================

  //===========================================================================
  //BEGIN VS derived entities
  class VSProject_c : public VSProject {
    public:
      VSProject_c(const QString &name, const QString &path_rlt);
      VSProject_c(const QString &name, const QUuid &uid, const QString &path_rlt);
      virtual ~VSProject_c();

    //VSProject intergace methods:
    //VSProject_c methods:
  };
  //END VS derived entities
  //===========================================================================

  //===========================================================================
  //BEGIN VS build entities
  class VSTool : public VSEntity{
    public:
      VSTool(e_VSBuildTool vstl);
      virtual ~VSTool();

    // VS Entity interface methods:
      virtual void setName(const QString &name);
      virtual const QString& getName() const;

      virtual QString getRelativePath() const = 0;
      virtual bool setRelativePath(const QString &path) = 0;
      virtual vse_p getByUID(const QUuid &uid) const = 0;
      virtual uivse_p getUI() const = 0;
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
      virtual QString getRelativePath() const;
      virtual uivse_p getUI() const;
      virtual vse_p getParent() const;
    private:
  };

  class VSToolLinkerMSVC : public VSTool {
    public:
      VSToolLinkerMSVC();
      virtual ~VSToolLinkerMSVC();

    // VS Entity interface methods:
      virtual QString getRelativePath() const;
      virtual uivse_p getUI() const;
      virtual vse_p getParent() const;
    private:
  };

  class VSToolCompilerMSMidl : public VSTool {
    public:
      VSToolCompilerMSMidl();
      virtual ~VSToolCompilerMSMidl();

    // VS Entity interface methods:
      virtual QString getRelativePath() const;
      virtual uivse_p getUI() const;
      virtual vse_p getParent() const;
    private:
  };

  class VSPlatform {
    public:
      VSPlatform(e_VSPlatform platform);
      virtual ~VSPlatform();

      e_VSPlatform name() const { return vspl; }

      static const VSPlatform* getVSPlatform(e_VSPlatform vspl);

      //TODO: methods that configure tools like linker or midl
    protected:
      e_VSPlatform vspl;
  };

  class VSConfig : public VSEntity {
    public:
      VSConfig(const QString &name, e_VSPlatform vspl);
      VSConfig(const QString &name, const QString &vspl);
      VSConfig(const QString &config);
      virtual ~VSConfig();

    // VS Entity interface methods:
      virtual QString getRelativePath() const;
      virtual bool setRelativePath(const QString &path);
      virtual vse_p getByUID(const QUuid &uid) const;
      virtual uivse_p getUI() const;
      virtual vse_p getParent() const;

    // VS Config interface methods:
      e_VSPlatform platform() const { return vspl.name(); }
      QString toString();

      bool operator ==(const VSConfig &config) const;
    private:
      const VSPlatform &vspl;
  };
  //END VS build entities
  //===========================================================================

  vss_p getParentSln(vse_p entity);
};
//END // VStudio namespace
#endif /*__KDEVPART_VSTUDIOPART_SOLUTION_H__ */
