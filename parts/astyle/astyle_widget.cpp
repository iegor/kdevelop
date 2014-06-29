//kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;
#include <qradiobutton.h>
#include <qtabwidget.h>
#include <qmultilineedit.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kdevcore.h>

#include "astyle_part.h"
#include "astyle_adaptor.h"
#include "astyle_widget.h"

static const QString astyle_example(
"#include <stdio.h>\n"
"\n"
"/*! This is an example of astyle formatting source code\n"
" * Doxy comment\n"
" */\n"
"\n"
"//NOTE: make a note\n"
"//  starts here\n"
"class CPoint2D {\n"
"  public:\n"
"    // Class {ctor}\n"
"    CPoint2D() {\n"
"    }\n"
"    ~CPoint2D() {}\n"
"\n"
"    void foo1(int t) { if(t==1) { never_exec(2*4); } }\n"
"\n"
"       void foo(  char t )\n"
"    {\n"
"      switch(result)\n"
"    {\n"
"    case 1: {\n"
"    break; }\n"
"    case 2: return;\n"
"    case 3:\n"
"             while(true) do_foo();\n"
"    case 4:\n"
"    case 5:\n"
"    {\n"
"        return;\n"
"      }\n"
"      }\n"
"    }\n"
"\n"
"    /*! This is a test method\n"
"     * Doxy \\a comment\n"
"     */\n"
"    inline void oneliner() { /*comment*/ if(true) makefoo(); }\n"
"\n"
"  public:\n"
"    class A {\n"
"      public:\n"
"        class B {\n"
"          int a1,a2,a3;\n"
"        };\n"
"        unsigned short a,b,c;\n"
"    };\n"
"    struct\n"
"    {\n"
"      struct {\n"
"        int a,b,c;\n"
"      };\n"
"      int b,c,a;\n"
"    };\n"
"    union {\n"
"      struct\n"
"      {\n"
"        int x;\n"
"        int y;\n"
"      };\n"
"      int pv[2];\n"
"    };\n"
"};\n"
"\n"
"int main (int argc, char *argv[]) {\n"
"  return 0;\n"
"}\n");

