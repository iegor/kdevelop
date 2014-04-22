/*
*kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;
*
*  C/C++ Implementation: vs_common
*
* Description:
*
*
* Author: iegor <rmtdev@gmail.com>, (C) 2014
*
* Copyright: See COPYING file that comes with this distribution
*/
#ifndef __KDEVPART_VSTUDIOPART_COMMON_H__
#define __KDEVPART_VSTUDIOPART_COMMON_H__

#include <config.h>

#include <qtextstream.h>
#include <quuid.h>

// Debug messaging
#define kddbg kdDebug() << "[-- VS PART --]: "

#define VSPART_SOLUTION_ITEM "vs_solution"
#define VSSOLUTION_VERSION 10

#define VSPART_PRJSECTION_DEPENDENCIES "ProjectDependencies"

//Actions
#define VSPART_ACTION_TOOLS_GROUP "vstools"

#define VSPART_ACTION_ADD_SOLUTION "project_addsolution"
#define VSPART_ACTION_ADD_SOLUTION_TIP "Add solution into kdevelop project"
#define VSPART_ACTION_ADD_SOLUTION_WIT "Adds solution that exist in current kdevelop project"

#define VSPART_ACTION_BUILD_SOLUTION "build_buildsolution"
#define VSPART_ACTION_BUILD_SOLUTION_KEY Key_F8
#define VSPART_ACTION_BUILD_SOLUTION_TIP "Build solution"
#define VSPART_ACTION_BUILD_SOLUTION_WIT "Builds selected solution"
#define VSPART_ACTION_BUILD_SOLUTION_WIT_LONG \
"<b>Build solution</b> <p>Runs <b>build process</b>from the project directory.<br>\
Environment variables and make arguments can be specified in the project settings dialog, <b>Make Options</b> tab."

#define VSPART_ACTION_REBUILD_SOLUTION "build_rebuildsolution"
#define VSPART_ACTION_REBUILD_SOLUTION_TIP "Rebuild solution"
#define VSPART_ACTION_REBUILD_SOLUTION_WIT "Rebuilds selected solution"

#define VSPART_ACTION_CLEAN_SOLUTION "build_cleansolution"
#define VSPART_ACTION_CLEAN_SOLUTION_TIP "Clean solution"
#define VSPART_ACTION_CLEAN_SOLUTION_WIT "Cleans out outputs from latest build of selected solution"

#define VSPART_ACTION_ADD_PROJECT "project_addproject"
#define VSPART_ACTION_ADD_PROJECT_TIP "Add project into selected solution"
#define VSPART_ACTION_ADD_PROJECT_WIT "Adds existing project into selected solution"

#define VSPART_ACTION_BUILD_PROJECT "build_buildproject"
#define VSPART_ACTION_BUILD_PROJECT_KEY Key_F7
#define VSPART_ACTION_BUILD_PROJECT_TIP "Build project"
#define VSPART_ACTION_BUILD_PROJECT_WIT "Builds selected project"
#define VSPART_ACTION_BUILD_PROJECT_WIT_LONG \
"<b>Build project</b><p>Runs <b>build process</b> from the project directory.<br> \
Environment variables and make arguments can be specified in the project settings dialog, <b>Make Options</b> tab."

#define VSPART_ACTION_REBUILD_PROJECT "build_rebuildproject"
#define VSPART_ACTION_REBUILD_PROJECT_TIP "Rebuild project"
#define VSPART_ACTION_REBUILD_PROJECT_WIT "Rebuilds selected project"

#define VSPART_ACTION_CLEAN_PROJECT "build_cleanproject"
#define VSPART_ACTION_CLEAN_PROJECT_TIP "Clean project"
#define VSPART_ACTION_CLEAN_PROJECT_WIT "Cleans out outputs from latest build of selected project"

#define VSPART_ACTION_ADD_FILE "project_addfile"
#define VSPART_ACTION_ADD_FILE_TIP "Add file"
#define VSPART_ACTION_ADD_FILE_WIT "Adds or creates file into selected project"

#define VSPART_ACTION_BUILD_FILE "build_buildfile"
#define VSPART_ACTION_BUILD_FILE_TIP "Compile file"
#define VSPART_ACTION_BUILD_FILE_WIT "Compiles selected file and creates object code file .obj for it"

#define VSPART_ACTION_CLEAN_FILE "build_cleanfile"
#define VSPART_ACTION_CLEAN_FILE_TIP "Clean file"
#define VSPART_ACTION_CLEAN_FILE_WIT "Cleans out compiled .obj for selected file"

#define VSPART_ACTION_ADD_FILTER "project_addfilter"
#define VSPART_ACTION_ADD_FILTER_TIP "Add filter"
#define VSPART_ACTION_ADD_FILTER_WIT "Adds filter into selected solution or project"

#define VSPART_ACTION_BUILD_FILTER "build_buildfilter"
#define VSPART_ACTION_BUILD_FILTER_TIP "Build filter"
#define VSPART_ACTION_BUILD_FILTER_WIT "Builds solutions and/or projects under selected filter"

#define VSPART_ACTION_CLEAN_FILTER "build_cleanfilter"
#define VSPART_ACTION_CLEAN_FILTER_TIP "Clean filter"
#define VSPART_ACTION_CLEAN_FILTER_WIT "Cleans out build outputs of solutions/or projects under selected filter"

//BEGIN //VStudio namespace
namespace VStudio {
  enum e_VSEntityType {
    vs_file = 0,
    vs_filter,
    vs_project,
    vs_solution,
  };
  /**
  * Unique identifier class helper
  * used mostly in VS entities
  */
  /*
  class UUID {
  public:
    UUID();
  private:
    union {
      unsigned char m1[32];
      struct {
        unsigned int f1;
        unsigned short s1;
        unsigned short s2;
        unsigned short s3;
        union {
          unsigned char m2[12];
          struct {
            unsigned int f2;
            unsigned short s4;
          };
          struct {
            unsigned short sx1;
            unsigned short sx2;
            unsigned short sx3;
          };
        };
      };
    };
  };
  */
  bool readGUID(QTextStream &tstream, QUuid &uid);
  QString guid2String(const QUuid &uid);
};
//END // VStudio namespace
#endif