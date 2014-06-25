//kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;
#include "astyle_part.h"
#include "astyle_adaptor.h"
#include "astyle_widget.h"

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
"    void foo1() { if(false) { never_exec(); } }\n"
"\n"
"    void foo()\n"
"    {\n"
"    }\n"
"\n"
"    /*! This is a test method\n"
"     * Doxy /a comment\n"
"     */\n"
"    inline void oneliner() { /*comment*/ if(true) makefoo();  }\n"
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
  qcmb_style->insertItem(QString("Defaul (NONE)"));
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

  connect(FillingGroup, SIGNAL(clicked(int)), this, SLOT(set_fill()));
  connect(Fill_ForceTabs, SIGNAL(clicked()), this, SLOT(set_fill()));
  connect(Fill_TabCount, SIGNAL(valueChanged(int)), this, SLOT(set_fill()));
  connect(Fill_SpaceCount, SIGNAL(valueChanged(int)), this, SLOT(set_fill()));
  connect(Continue_MaxStatement, SIGNAL(valueChanged(int)), this, SLOT(set_fill()));
  connect(Continue_MinConditional, SIGNAL(valueChanged(int)), this, SLOT(set_fill()));

  connect(Indent_Switches, SIGNAL(clicked()), this, SLOT(set_indent()));
  connect(Indent_Cases, SIGNAL(clicked()), this, SLOT(set_indent()));
  connect(Indent_Classes, SIGNAL(clicked()), this, SLOT(set_indent()));
  connect(Indent_Brackets, SIGNAL(clicked()), this, SLOT(set_indent()));
  connect(Indent_Namespaces, SIGNAL(clicked()), this, SLOT(set_indent()));
  connect(Indent_Labels, SIGNAL(clicked()), this, SLOT(set_indent()));
  connect(Indent_Blocks, SIGNAL(clicked()), this, SLOT(set_indent()));
  connect(Indent_Preprocessors, SIGNAL(clicked()), this, SLOT(set_indent()));

  connect(BracketGroup, SIGNAL(clicked(int)), this, SLOT(set_brackets()));
  connect(Brackets_CloseHeaders, SIGNAL(clicked()), this, SLOT(set_brackets()));

  connect(Block_Break, SIGNAL(clicked()), this, SLOT(set_blocks()));
  connect(Block_BreakAll, SIGNAL(clicked()), this, SLOT(set_blocks()));
  connect(Block_IfElse, SIGNAL(clicked()), this, SLOT(set_blocks()));

  connect(Keep_Statements, SIGNAL(clicked()), this, SLOT(set_oneliners()));
  connect(Keep_Blocks, SIGNAL(clicked()), this, SLOT(set_oneliners()));

  connect(Pad_ParenthesesIn, SIGNAL(clicked()), this, SLOT(set_padding()));
  connect(Pad_ParenthesesOut, SIGNAL(clicked()), this, SLOT(set_padding()));
  connect(Pad_ParenthesesUn, SIGNAL(clicked()), this, SLOT(set_padding()));
  connect(Pad_Operators, SIGNAL(clicked()), this, SLOT(set_padding()));

  switch(m_context) {
    case GLOBAL: fillcontrols(m_part->getGlobalOptions()); break;
    case PROJECT: fillcontrols(m_part->getProjectOptions()); break;
    default:
      break;
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
    case GLOBAL:
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
    case PROJECT:
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
//   m_lastExt=GeneralExtension->text();

  //Filling and Continuation:
  int wsCount = options[ASOPTS_FILLCOUNT].toInt();
  if(options[ASOPTS_FILL].toString() == "Tabs") {
    Fill_Tabs->setChecked(true);
    Fill_TabCount->setValue(wsCount);
    Fill_ForceTabs->setChecked(options[ASOPTS_FILLFORCE].toBool());
    if(!m_bUseGlobalOpts && (options[ASOPTS_FSTYLE] == ASOPTS_FSTYLE_USERDEFINED)) {
      Fill_TabCount->setEnabled(true);
      Fill_ForceTabs->setEnabled(true);
    }
  } else {
    Fill_Spaces->setChecked(true);
    Fill_SpaceCount->setValue(wsCount);
    Fill_ConvertTabs->setChecked(options[ASOPTS_FILLFORCE].toBool());
    if(!m_bUseGlobalOpts && (options[ASOPTS_FSTYLE] == ASOPTS_FSTYLE_USERDEFINED)) {
      Fill_SpaceCount->setEnabled(true);
      Fill_ConvertTabs->setEnabled(true);
    }
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

  // Brackets:
  s = options[ASOPTS_BRACKETS].toString();
  if(s == "Break") Brackets_Break->setChecked(true);
  else if(s == "Attach") Brackets_Attach->setChecked(true);
  else if(s == "Linux") Brackets_Linux->setChecked(true);
  else Brackets_None->setChecked(true);
  Brackets_CloseHeaders->setChecked(options[ASOPTS_BRACKETS_CH].toBool());

  // Blocks:
  Block_Break->setChecked(options[ASOPTS_BLOCK_BREAK].toBool());
  Block_BreakAll->setChecked(options[ASOPTS_BLOCK_BREAKALL].toBool());
  Block_IfElse->setChecked(options[ASOPTS_BLOCK_BREAKIFELSE].toBool());

  // Oneliners
  Keep_Statements->setChecked(options[ASOPTS_KEEPSTATEMENTS].toBool());
  Keep_Blocks->setChecked(options[ASOPTS_KEEPBLOCKS].toBool());

  // Padding:
  Pad_ParenthesesIn->setChecked(options[ASOPTS_PADPARENTH_IN].toBool());
  Pad_ParenthesesOut->setChecked(options[ASOPTS_PADPARENTH_OUT].toBool());
  Pad_ParenthesesUn->setChecked(options[ASOPTS_PADPARENTH_UN].toBool());
  Pad_Operators->setChecked(options[ASOPTS_PADOPERATORS].toBool());
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
  QMap<QString, QVariant>* options;

  switch(m_context) {
    case GLOBAL:
      options = &(m_part->getGlobalOptions());
      if(qcmb_style->currentItem() == ASOPTS_FSTYLE_USERDEFINED_ID) {
        // General:
        (*options)[ASOPTS_FSTYLE] = ASOPTS_FSTYLE_USERDEFINED;
      } else {
        (*options)[ASOPTS_FSTYLE] = fmtstyle2string((enum astyle::FormatStyle)qcmb_style->currentItem());
      }
      m_part->setExtensions(qtxe_generalext->text(), true);

      // Check and update project settings accordingly
      options = &(m_part->getProjectOptions());
      m_bUseGlobalOpts = (*options)[ASOPTS_USEGLOBAL].toBool();
      if(m_bUseGlobalOpts) {
        (*options) = m_part->getGlobalOptions();
        (*options)[ASOPTS_USEGLOBAL] = m_bUseGlobalOpts;
      }
      break;
    case PROJECT:
      options = &(m_part->getProjectOptions());
      if(m_bUseGlobalOpts) {
        QMap<QString,QVariant>& global = m_part->getGlobalOptions();
//         QMap<QString,QVariant>& project = m_part->getProjectOptions();
        (*options) = global;
        (*options)[ASOPTS_USEGLOBAL] = m_bUseGlobalOpts;
        m_part->saveGlobal();
        return;
      } else {
        (*options)[ASOPTS_FSTYLE] = fmtstyle2string((enum astyle::FormatStyle)qcmb_style->currentItem());
      }
      m_part->setExtensions(qtxe_generalext->text(), false);
      break;
    default: break;
  }

  if(qcmb_style->currentItem() == ASOPTS_FSTYLE_USERDEFINED_ID) {
    // Filling and continuation:
    if(Fill_Tabs->isChecked()) {
      (*options)[ASOPTS_FILL] = "Tabs";
      (*options)[ASOPTS_FILLCOUNT] = Fill_TabCount->value();
      (*options)[ASOPTS_FILLFORCE] = Fill_ForceTabs->isChecked();
    } else {
      (*options)[ASOPTS_FILL] = "Spaces";
      (*options)[ASOPTS_FILLCOUNT] = Fill_SpaceCount->value();
      (*options)[ASOPTS_FILLFORCE] = Fill_ConvertTabs->isChecked();
    }
    (*options)[ASOPTS_FILLEMPTYLINES] = Fill_EmptyLines->isChecked();

    // Indent and continuation:
    (*options)[ASOPTS_INDENTSWITCHES] = Indent_Switches->isChecked();
    (*options)[ASOPTS_INDENTCASES] = Indent_Cases->isChecked();
    (*options)[ASOPTS_INDENTCLASSES]= Indent_Classes->isChecked();
    (*options)[ASOPTS_INDENTBRACKETS] = Indent_Brackets->isChecked();
    (*options)[ASOPTS_INDENTNAMESPACES] = Indent_Namespaces->isChecked();
    (*options)[ASOPTS_INDENTLABELS] = Indent_Labels->isChecked();
    (*options)[ASOPTS_INDENTBLOCKS] = Indent_Blocks->isChecked();
    (*options)[ASOPTS_INDENTPREPROCS] = Indent_Preprocessors->isChecked();
    (*options)[ASOPTS_MAXSTATEMENT] = Continue_MaxStatement->value();
    (*options)[ASOPTS_MINCONDITIONAL] = Continue_MinConditional->value();

    // Brackets:
    if(Brackets_None->isChecked()) (*options)[ASOPTS_BRACKETS] = "None";
    else if(Brackets_Break->isChecked()) (*options)[ASOPTS_BRACKETS] = "Break";
    else if(Brackets_Attach->isChecked()) (*options)[ASOPTS_BRACKETS] = "Attach";
    else if(Brackets_Linux->isChecked()) (*options)[ASOPTS_BRACKETS] = "Linux";
    (*options)[ASOPTS_BRACKETS_CH] = Brackets_CloseHeaders->isChecked();

    // Blocks:
    (*options)[ASOPTS_BLOCK_BREAK] = Block_Break->isChecked();
    (*options)[ASOPTS_BLOCK_BREAKALL] = Block_BreakAll->isChecked();
    (*options)[ASOPTS_BLOCK_BREAKIFELSE] = Block_IfElse->isChecked();

    // Oneliners:
    (*options)[ASOPTS_KEEPSTATEMENTS] = Keep_Statements->isChecked();
    (*options)[ASOPTS_KEEPBLOCKS] = Keep_Blocks->isChecked();

    // Padding:
    (*options)[ASOPTS_PADPARENTH_IN] = Pad_ParenthesesIn->isChecked();
    (*options)[ASOPTS_PADPARENTH_OUT] = Pad_ParenthesesOut->isChecked();
    (*options)[ASOPTS_PADPARENTH_UN] = Pad_ParenthesesUn->isChecked();
    (*options)[ASOPTS_PADOPERATORS] = Pad_Operators->isChecked();
  }

#ifdef DEBUG
  for(QMap<QString, QVariant>::ConstIterator iter = options->begin(); iter != options->end(); iter++) {
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
  switch(m_context) {
    case GLOBAL: {
      QMap<QString, QVariant> &options = m_part->getGlobalOptions();
      if(qcmb_style->currentItem() == ASOPTS_FSTYLE_USERDEFINED_ID) {
        options[ASOPTS_FSTYLE] = ASOPTS_FSTYLE_USERDEFINED;
      } else {
        options[ASOPTS_FSTYLE] = fmtstyle2string((enum astyle::FormatStyle)qcmb_style->currentItem());
      }
      fillcontrols(options);
    } break;
    case PROJECT: {
      QMap<QString, QVariant> &options = m_part->getProjectOptions();
      if(qcmb_style->currentItem() == ASOPTS_FSTYLE_USERDEFINED_ID) {
        options[ASOPTS_FSTYLE] = ASOPTS_FSTYLE_USERDEFINED;
      } else {
        options[ASOPTS_FSTYLE] = fmtstyle2string((enum astyle::FormatStyle)qcmb_style->currentItem());
      }
      fillcontrols(options);
    } break;
    default: break;
  }
  set_example();
}

void AStyleWidget::set_fill() {
  if(qcmb_style->currentItem() == ASOPTS_FSTYLE_USERDEFINED_ID) {
    switch(m_context) {
      case GLOBAL: {
        QMap<QString, QVariant> &options = m_part->getGlobalOptions();
        if(Fill_Tabs->isChecked()) {
          options[ASOPTS_FILL] = "Tabs";
          options[ASOPTS_FILLCOUNT] = Fill_TabCount->value();
          options[ASOPTS_FILLFORCE] = Fill_ForceTabs->isChecked();
        } else {
          options[ASOPTS_FILL] = "Spaces";
          options[ASOPTS_FILLCOUNT] = Fill_SpaceCount->value();
          options[ASOPTS_FILLFORCE] = Fill_ConvertTabs->isChecked();
        }
        options[ASOPTS_FILLEMPTYLINES] = Fill_EmptyLines->isChecked();
        options[ASOPTS_MAXSTATEMENT] = Continue_MaxStatement->value();
        options[ASOPTS_MINCONDITIONAL] = Continue_MinConditional->value();
      } break;
      case PROJECT: {
        QMap<QString, QVariant> &options = m_part->getProjectOptions();
        if(Fill_Tabs->isChecked()) {
          options[ASOPTS_FILL] = "Tabs";
          options[ASOPTS_FILLCOUNT] = Fill_TabCount->value();
          options[ASOPTS_FILLFORCE] = Fill_ForceTabs->isChecked();
        } else {
          options[ASOPTS_FILL] = "Spaces";
          options[ASOPTS_FILLCOUNT] = Fill_SpaceCount->value();
          options[ASOPTS_FILLFORCE] = Fill_ConvertTabs->isChecked();
        }
        options[ASOPTS_FILLEMPTYLINES] = Fill_EmptyLines->isChecked();
        options[ASOPTS_MAXSTATEMENT] = Continue_MaxStatement->value();
        options[ASOPTS_MINCONDITIONAL] = Continue_MinConditional->value();
      } break;
      default: break;
    }
    set_example();
  }
}

void AStyleWidget::set_indent() {
  if(qcmb_style->currentItem() == ASOPTS_FSTYLE_USERDEFINED_ID) {
    switch(m_context) {
      case GLOBAL: {
        QMap<QString, QVariant> &options = m_part->getGlobalOptions();
        options[ASOPTS_INDENTSWITCHES] = Indent_Switches->isChecked();
        options[ASOPTS_INDENTCASES] = Indent_Cases->isChecked();
        options[ASOPTS_INDENTCLASSES] = Indent_Classes->isChecked();
        options[ASOPTS_INDENTBRACKETS] = Indent_Brackets->isChecked();
        options[ASOPTS_INDENTNAMESPACES] = Indent_Namespaces->isChecked();
        options[ASOPTS_INDENTLABELS] = Indent_Labels->isChecked();
        options[ASOPTS_INDENTBLOCKS] = Indent_Blocks->isChecked();
        options[ASOPTS_INDENTPREPROCS] = Indent_Preprocessors->isChecked();
      } break;
      case PROJECT: {
        QMap<QString, QVariant> &options = m_part->getProjectOptions();
        options[ASOPTS_INDENTSWITCHES] = Indent_Switches->isChecked();
        options[ASOPTS_INDENTCASES] = Indent_Cases->isChecked();
        options[ASOPTS_INDENTCLASSES] = Indent_Classes->isChecked();
        options[ASOPTS_INDENTBRACKETS] = Indent_Brackets->isChecked();
        options[ASOPTS_INDENTNAMESPACES] = Indent_Namespaces->isChecked();
        options[ASOPTS_INDENTLABELS] = Indent_Labels->isChecked();
        options[ASOPTS_INDENTBLOCKS] = Indent_Blocks->isChecked();
        options[ASOPTS_INDENTPREPROCS] = Indent_Preprocessors->isChecked();
      } break;
      default: break;
    }
    set_example();
  }
}

void AStyleWidget::set_brackets() {
  if(qcmb_style->currentItem() == ASOPTS_FSTYLE_USERDEFINED_ID) {
    switch(m_context) {
      case GLOBAL: {
        QMap<QString, QVariant> &options = m_part->getGlobalOptions();
        if(Brackets_None->isChecked()) options[ASOPTS_BRACKETS] = "None";
        else if(Brackets_Break->isChecked()) options[ASOPTS_BRACKETS] = "Break";
        else if(Brackets_Attach->isChecked()) options[ASOPTS_BRACKETS] = "Attach";
        else if(Brackets_Linux->isChecked()) options[ASOPTS_BRACKETS] = "Linux";
        options[ASOPTS_BRACKETS_CH] = Brackets_CloseHeaders->isChecked();
      } break;
      case PROJECT: {
        QMap<QString, QVariant> &options = m_part->getProjectOptions();
        if(Brackets_None->isChecked()) options[ASOPTS_BRACKETS] = "None";
        else if(Brackets_Break->isChecked()) options[ASOPTS_BRACKETS] = "Break";
        else if(Brackets_Attach->isChecked()) options[ASOPTS_BRACKETS] = "Attach";
        else if(Brackets_Linux->isChecked()) options[ASOPTS_BRACKETS] = "Linux";
        options[ASOPTS_BRACKETS_CH] = Brackets_CloseHeaders->isChecked();
      } break;
      default: break;
    }
    set_example();
  }
}

void AStyleWidget::set_blocks() {
  if(qcmb_style->currentItem() == ASOPTS_FSTYLE_USERDEFINED_ID) {
    switch(m_context) {
      case GLOBAL: {
        QMap<QString, QVariant> &options = m_part->getGlobalOptions();
        options[ASOPTS_BLOCK_BREAK] = Block_Break->isChecked();
        options[ASOPTS_BLOCK_BREAKALL] = Block_BreakAll->isChecked();
        options[ASOPTS_BLOCK_BREAKIFELSE] = Block_IfElse->isChecked();
      } break;
      case PROJECT: {
        QMap<QString, QVariant> &options = m_part->getProjectOptions();
        options[ASOPTS_BLOCK_BREAK] = Block_Break->isChecked();
        options[ASOPTS_BLOCK_BREAKALL] = Block_BreakAll->isChecked();
        options[ASOPTS_BLOCK_BREAKIFELSE] = Block_IfElse->isChecked();
      } break;
      default: break;
    }
    set_example();
  }
}

void AStyleWidget::set_oneliners() {
  if(qcmb_style->currentItem() == ASOPTS_FSTYLE_USERDEFINED_ID) {
    switch(m_context) {
      case GLOBAL: {
        QMap<QString, QVariant> &options = m_part->getGlobalOptions();
        options[ASOPTS_KEEPSTATEMENTS] = Keep_Statements->isChecked();
        options[ASOPTS_KEEPBLOCKS] = Keep_Blocks->isChecked();
      } break;
      case PROJECT: {
        QMap<QString, QVariant> &options = m_part->getProjectOptions();
        options[ASOPTS_KEEPSTATEMENTS] = Keep_Statements->isChecked();
        options[ASOPTS_KEEPBLOCKS] = Keep_Blocks->isChecked();
      } break;
      default: break;
    }
    set_example();
  }
}

void AStyleWidget::set_padding() {
  if(qcmb_style->currentItem() == ASOPTS_FSTYLE_USERDEFINED_ID) {
    switch(m_context) {
      case GLOBAL: {
        QMap<QString, QVariant> &options = m_part->getGlobalOptions();
        options[ASOPTS_PADPARENTH_IN] = Pad_ParenthesesIn->isChecked();
        options[ASOPTS_PADPARENTH_OUT] = Pad_ParenthesesOut->isChecked();
        options[ASOPTS_PADPARENTH_UN] = Pad_ParenthesesUn->isChecked();
        options[ASOPTS_PADOPERATORS] = Pad_Operators->isChecked();
      } break;
      case PROJECT: {
        QMap<QString, QVariant> &options = m_part->getProjectOptions();
        options[ASOPTS_PADPARENTH_IN] = Pad_ParenthesesIn->isChecked();
        options[ASOPTS_PADPARENTH_OUT] = Pad_ParenthesesOut->isChecked();
        options[ASOPTS_PADPARENTH_UN] = Pad_ParenthesesUn->isChecked();
        options[ASOPTS_PADOPERATORS] = Pad_Operators->isChecked();
      } break;
      default: break;
    }
    set_example();
  }
}

/**
 * Change the sample formatting text depending on settings.
 */
void AStyleWidget::set_example() {
  StyleExample->clear();
  switch(m_context) {
    case GLOBAL: {
      StyleExample->setText(m_part->formatSource(astyle_example, this, m_part->getGlobalOptions()));
    } break;
    case PROJECT: {
      StyleExample->setText(m_part->formatSource(astyle_example, this, m_part->getProjectOptions()));
    } break;
    default: break;
  }
}

void AStyleWidget::enable_controls() {
  qtxe_generalext->setEnabled(true);
  Fill_Tabs->setEnabled(true);
  Fill_TabCount->setEnabled(true);
  Fill_ForceTabs->setEnabled(true);
  Fill_Spaces->setEnabled(true);
  Fill_SpaceCount->setEnabled(true);
  Fill_ConvertTabs->setEnabled(true);
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
  Brackets_Attach->setEnabled(true);
  Brackets_Linux->setEnabled(true);
  Brackets_Break->setEnabled(true);
  Brackets_None->setEnabled(true);
  Brackets_CloseHeaders->setEnabled(true);
  Block_Break->setEnabled(true);
  Block_BreakAll->setEnabled(true);
  Block_IfElse->setEnabled(true);
  Keep_Blocks->setEnabled(true);
  Keep_Statements->setEnabled(true);
  Pad_ParenthesesIn->setEnabled(true);
  Pad_ParenthesesOut->setEnabled(true);
  Pad_ParenthesesUn->setEnabled(true);
  Pad_Operators->setEnabled(true);
}

void AStyleWidget::disable_controls() {
  qtxe_generalext->setEnabled(false);
  Fill_Tabs->setEnabled(false);
  Fill_TabCount->setEnabled(false);
  Fill_ForceTabs->setEnabled(false);
  Fill_Spaces->setEnabled(false);
  Fill_SpaceCount->setEnabled(false);
  Fill_ConvertTabs->setEnabled(false);
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
  Brackets_Attach->setEnabled(false);
  Brackets_Linux->setEnabled(false);
  Brackets_Break->setEnabled(false);
  Brackets_None->setEnabled(false);
  Brackets_CloseHeaders->setEnabled(false);
  Block_Break->setEnabled(false);
  Block_BreakAll->setEnabled(false);
  Block_IfElse->setEnabled(false);
  Keep_Blocks->setEnabled(false);
  Keep_Statements->setEnabled(false);
  Pad_ParenthesesIn->setEnabled(false);
  Pad_ParenthesesOut->setEnabled(false);
  Pad_ParenthesesUn->setEnabled(false);
  Pad_Operators->setEnabled(false);
}
#include "astyle_widget.moc"
