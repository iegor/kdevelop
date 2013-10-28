/***************************************************************************
 *   Copyright (C) 2001-2002 by Bernd Gehrmann                             *
 *   bernd@kdevelop.org                                                    *
 *                                                                         *
 *   Copyright (C) 2002 by Victor Roeder                                   *
 *   victor_roeder@gmx.de                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <config.h>

#include "vstudiopart.h"
#include "vslistviewitems.h"
// #include "configureoptionswidget.h"
// #include "addtranslationdlg.h"
// #include "addicondlg.h"
#include "vsmanagerwidget.h"

#include <qdom.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qpopupmenu.h>
#include <qstringlist.h>
#include <qwhatsthis.h>
#include <qregexp.h>
#include <qgroupbox.h>

#include <kapplication.h>
#include <kconfig.h>
#include <kaction.h>
#include <kdebug.h>
#include <kdialogbase.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kparts/part.h>
#include <kdeversion.h>
#include <kprocess.h>

#include <domutil.h>
#include <kdevcore.h>
#include <kdevmakefrontend.h>
#include <kdevappfrontend.h>
#include <kdevmainwindow.h>
#include <kdevpartcontroller.h>
#include <makeoptionswidget.h>
#include <runoptionswidget.h>
#include <envvartools.h>

#include <configwidgetproxy.h>
#include <kdevplugininfo.h>
#include <urlutil.h>

#define CONFIGURE_OPTIONS 1
#define RUN_OPTIONS 2
#define MAKE_OPTIONS 3

static const KDevPluginInfo data ( "kdevvstudioproject" );
K_EXPORT_COMPONENT_FACTORY ( libkdevvstudioproject, VStudioFactory ( data ) )

VStudioPart::VStudioPart ( QObject *parent, const char *name, const QStringList &/*args*/ )
		: KDevBuildTool ( &data, parent, name ? name : "VStudioPart" )
//     , m_lastCompilationFailed(false)
{
	setInstance ( VStudioFactory::instance() );

	setXMLFile ( "kdevvstudioproject.rc" );

//   m_executeAfterBuild = false;
//    m_isKDE = ( args[0] == "kde" );
//   m_needMakefileCvs = false;

	m_widget = new vsw ( this ); //, m_isKDE);
	m_widget->setIcon ( SmallIcon ( info()->icon() ) );
	m_widget->setCaption ( i18n ( "VS Manager" ) );
	QWhatsThis::add ( m_widget, i18n ( "<b>VS manager</b><p>"
	                                   "The tree looks like a standard VS project explorer,"
	                                   "yet it has some additiona \"power\" features." ) );

	mainWindow()->embedSelectViewRight ( m_widget, i18n ( "VS Manager" ),
	                                     i18n ( "VS Manager, manages vs solution files" ) );

	KAction *action;

//   action = new KAction( i18n("Add Translation..."), 0,
//                         this, SLOT(slotAddTranslation()),
//                         actionCollection(), "project_addtranslation" );
//   action->setToolTip(i18n("Add translation"));
//   action->setWhatsThis(i18n("<b>Add translation</b><p>Creates .po file for the selected language."));
//   action->setGroup("autotools");
//   if (!m_isKDE) action->setEnabled(false);

	// Build solution action
	action = new KAction ( i18n ( "&Build Solution" ), "make_kdevelop", Key_F8, this,
	                       SLOT ( slotBuildSolution() ), actionCollection(), "build_build_solution" );
	action->setToolTip ( i18n ( "Builds entire solution" ) );
	action->setWhatsThis ( i18n ( "<b>Build solution</b><p>Runs <b>build process</b> from the project directory.<br>"
	                              "Environment variables and make arguments can be specified"
	                              "in the project settings dialog, <b>Make Options</b> tab." ) );
	action->setGroup ( "autotools" );

	// Build project action
	action = new KAction ( i18n ( "&Build Project" ), "make_kdevelop", Key_F8, this,
	                       SLOT ( slotBuildProject() ), actionCollection(), "build_build_project" );
	action->setToolTip ( i18n ( "Builds selected(active) project" ) );
	action->setWhatsThis ( i18n ( "<b>Build project</b><p>Runs <b>build process</b> from the project directory.<br>"
	                              "Environment variables and make arguments can be specified"
	                              "in the project settings dialog, <b>Make Options</b> tab." ) );
	action->setGroup ( "autotools" );

	// Clean solution action
	action = new KAction ( i18n ( "Clean solution" ), "make_kdevelop", Key_F7, this,
	                       SLOT ( slotCleanSolution() ), actionCollection(), "build_clean_solution" );
	action->setToolTip ( i18n ( "Cleans whole solution from produced binaries." ) );
	action->setWhatsThis ( i18n ( "<b>Clean solution</b><p>Cleans out all output produced previously." ) );
	action->setGroup ( "autotools" );

	// Build file action
	action = new KAction ( i18n ( "Compile &File" ), "make_kdevelop", this,
	                       SLOT ( slotCompileFile() ), actionCollection(), "build_build_file" );
	action->setToolTip ( i18n ( "Compile file" ) );
	action->setWhatsThis ( i18n ( "<b>Compile file</b><p>Runs <b>build process</b> command from the directory where 'filename'"
	                              "is the name of currently opened file.<br> Environment variables and make"
	                              "arguments can be specified in the project settings dialog, <b>Make Options</b> tab." ) );
	action->setGroup ( "autotools" );

	// Build configuration action
	buildConfigAction = new KSelectAction ( i18n ( "Build Configuration" ), 0, actionCollection(), "project_configuration" );
	buildConfigAction->setToolTip ( i18n ( "Build configuration menu" ) );
	buildConfigAction->setWhatsThis ( i18n ( "<b>Build configuration menu</b><p>Allows to switch between project build configurations.<br>"
	                                  "Build configuration is a set of build and top source directory settings, "
	                                  "configure flags and arguments, compiler flags, etc.<br>"
	                                  "Modify build configurations in project settings dialog, <b>Configure Options</b> tab." ) );
	buildConfigAction->setGroup ( "autotools" );

	QDomDocument &dom = *projectDom();
	if ( !DomUtil::readBoolEntry ( dom, "/kdevvstudio/run/disable_default" ) )
	{
		//ok we handle the execute in this kpart
		action = new KAction ( i18n ( "Execute Program" ), "exec", SHIFT+Key_F9,
		                       this, SLOT ( slotExecute() ),
		                       actionCollection(), "build_execute" );
		action->setToolTip ( i18n ( "Execute program" ) );
		action->setWhatsThis ( i18n ( "<b>Execute program</b><p>Executes the currently"
		                              "active target or the main program specified in"
		                              "project settings, <b>Run Options</b> tab." ) );
		action->setGroup ( "autotools" );
	}

	connect ( buildConfigAction, SIGNAL ( activated ( const QString& ) ),
	          this, SLOT ( slotBuildConfigChanged ( const QString& ) ) );
	connect ( buildConfigAction->popupMenu(), SIGNAL ( aboutToShow() ),
	          this, SLOT ( slotBuildConfigAboutToShow() ) );

//    connect( core(), SIGNAL(projectConfigWidget(KDialogBase*)), this, SLOT(projectConfigWidget(KDialogBase*)) );

//   _configProxy = new ConfigWidgetProxy( core() );
//   _configProxy->createProjectConfigPage( i18n("Configure Options"), CONFIGURE_OPTIONS, info()->icon() );
//   _configProxy->createProjectConfigPage( i18n("Run Options"), RUN_OPTIONS, info()->icon() );
//   _configProxy->createProjectConfigPage( i18n("Make Options"), MAKE_OPTIONS, info()->icon() );
//   connect( _configProxy, SIGNAL(insertConfigWidget(const KDialogBase*, QWidget*, unsigned int )),
//            this, SLOT(insertConfigWidget(const KDialogBase*, QWidget*, unsigned int )) );
//
//
//   connect( makeFrontend(), SIGNAL(commandFinished(const QString&)),
//            this, SLOT(slotCommandFinished(const QString&)) );
//   connect( makeFrontend(), SIGNAL(commandFailed(const QString&)),
//            this, SLOT(slotCommandFailed(const QString&)) );

//   setWantautotools();
}

