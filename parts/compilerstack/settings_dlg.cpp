/*C/C++ Implementation: settings_dlg
*
* Description: KDevelop compilers settings page
*
* Author: iegor <rmtdev@gmail.com>, (C) 2014
* Copyright: See COPYING file that comes with this distribution
*/

/* Qt */
#include <qlayout.h>

/* KDE */
#include <kdebug.h>

/* Compiler */
#include "settings_dlg.h"

CompilerSettingsDlg::CompilerSettingsDlg(QWidget *pnt/*=0*/, const char *nm/*=0*/, WFlags fl/*=0*/)
  : QWidget(pnt, nm, fl) {
  QVBoxLayout *qvl_dlg = new QVBoxLayout(this, 2, 0, "qvl_dlg");
  qtb_compilers = new QToolBox(this, "qtb_compilers", 0);

  kdDebug(9000) << "hello\n";
}

CompilerSettingsDlg::~CompilerSettingsDlg() {
}

void CompilerSettingsDlg::accept() {
}