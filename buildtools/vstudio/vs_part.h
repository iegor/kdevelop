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
#include <qmap.h>
#include <qdatetime.h>
#include <qdir.h>
#include <kdevgenericfactory.h>
#include "kdevbuildtool.h"
//#include "vsmanagerwidget.h"
// #include <kdevplugin.h>
// #include <codemodel.h>
// #include <codebrowserfrontend.h>
#include <vs_explorer.h>  // VS Explorer widget
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

class VSPart : public KDevBuildTool {
  Q_OBJECT

public:
  VSPart(QObject *parent, const char *name, const QStringList &args);
  virtual ~VSPart();

  /**
   * Implementation of the KDevProject interface.
   */
  virtual QString projectDirectory() const;
  virtual QString projectName() const;

  virtual QString activeDirectory() const;
  virtual QStringList allFiles() const;
  virtual void addFile(const QString &fileName);
  virtual void addFiles(const QStringList& fileList);
  virtual void removeFile(const QString &fileName);
  virtual void removeFiles(const QStringList& fileList);
  virtual Options options() const;

  /**
  * A set of XML routines for loading vs projects and solutions
  */
  void loadVsSolution(const QString &sln_path);
  void unloadVsSolution(const QString &sln_path);
  void loadVsProject(const QString &prj_path);
  void unloadVsProject(const QString &prj_path);

  /**
   * Implementation of the KDevPlugin interface.
   */
  virtual void restorePartialProjectSession(const QDomElement* el);
  virtual void savePartialProjectSession(QDomElement* el);

private slots:
  void slotBuild();
  void slotCompileFile();
  void slotClean();

private:
  QGuardedPtr<VSExplorer> m_explorer_widget;
  QString m_projectName;
  QString m_projectPath;
  QMap<QString, QDateTime> m_timestamp;
};

typedef KDevGenericFactory<VSPart> VStudioFactory;

#endif