VStudioPart::~VStudioPart()
{
	if ( m_widget )
	{
		mainWindow()->removeView ( m_widget );
	}
	delete m_widget;
//   delete _configProxy;
}

// void VStudioPart::insertConfigWidget( const KDialogBase* dlg, QWidget * page, unsigned int pagenumber ) {
//   switch ( pagenumber )
//   {
//   case CONFIGURE_OPTIONS:
//   {
//     ConfigureOptionsWidget *w2 = new ConfigureOptionsWidget(this, page );
//     connect( dlg, SIGNAL(okClicked()), w2, SLOT(accept()) );
//   }
//   break;
//
//   case RUN_OPTIONS:
//   {
//     QDomDocument &dom = *projectDom();
//     if (!DomUtil::readBoolEntry(dom, "/kdevvstudio/run/disable_default"))
//     {
//       //ok we handle the execute in this kpart
//       RunOptionsWidget *w3 = new RunOptionsWidget(*projectDom(), "/kdevvstudio", buildDirectory(), page );
//       connect( dlg, SIGNAL(okClicked()), w3, SLOT(accept()) );
//     }
//   }
//   break;
//
//   case MAKE_OPTIONS:
//   {
//     MakeOptionsWidget *w4 = new MakeOptionsWidget(*projectDom(), "/kdevvstudio", page );
//     connect( dlg, SIGNAL(okClicked()), w4, SLOT(accept()) );
//   }
//   break;
//   }
// }

void VStudioPart::openProject ( const QString &dirName, const QString &projectName )
{
// 	m_projectName = projectName;
// 	m_projectPath = dirName;
//
// 	m_widget->openProject ( dirName );
//
// 	QDomDocument &dom = *projectDom();
// 	QString activeTarget = DomUtil::readEntry ( dom, "/kdevvstudio/general/activetarget" );
// 	kdDebug ( 9020 ) << k_funcinfo << "activeTarget " << activeTarget << endl;
//   if (!activeTarget.isEmpty())
//     m_widget->setActiveTarget(activeTarget);
//   else
//   {
//     KMessageBox::information( m_widget, i18n("No active target specified, running the application will\n"
//                               "not work until you make a target active in the VS Manager\n"
//                               "on the right side or use the Main Program options under\n"
//                               "Project -> Project Options -> Run Options"), i18n("No active target specified"),  "kdevelop_open_project_no_active_target");
//   }

	KDevProject::openProject ( dirName, projectName );
}

void VStudioPart::loadVsSolution ( const QString &/*sln_path*/ )
{
}

void VStudioPart::unloadVsSolution ( const QString &/*sln_path*/ )
{
}

void VStudioPart::loadVsProject ( const QString &/*prj_path*/ )
{
}

void VStudioPart::unloadVsProject ( const QString &/*prj_path*/ )
{
}

void VStudioPart::closeProject()
{
// 	m_widget->unloadProject ( "" );
}

QString VStudioPart::projectDirectory() const
{
	return m_projectPath;
}

QString VStudioPart::projectName() const
{
	return m_projectName;
}

/** Retuns a PairList with the run environment variables */
DomUtil::PairList VStudioPart::runEnvironmentVars() const
{
	return DomUtil::readPairListEntry ( *projectDom(), "/kdevvstudio/run/envvars", "envvar", "name", "value" );
}

/** Retuns the currently selected run directory
  * If no main Program was selected in the Run Options dialog
  * use the currently active target instead to calculate it.
  * The returned string can be:
  *   if /kdevautoproject/run/directoryradio == executable
  *        The directory where the executable is
  *   if /kdevautoproject/run/directoryradio == build
  *        The directory where the executable is relative to build directory
  *   if /kdevautoproject/run/directoryradio == custom
  *        The custom directory absolute path
  */
QString VStudioPart::runDirectory() const
{
//
//   QDomDocument &dom = *projectDom();
//
//   QString cwd;
//   if ( DomUtil::readBoolEntry(dom, "/kdevvstudio/run/useglobalprogram", false) || !m_widget->activeTarget() )
//   {
//     cwd = defaultRunDirectory("kdevvstudio");
//   }else
//   {
//     cwd = DomUtil::readEntry( dom, "/kdevvstudio/run/cwd/"+m_widget->activeTarget()->name );
//   }
//   if ( cwd.isEmpty() )
//     cwd = buildDirectory() +"/"+ URLUtil::getRelativePath( topsourceDirectory(), projectDirectory() ) +"/"+m_widget->activeDirectory();
//
//   return cwd;
	return "";
}