AStyleWidget::AStyleWidget(AStylePart * part, AStyleWidget::context ctx, QWidget *parent, const char *name)
: AStyleConfig(parent, name)
, m_part(part)
, m_context(ctx)
, m_bUseGlobalOpts(false)
{
  //connect(tb_astyle, SIGNAL(currentChanged(int)), this, SLOT(slotToolsPage(int)));
  // Style
  qcmb_style->insertItem(QString("Default (NONE)"));
  qcmb_style->insertItem(QString("Allman (ALLMAN)"));
  qcmb_style->insertItem(QString("Java (JAVA)"));
  qcmb_style->insertItem(QString("K&R (KR)"));
  qcmb_style->insertItem(QString("Stroustrup (STROUSTRUP)"));
  qcmb_style->insertItem(QString("Whitesmith (WHITESMITH)"));
  qcmb_style->insertItem(QString("Banner (BANNER)"));
  qcmb_style->insertItem(QString("GNU (GNU)"));
  qcmb_style->insertItem(QString("Linux (LINUX)"));
  qcmb_style->insertItem(QString("Horstmann (HORSTMANN)"));
  qcmb_style->insertItem(QString("1Tbs (1TBS)"));
  qcmb_style->insertItem(QString("Google (GOOGLE)"));
  qcmb_style->insertItem(QString("Pico (PICO)"));
  qcmb_style->insertItem(QString("List (LISP)"));
  qcmb_style->insertItem(QString("User Defined"));

  connect(qcmb_style, SIGNAL(activated(int)), this, SLOT(set_style()));
  connect(qchb_useglobal, SIGNAL(clicked()), this, SLOT(set_useglobal()));

  // Blocks and Brackets
  cmb_brackets->insertItem("No change");
  cmb_brackets->insertItem("Attach");
  cmb_brackets->insertItem("Break");
  cmb_brackets->insertItem("Linux");
  cmb_brackets->insertItem("Straustrup");
  cmb_brackets->insertItem("Run in");

  connect(cmb_brackets, SIGNAL(activated(int)), this, SLOT(slotSetBrackets(int)));
  connect(Brackets_CloseHeaders, SIGNAL(clicked()), this, SLOT(set_blocks_and_brackets()));
  connect(chb_add_brackets, SIGNAL(clicked()), this, SLOT(set_blocks_and_brackets()));
  connect(chb_add_brackets_oneline, SIGNAL(clicked()), this, SLOT(set_blocks_and_brackets()));
  connect(Block_Break, SIGNAL(clicked()), this, SLOT(set_blocks_and_brackets()));
  connect(Block_BreakAll, SIGNAL(clicked()), this, SLOT(set_blocks_and_brackets()));
  connect(Block_IfElse, SIGNAL(clicked()), this, SLOT(set_blocks_and_brackets()));
  connect(Keep_Blocks, SIGNAL(clicked()), this, SLOT(set_blocks_and_brackets()));
  connect(Keep_Statements, SIGNAL(clicked()), this, SLOT(set_blocks_and_brackets()));

  // Filling and Continuation
  cmb_indentwith->insertItem(ASOPTS_FILL_SPACES);
  cmb_indentwith->insertItem(ASOPTS_FILL_TABS);

  connect(cmb_indentwith, SIGNAL(activated(int)), this, SLOT(slotSetIndentMode(int)));
  connect(chb_indentforce, SIGNAL(clicked()), this, SLOT(set_fill()));
  connect(sb_count, SIGNAL(valueChanged(int)), this, SLOT(set_fill()));
  connect(Continue_MaxStatement, SIGNAL(valueChanged(int)), this, SLOT(set_fill()));
  connect(Continue_MinConditional, SIGNAL(valueChanged(int)), this, SLOT(set_fill()));

  // Indentation
  connect(Indent_Switches, SIGNAL(clicked()), this, SLOT(set_indent()));
  connect(Indent_Cases, SIGNAL(clicked()), this, SLOT(set_indent()));
  connect(Indent_Classes, SIGNAL(clicked()), this, SLOT(set_indent()));
  connect(Indent_Brackets, SIGNAL(clicked()), this, SLOT(set_indent()));
  connect(Indent_Namespaces, SIGNAL(clicked()), this, SLOT(set_indent()));
  connect(Indent_Labels, SIGNAL(clicked()), this, SLOT(set_indent()));
  connect(Indent_Blocks, SIGNAL(clicked()), this, SLOT(set_indent()));
  connect(Indent_Preprocessors, SIGNAL(clicked()), this, SLOT(set_indent()));

  // Padding and placement
  connect(Pad_ParenthesesIn, SIGNAL(clicked()), this, SLOT(set_padding()));
  connect(Pad_ParenthesesOut, SIGNAL(clicked()), this, SLOT(set_padding()));
  connect(Pad_ParenthesesUn, SIGNAL(clicked()), this, SLOT(set_padding()));
  connect(Pad_Operators, SIGNAL(clicked()), this, SLOT(set_padding()));
  connect(chb_parens_header, SIGNAL(clicked()), this, SLOT(set_padding()));

  switch(m_context) {
    case GLOBAL: { fillcontrols(m_part->getGlobalOptions()); break; }
    case PROJECT: { fillcontrols(m_part->getProjectOptions()); break; }
    default: { break; }
  }
  set_example();
}

AStyleWidget::~AStyleWidget() {
}

int AStyleWidget::get_selected_style() {
  return qcmb_style->currentItem();
}

