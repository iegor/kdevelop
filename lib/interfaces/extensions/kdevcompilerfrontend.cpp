/* C/C++ Implementation: kdevcompilerfrontend
*
* Description: KDevCompilerFrontend methods and properties definitions
*
* Author: iegor <rmtdev@gmail.com>, (C) 2014
* Copyright: See COPYING file that comes with this distribution
*/

/* KDevelop:Extensions:KDevCompilerFrontend */
#include "kdevcompilerfrontend.h"

KDevCompilerFrontend::KDevCompilerFrontend(const KDevPluginInfo *info, QObject *pnt/*=0*/, const char *nm/*=0*/)
: KDevPlugin(info, pnt, nm ? nm : "KDevCompilerFrontend") {
}

KDevCompilerFrontend::~KDevCompilerFrontend() {
}

#include "kdevcompilerfrontend.moc"