/** Retuns the currently selected main program
 * If no main Program was selected in the Run Options dialog
 * use the currently active target instead.
 * The returned string can be:
 *   if /kdevvstudio/run/directoryradio == executable
 *        The executable name
 *   if /kdevvstudio/run/directoryradio == build
 *        The path to executable relative to build directory
 *   if /kdevvstudio/run/directoryradio == custom or relative == false
 *        The absolute path to executable
*/
QString VStudioPart::mainProgram() const
{
//   QDomDocument * dom = projectDom();
//
//   if ( !dom ) return QString();
//
//   if ( DomUtil::readBoolEntry(*dom, "/kdevvstudio/run/useglobalprogram", false) )
//   {
//     QString DomMainProgram = DomUtil::readEntry(*dom, "/kdevvstudio/run/mainprogram");
//
//     if ( DomMainProgram.isEmpty() ) return QString();
//
//     if ( DomMainProgram.startsWith("/") )   // assume absolute path
//     {
//       return DomMainProgram;
//     }
//     else // assume builddir relative path
//     {
//       QString relprojectpath = URLUtil::getRelativePath( topsourceDirectory(), projectDirectory() );
//       if ( !relprojectpath.isEmpty() )
//         relprojectpath = "/" + relprojectpath;
//       return buildDirectory() + relprojectpath + "/" + DomMainProgram;
//     }
//
//   }
//   else // If no Main Program was specified, return the active target
//   {
//     TargetItem* titem = m_widget->activeTarget();
//
//     if ( !titem ) {
//       KMessageBox::error( m_widget, i18n("There's no active target!\n"
//                                          "Unable to determine the main program"), i18n("No active target found") );
//       kdDebug ( 9020 ) << k_funcinfo << "Error! : There's no active target! -> Unable to determine the main program in AutoProjectPart::mainProgram()" << endl;
//       return QString::null;
//     }
//
//     if ( titem->primary != "PROGRAMS" ) {
//       KMessageBox::error( m_widget, i18n("Active target \"%1\" isn't binary ( %2 ) !\n"
//                                          "Unable to determine the main program. If you want this\n"
//                                          "to be the active target, set a main program under\n"
//                                          "Project -> Project Options -> Run Options").arg(titem->name).arg(titem->primary), i18n("Active target is not a library") );
//       kdDebug ( 9020 ) << k_funcinfo << "Error! : Active target isn't binary (" << titem->primary << ") ! -> Unable to determine the main program in AutoProjectPart::mainProgram()" << endl;
//       return QString::null;
//     }
//
//     QString relprojectpath = URLUtil::getRelativePath( topsourceDirectory(), projectDirectory() );
//     if ( !relprojectpath.isEmpty() )
//       relprojectpath = "/" + relprojectpath;
//     return buildDirectory() + relprojectpath + "/" + activeDirectory() + "/" + titem->name;
//   }
	return "";
}

/** Retuns a QString with the debug command line arguments */
QString VStudioPart::debugArguments() const
{
//   QDomDocument &dom = *projectDom();
//
//   if ( DomUtil::readBoolEntry(dom, "/kdevvstudio/run/useglobalprogram", false) || !m_widget->activeTarget() )
//   {
//     return DomUtil::readEntry(dom, "/kdevvstudio/run/globaldebugarguments");
//   }else
//   {
//     return DomUtil::readEntry(dom, "/kdevvstudio/run/debugarguments/" + m_widget->activeTarget()->name);
//   }
	return "";
}

/** Retuns a QString with the run command line arguments */
QString VStudioPart::runArguments() const
{
//   QDomDocument &dom = *projectDom();
//
//   if ( DomUtil::readBoolEntry(dom, "/kdevvstudio/run/useglobalprogram", false) || !m_widget->activeTarget() )
//   {
//     return DomUtil::readEntry(dom, "/kdevvstudio/run/programargs");
//   }else
//   {
//     return DomUtil::readEntry(dom, "/kdevvstudio/run/runarguments/" + m_widget->activeTarget()->name);
//   }
	return "";
}

QString VStudioPart::activeDirectory() const
{
//   return m_widget->activeDirectory();
	return "";
}

QStringList VStudioPart::allFiles() const
{
//   return m_widget->allFiles();
	return "";
}

// void VStudioPart::setWantautotools() {
//   QDomDocument &dom = *projectDom();
//   QDomElement el  = DomUtil::elementByPath(dom, "/kdevvstudio/make");
//   if ( el.namedItem("envvars").isNull() ) {
//     DomUtil::PairList list;
//     list << DomUtil::Pair("WANT_AUTOCONF_2_5", "1");
//     list << DomUtil::Pair("WANT_AUTOMAKE_1_6", "1");
//     DomUtil::writePairListEntry(dom, "/kdevvstudio/make/envvars", "envvar", "name", "value", list);
//   }
// }


// QString VStudioPart::makeEnvironment() const {
//   // Get the make environment variables pairs into the environstr string
//   // in the form of: "ENV_VARIABLE=ENV_VALUE"
//   // Note that we quote the variable value due to the possibility of
//   // embedded spaces
//   DomUtil::PairList envvars =
//     DomUtil::readPairListEntry(*projectDom(), "/kdevvstudio/make/envvars", "envvar", "name", "value");
//
//   QString environstr;
//   DomUtil::PairList::ConstIterator it;
//   for (it = envvars.begin(); it != envvars.end(); ++it) {
//     environstr += (*it).first;
//     environstr += "=";
//     environstr += EnvVarTools::quote((*it).second);
//     environstr += " ";
//   }
//
//   KConfigGroup grp( kapp->config(), "MakeOutputView" );
//   if ( grp.readBoolEntry( "ForceCLocale", true ) )
//     environstr += "LC_MESSAGES="+EnvVarTools::quote("C")+" "+"LC_CTYPE="+EnvVarTools::quote("C")+" ";
//
//   return environstr;
// }

void VStudioPart::addFile ( const QString &/*fileName*/ )
{
//   QStringList fileList;
//   fileList.append ( fileName );
//
//   this->addFiles ( fileList );
}

void VStudioPart::addFiles ( const QStringList& /*fileList*/ )
{
//   QString directory, name;
//   QStringList::ConstIterator it;
//   bool messageBoxShown = false;
//
//   for ( it = fileList.begin(); it != fileList.end(); ++it ) {
//     int pos = ( *it ).findRev('/');
//     if (pos != -1) {
//       directory = ( *it ).left(pos);
//       name = ( *it ).mid(pos+1);
//     }
//     else {
//       directory = "";
//       name = ( *it );
//     }
//
//     if (directory != m_widget->activeDirectory() || directory.isEmpty()) {
//       if ( !messageBoxShown ) {
//         KMessageBox::information(m_widget, i18n("The directory you selected is not the active directory.\n"
//                                                 "You should 'activate' the target you're currently working on in VS Manager.\n"
//                                                 "Just right-click a target and choose 'Make Target Active'."),
//                                  i18n ( "No Active Target Found" ), "No vs manager active target warning" );
//         messageBoxShown = true;
//       }
//     }
//   }
//
//   m_widget->addFiles(fileList);
}

void VStudioPart::removeFile ( const QString &/*fileName*/ )
{
//   QStringList fileList;
//   fileList.append ( fileName );
//
//   this->removeFiles ( fileList );
}

void VStudioPart::removeFiles ( const QStringList& /*fileList*/ )
{
	/// \FIXME m_widget->removeFiles does nothing!
//   m_widget->removeFiles ( fileList );
//   emit removedFilesFromProject ( fileList );
}