/**
*  Fills in all controls and sets values in them from options map
*/
void AStyleWidget::fillcontrols(QMap<QString, QVariant> &options) {
  // General:
  QString s = options[ASOPTS_FSTYLE].toString();

  switch(m_context) {
    case GLOBAL: {
      qchb_useglobal->setEnabled(false);
      qchb_useglobal->hide();
      qtxe_generalext->setText(m_part->getGlobalExtensions());
      if(s == ASOPTS_FSTYLE_USERDEFINED) {
        enable_controls();
        qcmb_style->setCurrentItem(ASOPTS_FSTYLE_USERDEFINED_ID);
      } else {
        enum astyle::FormatStyle fstyle = string2fmtstyle(s);
        qcmb_style->setCurrentItem((int)fstyle);
        disable_controls();
      }
      break;
    }
    case PROJECT: {
      qchb_useglobal->show();
      m_bUseGlobalOpts = options[ASOPTS_USEGLOBAL].toBool();
      qchb_useglobal->setChecked(m_bUseGlobalOpts);
      qcmb_style->setEnabled(!m_bUseGlobalOpts);
      if(m_bUseGlobalOpts) {
        disable_controls();
        qtxe_generalext->setText(m_part->getGlobalExtensions());
        if(s == ASOPTS_FSTYLE_USERDEFINED) {
          qcmb_style->setCurrentItem(ASOPTS_FSTYLE_USERDEFINED_ID);
        } else {
          enum astyle::FormatStyle fstyle = string2fmtstyle(s);
          qcmb_style->setCurrentItem((int)fstyle);
          disable_controls();
        }
      } else {
        qtxe_generalext->setText(m_part->getProjectExtensions());
        if(s == ASOPTS_FSTYLE_USERDEFINED) {
          enable_controls();
          qcmb_style->setCurrentItem(ASOPTS_FSTYLE_USERDEFINED_ID);
        } else {
          enum astyle::FormatStyle fstyle = string2fmtstyle(s);
          qcmb_style->setCurrentItem((int)fstyle);
          disable_controls();
        }
      }
      break;
    }
  }
  // m_lastExt=GeneralExtension->text();

  // Filling and Continuation:
  e_AStyle_IndentWith mode = static_cast<e_AStyle_IndentWith>(string2IndentMode(options[ASOPTS_FILL].toString()));
  cmb_indentwith->setCurrentItem(mode);

  switch(mode) {
    case AS_SPACES: {
      chb_indentforce->setText("Con&vert tabs");
      break;
    }
    case AS_TABS: {
      chb_indentforce->setText("F&orce tabs");
      break;
    }
    default: {
      break;
    }
  }

  sb_count->setValue(options[ASOPTS_FILLCOUNT].toInt());
  chb_indentforce->setChecked(options[ASOPTS_FILLFORCE].toBool());

  if(!m_bUseGlobalOpts && (options[ASOPTS_FSTYLE] == ASOPTS_FSTYLE_USERDEFINED)) {
    sb_count->setEnabled(true);
    chb_indentforce->setEnabled(true);
  }

  Fill_EmptyLines->setChecked(options[ASOPTS_FILLEMPTYLINES].toBool());
  Continue_MaxStatement->setValue(options[ASOPTS_MAXSTATEMENT].toInt());
  Continue_MinConditional->setValue(options[ASOPTS_MINCONDITIONAL].toInt());

  // Indentation:
  Indent_Switches->setChecked(options[ASOPTS_INDENTSWITCHES].toBool());
  Indent_Cases->setChecked(options[ASOPTS_INDENTCASES].toBool());
  Indent_Classes->setChecked(options[ASOPTS_INDENTCLASSES].toBool());
  Indent_Brackets->setChecked(options[ASOPTS_INDENTBRACKETS].toBool());
  Indent_Namespaces->setChecked(options[ASOPTS_INDENTNAMESPACES].toBool());
  Indent_Labels->setChecked(options[ASOPTS_INDENTLABELS].toBool());
  Indent_Blocks->setChecked(options[ASOPTS_INDENTBLOCKS].toBool());
  Indent_Preprocessors->setChecked(options[ASOPTS_INDENTPREPROCS].toBool());

  // Brackets and Blocks:
  cmb_brackets->setCurrentItem(string2BrackeMode(options[ASOPTS_BRACKETS].toString()));
  Brackets_CloseHeaders->setChecked(options[ASOPTS_BRACKETS_CH].toBool());
  chb_add_brackets->setChecked(options[ASOPTS_BRACKETS_ADD].toBool());
  chb_add_brackets_oneline->setChecked(options[ASOPTS_BRACKETS_ADD_ONELINE].toBool());
  Block_Break->setChecked(options[ASOPTS_BLOCK_BREAK].toBool());
  Block_BreakAll->setChecked(options[ASOPTS_BLOCK_BREAKALL].toBool());
  Block_IfElse->setChecked(options[ASOPTS_BLOCK_BREAKIFELSE].toBool());

  // Oneliners:
  Keep_Statements->setChecked(options[ASOPTS_KEEPSTATEMENTS].toBool());
  Keep_Blocks->setChecked(options[ASOPTS_KEEPBLOCKS].toBool());

  // Padding:
  Pad_ParenthesesIn->setChecked(options[ASOPTS_PADPARENTH_IN].toBool());
  Pad_ParenthesesOut->setChecked(options[ASOPTS_PADPARENTH_OUT].toBool());
  Pad_ParenthesesUn->setChecked(options[ASOPTS_PADPARENTH_UN].toBool());
  Pad_Operators->setChecked(options[ASOPTS_PADOPERATORS].toBool());
  chb_parens_header->setChecked(options[ASOPTS_PADHEADERS].toBool());
}

