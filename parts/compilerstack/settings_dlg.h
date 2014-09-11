/*C/C++ Implementation: settings_dlg
*
* Description: KDevelop compilers settings dialog
*
* Author: iegor <rmtdev@gmail.com>, (C) 2014
* Copyright: See COPYING file that comes with this distribution
*/
#ifndef _KDEVELOP_COMPILER_SETTINGS_DLG_H_
#define _KDEVELOP_COMPILER_SETTINGS_DLG_H_

/* Qt */
#include <qtoolbox.h>

class CompilerSettingsDlg : public QWidget {
  Q_OBJECT

  public:
    CompilerSettingsDlg(QWidget *parent=0, const char *name=0, WFlags fl=0);
    virtual ~CompilerSettingsDlg();

  public slots:
    void accept();

  private:
    QToolBox *qtb_compilers;
};

#endif /* _KDEVELOP_COMPILER_SETTINGS_DLG_H_ */