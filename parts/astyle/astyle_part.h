/*
 *  Copyright (C) 2001 Matthias H�zer-Klpfel <mhk@caldera.de>
 */


#ifndef __KDEVPART_ASTYLE_H__
#define __KDEVPART_ASTYLE_H__

class KDialogBase;
#include <kdevsourceformatter.h>
#include <kaction.h>
#include <kparts/part.h>
#include <kdevplugin.h>
#include <kdebug.h>
#include <qmap.h>
#include <qvariant.h>

class AStyleWidget;
class QPopupMenu;
class Context;
class ConfigWidgetProxy;

#define ASOPTS_FSTYLE           "FStyle"
#define ASOPTS_USEGLOBAL        "UseGlobal"
#define ASOPTS_FILLCOUNT        "FillCount"
#define ASOPTS_FILLFORCE        "FillForce"
#define ASOPTS_FILLEMPTYLINES   "FillEmptyLines"
#define ASOPTS_FILL             "Fill"
#define ASOPTS_MAXSTATEMENT     "MaxStatement"
#define ASOPTS_MINCONDITIONAL   "MinConditional"
// Indentation:
#define ASOPTS_INDENTSWITCHES   "IndentSwitches"
#define ASOPTS_INDENTCASES      "IndentCases"
#define ASOPTS_INDENTCLASSES    "IndentClasses"
#define ASOPTS_INDENTBRACKETS   "IndentBrackets"
#define ASOPTS_INDENTNAMESPACES "IndentNamespaces"
#define ASOPTS_INDENTLABELS     "IndentLabels"
#define ASOPTS_INDENTBLOCKS     "IndentBlocks"
#define ASOPTS_INDENTPREPROCS   "IndentPreprocessors"
// Brackets style:
#define ASOPTS_BRACKETS         "Brackets"
#define ASOPTS_BRACKETS_CH      "BracketsCloseHeaders"
// Block brake:
#define ASOPTS_BLOCK_BREAK      "BlockBreak"
#define ASOPTS_BLOCK_BREAKALL   "BlockBreakAll"
#define ASOPTS_BLOCK_BREAKIFELSE "BlockIfElse"
// Oneliners:
#define ASOPTS_KEEPSTATEMENTS   "KeepStatements"
#define ASOPTS_KEEPBLOCKS       "KeepBlocks"
// Padding:
#define ASOPTS_PADPARENTH_IN    "PadParenthesesIn"
#define ASOPTS_PADPARENTH_OUT   "PadParenthesesOut"
#define ASOPTS_PADPARENTH_UN    "PadParenthesesUn"
#define ASOPTS_PADOPERATORS     "PadOperators"

class AStylePart : public KDevSourceFormatter
{
  Q_OBJECT

public:

  AStylePart(QObject *parent, const char *name, const QStringList &);
  ~AStylePart();

  QString formatSource(const QString text, AStyleWidget * widget, const QMap<QString, QVariant>& options);
  virtual QString formatSource(const QString text);
  QString indentString() const;
  void saveGlobal();
  void setExtensions(QString ext, bool global);
  QString getProjectExtensions();
  QString getGlobalExtensions();
  void restorePartialProjectSession(const QDomElement * el);
  void savePartialProjectSession(QDomElement * el);

  QMap<QString, QVariant>& getProjectOptions(){return m_project;}
  QMap<QString, QVariant>& getGlobalOptions(){return m_global;}


private slots:

  void activePartChanged(KParts::Part *part);
  void beautifySource();
  void formatFiles();
  void formatFilesSelect();
  void insertConfigWidget(const KDialogBase *dlg, QWidget *page, unsigned int pageNo);
  void contextMenu(QPopupMenu *popup, const Context *context);

private:

  void cursorPos( KParts::Part *part, uint * col, uint * line );
  void setCursorPos( KParts::Part *part, uint col, uint line );

  void loadGlobal();

  ConfigWidgetProxy *m_configProxy;
  KAction *formatTextAction;
  KAction *formatFileAction;

  // the configurable options.
  QMap<QString, QVariant>  m_project;
  QMap<QString, QVariant> m_global;
  QStringList m_projectExtensions;
  QStringList m_globalExtensions;
  QMap<QString, QString> m_searchExtensions;
  KURL::List m_urls;
};


#endif