/**
*  Reads all controls values into a map
*/
void AStyleWidget::readcontrols(QMap<QString, QVariant> &/*options*/) {
}

/**
  Accepts settings from dialog and writes them into part
*/
void AStyleWidget::accept() {
  QMap<QString, QVariant>& op = getContextOptions();

  switch(m_context) {
    case GLOBAL: {
      if(qcmb_style->currentItem() == ASOPTS_FSTYLE_USERDEFINED_ID) {
        // General:
        op[ASOPTS_FSTYLE] = ASOPTS_FSTYLE_USERDEFINED;
      } else {
        op[ASOPTS_FSTYLE] = fmtstyle2string((enum astyle::FormatStyle)qcmb_style->currentItem());
      }
      m_part->setExtensions(qtxe_generalext->text(), true);

      // Check and update project settings accordingly
      QMap<QString, QVariant>& p_op = m_part->getProjectOptions();
      m_bUseGlobalOpts = p_op[ASOPTS_USEGLOBAL].toBool();
      if(m_bUseGlobalOpts) {
        p_op = m_part->getGlobalOptions();
        p_op[ASOPTS_USEGLOBAL] = m_bUseGlobalOpts;
      }
      break;
    }
    case PROJECT: {
      if(m_bUseGlobalOpts) {
        // QMap<QString,QVariant>& global = m_part->getGlobalOptions();
        // QMap<QString,QVariant>& project = m_part->getProjectOptions();
        op = m_part->getGlobalOptions();
        op[ASOPTS_USEGLOBAL] = m_bUseGlobalOpts;
        m_part->saveGlobal();
        return;
      } else {
        op[ASOPTS_FSTYLE] = fmtstyle2string(static_cast<astyle::FormatStyle>(qcmb_style->currentItem()));
      }
      m_part->setExtensions(qtxe_generalext->text(), false);
      break;
    }
    default: { break; }
  }

  if(qcmb_style->currentItem() == ASOPTS_FSTYLE_USERDEFINED_ID) {
    // Filling and continuation:
    op[ASOPTS_FILL] = indentMode2String(static_cast<e_AStyle_IndentWith>(cmb_indentwith->currentItem()));

    op[ASOPTS_FILLCOUNT] = sb_count->value();
    op[ASOPTS_FILLFORCE] = chb_indentforce->isChecked();

    op[ASOPTS_FILLEMPTYLINES] = Fill_EmptyLines->isChecked();

    // Indent and continuation:
    op[ASOPTS_INDENTSWITCHES] = Indent_Switches->isChecked();
    op[ASOPTS_INDENTCASES] = Indent_Cases->isChecked();
    op[ASOPTS_INDENTCLASSES]= Indent_Classes->isChecked();
    op[ASOPTS_INDENTBRACKETS] = Indent_Brackets->isChecked();
    op[ASOPTS_INDENTNAMESPACES] = Indent_Namespaces->isChecked();
    op[ASOPTS_INDENTLABELS] = Indent_Labels->isChecked();
    op[ASOPTS_INDENTBLOCKS] = Indent_Blocks->isChecked();
    op[ASOPTS_INDENTPREPROCS] = Indent_Preprocessors->isChecked();
    op[ASOPTS_MAXSTATEMENT] = Continue_MaxStatement->value();
    op[ASOPTS_MINCONDITIONAL] = Continue_MinConditional->value();

    // Brackets:
    op[ASOPTS_BRACKETS] = bracketMode2String(static_cast<astyle::BracketMode>(cmb_brackets->currentItem()));
    op[ASOPTS_BRACKETS_CH] = Brackets_CloseHeaders->isChecked();
    op[ASOPTS_BRACKETS_ADD] = chb_add_brackets->isChecked();
    op[ASOPTS_BRACKETS_ADD_ONELINE] = chb_add_brackets_oneline->isChecked();

    // Blocks:
    op[ASOPTS_BLOCK_BREAK] = Block_Break->isChecked();
    op[ASOPTS_BLOCK_BREAKALL] = Block_BreakAll->isChecked();
    op[ASOPTS_BLOCK_BREAKIFELSE] = Block_IfElse->isChecked();

    // Oneliners:
    op[ASOPTS_KEEPSTATEMENTS] = Keep_Statements->isChecked();
    op[ASOPTS_KEEPBLOCKS] = Keep_Blocks->isChecked();

    // Padding:
    op[ASOPTS_PADPARENTH_IN] = Pad_ParenthesesIn->isChecked();
    op[ASOPTS_PADPARENTH_OUT] = Pad_ParenthesesOut->isChecked();
    op[ASOPTS_PADPARENTH_UN] = Pad_ParenthesesUn->isChecked();
    op[ASOPTS_PADOPERATORS] = Pad_Operators->isChecked();
    op[ASOPTS_PADHEADERS] = chb_parens_header->isChecked();
  }

#ifdef DEBUG
  for(QMap<QString, QVariant>::ConstIterator iter = op.begin(); iter != op.end(); iter++) {
    kdDebug(9009) << "widget: " << iter.key() << "=" << iter.data()  << endl;
  }
#endif
}

