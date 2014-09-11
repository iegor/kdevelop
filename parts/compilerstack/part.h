/*  C/C++ Implementation: compilerpart
*
* Description: KDevelop compilers settings and tuning part
*
* Author: iegor <rmtdev@gmail.com>, (C) 2014
* Copyright: See COPYING file that comes with this distribution
*/
#ifndef _KDEVELOP_COMPILERSTACK_PART_H_
#define _KDEVELOP_COMPILERSTACK_PART_H_

/* Qt */
#include <qstringlist.h>

/* KDevelop */
// #include <kdevplugin.h>
#include <kdevcompilerfrontend.h>
#include <configwidgetproxy.h>

/* KDevelop:Parts:CompilerStack */
// #include <settings_dlg.h>

class CompilerStack : public KDevCompilerFrontend {
  Q_OBJECT

  public:
    CompilerStack(QObject *parent, const char *name, const QStringList &);
    virtual ~CompilerStack();

  private slots:
    void insertConfigWidget(const KDialogBase *dlg, QWidget *page, unsigned int pageNo);

  private:
    ConfigWidgetProxy *m_configProxy;
    QString m_compiler; /// Compiler of choise
};

#endif /* _KDEVELOP_COMPILERSTACK_PART_H_ */