//kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;

#include "astyle_adaptor.h"
#include "astyle_widget.h"

#include <string>

/* Qt */
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qcombobox.h>

/* KDE */
#include <kdebug.h>
#include <kapplication.h>
#include <kconfig.h>

/**
  Converts style string from kdevelop config to astyle enum value
 */
enum astyle::FormatStyle string2fmtstyle(QString &s) {
  if(s == "ALLMAN") return astyle::STYLE_ALLMAN;
  else if(s == "JAVA") return astyle::STYLE_JAVA;
  else if(s == "KR") return astyle::STYLE_KR;
  else if(s == "STROUSTRUP") return astyle::STYLE_STROUSTRUP;
  else if(s == "WHITESMITH") return astyle::STYLE_WHITESMITH;
  else if(s == "BANNER") return astyle::STYLE_BANNER;
  else if(s == "GNU") return astyle::STYLE_GNU;
  else if(s == "Linux") return astyle::STYLE_LINUX;
  else if(s == "HORSTMANN") return astyle::STYLE_HORSTMANN;
  else if(s == "1TBS") return astyle::STYLE_1TBS;
  else if(s == "GOOGLE") return astyle::STYLE_GOOGLE;
  else if(s == "PICO") return astyle::STYLE_PICO;
  else if(s == "LISP") return astyle::STYLE_LISP;
  else return astyle::STYLE_NONE;
}

/**
  Converts astyle enum value to string usable in internal configuration
 */
QString fmtstyle2string(enum astyle::FormatStyle style) {
  switch(style) {
    case astyle::STYLE_ALLMAN: return "ALLMAN";
    case astyle::STYLE_JAVA: return "JAVA";
    case astyle::STYLE_KR: return "KR";
    case astyle::STYLE_STROUSTRUP: return "STROUSTRUP";
    case astyle::STYLE_WHITESMITH: return "WHITESMITH";
    case astyle::STYLE_BANNER: return "BANNER";
    case astyle::STYLE_GNU: return "GNU";
    case astyle::STYLE_LINUX: return "Linux";
    case astyle::STYLE_HORSTMANN: return "HORSTMANN";
    case astyle::STYLE_1TBS: return "1TBS";
    case astyle::STYLE_GOOGLE: return "GOOGLE";
    case astyle::STYLE_PICO: return "PICO";
    case astyle::STYLE_LISP: return "LISP";
    default: return "NONE";
  }
}

/**
 * Converts QString to astyle::BracketMode
 */
astyle::BracketMode string2BrackeMode(const QString &mode) {
  if(mode == ASOPTS_BRACKETS_ATTACH) { return astyle::ATTACH_MODE; }
  else if(mode == ASOPTS_BRACKETS_BREAK) { return astyle::BREAK_MODE; }
  else if(mode == ASOPTS_BRACKETS_LINUX) { return astyle::LINUX_MODE; }
  else if(mode == ASOPTS_BRACKETS_STRAUSTRUP) { return astyle::STROUSTRUP_MODE; }
  else if(mode == ASOPTS_BRACKETS_RUNIN) { return astyle::RUN_IN_MODE; }
  else { return astyle::NONE_MODE; }
}

/*!
 * Converts astyle::BracketMode to QString
 */
QString bracketMode2String(astyle::BracketMode mode) {
  switch(mode) {
    case astyle::ATTACH_MODE: { return ASOPTS_BRACKETS_ATTACH; }
    case astyle::BREAK_MODE: { return ASOPTS_BRACKETS_BREAK; }
    case astyle::LINUX_MODE: { return ASOPTS_BRACKETS_LINUX; }
    case astyle::STROUSTRUP_MODE: { return ASOPTS_BRACKETS_STRAUSTRUP; }
    case astyle::RUN_IN_MODE: { return ASOPTS_BRACKETS_RUNIN; }
    default: { return "unknown"; }
  }
}

/*!
 * Converts e_AStyle_IndentWith to QString
 */
QString indentMode2String(e_AStyle_IndentWith mode) {
  switch(mode) {
    case AS_SPACES: { return ASOPTS_FILL_SPACES; }
    case AS_TABS: { return ASOPTS_FILL_TABS; }
    default: { return "unknown"; }
  }
}

/*!
 * Converts QString to e_AStyle_IndentWith
 */
e_AStyle_IndentWith string2IndentMode(const QString &str) {
  if(str.compare(ASOPTS_FILL_SPACES) == 0) { return AS_SPACES; }
  return AS_TABS;
}

ASStringIterator::ASStringIterator(const QString &txt)
  : ASSourceIterator(), text(txt)
{
  contents = QStringList::split('\n', text, TRUE);
  fetch = contents.begin();
  peek = fetch;

#ifdef DEBUG
  QStringList::const_iterator it;
  for(it=contents.begin(); it!=contents.end(); ++it) {
    kdDebug(9009) << "[ ASTYLE ] cnt line: " << (*it).utf8() << endl;
  }
#endif
}