void AStyleWidget::set_useglobal() {
  if(m_context == PROJECT) {
    QMap<QString, QVariant> &options = m_part->getProjectOptions();
    m_bUseGlobalOpts = qchb_useglobal->isChecked();
    options[ASOPTS_USEGLOBAL] = m_bUseGlobalOpts;
    fillcontrols(options);
    set_example();
  }
}

void AStyleWidget::set_style() {
  QMap<QString, QVariant> &op = getContextOptions();
  if(qcmb_style->currentItem() == ASOPTS_FSTYLE_USERDEFINED_ID) {
    op[ASOPTS_FSTYLE] = ASOPTS_FSTYLE_USERDEFINED;
  } else {
    op[ASOPTS_FSTYLE] = fmtstyle2string((astyle::FormatStyle)qcmb_style->currentItem());
  }
  fillcontrols(op);

  set_example();
}

void AStyleWidget::set_fill() {
  if(qcmb_style->currentItem() == ASOPTS_FSTYLE_USERDEFINED_ID) {
    QMap<QString, QVariant> &op = getContextOptions();

    op[ASOPTS_FILL] = indentMode2String(static_cast<e_AStyle_IndentWith>(cmb_indentwith->currentItem()));

    op[ASOPTS_FILLCOUNT] = sb_count->value();
    op[ASOPTS_FILLFORCE] = chb_indentforce->isChecked();

    op[ASOPTS_FILLEMPTYLINES] = Fill_EmptyLines->isChecked();
    op[ASOPTS_MAXSTATEMENT] = Continue_MaxStatement->value();
    op[ASOPTS_MINCONDITIONAL] = Continue_MinConditional->value();
    set_example();
  }
}

void AStyleWidget::set_indent() {
  if(qcmb_style->currentItem() == ASOPTS_FSTYLE_USERDEFINED_ID) {
    QMap<QString, QVariant> &op = getContextOptions();
    op[ASOPTS_INDENTSWITCHES] = Indent_Switches->isChecked();
    op[ASOPTS_INDENTCASES] = Indent_Cases->isChecked();
    op[ASOPTS_INDENTCLASSES] = Indent_Classes->isChecked();
    op[ASOPTS_INDENTBRACKETS] = Indent_Brackets->isChecked();
    op[ASOPTS_INDENTNAMESPACES] = Indent_Namespaces->isChecked();
    op[ASOPTS_INDENTLABELS] = Indent_Labels->isChecked();
    op[ASOPTS_INDENTBLOCKS] = Indent_Blocks->isChecked();
    op[ASOPTS_INDENTPREPROCS] = Indent_Preprocessors->isChecked();
    set_example();
  }
}