// QStringList VStudioPart::allBuildConfigs() const {
//   QDomDocument &dom = *projectDom();
//
//   QStringList allConfigs;
//   allConfigs.append("default");
//
//   QDomNode node = dom.documentElement().namedItem("kdevvstudio").namedItem("configurations");
//   QDomElement childEl = node.firstChild().toElement();
//   while (!childEl.isNull()) {
//     QString config = childEl.tagName();
//     kdDebug(9020) << k_funcinfo << "Found config " << config << endl;
//     if (config != "default")
//       allConfigs.append(config);
//     childEl = childEl.nextSibling().toElement();
//   }
//
//   return allConfigs;
// }

// QString VStudioPart::currentBuildConfig() const {
//   QDomDocument &dom = *projectDom();
//
//   QString config = DomUtil::readEntry(dom, "/kdevvstudio/general/useconfiguration");
//   if (config.isEmpty() || !allBuildConfigs().contains(config))
//     config = "default";
//
//   return config;
// }

QString VStudioPart::buildDirectory() const
{
//   QString prefix = "/kdevvstudio/configurations/" + currentBuildConfig() + "/";
//
//	QString builddir = DomUtil::readEntry ( *projectDom(), prefix + "builddir" );
//   if (builddir.isEmpty())            return topsourceDirectory();
//   else if (builddir.startsWith("/")) return builddir;
//   else                               return projectDirectory() + "/" + builddir;
//	return projectDirectory() + "/" + builddir;
	return "";
}

// QString VStudioPart::topsourceDirectory() const
// {
//   QString prefix = "/kdevvstudio/configurations/" + currentBuildConfig() + "/";
//
//   QString topsourcedir = DomUtil::readEntry(*projectDom(), prefix + "topsourcedir");
//   if (topsourcedir.isEmpty())
//     return projectDirectory();
//   else if (topsourcedir.startsWith("/"))
//     return topsourcedir;
//   else
//     return projectDirectory() + "/" + topsourcedir;
// }

// QString VStudioPart::constructMakeCommandLine(const QString &dir, const QString &target) const {
//   QString preCommand;
//   QFileInfo fi1();
//   kdDebug(9020) << k_funcinfo << "Looking for Makefile in " << dir << endl;
//   if ( !QFile::exists(dir + "/GNUmakefile") && !QFile::exists(dir + "/makefile")
//        && ! QFile::exists(dir + "/Makefile") )
//   {
//     if (!QFile::exists(buildDirectory() + "/configure"))
//     {
//       int r = KMessageBox::questionYesNo(m_widget, i18n("%1\nThere is no Makefile in this directory\n"
//                                          "and no configure script for this project.\n"
//                                          "Run automake & friends and configure first?").arg(buildDirectory()), QString::null, i18n("Run Them"), i18n("Do Not Run"));
//       if (r == KMessageBox::No)
//         return QString::null;
//       preCommand = makefileCvsCommand();
//       if (preCommand.isNull())
//         return QString::null;
//       preCommand += " && ";
//       preCommand += configureCommand() + " && ";
//     }
//     else
//     {
//       int r = KMessageBox::questionYesNo(m_widget, i18n("%1\nThere is no Makefile in this directory. Run 'configure' first?").arg(dir), QString::null, i18n("Run configure"), i18n("Do Not Run"));
//       if (r == KMessageBox::No)
//         return QString::null;
//       preCommand = configureCommand() + " && ";
//     }
//   }
//   QDomDocument &dom = *projectDom();
//
//   QString cmdline = DomUtil::readEntry(dom, "/kdevvstudio/make/makebin");
//   int prio = DomUtil::readIntEntry(dom, "/kdevvstudio/make/prio");
//   QString nice;
//   kdDebug(9020) << k_funcinfo << "nice = " << prio<< endl;
//   if (prio != 0)
//   {
//     nice = QString("nice -n%1 ").arg(prio);
//   }
//
//   if (cmdline.isEmpty())
//     cmdline = MAKE_COMMAND;
//   if (!DomUtil::readBoolEntry(dom, "/kdevvstudio/make/abortonerror"))
//     cmdline += " -k";
//   bool runmultiple = DomUtil::readBoolEntry(dom, "/kdevvstudio/make/runmultiplejobs");
//   int jobs = DomUtil::readIntEntry(dom, "/kdevvstudio/make/numberofjobs");
//   if (runmultiple && jobs != 0)
//   {
//     cmdline += " -j";
//     cmdline += QString::number(jobs);
//   }
//   if (DomUtil::readBoolEntry(dom, "/kdevvstudio/make/dontact"))
//     cmdline += " -n";
//
//   cmdline += " ";
//   cmdline += target;
//   cmdline.prepend(nice);
//   cmdline.prepend(makeEnvironment());
//
//   QString dircmd = "cd ";
//   dircmd += KProcess::quote(dir);
//   dircmd += " && ";
//
//   return preCommand + dircmd + cmdline;
// }

// void VStudioPart::startMakeCommand(const QString &dir, const QString &target, bool withKdesu) {
//   if (partController()->saveAllFiles()==false)
//     return; //user cancelled
//   kdDebug(9020) << "startMakeCommand:" << dir << ": "<< target << endl;
//   m_buildCommand = constructMakeCommandLine(dir, target);
//
//   if (withKdesu)
//     m_buildCommand = "kdesu -t -c '" + m_buildCommand + "'";
//
//   if (!m_buildCommand.isNull())
//     makeFrontend()->queueCommand(dir, m_buildCommand);
// }

/** Adds the make command for the libraries that the target depends on
  * to the make frontend queue (this is a recursive function) */
// bool VStudioPart::queueInternalLibDependenciesBuild(TargetItem* titem, QStringList& alreadyScheduledDeps) {
//   QString addstr = (titem->primary == "PROGRAMS")? titem->ldadd : titem->libadd;
//   QStringList l2 = QStringList::split(QRegExp("[ \t]"), addstr); // list of dependencies
//   QString tdir;          // temp target directory
//   QString tname;         // temp target name
//   QString tcmd;          // temp command line
//   QStringList::Iterator l2it;
//   for (l2it = l2.begin(); l2it != l2.end(); ++l2it)
//   {
//     QString dependency = *l2it;
//     if (dependency.startsWith("$(top_builddir)/"))
//     {
//       // These are the internal libraries
//       dependency.remove("$(top_builddir)/");
//
//       if ( !alreadyScheduledDeps.contains(*l2it) )
//       {
//         alreadyScheduledDeps << *l2it;
//         tdir = buildDirectory();
//         if (!tdir.endsWith("/") && !tdir.isEmpty())
//           tdir += "/";
//         int pos = dependency.findRev('/');
//         if (pos == -1)
//         {
//           tname = dependency;
//         }
//         else
//         {
//           tdir += dependency.left(pos+1);
//           tname = dependency.mid(pos+1);
//         }
//         kdDebug(9020) << "Scheduling : <" << tdir << ">  target <" << tname << ">" << endl;
//         // Recursively queue the dependencies for building
//         SubprojectItem *spi = m_widget->subprojectItemForPath( dependency.left(pos) );
//         if (spi)
//         {
//           QPtrList< TargetItem > tl = spi->targets;
//           // Cycle through the list of targets to find the one we're looking for
//           TargetItem *ti = tl.first();
//           do
//           {
//             if (ti->name == tname)
//             {
//               // found it: queue it and stop looking
//               if ( !queueInternalLibDependenciesBuild(ti, alreadyScheduledDeps) )
//                 return false;
//               break;
//             }
//             ti = tl.next();
//           }
//           while (ti);
//         }
//
//         kdDebug(9020) << "queueInternalLibDependenciesBuild:" << tdir << ": "<< tname << endl;
//         tcmd = constructMakeCommandLine(tdir, tname);
//         if (!tcmd.isNull())
//         {
//           makeFrontend()->queueCommand( tdir, tcmd);
//         }
//       }else
//       {
//         //Message box about circular deps.
//         tdir = buildDirectory();
//         if (!tdir.endsWith("/") && !tdir.isEmpty())
//           tdir += "/";
//         int pos = dependency.findRev('/');
//         if (pos == -1)
//         {
//           tname = dependency;
//         }
//         else
//         {
//           tdir += dependency.left(pos+1);
//           tname = dependency.mid(pos+1);
//         }
//         KMessageBox::error( 0, i18n("Found a circular dependecy in the project, between this target and %1.\nCan't build this project until this is resolved").arg(tname), i18n("Circular Dependecy found") );
//         return false;
//       }
//     }
//   }
//   return true;
// }