ASStringIterator::~ASStringIterator() {
}

bool ASStringIterator::hasMoreLines() const {
  return (fetch != contents.end());
}

std::string ASStringIterator::nextLine(bool /*deleted*/) {
  std::string str = (*fetch).utf8().data();
  ++fetch;
  return str;
}

std::string ASStringIterator::peekNextLine() {
  std::string str = (*peek).utf8().data();
  ++peek;
  return str;
}

void ASStringIterator::peekReset() {
  peek = fetch;
}

KDevFormatter::KDevFormatter(const QMap<QString, QVariant>& options)
{
#ifdef DEBUG
	for(QMap<QString, QVariant>::ConstIterator iter = options.begin(); iter != options.end(); iter++) {
		kdDebug(9009) << "format: " << iter.key() << "=" << iter.data()  << endl;
	}
#endif

	setCStyle();

	// Style:
  QString s = options[ASOPTS_FSTYLE].toString();
	if(s != ASOPTS_FSTYLE_USERDEFINED) {
    int style = string2fmtstyle(s);
    setFormattingStyle((enum astyle::FormatStyle)style);
    return;
	}

  // fill
  switch(string2IndentMode(options[ASOPTS_FILL].toString())) {
  case AS_SPACES: {
    setSpaceIndentation(options[ASOPTS_FILLCOUNT].toInt());
    m_indentString = "";
    m_indentString.fill(' ', options[ASOPTS_FILLCOUNT].toInt());
    setTabSpaceConversionMode(options[ASOPTS_FILLFORCE].toBool());
    break;
  }
  case AS_TABS: {
    setTabIndentation(options[ASOPTS_FILLCOUNT].toInt(), options[ASOPTS_FILLFORCE].toBool());
    m_indentString = "\t";
    break;
  }
  default: { break; }
  }

  setEmptyLineFill(options[ASOPTS_FILLEMPTYLINES].toBool());

  // indent
  setSwitchIndent(options[ASOPTS_INDENTSWITCHES].toBool());
  setClassIndent(options[ASOPTS_INDENTCLASSES].toBool());
  setCaseIndent(options[ASOPTS_INDENTCASES].toBool());
  setBracketIndent(options[ASOPTS_INDENTBRACKETS].toBool());
  setNamespaceIndent(options[ASOPTS_INDENTNAMESPACES].toBool());
  setLabelIndent(options[ASOPTS_INDENTLABELS].toBool());
  setBlockIndent(options[ASOPTS_INDENTBLOCKS].toBool());
  setPreprocDefineIndent(options[ASOPTS_INDENTPREPROCS].toBool());
  setPreprocConditionalIndent(false);

  // continuation
  setMaxInStatementIndentLength(options[ASOPTS_MAXSTATEMENT].toInt());
  if (options[ASOPTS_MINCONDITIONAL].toInt() != -1) {
    setMinConditionalIndentOption(options[ASOPTS_MINCONDITIONAL].toInt());
    setMinConditionalIndentLength();
  }

  // brackets
  /*
  s = options[ASOPTS_BRACKETS].toString();
  if(s == "Break") setBracketFormatMode(astyle::BREAK_MODE);
  else if(s == "Attach") setBracketFormatMode(astyle::ATTACH_MODE);
  else if(s == "Linux") setBracketFormatMode(astyle::LINUX_MODE);
  else setBracketFormatMode(astyle::NONE_MODE);
  */
  setBracketFormatMode(string2BrackeMode(options[ASOPTS_BRACKETS].toString()));
  setAddBracketsMode(options[ASOPTS_BRACKETS_ADD].toBool());
  setAddOneLineBracketsMode(options[ASOPTS_BRACKETS_ADD_ONELINE].toBool());
  setBreakClosingHeaderBracketsMode(options[ASOPTS_BRACKETS_CH].toBool());

  // blocks
  setBreakBlocksMode(options[ASOPTS_BLOCK_BREAK].toBool());
  if (options[ASOPTS_BLOCK_BREAKALL].toBool()){
    setBreakBlocksMode(true);
    setBreakClosingHeaderBlocksMode(true);
  }
  setBreakElseIfsMode(options[ASOPTS_BLOCK_BREAKIFELSE].toBool());

  // padding
  setOperatorPaddingMode(options[ASOPTS_PADOPERATORS].toBool());
  setParensInsidePaddingMode(options[ASOPTS_PADPARENTH_IN].toBool());
  setParensOutsidePaddingMode(options[ASOPTS_PADPARENTH_OUT].toBool());
  setParensUnPaddingMode(options[ASOPTS_PADPARENTH_UN].toBool());
  setParensHeaderPaddingMode(options[ASOPTS_PADHEADERS].toBool());

  // oneliner
  setBreakOneLineBlocksMode(!options[ASOPTS_KEEPBLOCKS].toBool());
  setSingleStatementsMode(!options[ASOPTS_KEEPSTATEMENTS].toBool());

  /*NOTE: For now this is hardcoded, but in the future this should be initialized from file settings or
   in project settings or in KDevelop settings, somewhere, maybe something ierarchical */
  //setLineEndFormat(astyle::LINEEND_LINUX);

  setDeleteEmptyLinesMode(false);
}