void AStyleWidget::set_blocks_and_brackets() {
  if(qcmb_style->currentItem() == ASOPTS_FSTYLE_USERDEFINED_ID) {
    QMap<QString, QVariant> &op = getContextOptions();
    // Blocks
    op[ASOPTS_BLOCK_BREAK] = Block_Break->isChecked();
    op[ASOPTS_BLOCK_BREAKALL] = Block_BreakAll->isChecked();
    op[ASOPTS_BLOCK_BREAKIFELSE] = Block_IfElse->isChecked();
    op[ASOPTS_BRACKETS_CH] = Brackets_CloseHeaders->isChecked();
        // Brackets
    op[ASOPTS_BRACKETS_CH] = Brackets_CloseHeaders->isChecked();
    op[ASOPTS_BRACKETS_ADD] = chb_add_brackets->isChecked();
    op[ASOPTS_BRACKETS_ADD_ONELINE] = chb_add_brackets_oneline->isChecked();
        // One liners
    op[ASOPTS_KEEPSTATEMENTS] = Keep_Statements->isChecked();
    op[ASOPTS_KEEPBLOCKS] = Keep_Blocks->isChecked();

    /* NOTE: Since break-blocks=all is settings break-blocks to true too
        Block_Break should become disabled to show that influence */
    /* if(Block_BreakAll->isChecked()) {
      Block_Break->setEnabled(false);
      Block_Break->setChecked(true);
    }
    else {
      Block_Break->setEnabled(true);
    } */

    set_example();
  }
}

/*! AStyleWidget::slotSetBrackets()
 * \brief Sets value in options map (depending on context used), to determine how brackets would be styled
 * <B>Also enables break-closing-brackets toggle</B>
 * <A>
 * --break-closing-brackets  OR  -y
 * Break brackets before closing headers (e.g. 'else', 'catch', ...).
 * Use with --brackets=attach, --brackets=linux,
 * or --brackets=stroustrup.
 * </A>
 */
void AStyleWidget::slotSetBrackets(int mode) {
  if(mode < 6) {
    if(qcmb_style->currentItem() == ASOPTS_FSTYLE_USERDEFINED_ID) {
      QString brackets_mode;
      switch(static_cast<astyle::BracketMode>(mode)) {
        case astyle::ATTACH_MODE: {
          brackets_mode = ASOPTS_BRACKETS_ATTACH;
          Brackets_CloseHeaders->setEnabled(true);
          break;
        }
        case astyle::BREAK_MODE: {
          brackets_mode = ASOPTS_BRACKETS_BREAK;
          Brackets_CloseHeaders->setEnabled(false);
          break;
        }
        case astyle::LINUX_MODE: {
          brackets_mode = ASOPTS_BRACKETS_LINUX;
          Brackets_CloseHeaders->setEnabled(true);
          break;
        }
        case astyle::STROUSTRUP_MODE: {
          brackets_mode = ASOPTS_BRACKETS_STRAUSTRUP;
          Brackets_CloseHeaders->setEnabled(true);
          break;
        }
        case astyle::RUN_IN_MODE: {
          brackets_mode = ASOPTS_BRACKETS_RUNIN;
          Brackets_CloseHeaders->setEnabled(false);
          break;
        }
        default: {
          brackets_mode = ASOPTS_BRACKETS_NONE;
          Brackets_CloseHeaders->setEnabled(false);
          break;
        }
      }

      switch(m_context) {
        case GLOBAL: { m_part->getGlobalOptions()[ASOPTS_BRACKETS] = brackets_mode; break; }
        case PROJECT: { m_part->getProjectOptions()[ASOPTS_BRACKETS] = brackets_mode; break; }
      }

      set_example();
    }
  }
}

void AStyleWidget::slotSetIndentMode(int mode) {
  if(qcmb_style->currentItem() == ASOPTS_FSTYLE_USERDEFINED_ID) {
    QMap<QString, QVariant> &op = getContextOptions();

    op[ASOPTS_FILL] = indentMode2String(static_cast<e_AStyle_IndentWith>(mode));

    switch(mode) {
      case AS_SPACES: {
        chb_indentforce->setText("Con&vert tabs");
        break;
      }
      case AS_TABS: {
        chb_indentforce->setText("F&orce tabs");
        break;
      }
      default: { break; }
    }

    chb_indentforce->setChecked(op[ASOPTS_FILLFORCE].toBool());
    sb_count->setValue(op[ASOPTS_FILLCOUNT].toInt());

    set_example();
  }
}

void AStyleWidget::set_oneliners() {
  if(qcmb_style->currentItem() == ASOPTS_FSTYLE_USERDEFINED_ID) {
    QMap<QString, QVariant> &op = getContextOptions();
    op[ASOPTS_KEEPSTATEMENTS] = Keep_Statements->isChecked();
    op[ASOPTS_KEEPBLOCKS] = Keep_Blocks->isChecked();
    set_example();
  }
}

