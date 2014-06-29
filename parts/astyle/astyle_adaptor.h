//kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;
#ifndef __ASTYLE_ADAPTOR_H__
#define __ASTYLE_ADAPTOR_H__

#include <qstring.h>
#include <qtextstream.h>
#include <qmap.h>
#include <qvariant.h>

#include "astyle.h"

#define GLOBALDOC_OPTIONS 1
#define PROJECTDOC_OPTIONS 2

#define ASOPTS_FSTYLE                   "FStyle"
#define ASOPTS_FSTYLE_USERDEFINED       "UserDefined"
#define ASOPTS_FSTYLE_USERDEFINED_ID    14
#define ASOPTS_USEGLOBAL                "UseGlobal"
#define ASOPTS_FILLCOUNT                "FillCount"
#define ASOPTS_FILLFORCE                "FillForce"
#define ASOPTS_FILLEMPTYLINES           "FillEmptyLines"
#define ASOPTS_FILL                     "Fill"
#define ASOPTS_FILL_TABS                "Tabs"
#define ASOPTS_FILL_SPACES              "Spaces"
#define ASOPTS_MAXSTATEMENT             "MaxStatement"
#define ASOPTS_MINCONDITIONAL           "MinConditional"
// Indentation:
#define ASOPTS_INDENTSWITCHES           "IndentSwitches"
#define ASOPTS_INDENTCASES              "IndentCases"
#define ASOPTS_INDENTCLASSES            "IndentClasses"
#define ASOPTS_INDENTBRACKETS           "IndentBrackets"
#define ASOPTS_INDENTNAMESPACES         "IndentNamespaces"
#define ASOPTS_INDENTLABELS             "IndentLabels"
#define ASOPTS_INDENTBLOCKS             "IndentBlocks"
#define ASOPTS_INDENTPREPROCS           "IndentPreprocessors"
// Brackets style:
#define ASOPTS_BRACKETS                 "bm"
#define ASOPTS_BRACKETS_NONE            "bm_none"
#define ASOPTS_BRACKETS_ATTACH          "bm_attach"
#define ASOPTS_BRACKETS_BREAK           "bm_break"
#define ASOPTS_BRACKETS_LINUX           "bm_linux"
#define ASOPTS_BRACKETS_STRAUSTRUP      "bm_straustrup"
#define ASOPTS_BRACKETS_RUNIN           "bm_runin"
#define ASOPTS_BRACKETS_CH              "bm_cheaders"
#define ASOPTS_BRACKETS_ADD             "b_add"
#define ASOPTS_BRACKETS_ADD_ONELINE     "b_add_oneline"
// Block brake:
#define ASOPTS_BLOCK_BREAK              "blk_break"
#define ASOPTS_BLOCK_BREAKALL           "blk_breakall"
#define ASOPTS_BLOCK_BREAKIFELSE        "blk_breakifelse"
// Oneliners:
#define ASOPTS_KEEPSTATEMENTS           "KeepStatements"
#define ASOPTS_KEEPBLOCKS               "KeepBlocks"
// Padding:
#define ASOPTS_PADPARENTH_IN            "PadParenthesesIn"
#define ASOPTS_PADPARENTH_OUT           "PadParenthesesOut"
#define ASOPTS_PADPARENTH_UN            "PadParenthesesUn"
#define ASOPTS_PADOPERATORS             "PadOperators"
#define ASOPTS_PADHEADERS               "pad_header"

#define ASOPTS_PRJCONFIGELEMENT_ROOT            "AStyle"
#define ASOPTS_PRJCONFIGELEMENT_EXT             "Extensions"
#define ASOPTS_PRJCONFIGATTR_EXT                "Ext"

enum e_AStyle_IndentWith {
  AS_SPACES = 0,
  AS_TABS,
};

class ASStringIterator : public astyle::ASSourceIterator
{
public:

  ASStringIterator(const QString &string);
  virtual ~ASStringIterator();

  virtual bool hasMoreLines() const;
  virtual std::string nextLine(bool emptyLineWasDeleted=false);
  virtual std::string peekNextLine();
  virtual void peekReset();

private:
  QString text;
  QStringList contents;
  QStringList::const_iterator fetch;
  QStringList::const_iterator peek;
};

class AStyleWidget;

class KDevFormatter : public astyle::ASFormatter
{
public:

  KDevFormatter(const QMap<QString, QVariant>& options);
  KDevFormatter( AStyleWidget * widget );
  QString indentString() const
  {
	  return m_indentString;
  }

private:
	bool predefinedStyle( const QString & style );

private:
	QString m_indentString;
};

QString fmtstyle2string(astyle::FormatStyle style);
astyle::FormatStyle string2fmtstyle(QString &s);

astyle::BracketMode string2BrackeMode(const QString &mode);
QString bracketMode2String(astyle::BracketMode mode);

QString indentMode2String(e_AStyle_IndentWith mode);
e_AStyle_IndentWith string2IndentMode(const QString &str);

#endif
