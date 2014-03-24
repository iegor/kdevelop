//kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;

#include "astyle_adaptor.h"
#include "astyle_widget.h"

#include <string>

#include <qradiobutton.h>
#include <qspinbox.h>
#include <qcheckbox.h>
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

ASStringIterator::ASStringIterator(const QString &text)
  : ASSourceIterator(), _content(text)
{
  _is = new QTextStream(&_content, IO_ReadOnly);
}


ASStringIterator::~ASStringIterator()
{
  delete _is;
}


bool ASStringIterator::hasMoreLines() const
{
  return !_is->eof();
}

std::string ASStringIterator::nextLine(bool /*emptyLineWasDeleted*/) {
  return _is->readLine().utf8().data();
}

std::string ASStringIterator::peekNextLine() {
  return _is->readLine().utf8().data();
}

void ASStringIterator::peekReset() {
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
	int wsCount = options[ASOPTS_FILLCOUNT].toInt();
  if (options[ASOPTS_FILL].toString() == "Tabs")
  {
	  setTabIndentation(wsCount, options[ASOPTS_FILLFORCE].toBool() );
	  m_indentString = "\t";
  } else
  {
	  setSpaceIndentation(wsCount);
	  m_indentString = "";
	  m_indentString.fill(' ', wsCount);
  }

  setTabSpaceConversionMode(options[ASOPTS_FILLFORCE].toBool());
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
  s = options[ASOPTS_BRACKETS].toString();
  if(s == "Break") setBracketFormatMode(astyle::BREAK_MODE);
  else if(s == "Attach") setBracketFormatMode(astyle::ATTACH_MODE);
  else if(s == "Linux") setBracketFormatMode(astyle::LINUX_MODE);
  else setBracketFormatMode(astyle::NONE_MODE);

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

  // oneliner
  setBreakOneLineBlocksMode(!options[ASOPTS_KEEPBLOCKS].toBool());
  setSingleStatementsMode(!options[ASOPTS_KEEPSTATEMENTS].toBool());
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
	if ( widget->Fill_Tabs->isChecked() )
	{
		setTabIndentation(widget->Fill_TabCount->value(), widget->Fill_ForceTabs->isChecked());
		m_indentString = "\t";
	}
	else
	{
		setSpaceIndentation( widget->Fill_SpaceCount->value() );
		m_indentString = "";
		m_indentString.fill(' ', widget->Fill_SpaceCount->value());
	}

	setTabSpaceConversionMode(widget->Fill_ConvertTabs->isChecked());
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

	// continuation
	setMaxInStatementIndentLength( widget->Continue_MaxStatement->value() );
	setMinConditionalIndentOption( widget->Continue_MinConditional->value() );
    setMinConditionalIndentLength();

	// brackets
	if ( widget->Brackets_Break->isChecked() )
	{
		setBracketFormatMode( astyle::BREAK_MODE );
	}
	else if ( widget->Brackets_Attach->isChecked() )
	{
		setBracketFormatMode( astyle::ATTACH_MODE );
	}
	else if ( widget->Brackets_Linux->isChecked())
	{
		setBracketFormatMode( astyle::LINUX_MODE );
	}
	else{
		setBracketFormatMode( astyle::NONE_MODE );
	}

	setBreakClosingHeaderBracketsMode( widget->Brackets_CloseHeaders->isChecked());

	// blocks
	setBreakBlocksMode(widget->Block_Break->isChecked());
	if (widget->Block_BreakAll->isChecked()){
		setBreakBlocksMode(true);
		setBreakClosingHeaderBlocksMode(true);
	}
	setBreakElseIfsMode(widget->Block_IfElse->isChecked());

	// padding
	setOperatorPaddingMode( widget->Pad_Operators->isChecked() );

	setParensInsidePaddingMode( widget->Pad_ParenthesesIn->isChecked() );
	setParensOutsidePaddingMode( widget->Pad_ParenthesesOut->isChecked() );
	setParensUnPaddingMode( widget->Pad_ParenthesesUn->isChecked() );

	// oneliner
	setBreakOneLineBlocksMode( !widget->Keep_Blocks->isChecked() );
	setSingleStatementsMode( !widget->Keep_Statements->isChecked() );
}

bool KDevFormatter::predefinedStyle( const QString & style )
{
	/*if (style == "ANSI") {
		setBracketIndent(false);
		setSpaceIndentation(4);
		setBracketFormatMode(astyle::BREAK_MODE);
		setClassIndent(false);
		setSwitchIndent(false);
		setNamespaceIndent(false);
		return true;
	} else */if (style == "KR") {
		setBracketIndent(false);
		setSpaceIndentation(4);
		setBracketFormatMode(astyle::ATTACH_MODE);
		setClassIndent(false);
		setSwitchIndent(false);
		setNamespaceIndent(false);
		return true;
	} else if (style == "Linux") {
		setBracketIndent(false);
		setSpaceIndentation(8);
		setBracketFormatMode(astyle::LINUX_MODE);
		setClassIndent(false);
		setSwitchIndent(false);
		setNamespaceIndent(false);
		return true;
	} else if (style == "GNU") {
		setBlockIndent(true);
		setSpaceIndentation(2);
		setBracketFormatMode(astyle::BREAK_MODE);
		setClassIndent(false);
		setSwitchIndent(false);
		setNamespaceIndent(false);
		return true;
	} else if (style == "JAVA") {
		setJavaStyle();
		setBracketIndent(false);
		setSpaceIndentation(4);
		setBracketFormatMode(astyle::ATTACH_MODE);
		setSwitchIndent(false);
		return true;
	}

	return false;
}