void VStudioPart::slotBuild()
{
//   m_lastCompilationFailed = false;
//
//   if ( m_needMakefileCvs )
//   {
//     slotMakefilecvs();
//     slotConfigure();
//     m_needMakefileCvs = false;
//   }
//
//   startMakeCommand(buildDirectory(), QString::fromLatin1(""));
}

void VStudioPart::buildTarget ( QString /*relpath*/, TargetItem* /*titem*/ )
{
//   if ( !titem )
//     return;
//
//   //m_lastCompilationFailed = false;
//
//   // Calculate the complete name of the target and store it in name
//   QString name = titem->name;
//   if ( titem->primary == "KDEDOCS" )
//     name = "index.cache.bz2";
//
//   // Calculate the full path of the target and store it in path
//   QString path = buildDirectory();
//   if (!path.endsWith("/") && !path.isEmpty())
//     path += "/";
//   if (relpath.at(0) == '/')
//     path += relpath.mid(1);
//   else
//     path += relpath;
//
//   // Save all files once
//   partController()->saveAllFiles();
//
//   // Add the make command for the libraries that the target depends on to the make frontend queue
//   // if this recursive behavour is un-wanted comment the next line
//   QStringList deps;
//   if ( !queueInternalLibDependenciesBuild(titem, deps) )
//     return;
//
//   // Calculate the "make" command line for the target
//   //QString relpath = dir.path().mid( projectDirectory().length() );
//   m_runProg=buildDirectory() + "/" + relpath+"/"+name;
//   kdDebug(9020) << "buildTarget:" << buildDirectory()<< endl;
//   kdDebug(9020) << "buildTarget:" << relpath << "  " << path << ": "<< name << " : " << m_runProg << endl;
//   QString tcmd = constructMakeCommandLine( path, name );
//
//   // Call make
//   if (!tcmd.isNull())
//   {
//     m_buildCommand = tcmd;
//     makeFrontend()->queueCommand( path, tcmd);
//   }
}

void VStudioPart::slotBuildActiveTarget()
{
//   // Get a pointer to the active target
//   TargetItem* titem = m_widget->activeTarget();
//
//   if ( !titem )
//     return;
//
//   // build it
//   buildTarget( URLUtil::getRelativePath( topsourceDirectory(), projectDirectory() ) + "/" + activeDirectory(), titem);
}

void VStudioPart::slotCompileFile()
{
//   KParts::ReadWritePart *part = dynamic_cast<KParts::ReadWritePart*>(partController()->activePart());
//   if (!part || !part->url().isLocalFile())
//     return;
//
//   QString fileName = part->url().path();
//   QFileInfo fi(fileName);
//   QString sourceDir = fi.dirPath();
//   QString baseName = fi.baseName(true);
//   kdDebug(9020) << "Compiling " << fileName
//   << " in dir " << sourceDir
//   << " with baseName " << baseName << endl;
//
//   QString projectDir = projectDirectory();
//   if (!sourceDir.startsWith(projectDir)) {
//     KMessageBox::sorry(m_widget, i18n("Can only compile files in directories which belong to the project."));
//     return;
//   }
//
//   QString buildDir = buildDirectory() + sourceDir.mid(projectDir.length());
//   QString target = baseName + ".lo";
//   kdDebug(9020) << "builddir " << buildDir << ", target " << target << endl;
//
//   startMakeCommand(buildDir, target);
}

// QString VStudioPart::configureCommand() const {
//   QDomDocument &dom = *projectDom();
//   QString prefix = "/kdevautoproject/configurations/" + currentBuildConfig() + "/";
//
//   QString cmdline = "\"" + topsourceDirectory();
//   cmdline += "/configure\"";
//   QString cc = DomUtil::readEntry(dom, prefix + "ccompilerbinary");
//   if (!cc.isEmpty())
//     cmdline.prepend(QString("CC=%1 ").arg(cc));
//   QString cflags = DomUtil::readEntry(dom, prefix + "cflags");
//   if (!cflags.isEmpty())
//     cmdline.prepend(QString("CFLAGS=\"%1\" ").arg(cflags));
//   QString cxx = DomUtil::readEntry(dom, prefix + "cxxcompilerbinary");
//   if (!cxx.isEmpty())
//     cmdline.prepend(QString("CXX=%1 ").arg(cxx));
//   QString cxxflags = DomUtil::readEntry(dom, prefix + "cxxflags");
//   if (!cxxflags.isEmpty())
//     cmdline.prepend(QString("CXXFLAGS=\"%1\" ").arg(cxxflags));
//   QString f77 = DomUtil::readEntry(dom, prefix + "f77compilerbinary");
//   if (!f77.isEmpty())
//     cmdline.prepend(QString("F77=%1 ").arg(f77));
//   QString fflags = DomUtil::readEntry(dom, prefix + "f77flags");
//   if (!fflags.isEmpty())
//     cmdline.prepend(QString("FFLAGS=\"%1\" ").arg(fflags));
//   QString cppflags = DomUtil::readEntry(dom, prefix + "cppflags");
//   if (!cppflags.isEmpty())
//     cmdline.prepend(QString("CPPFLAGS=\"%1\" ").arg(cppflags));
//   QString ldflags = DomUtil::readEntry(dom, prefix + "ldflags");
//   if (!ldflags.isEmpty())
//     cmdline.prepend(QString("LDFLAGS=\"%1\" ").arg(ldflags));
//
//   QString configargs = DomUtil::readEntry(dom, prefix + "configargs");
//   if (!configargs.isEmpty()) {
//     cmdline += " ";
//     cmdline += configargs;
//   }
//
//   DomUtil::PairList envvars =
//     DomUtil::readPairListEntry(*projectDom(), prefix + "envvars", "envvar", "name", "value");
//
//   QString environstr;
//   DomUtil::PairList::ConstIterator it;
//   for (it = envvars.begin(); it != envvars.end(); ++it) {
//     environstr += (*it).first;
//     environstr += "=";
//     environstr += EnvVarTools::quote((*it).second);
//     environstr += " ";
//   }
//   cmdline.prepend(environstr);
//
//   QString builddir = buildDirectory();
//   QString dircmd;
//
//   // if the build directory doesn't exist, add it's creation to the configureCommand
//   if ( !QFile::exists(builddir)) {
//     dircmd = "mkdir ";
//     dircmd += KProcess::quote(builddir);
//     dircmd += " && ";
//   }
//
//   // add "cd into the build directory" to the configureCommand
//   dircmd += "cd ";
//   dircmd += KProcess::quote(builddir);
//   dircmd += " && ";
//
//   return dircmd + cmdline;
// }

