/* C/C++ Implementation: compilerpart
*
* Description: KDevelop compilers settings and tuning page, implementation
*
* Author: iegor <rmtdev@gmail.com>, (C) 2014
* Copyright: See COPYING file that comes with this distribution
*/

/* Qt */
#include <qwhatsthis.h>

/* KDE */
#include <klocale.h>
#include <kdebug.h>
#include <kdialogbase.h>
#include <kiconloader.h>

/* KDevelop */
#include <kdevplugininfo.h>
#include <kdevgenericfactory.h>
#include <kdevproject.h>
#include <kdevcore.h>
#include <kdevmainwindow.h>

/* KDevelop:Parts:CompilerStack */
#include "settings_dlg.h"
#include "part.h"

static const KDevPluginInfo data("compilerstack");
typedef KDevGenericFactory < CompilerStack, KDevCompilerFrontend > CompilerStackFactory;
K_EXPORT_COMPONENT_FACTORY(libcompilerstack, CompilerStackFactory(data));

CompilerStack::CompilerStack(QObject *pnt, const char *nm, const QStringList &)
  : KDevCompilerFrontend(&data, pnt, nm) {
  setInstance(CompilerStackFactory::instance());
  setXMLFile("compilerstack.rc");

  m_configProxy = new ConfigWidgetProxy(core());
  m_configProxy->createGlobalConfigPage(i18n("Compilers"), 1, "gear");

  connect(m_configProxy, SIGNAL(insertConfigWidget(const KDialogBase* ,QWidget*,unsigned int)), this,
          SLOT(insertConfigWidget(const KDialogBase*,QWidget*,unsigned int)));

  kdDebug(9000) << "Compiler part is initialized\n";
}

CompilerStack::~CompilerStack() {
}

void CompilerStack::insertConfigWidget(const KDialogBase *dlg, QWidget *page, unsigned int /*pageNo*/) {
  CompilerSettingsDlg *w = new CompilerSettingsDlg(page, "compilers config widget");
  connect(dlg, SIGNAL(okClicked()), w, SLOT(accept()));
}
