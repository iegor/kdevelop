/*kate: space-indent on; tab-width 4; indent-width 4; indent-mode cstyle; encoding UTF-8;
 *  Copyright (C) 2013 Iegor Danylchenko (rmtdev@gmail.com)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 *
*/
#ifndef __KDEVPART_VSTUDIOPART_H__
#define __KDEVPART_VSTUDIOPART_H__

// #include "kdevlanguagesupport.h"
// #include "viewcombos.h"

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

// using namespace Extensions;

namespace KParts { class Part; }
namespace KTextEditor
{
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

class VStudioPart : public KDevBuildTool
{
		Q_OBJECT

	public:
		VStudioPart ( QObject *parent, const char *name, const QStringList &args );
		virtual ~VStudioPart();

		/**
		 * Implementation of the KDevProject interface.
		 */
		virtual QString projectDirectory() const;
		virtual QString projectName() const;
		virtual DomUtil::PairList runEnvironmentVars() const;
		virtual QString runDirectory() const;
		virtual QString mainProgram() const;
		virtual QString debugArguments() const;
		virtual QString runArguments() const;
//   virtual QString environString() const;
		virtual QString activeDirectory() const;
		virtual QStringList allFiles() const;
		virtual void addFile ( const QString &fileName );
		virtual void addFiles ( const QStringList& fileList );
		virtual void removeFile ( const QString &fileName );
		virtual void removeFiles ( const QStringList& fileList );
		virtual QString buildDirectory() const;
		virtual Options options() const;

    /**
    * A set of XML routines for loading vs projects and solutions
    */
    void loadVsSolution ( const QString &sln_path );
    void unloadVsSolution ( const QString &sln_path );
    void loadVsProject ( const QString &prj_path );
    void unloadVsProject ( const QString &prj_path );

		/**
		 * Implementation of the KDevPlugin interface.
		 */
		virtual void restorePartialProjectSession ( const QDomElement* el );
		virtual void savePartialProjectSession ( QDomElement* el );

		/**
		 * automake specific methods.
		 */
//   QStringList allBuildConfigs() const;
//   QString currentBuildConfig() const;
//   QString topsourceDirectory() const;
//   void startMakeCommand( const QString &dir, const QString &target, bool withKdesu = false );
//   void startSimpleMakeCommand( const QString &dir, const QString &command, bool withKdesu = false );
		void buildTarget ( QString relpath, TargetItem* titem );
//   void executeTarget( const QDir& dir, const TargetItem* titem );

//     void needMakefileCvs();
		bool isDirty();
//     bool isKDE() const;
		QStringList distFiles() const;
//   QString getAutoConfFile(const QString& dir);

	protected:
		/**
		 * Reimplemented from KDevProject. These methods are only
		 * for use by the application core.
		 */
		virtual void openProject ( const QString &dirName, const QString &projectName );
		virtual void closeProject();

	private slots:
		//    void projectConfigWidget(KDialogBase *dlg);
		void slotAddTranslation();
		void slotBuild();
		void slotBuildActiveTarget();
		void slotCompileFile();
		void slotClean();
//   void slotDistClean();
//   void slotInstall();
//   void slotInstallWithKdesu();
//   void slotMakefilecvs();
//   void slotMakeMessages();
//   void slotConfigure();
//   void slotExecute();
//   void slotExecute2();
//   void slotExecuteTargetAfterBuild( const QString& command );
//   void slotNotExecuteTargetAfterBuildFailed( const QString& command );
//   void slotBuildConfigChanged( const QString &config );
//   void slotBuildConfigAboutToShow();
//   void slotCommandFinished( const QString& command );
//   void slotCommandFailed( const QString& command );
//   void slotImportExisting();
//   void insertConfigWidget( const KDialogBase* dlg, QWidget * page, unsigned int );


	private:
		QGuardedPtr<VSManagerWidget> m_widget;
		QString m_projectName;
		QString m_projectPath;
		KSelectAction *buildConfigAction;

//   QString makeEnvironment() const;
//   void setWantautotools();
//   QString makefileCvsCommand() const;
//   QString configureCommand() const;

		QMap<QString, QDateTime> m_timestamp;
//   bool m_executeAfterBuild;
		QString m_buildCommand;
//   bool m_needMakefileCvs;
//   bool m_lastCompilationFailed;
//     bool m_isKDE;
//   QPair<QDir, TargetItem*> m_executeTargetAfterBuild;
//   QString m_runProg;

//   ConfigWidgetProxy * _configProxy;

		// Enble AutoProjectWidget to emit our signals
		friend class VSManagerWidget;
		friend class AddSubprojectDialog;
		friend class FileItem;

		// For make commands queuing
//   QString constructMakeCommandLine( const QString &dir, const QString &target ) const;
//   bool queueInternalLibDependenciesBuild( TargetItem* titem, QStringList& list );
};

typedef KDevGenericFactory<VStudioPart> VStudioFactory;

#endif