// void VStudioPart::slotConfigure()
// {
//   QString cmdline = configureCommand();
//   if (cmdline.isNull())
//     return;
//
//   makeFrontend()->queueCommand(buildDirectory(), cmdline);
// }

// QString VStudioPart::makefileCvsCommand() const
// {
//   kdDebug(9020) << "makefileCvsCommand: runDirectory       :" << runDirectory() << ":" <<endl;
//   kdDebug(9020) << "makefileCvsCommand: topsourceDirectory :" << topsourceDirectory() << ":" <<endl;
//   kdDebug(9020) << "makefileCvsCommand: makeEnvironment    :" << makeEnvironment() << ":" <<endl;
//   kdDebug(9020) << "makefileCvsCommand: currentBuildConfig :" << currentBuildConfig() << ":" <<endl;
//
//
//   QString cmdline = DomUtil::readEntry(*projectDom(), "/kdevvstudio/make/makebin");
//   if (cmdline.isEmpty())
//     cmdline = MAKE_COMMAND;
//
//   int prio = DomUtil::readIntEntry(*projectDom(), "/kdevvstudio/make/prio");
//   QString nice;
//   kdDebug(9020) << "makefileCvsCommand() nice = " << prio<< endl;
//   if (prio != 0) {
//     nice = QString("nice -n%1 ").arg(prio);
//   }
//
//   if (QFile::exists(topsourceDirectory() + "/Makefile.cvs"))
//     cmdline += " -f Makefile.cvs";
//   else if (QFile::exists(topsourceDirectory() + "/Makefile.dist"))
//     cmdline += " -f Makefile.dist";
//   else if (QFile::exists(topsourceDirectory() + "/autogen.sh"))
//     cmdline = "./autogen.sh";
//   else {
//     KMessageBox::sorry(m_widget, i18n("There is neither a Makefile.cvs file nor an "
//                                       "autogen.sh script in the project directory."));
//     return QString::null;
//   }
//
//   cmdline.prepend(nice);
//   cmdline.prepend(makeEnvironment());
//
//   QString dircmd = "cd ";
//   dircmd += KProcess::quote(topsourceDirectory());
//   dircmd += " && ";
//
//   return dircmd + cmdline;
// }

// void VStudioPart::slotMakefilecvs() {
//   QString cmdline = makefileCvsCommand();
//   if ( cmdline.isNull() )
//     return;
//
//   makeFrontend()->queueCommand(projectDirectory(), cmdline);
// }

// void VStudioPart::slotInstall() {
//   startMakeCommand(buildDirectory(), QString::fromLatin1("install"));
// }

// void VStudioPart::slotInstallWithKdesu() {
//   // First issue "make" to build the entire project with the current user
//   // This way we make sure all files are up to date before we do the "make install"
//   slotBuild();
//
//   // After that issue "make install" with the root user
//   startMakeCommand(buildDirectory(), QString::fromLatin1("install"), true);
// }

void VStudioPart::slotClean()
{
//   startMakeCommand(buildDirectory(), QString::fromLatin1("clean"));
}

// void VStudioPart::slotDistClean() {
//   startMakeCommand(buildDirectory(), QString::fromLatin1("distclean"));
// }

// void VStudioPart::slotMakeMessages() {
//   startMakeCommand(buildDirectory(), QString::fromLatin1("package-messages"));
// }

/** Checks if the currently selected main program or,
  * if no main Program was selected in the Run Options dialog,
  * the currently active target is up-to-date and builds it if necessary.
  * In the end checks if the program is already running and if not calls the
  * slotExecute2() function to execute it or asks the user what to do.
  */
// void VStudioPart::slotExecute() {
//   partController()->saveAllFiles();
//   QDomDocument &dom = *projectDom();
//
//   m_runProg=m_runProg.isEmpty()?mainProgram():m_runProg;
//
//   bool _auto = false;
//   if ( DomUtil::readBoolEntry(dom, "/kdevvstudio/run/autocompile", true) && isDirty() ){
//     m_executeAfterBuild = true;
//     if ( DomUtil::readBoolEntry(dom, "/kdevvstudio/run/useglobalprogram", false) ){
//       // A Main Program was specified, build all targets because we don't know which is it
//       kdDebug(9020) << "slotExecute: before slotBuild" << endl;
//       slotBuild();
//
//     }
//     else{
//       // If no Main Program was specified, build the active target
//       kdDebug(9020) << "slotExecute: before slotBuildActiveTarget" << endl;
//       slotBuildActiveTarget();
//     }
//     _auto = true;
//   }
//
//   if ( DomUtil::readBoolEntry(dom, "/kdevvstudio/run/autoinstall", false) && isDirty() ){
//     m_executeAfterBuild = true;
//     // Use kdesu??
//     if ( DomUtil::readBoolEntry(dom, "/kdevvstudio/run/autokdesu", false) ){
//       //slotInstallWithKdesu assumes that it hasn't just been build...
//       kdDebug(9020) << "slotExecute: before startMakeCommand" << endl;
//       _auto ? slotInstallWithKdesu() : startMakeCommand(buildDirectory(), QString::fromLatin1("install"), true);
//     }
//     else{
//       kdDebug(9020) << "slotExecute: before slotInstall" << endl;
//       slotInstall();
//     }
//     _auto = true;
//   }
//
//   if ( _auto ){
//     m_runProg.truncate(0);
//     return;
//   }
//
//   if (appFrontend()->isRunning()) {
//     if (KMessageBox::questionYesNo(m_widget, i18n("Your application is currently running. Do you want to restart it?"), i18n("Application Already Running"), i18n("&Restart Application"), i18n("Do &Nothing")) == KMessageBox::No)
//       return;
//     connect(appFrontend(), SIGNAL(processExited()), SLOT(slotExecute2()));
//     appFrontend()->stopApplication();
//     return;
//   }
//   kdDebug(9020) << "slotExecute: before slotExecute2" << endl;
//   slotExecute2();
// }