void AStyleWidget::set_padding() {
  if(qcmb_style->currentItem() == ASOPTS_FSTYLE_USERDEFINED_ID) {
    QMap<QString, QVariant> &op = getContextOptions();
    op[ASOPTS_PADPARENTH_IN] = Pad_ParenthesesIn->isChecked();
    op[ASOPTS_PADPARENTH_OUT] = Pad_ParenthesesOut->isChecked();
    op[ASOPTS_PADPARENTH_UN] = Pad_ParenthesesUn->isChecked();
    op[ASOPTS_PADOPERATORS] = Pad_Operators->isChecked();
    op[ASOPTS_PADHEADERS] = chb_parens_header->isChecked();
    set_example();
  }
}

void AStyleWidget::slotToolsPage(int page) {
  /*TODO: If "user defined" mode then deactivate all UI items" */
}

/**
 * Change the sample formatting text depending on settings.
 */
void AStyleWidget::set_example() {
  me_formatted->clear();
  me_example->clear();
  me_example->setText(astyle_example);
  switch(m_context) {
    case GLOBAL: {
      me_formatted->setText(m_part->formatSource(astyle_example, this, m_part->getGlobalOptions()));
      break;
    }
    case PROJECT: {
      me_formatted->setText(m_part->formatSource(astyle_example, this, m_part->getProjectOptions()));
      break;
    }
    default: { break; }
  }
}

void AStyleWidget::enable_controls() {
  qtxe_generalext->setEnabled(true);
  cmb_indentwith->setEnabled(true);
  sb_count->setEnabled(true);
  chb_indentforce->setEnabled(true);
  Continue_MaxStatement->setEnabled(true);
  Continue_MinConditional->setEnabled(true);
  Fill_EmptyLines->setEnabled(true);
  Indent_Blocks->setEnabled(true);
  Indent_Classes->setEnabled(true);
  Indent_Labels->setEnabled(true);
  Indent_Namespaces->setEnabled(true);
  Indent_Cases->setEnabled(true);
  Indent_Switches->setEnabled(true);
  Indent_Preprocessors->setEnabled(true);
  Indent_Brackets->setEnabled(true);
  cmb_brackets->setEnabled(true);
  Brackets_CloseHeaders->setEnabled(true);
  chb_add_brackets->setEnabled(true);
  chb_add_brackets_oneline->setEnabled(true);
  Block_Break->setEnabled(true);
  Block_BreakAll->setEnabled(true);
  Block_IfElse->setEnabled(true);
  Keep_Blocks->setEnabled(true);
  Keep_Statements->setEnabled(true);
  Pad_ParenthesesIn->setEnabled(true);
  Pad_ParenthesesOut->setEnabled(true);
  Pad_ParenthesesUn->setEnabled(true);
  Pad_Operators->setEnabled(true);
  chb_parens_header->setEnabled(true);
}

void AStyleWidget::disable_controls() {
  qtxe_generalext->setEnabled(false);
  cmb_indentwith->setEnabled(false);
  sb_count->setEnabled(false);
  chb_indentforce->setEnabled(false);
  Continue_MaxStatement->setEnabled(false);
  Continue_MinConditional->setEnabled(false);
  Fill_EmptyLines->setEnabled(false);
  Indent_Blocks->setEnabled(false);
  Indent_Classes->setEnabled(false);
  Indent_Labels->setEnabled(false);
  Indent_Namespaces->setEnabled(false);
  Indent_Cases->setEnabled(false);
  Indent_Switches->setEnabled(false);
  Indent_Preprocessors->setEnabled(false);
  Indent_Brackets->setEnabled(false);
  cmb_brackets->setEnabled(false);
  Brackets_CloseHeaders->setEnabled(false);
  chb_add_brackets->setEnabled(false);
  chb_add_brackets_oneline->setEnabled(false);
  Block_Break->setEnabled(false);
  Block_BreakAll->setEnabled(false);
  Block_IfElse->setEnabled(false);
  Keep_Blocks->setEnabled(false);
  Keep_Statements->setEnabled(false);
  Pad_ParenthesesIn->setEnabled(false);
  Pad_ParenthesesOut->setEnabled(false);
  Pad_ParenthesesUn->setEnabled(false);
  Pad_Operators->setEnabled(false);
  chb_parens_header->setEnabled(false);
}

/*inline*/ QMap<QString, QVariant>& AStyleWidget::getContextOptions() {
  if(m_context == GLOBAL) { return m_part->getGlobalOptions(); }
  return m_part->getProjectOptions();
}

#include "astyle_widget.moc"