KDevFormatter::KDevFormatter( AStyleWidget * widget )
{
  setCStyle();

  int style = widget->get_selected_style();
  if(style != ASOPTS_FSTYLE_USERDEFINED_ID) {
    setFormattingStyle((enum astyle::FormatStyle)style);
    return;
  }

  // fill
  switch(static_cast<e_AStyle_IndentWith>(widget->cmb_indentwith->currentItem())) {
    case AS_SPACES: {
      setSpaceIndentation(widget->sb_count->value());
      m_indentString = "";
      m_indentString.fill(' ', widget->sb_count->value());
      setTabSpaceConversionMode(widget->chb_indentforce->isChecked());
      break;
    }
    case AS_TABS: {
      setTabIndentation(widget->sb_count->value(), widget->chb_indentforce->isChecked());
      m_indentString = "\t";
      break;
    }
    default: { break; }
  }

  setEmptyLineFill(widget->Fill_EmptyLines->isChecked());

  // indent
  setSwitchIndent( widget->Indent_Switches->isChecked() );
  setClassIndent( widget->Indent_Classes->isChecked() );
  setCaseIndent( widget->Indent_Cases->isChecked() );
  setBracketIndent( widget->Indent_Brackets->isChecked() );
  setNamespaceIndent( widget->Indent_Namespaces->isChecked() );
  setLabelIndent( widget->Indent_Labels->isChecked() );
  setBlockIndent( widget->Indent_Blocks->isChecked());
  setPreprocDefineIndent(widget->Indent_Preprocessors->isChecked());
  setPreprocConditionalIndent(false);

  // Continuation
  setMaxInStatementIndentLength( widget->Continue_MaxStatement->value() );
  setMinConditionalIndentOption( widget->Continue_MinConditional->value() );
  setMinConditionalIndentLength();

  // Brackets
  setBracketFormatMode(static_cast<astyle::BracketMode>(widget->cmb_brackets->currentItem()));
  setAddBracketsMode(widget->chb_add_brackets->isChecked());
  setAddOneLineBracketsMode(widget->chb_add_brackets_oneline->isChecked());
  setBreakClosingHeaderBracketsMode( widget->Brackets_CloseHeaders->isChecked());

  // Blocks
  setBreakElseIfsMode(widget->Block_IfElse->isChecked());
  if(widget->Block_BreakAll->isChecked()) {
    setBreakBlocksMode(true);
    setBreakClosingHeaderBlocksMode(true);
  }
  else {
    setBreakBlocksMode(widget->Block_Break->isChecked());
  }

  // padding
  setOperatorPaddingMode(widget->Pad_Operators->isChecked());
  setParensInsidePaddingMode(widget->Pad_ParenthesesIn->isChecked());
  setParensOutsidePaddingMode(widget->Pad_ParenthesesOut->isChecked());
  setParensUnPaddingMode(widget->Pad_ParenthesesUn->isChecked());
  setParensHeaderPaddingMode(widget->chb_parens_header->isChecked());

  // oneliner
  setBreakOneLineBlocksMode(!widget->Keep_Blocks->isChecked());
  setSingleStatementsMode(!widget->Keep_Statements->isChecked());

  setDeleteEmptyLinesMode(false);
}

bool KDevFormatter::predefinedStyle( const QString & style )
{
	/* if (style == "ANSI") {
		setBracketIndent(false);
		setSpaceIndentation(4);
		setBracketFormatMode(astyle::BREAK_MODE);
		setClassIndent(false);
		setSwitchIndent(false);
		setNamespaceIndent(false);
		return true;
	}
  else */if (style == "KR") {
		setBracketIndent(false);
		setSpaceIndentation(4);
		setBracketFormatMode(astyle::ATTACH_MODE);
		setClassIndent(false);
		setSwitchIndent(false);
		setNamespaceIndent(false);
		return true;
	}
  else if (style == "Linux") {
		setBracketIndent(false);
		setSpaceIndentation(8);
		setBracketFormatMode(astyle::LINUX_MODE);
		setClassIndent(false);
		setSwitchIndent(false);
		setNamespaceIndent(false);
		return true;
	}
  else if (style == "GNU") {
		setBlockIndent(true);
		setSpaceIndentation(2);
		setBracketFormatMode(astyle::BREAK_MODE);
		setClassIndent(false);
		setSwitchIndent(false);
		setNamespaceIndent(false);
		return true;
	}
  else if (style == "JAVA") {
		setJavaStyle();
		setBracketIndent(false);
		setSpaceIndentation(4);
		setBracketFormatMode(astyle::ATTACH_MODE);
		setSwitchIndent(false);
		return true;
	}

	return false;
}