// void VStudioPart::executeTarget(const QDir& dir, const TargetItem* titem) {
//   m_executeAfterBuild=true;
//   partController()->saveAllFiles();
//
//   bool is_dirty = false;
//   QDateTime t = QFileInfo(dir , titem->name ).lastModified();
//   QPtrListIterator<FileItem> it( titem->sources );
//   for ( ; it.current() ; ++it )
//   {
//     if ( t < QFileInfo(dir , (*it)->name).lastModified())
//       is_dirty = true;
//   }
//
//   if ( DomUtil::readBoolEntry(*projectDom(), "/kdevvstudio/run/autocompile", true) && is_dirty )
//   {
//     connect( makeFrontend(), SIGNAL(commandFinished(const QString&)), this, SLOT(slotExecuteTargetAfterBuild(const QString&)) );
//     connect( makeFrontend(), SIGNAL(commandFailed(const QString&)), this, SLOT(slotNotExecuteTargetAfterBuildFailed(const QString&)) );
//
//     m_runProg=titem->name;
//     m_executeTargetAfterBuild.first = dir;
//     m_executeTargetAfterBuild.second = const_cast<TargetItem*>(titem);
//
//     QString relpath = "/" + URLUtil::getRelativePath( topsourceDirectory(), projectDirectory() ) + "/" + m_widget->selectedSubproject()->subdir;
//     kdDebug(9020) << "executeTarget: before buildTarget " << relpath << endl;
//     buildTarget(relpath, const_cast<TargetItem*>(titem));
//     return;
//   }
//
//
//   bool inTerminal = DomUtil::readBoolEntry(*projectDom(), "/kdevvstudio/run/terminal");
//
//   QString program = environString();
//
//   if ( !titem ) {
//     KMessageBox::error( m_widget, i18n("There's no active target!\n"
//                                        "Unable to determine the main program"), i18n("No active target found") );
//     kdDebug ( 9020 ) << k_funcinfo << "Error! : There's no active target! -> Unable to determine the main program in AutoProjectPart::mainProgram()" << endl;
//     program += titem->name;
//   }else if ( titem->primary != "PROGRAMS" ) {
//     KMessageBox::error( m_widget, i18n("Active target \"%1\" isn't binary ( %2 ) !\n"
//                                        "Unable to determine the main program. If you want this\n"
//                                        "to be the active target, set a main program under\n"
//                                        "Project -> Project Options -> Run Options").arg(titem->name).arg(titem->primary), i18n("Active target is not a library") );
//     kdDebug ( 9020 ) << k_funcinfo << "Error! : Active target isn't binary (" << titem->primary << ") ! -> Unable to determine the main program in AutoProjectPart::mainProgram()" << endl;
//     program += titem->name;
//   }else
//     program += buildDirectory() + "/" + URLUtil::getRelativePath( topsourceDirectory(), projectDirectory() ) + "/" + m_widget->selectedSubproject()->relativePath()+ "/" + titem->name;
//
//   QString args = DomUtil::readEntry(*projectDom(), "/kdevvstudio/run/runarguments/" + titem->name);
//
//   program += " " + args;
//   kdDebug(9020) << "executeTarget:cmd=" << dir.path() << " " << program << endl;
//   appFrontend()->startAppCommand(dir.path(), program ,inTerminal);
//   m_executeAfterBuild=false;
// }

// void VStudioPart::slotExecuteTargetAfterBuild(const QString& command) {
//   kdDebug(9020) << "slotExecuteTargetAfterBuild " << command << endl;
//   if ( m_executeAfterBuild && constructMakeCommandLine(m_executeTargetAfterBuild.first.path(), m_executeTargetAfterBuild.second->name) == command )
//   {
//     disconnect( makeFrontend(), SIGNAL(commandFinished(const QString&)), this, SLOT(slotExecuteAfterTargetBuild()) );
//     disconnect( makeFrontend(), SIGNAL(commandFailed(const QString&)), this, SLOT(slotExecuteAfterTargetBuildFailed()) );
//     kdDebug(9020) << "slotExecuteTargetAfterBuild " << endl;
//     executeTarget(m_executeTargetAfterBuild.first, m_executeTargetAfterBuild.second);
//   }
// }

// void VStudioPart::slotNotExecuteTargetAfterBuildFailed(const QString& command) {
//   kdDebug(9020) << "executeTargetAfterBuildFailed" << endl;
//   if ( constructMakeCommandLine(m_executeTargetAfterBuild.first.path(), m_executeTargetAfterBuild.second->name) == command )
//   {
//     m_executeAfterBuild=false;
//     disconnect( makeFrontend(), SIGNAL(commandFinished(const QString&)), this, SLOT(slotExecuteTargetAfterBuild()) );
//     disconnect( makeFrontend(), SIGNAL(commandFailed(const QString&)), this, SLOT(slotNotExecuteTargetAfterBuildFailed()) );
//   }
// }

/* Get the run environment variables pairs into the environstr string
 * in the form of: "ENV_VARIABLE=ENV_VALUE"
 * Note that we quote the variable value due to the possibility of
 * embedded spaces. */
// QString VStudioPart::environString() const {
//   DomUtil::PairList envvars = runEnvironmentVars();
//   QString environstr;
//   DomUtil::PairList::ConstIterator it;
//   for (it = envvars.begin(); it != envvars.end(); ++it) {
//     environstr += (*it).first;
//     environstr += "=";
//     environstr += EnvVarTools::quote((*it).second);
//     environstr += " ";
//   }
//   return environstr;
// }

/** Executes the currently selected main program.
  * If no main Program was selected in the Run Options dialog
  * the currently active target is executed instead.
  */
// void VStudioPart::slotExecute2() {
//   disconnect(appFrontend(), SIGNAL(processExited()), this, SLOT(slotExecute2()));
//
//   if (m_runProg.isEmpty()){
//     // Do not execute non executable targets
//     return;
//   }
//
//   QString program = environString();
//   // Adds the ./ that is necessary to execute the program in bash shells
//   if (!m_runProg.startsWith("/")){
//     program += "./";
//   }
//   program += m_runProg;
//   program += " " + runArguments();
//
//   bool inTerminal = DomUtil::readBoolEntry(*projectDom(), "/kdevvstudio/run/terminal");
//
//   kdDebug(9020) << "slotExecute2: runDirectory: <" << runDirectory() << ">" <<endl;
//   kdDebug(9020) << "slotExecute2: environstr  : <" << environString() << ">" <<endl;
//   kdDebug(9020) << "slotExecute2: mainProgram : <" << mainProgram() << ">" <<endl;
//   kdDebug(9020) << "slotExecute2: runArguments: <" << runArguments() << ">" <<endl;
//   kdDebug(9020) << "slotExecute2: program     : <" << program << ">" <<endl;
//
//   appFrontend()->startAppCommand(runDirectory(), program, inTerminal);
//   m_executeAfterBuild=false;
//   m_runProg.truncate(0);
// }

