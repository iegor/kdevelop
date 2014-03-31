/*
*kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;
*
*  C/C++ Implementation: vs_part
*
* Description:
*  ! Parts were inherited from autoprojectpart src
*
* Author: iegor <rmtdev@gmail.com>, (C) 2013
*
* Copyright: See COPYING file that comes with this distribution
*/
#ifndef __KDEVPART_VSTUDIOPART_H__
#define __KDEVPART_VSTUDIOPART_H__

#include <qdict.h>
#include <qguardedptr.h>
#include <qptrlist.h>
#include <qmap.h>
#include <qdatetime.h>
#include <qdir.h>
#include <kdevgenericfactory.h>
#include "kdevbuildtool.h"
//#include "vsmanagerwidget.h"
// #include <kdevplugin.h>
// #include <codemodel.h>
// #include <codebrowserfrontend.h>
#include "vs_common.h"
#include "vs_explorer.h"  // VS Explorer widget
#include "vs_model.h"
// using namespace Extensions;

namespace KParts { class Part; }
namespace KTextEditor {
  class Document;
  class View;
  class EditInterface;
  class SelectionInterface;
  class ViewCursorInterface;
}

class QDomElement;
class QStringList;
class KDialogBase;
class KSelectAction;
class TargetItem;
class ConfigWidgetProxy;
class VSManagerWidget;

//BEGIN //VStudio namespace
namespace VStudio {
  class VSPart : public KDevBuildTool {
    Q_OBJECT

  public:
    VSPart(QObject *parent, const char *name, const QStringList &args);
    virtual ~VSPart();

    //===========================================================================
    // KDevProject methods
    //===========================================================================
    virtual void openProject(const QString &dirName, const QString &projectName);
    virtual void closeProject();
    virtual QString projectDirectory() const;
    virtual QString projectName() const;
    virtual DomUtil::PairList runEnvironmentVars() const;
    virtual QString mainProgram() const;
    virtual QString runDirectory() const;
    virtual QString runArguments() const;
    virtual QString debugArguments() const;
    virtual QString activeDirectory() const;
    virtual QString buildDirectory() const;
    virtual QStringList allFiles() const;
    virtual QStringList distFiles() const;
    virtual void addFile(const QString &fileName);
    virtual void addFiles(const QStringList& fileList);
    virtual void removeFile(const QString &fileName);
    virtual void removeFiles(const QStringList& fileList);

    virtual Options options() const;

    //===========================================================================
    // Implementation of the KDevPlugin interface.
    //===========================================================================
    virtual void restorePartialProjectSession(const QDomElement* el);
    virtual void savePartialProjectSession(QDomElement* el);

    //===========================================================================
    // A set of XML routines for loading vs projects and solutions
    //===========================================================================
    bool loadVsSolution(const QString &sln_name, const QString &sln_path);
    bool unloadVsSolution(const QString &sln_path);
    bool loadVsProject(const QString &prj_path);
    bool unloadVsProject(const QString &prj_path);

  private slots:
    void slotBuild();
    void slotCompileFile();
    void slotClean();

  private:
    QGuardedPtr<VSExplorer> m_explorer_widget;
    QString m_projectName;
    QString m_projectPath;
    QMap<QString, QDateTime> m_timestamp;
    QPtrList<VSEntity*> m_entities;
    QString m_prjpath;
    QString m_prjname;
  };
};
//END // VStudio namespace
#endif
