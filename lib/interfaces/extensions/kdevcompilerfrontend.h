/*  C/C++ Implementation: kdevcompilerfrontend
*
* Description: KDevelop compiler front end, will be used to set/get compiler properties, make actions like
*  - clean/build translation unit <project/file>, etc
*
* Author: iegor <rmtdev@gmail.com>, (C) 2014
*
* Copyright: See COPYING file that comes with this distribution
*/
#ifndef _KDEVELOP_COMPILER_FRONTEND_H_
#define _KDEVELOP_COMPILER_FRONTEND_H_

/* KDevelop */
#include <kdevplugin.h>

/** \class KDevCompilerFrontend
 * \brief Compiler fron end class, used to set\get compiler's properties and trigger actions
 */
class KDevCompilerFrontend : public KDevPlugin {
  Q_OBJECT
  Q_PROPERTY(QString compiler READ getCompiler WRITE setCompiler)
  Q_PROPERTY(QString linker READ getLinker WRITE setLinker)

  public:
    KDevCompilerFrontend(const KDevPluginInfo *info, QObject *parent=0, const char *name=0);
    virtual ~KDevCompilerFrontend();

  public:
    /* Compiler property */
    QString getCompiler() const { return compiler; }
    void setCompiler(const QString &cmp) {
      compiler = cmp;
      emit compilerChanged(compiler);
    }

    /* Linker property */
    QString getLinker() const { return linker; }
    void setLinker(const QString &lnk) {
      linker = lnk;
      emit linkerChanged(linker);
    }

  signals:
    void compilerChanged(QString compiler);
    void linkerChanged(QString linker);

  private:
    QString compiler;
    QString linker;
};

#endif /* _KDEVELOP_COMPILER_FRONTEND_H_ */