void VStudioPart::slotAddTranslation()
{
//   AddTranslationDialog dlg(this, m_widget);
//   dlg.exec();
}

// // // void VStudioPart::slotBuildConfigChanged(const QString &config) {
// // //   DomUtil::writeEntry(*projectDom(), "/kdevvstudio/general/useconfiguration", config);
// // //   kdDebug(9020) << "Changed used configuration to " << config << endl;
// // // }

// void VStudioPart::slotBuildConfigAboutToShow() {
//   QStringList l = allBuildConfigs();
//   buildConfigAction->setItems(l);
//   buildConfigAction->setCurrentItem(l.findIndex(currentBuildConfig()));
// }

void VStudioPart::restorePartialProjectSession ( const QDomElement* /*el*/ )
{
//   m_widget->restoreSession ( el );
}

void VStudioPart::savePartialProjectSession ( QDomElement* /*el*/ )
{
//   QDomDocument domDoc = el->ownerDocument();
//
//   KMessageBox::information ( 0, "Hallo, Welt!" );
//
//   kdDebug ( 9020 ) << k_funcinfo << "1" << endl;
//
//   if ( domDoc.isNull() )
//   {
//     kdDebug ( 9020 ) << k_funcinfo << "2" << endl;
//     return;
//   }
//
//   kdDebug ( 9020 ) << k_funcinfo << "3" << endl;
//
//   m_widget->saveSession ( el );
}

// void VStudioPart::slotCommandFinished( const QString& command ) {
//   kdDebug(9020) << k_funcinfo << endl;
//
//   if ( m_buildCommand != command )
//     return;
//
//   m_buildCommand = QString::null;
//
//   m_timestamp.clear();
//   QStringList fileList = allFiles();
//   QStringList::Iterator it = fileList.begin();
//   while ( it != fileList.end() ){
//     QString fileName = *it;
//     ++it;
//
//     m_timestamp[ fileName ] = QFileInfo( projectDirectory(), fileName ).lastModified();
//   }
//
//   emit projectCompiled();
//
//   // reset the "last compilation has failed" flag
//   m_lastCompilationFailed = false;
//
//   if ( m_executeAfterBuild ){
//     slotExecute();
//   }
// }

// void VStudioPart::slotCommandFailed( const QString& /*command*/ ) {
//   kdDebug(9020) << "slotCommandFinished " << k_funcinfo << endl;
//
//   m_lastCompilationFailed = true;
//   m_executeAfterBuild=false;
// }

bool VStudioPart::isDirty()
{
//   if (m_lastCompilationFailed) return true;
//
//   QStringList fileList = allFiles();
//   QStringList::Iterator it = fileList.begin();
//   while ( it != fileList.end() ){
//     QString fileName = *it;
//     ++it;
//
//     QMap<QString, QDateTime>::Iterator it = m_timestamp.find( fileName );
//     QDateTime t = QFileInfo( projectDirectory(), fileName ).lastModified();
//     if ( it == m_timestamp.end() || *it != t ){
//       return true;
//     }
//   }
	return false;
}

// void VStudioPart::needMakefileCvs() {
//   m_needMakefileCvs = true;
// }

// bool VStudioPart::isKDE() const
// {
// 	return m_isKDE;
// }

KDevProject::Options VStudioPart::options() const
{
	return UsesAutotoolsBuildSystem;
}

QStringList recursiveATFind ( const QString &currDir, const QString &baseDir )
{
	kdDebug ( 9020 ) << "Dir " << currDir << endl;
	QStringList fileList;

	if ( !currDir.contains ( "/.." ) && !currDir.contains ( "/." ) )
	{
		QDir dir ( currDir );
		QStringList dirList = dir.entryList ( QDir::Dirs );
		QStringList::Iterator idx = dirList.begin();
		for ( ; idx != dirList.end(); ++idx )
		{
			fileList += recursiveATFind ( currDir + "/" + ( *idx ),baseDir );
		}
		QStringList newFiles = dir.entryList ( "*.am *.in" );
		idx = newFiles.begin();
		for ( ; idx != newFiles.end(); ++idx )
		{
			QString file = currDir + "/" + ( *idx );
			fileList.append ( file.remove ( baseDir ) );
		}
	}

	return fileList;
}

QStringList VStudioPart::distFiles() const
{
	QStringList sourceList = allFiles();
//   // Scan current source directory for any .pro files.
//   QString projectDir = projectDirectory();
//   QDir dir(projectDir);
//   QDir admin(projectDir +"/admin");
//   QStringList files = dir.entryList( "Makefile.cvs Makefile.am configure* INSTALL README NEWS TODO ChangeLog COPYING AUTHORS stamp-h.in acinclude.m4 config.h.in Makefile.in install-sh config.sub config.guess mkinstalldirs missing ltmain.sh depcomp");
//   QStringList adminFiles = admin.entryList(QDir::Files);
//   QStringList::Iterator idx = adminFiles.begin();
//   for ( ; idx != adminFiles.end(); ++idx)
//   {
//     files.append( "admin/" + (*idx) );
//   }
//   QStringList srcDirs = dir.entryList(QDir::Dirs);
//   idx = srcDirs.begin();
//   for (; idx != srcDirs.end(); ++idx)
//   {
//     sourceList += recursiveATFind( projectDirectory() + "/" + (*idx), projectDirectory());
//   }
//   return sourceList + files;
	return sourceList;
}

// void VStudioPart::startSimpleMakeCommand( const QString & dir, const QString & command, bool withKdesu ) {
//   if (partController()->saveAllFiles()==false)
//     return; //user cancelled
//
//   // m_buildCommand = constructMakeCommandLine(dir, target);
//
//   QString cmdline = command;
//   cmdline.prepend(makeEnvironment());
//
//   QString dircmd = "cd ";
//   dircmd += KProcess::quote(dir);
//   dircmd += " && ";
//
//   m_buildCommand = dircmd + cmdline;
//
//   if (withKdesu)
//     m_buildCommand = "kdesu -t -c '" + m_buildCommand + "'";
//
//   if (!m_buildCommand.isNull())
//     makeFrontend()->queueCommand(dir, m_buildCommand);
// }

// QString VStudioPart::getAutoConfFile(const QString& dir) {
//
//   QFile inFile(dir + "/configure.in");
//   QFile acFile(dir + "/configure.ac");
//   if ( inFile.exists()){
//     return inFile.name();
//   }else if (acFile.exists()){
//     return acFile.name();
//   }
//   return acFile.name();;
// }

#include "vstudiopart.moc"
