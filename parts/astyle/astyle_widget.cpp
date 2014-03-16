//kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;
#include "astyle_part.h"
#include "astyle_widget.h"

#include <qradiobutton.h>
#include <qtabwidget.h>
#include <qmultilineedit.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kdevcore.h>

#include <lib/astyle/astyle.h>

/*!
  Converts style string from kdevelop config to astyle enum value
*/
astyle::FormatStyle string2fmtstyle(QString &s) {
  if(s == "ALLMAN") return astyle::FormatStyle::STYLE_ALLMAN;
  else if(s == "JAVA") return astyle::FormatStyle::STYLE_JAVA;
  else if(s == "KR") return astyle::FormatStyle::STYLE_KR;
  else if(s == "STROUSTRUP") return astyle::FormatStyle::STYLE_STROUSTRUP;
  else if(s == "WHITESMITH") return astyle::FormatStyle::STYLE_WHITESMITH;
  else if(s == "BANNER") return astyle::FormatStyle::STYLE_BANNER;
  else if(s == "GNU") return astyle::FormatStyle::STYLE_GNU;
  else if(s == "Linux") return astyle::FormatStyle::STYLE_LINUX;
  else if(s == "HORSTMANN") return astyle::FormatStyle::STYLE_HORSTMANN;
  else if(s == "1TBS") return astyle::FormatStyle::STYLE_1TBS;
  else if(s == "GOOGLE") return astyle::FormatStyle::STYLE_GOOGLE;
  else if(s == "PICO") return astyle::FormatStyle::STYLE_PICO;
  else if(s == "LISP") return astyle::FormatStyle::STYLE_LISP;
  else return astyle::FormatStyle::STYLE_NONE;
}

/*!
  Converts astyle enum value to string usable in internal configuration
*/
QString fmtstyle2string(astyle::FormatStyle style) {
  switch(style) {
    case astyle::FormatStyle::STYLE_ALLMAN: return "ALLMAN";
    case astyle::FormatStyle::STYLE_JAVA: return "JAVA";
    case astyle::FormatStyle::STYLE_KR: return "KR";
    case astyle::FormatStyle::STYLE_STROUSTRUP: return "STROUSTRUP";
    case astyle::FormatStyle::STYLE_WHITESMITH: return "WHITESMITH";
    case astyle::FormatStyle::STYLE_BANNER: return "BANNER";
    case astyle::FormatStyle::STYLE_GNU: return "GNU";
    case astyle::FormatStyle::STYLE_LINUX: return "Linux";
    case astyle::FormatStyle::STYLE_HORSTMANN: return "HORSTMANN";
    case astyle::FormatStyle::STYLE_1TBS: return "1TBS";
    case astyle::FormatStyle::STYLE_GOOGLE: return "GOOGLE";
    case astyle::FormatStyle::STYLE_PICO: return "PICO";
    case astyle::FormatStyle::STYLE_LISP: return "LISP";
    default: return "NONE";
  }
}

AStyleWidget::AStyleWidget(AStylePart * part, AStyleWidget::context ctx, QWidget *parent, const char *name)
: AStyleConfig(parent, name)
, m_part(part)
, m_context(ctx)
, m_bUseGlobalOpts(false)
{
  qcmb_style->insertItem("Defaul (NONE)");
  qcmb_style->insertItem("Allman (ALLMAN)");
  qcmb_style->insertItem("Java (JAVA)");
  qcmb_style->insertItem("K&R (KR)");
  qcmb_style->insertItem("Stroustrup (STROUSTRUP)");
  qcmb_style->insertItem("Whitesmith (WHITESMITH)");
  qcmb_style->insertItem("Banner (BANNER)");
  qcmb_style->insertItem("GNU (GNU)");
  qcmb_style->insertItem("Linux (LINUX)");
  qcmb_style->insertItem("Horstmann (HORSTMANN)");
  qcmb_style->insertItem("1Tbs (1TBS)");
  qcmb_style->insertItem("Google (GOOGLE)");
  qcmb_style->insertItem("Pico (PICO)");
  qcmb_style->insertItem("List (LISP)");
  qcmb_style->insertItem("Userdefined");

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
#ifdef DEBUG
      kdDebug(9009) << "widget: Error: unknown context!" << endl;
#endif
      break;
  }
}

AStyleWidget::~AStyleWidget() {
}

void AStyleWidget::fillcontrols(QMap<QString, QVariant> &options) {
  // General:
  switch(m_context) {
    case GLOBAL:
      qchb_useglobal->setEnabled(false);
      qchb_useglobal->hide();
      qtxe_generalext->setEnabled(true);
      qtxe_generalext->setText(m_part->getGlobalExtensions());

      if(options[ASOPTS_FSTYLE] == "Userdefined") {
        enable_controls();
      } else {
        disable_controls();
      }
      break;
    case PROJECT:
      qchb_useglobal->show();
      m_bUseGlobalOpts = options[ASOPTS_USEGLOBAL].toBool();
      if(m_bUseGlobalOpts) {
        disable_controls();
        qtxe_generalext->setText(m_part->getGlobalExtensions());
      } else {
        qtxe_generalext->setText(m_part->getProjectExtensions());
        qtxe_generalext->setEnabled(true);
      }
      break;
  }
//   m_lastExt=GeneralExtension->text();
  QString s = options[ASOPTS_FSTYLE].toString();
  astyle::FormatStyle fstyle = string2fmtstyle(s);
  qcmb_style->setCurrentItem((int)fstyle);

  //Filling and Continuation:
  int wsCount = options[ASOPTS_FILLCOUNT].toInt();
  if (options[ASOPTS_FILL].toString() == "Tabs")
  {
    Fill_Tabs->setChecked(true);
    Fill_TabCount->setValue(wsCount);
    Fill_TabCount->setEnabled(true);
    Fill_ForceTabs->setChecked(option[ASOPTS_FILLFORCE].toBool());
    Fill_ForceTabs->setEnabled(true);
  }
  else
  {
    Fill_Spaces->setChecked(true);
    Fill_SpaceCount->setValue(wsCount);
    Fill_SpaceCount->setEnabled(true);
    Fill_ConvertTabs->setChecked(option[ASOPTS_FILLFORCE].toBool());
    Fill_ConvertTabs->setEnabled(true);
  }
  Fill_EmptyLines->setChecked(option[ASOPTS_FILLEMPTYLINES].toBool());
  Continue_MaxStatement->setValue(option[ASOPTS_MAXSTATEMENT].toInt());
  Continue_MinConditional->setValue(option[ASOPTS_MINCONDITIONAL].toInt());

  // Indentation:
  Indent_Switches->setChecked(option[ASOPTS_INDENTSWITCHES].toBool());
  Indent_Cases->setChecked(option[ASOPTS_INDENTCASES].toBool());
  Indent_Classes->setChecked(option[ASOPTS_INDENTCLASSES].toBool());
  Indent_Brackets->setChecked(option[ASOPTS_INDENTBRACKETS].toBool());
  Indent_Namespaces->setChecked(option[ASOPTS_INDENTNAMESPACES].toBool());
  Indent_Labels->setChecked(option[ASOPTS_INDENTLABELS].toBool());
  Indent_Blocks->setChecked(option[ASOPTS_INDENTBLOCKS].toBool());
  Indent_Preprocessors->setChecked(option[ASOPTS_INDENTPREPROCS].toBool());

  // Brackets:
  s = option[ASOPTS_BRACKETS].toString();
  if (s == "Break") Brackets_Break->setChecked(true);
  else if (s == "Attach") Brackets_Attach->setChecked(true);
  else if (s == "Linux") Brackets_Linux->setChecked(true);
  else Brackets_None->setChecked(true);
  Brackets_CloseHeaders->setChecked(option[ASOPTS_BRACKETS_CH].toBool());

  // Blocks:
  Block_Break->setChecked(option[ASOPTS_BLOCK_BREAK].toBool());
  Block_BreakAll->setChecked(option[ASOPTS_BLOCK_BREAKALL].toBool());
  Block_IfElse->setChecked(option[ASOPTS_BLOCK_BREAKIFELSE].toBool());

  // Oneliners
  Keep_Statements->setChecked(option[ASOPTS_KEEPSTATEMENTS].toBool());
  Keep_Blocks->setChecked(option[ASOPTS_KEEPBLOCKS].toBool());

  // Padding:
  Pad_ParenthesesIn->setChecked(option[ASOPTS_PADPARENTH_IN].toBool());
  Pad_ParenthesesOut->setChecked(option[ASOPTS_PADPARENTH_OUT].toBool());
  Pad_ParenthesesUn->setChecked(option[ASOPTS_PADPARENTH_UN].toBool());
  Pad_Operators->setChecked(option[ASOPTS_PADOPERATORS].toBool());
}

void AStyleWidget::accept() {
  QMap<QString, QVariant>* options;

  switch(m_context) {
    case GLOBAL:
      options = &(m_part->getGlobalOptions());
      m_part->setExtensions(qtxe_generalext->text(), true);
      break;
    case PROJECT:
      options = &(m_part->getProjectOptions());
      if(m_bUseGlobalOpts) {
        QMap<QString,QVariant>& global = m_part->getGlobalOptions();
        QMap<QString,QVariant>& project = m_part->getProjectOptions();
        project = global;
        project[ASOPTS_USEGLOBAL] = m_bUseGlobalOpts;
        return;
      }
      m_part->setExtensions(qtxe_generalext->text(), false);
      break;
    default: break;
  }

  // General:
  (*m_option)[ASOPTS_FSTYLE] = fmtstyle2string((astyle::FormatStyle)qcmb_style->currentItem());
  if(m_bUseGlobalOpts) {

  } else if(Style_UserDefined->isChecked()) {
    (*m_option)["FStyle"] = "UserDefined";

    // Filling and continuation:
    if(Fill_Tabs->isChecked()) {
      (*m_option)["Fill"] = "Tabs";
      (*m_option)["FillCount"] = Fill_TabCount->value();
      (*m_option)["FillForce"] = Fill_ForceTabs->isChecked();
    } else {
      (*m_option)["Fill"] = "Spaces";
      (*m_option)["FillCount"] = Fill_SpaceCount->value();
      (*m_option)["FillForce"] = Fill_ConvertTabs->isChecked();
    }
    (*m_option)["FillEmptyLines"] = Fill_EmptyLines->isChecked();

    // Indent and continuation:
    (*m_option)["IndentSwitches"] = Indent_Switches->isChecked();
    (*m_option)["IndentCases"] = Indent_Cases->isChecked();
    (*m_option)["IndentClasses"] = Indent_Classes->isChecked();
    (*m_option)["IndentBrackets"] = Indent_Brackets->isChecked();
    (*m_option)["IndentNamespaces"] = Indent_Namespaces->isChecked();
    (*m_option)["IndentLabels"] = Indent_Labels->isChecked();
    (*m_option)["IndentBlocks"] = Indent_Blocks->isChecked();
    (*m_option)["IndentPreprocessors"] = Indent_Preprocessors->isChecked();
    (*m_option)["MaxStatement"] = Continue_MaxStatement->value();
    (*m_option)["MinConditional"] = Continue_MinConditional->value();

    // Brackets:
    if(Brackets_None->isChecked()) (*m_option)["Brackets"] = "None";
    else if(Brackets_Break->isChecked()) (*m_option)["Brackets"] = "Break";
    else if(Brackets_Attach->isChecked()) (*m_option)["Brackets"] = "Attach";
    else if(Brackets_Linux->isChecked()) (*m_option)["Brackets"] = "Linux";
    (*m_option)["BracketsCloseHeaders"] = Brackets_CloseHeaders->isChecked();

    // Blocks:
    (*m_option)["BlockBreak"] = Block_Break->isChecked();
    (*m_option)["BlockBreakAll"] = Block_BreakAll->isChecked();
    (*m_option)["BlockIfElse"] = Block_IfElse->isChecked();

    // Oneliners:
    (*m_option)["KeepStatements"] = Keep_Statements->isChecked();
    (*m_option)["KeepBlocks"] = Keep_Blocks->isChecked();

    // Padding:
    (*m_option)["PadParenthesesIn"] = Pad_ParenthesesIn->isChecked();
    (*m_option)["PadParenthesesOut"] = Pad_ParenthesesOut->isChecked();
    (*m_option)["PadParenthesesUn"] = Pad_ParenthesesUn->isChecked();
    (*m_option)["PadOperators"] = Pad_Operators->isChecked();
  }

  if(isGlobalWidget) {
    QMap<QString, QVariant>& project = m_part->getProjectOptions();
    if(project["FStyle"] == "GLOBAL") {
      project = m_part->getGlobalOptions();
      project["FStyle"] = "GLOBAL";
    }
    m_part->saveGlobal();
  }

#ifdef DEBUG
  for(QMap<QString, QVariant>::ConstIterator iter = m_option->begin();iter != m_option->end();iter++) {
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
    case GLOBAL:
      QMap<QString, QVariant> &options = m_part->getGlobalOptions();
      options[ASOPTS_FSTYLE] = fmtstyle2string(qcmb_style->currentItem());
      fillcontrols(options);
      break;
    case PROJECT:
      QMap<QString, QVariant> &options = m_part->getProjectOptions();
      options[ASOPTS_FSTYLE] = fmtstyle2string(qcmb_style->currentItem());
      fillcontrols(options);
      break;
    default: break;
  }
  set_example();
}

void AStyleWidget::set_fill() {
  if(qcmb_style.currentItem() == 14) {
    switch(m_context) {
      case GLOBAL:
        QMap<QString, QVariant> &options = m_part->getGlobalOptions();
        if(Fill_Tabs->isChecked()) {
          options[ASOPTS_FILL] = = "Tabs";
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
        break;
      case PROJECT:
        QMap<QString, QVariant> &options = m_part->getProjectOptions();
        if(Fill_Tabs->isChecked()) {
          options[ASOPTS_FILL] = = "Tabs";
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
        break;
      default: break;
    }
    set_example();
  }
}

void AStyleWidget::set_indent() {
  if(qcmb_style.currentItem() == 14) {
    switch(m_context) {
      case GLOBAL:
        QMap<QString, QVariant> &options = m_part->getGlobalOptions();
        options[ASOPTS_INDENTSWITCHES] = Indent_Switches->isChecked();
        options[ASOPTS_INDENTCASES] = Indent_Cases->isChecked();
        options[ASOPTS_INDENTCLASSES] = Indent_Classes->isChecked();
        options[ASOPTS_INDENTBRACKETS] = Indent_Brackets->isChecked();
        options[ASOPTS_INDENTNAMESPACES] = Indent_Namespaces->isChecked();
        options[ASOPTS_INDENTLABELS] = Indent_Labels->isChecked();
        options[ASOPTS_INDENTBLOCKS] = Indent_Blocks->isChecked();
        options[ASOPTS_INDENTPREPROCS] = Indent_Preprocessors->isChecked();
        break;
      case PROJECT:
        QMap<QString, QVariant> &options = m_part->getProjectOptions();
        options[ASOPTS_INDENTSWITCHES] = Indent_Switches->isChecked();
        options[ASOPTS_INDENTCASES] = Indent_Cases->isChecked();
        options[ASOPTS_INDENTCLASSES] = Indent_Classes->isChecked();
        options[ASOPTS_INDENTBRACKETS] = Indent_Brackets->isChecked();
        options[ASOPTS_INDENTNAMESPACES] = Indent_Namespaces->isChecked();
        options[ASOPTS_INDENTLABELS] = Indent_Labels->isChecked();
        options[ASOPTS_INDENTBLOCKS] = Indent_Blocks->isChecked();
        options[ASOPTS_INDENTPREPROCS] = Indent_Preprocessors->isChecked();
        break;
      default: break;
    }
    set_example();
  }
}

void AStyleWidget::set_brackets() {
  if(qcmb_style.currentItem() == 14) {
    switch(m_context) {
      case GLOBAL:
        QMap<QString, QVariant> &options = m_part->getGlobalOptions();
        if(Brackets_None->isChecked()) options[ASOPTS_BRACKETS] = "None";
        else if(Brackets_Break->isChecked()) options[ASOPTS_BRACKETS] = "Break";
        else if(Brackets_Attach->isChecked()) options[ASOPTS_BRACKETS] = "Attach";
        else if(Brackets_Linux->isChecked()) options[ASOPTS_BRACKETS] = "Linux";
        options[ASOPTS_BRACKETS_CH] = Brackets_CloseHeaders->isChecked();
        break;
      case PROJECT:
        QMap<QString, QVariant> &options = m_part->getProjectOptions();
        if(Brackets_None->isChecked()) options[ASOPTS_BRACKETS] = "None";
        else if(Brackets_Break->isChecked()) options[ASOPTS_BRACKETS] = "Break";
        else if(Brackets_Attach->isChecked()) options[ASOPTS_BRACKETS] = "Attach";
        else if(Brackets_Linux->isChecked()) options[ASOPTS_BRACKETS] = "Linux";
        options[ASOPTS_BRACKETS_CH] = Brackets_CloseHeaders->isChecked();
        break;
      default: break;
    }
    set_example();
  }
}

void AStyleWidget::set_blocks() {
  if(qcmb_style.currentItem() == 14) {
    switch(m_context) {
      case GLOBAL:
        QMap<QString, QVariant> &options = m_part->getGlobalOptions();
        options[ASOPTS_BLOCK_BREAK] = Block_Break->isChecked();
        options[ASOPTS_BLOCK_BREAKALL] = Block_BreakAll->isChecked();
        options[ASOPTS_BLOCK_BREAKIFELSE] = Block_IfElse->isChecked();
        break;
      case PROJECT:
        QMap<QString, QVariant> &options = m_part->getProjectOptions();
        options[ASOPTS_BLOCK_BREAK] = Block_Break->isChecked();
        options[ASOPTS_BLOCK_BREAKALL] = Block_BreakAll->isChecked();
        options[ASOPTS_BLOCK_BREAKIFELSE] = Block_IfElse->isChecked();
        break;
      default: break;
    }
    set_example();
  }
}

void AStyleWidget::set_oneliners() {
  if(qcmb_style.currentItem() == 14) {
    switch(m_context) {
      case GLOBAL:
        QMap<QString, QVariant> &options = m_part->getGlobalOptions();
        options[ASOPTS_KEEPSTATEMENTS] = Keep_Statements->isChecked();
        options[ASOPTS_KEEPBLOCKS] = Keep_Blocks->isChecked();
        break;
      case PROJECT:
        QMap<QString, QVariant> &options = m_part->getProjectOptions();
        options[ASOPTS_KEEPSTATEMENTS] = Keep_Statements->isChecked();
        options[ASOPTS_KEEPBLOCKS] = Keep_Blocks->isChecked();
        break;
      default: break;
    }
    set_example();
  }
}

void AStyleWidget::set_padding() {
  if(qcmb_style.currentItem() == 14) {
    switch(m_context) {
      case GLOBAL:
        QMap<QString, QVariant> &options = m_part->getGlobalOptions();
        options[ASOPTS_PADPARENTH_IN] = Pad_ParenthesesIn->isChecked();
        options[ASOPTS_PADPARENTH_OUT] = Pad_ParenthesesOut->isChecked();
        options[ASOPTS_PADPARENTH_UN] = Pad_ParenthesesUn->isChecked();
        options[ASOPTS_PADOPERATORS] = Pad_Operators->isChecked();
        break;
      case PROJECT:
        QMap<QString, QVariant> &options = m_part->getProjectOptions();
        options[ASOPTS_PADPARENTH_IN] = Pad_ParenthesesIn->isChecked();
        options[ASOPTS_PADPARENTH_OUT] = Pad_ParenthesesOut->isChecked();
        options[ASOPTS_PADPARENTH_UN] = Pad_ParenthesesUn->isChecked();
        options[ASOPTS_PADOPERATORS] = Pad_Operators->isChecked();
        break;
      default: break;
    }
    set_example();
  }
}

/**
 * Change the sample formatting text depending on settings.
 */
void AStyleWidget::set_example() {
  QString example_text = "/* This file is part of the KDE project\
*\
* Copyright (C) 2004, 2005 Jakub Stachowski <qbast@go2.pl>\
*\
* This library is free software; you can redistribute it and/or\
* modify it under the terms of the GNU Library General Public\
* License as published by the Free Software Foundation; either\
* version 2 of the License, or (at your option) any later version.\
*\
* This library is distributed in the hope that it will be useful,\
* but WITHOUT ANY WARRANTY; without even the implied warranty of\
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\
* Library General Public License for more details.\
*\
* You should have received a copy of the GNU Library General Public License\
* along with this library; see the file COPYING.LIB.  If not, write to\
* the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,\
* Boston, MA 02110-1301, USA.\
*/\
\
#ifndef DNSSDREMOTESERVICE_H\
#define DNSSDREMOTESERVICE_H\
\
#include <qobject.h>\
#include <dnssd/servicebase.h>\
\
class QDataStream;\
class KURL;\
namespace DNSSD\
{\
class RemoteServicePrivate;\
\
/**\
RemoteService class allows to resolve service announced on remote machine. In most cases objects\
of this class are created by ServiceBrowser, but it is not required. Only fields valid before \
service is resolved are name, type.and domain. \
\
\
@short class representing service announced on remote machine.\
@author Jakub Stachowski\
*/\
class KDNSSD_EXPORT RemoteService : public QObject, public ServiceBase\
{\
  Q_OBJECT\
  public:\
  typedef KSharedPtr<RemoteService> Ptr;\
  \
/**\
  Creates unresolved service from given DNS label\
  @param label Data returned by PTR query - it is decoded into name, type and \
  domain\
  */\
  RemoteService(const QString& label);\
  \
/**\
  Creates unresolved remote service with given name, type and domain.\
  */\
  RemoteService(const QString& name,const QString& type,const QString& domain);\
  \
/**\
  Creates resolved remote service from invitation URL constructed by PublicService::toInvitation.\
  If URL was invalid, service is set to unresolved and other fields should not be used.\
  */\
  RemoteService(const KURL& url);\
  \
  virtual ~RemoteService();\
  \
/**\
  Resolves host name and port of service. Host name is not resolved into numeric\
  address - use KResolver for that. Signal resolved(bool) will be emitted \
  when finished or even before return of this function - in case of immediate failure.\
  */\
  void resolveAsync();\
  \
/**\
  Synchronous version of resolveAsync(). Note that resolved(bool) is emitted \
  before this function returns, \
  @return TRUE is successful\
  */\
  bool resolve();\
  \
/**\
  Returns true if service has been successfully resolved\
  */\
  bool isResolved() const;\
  \
  signals:\
/**\
  Emitted when resolving is complete. Parameter is set to TRUE if it was successful.\
  If operating in asynchronous mode this signal can be emitted several times (when \
  service change)\
  */\
  void resolved(bool);\
  \
  protected:\
  virtual void virtual_hook(int id, void *data);\
  virtual void customEvent(QCustomEvent* event);\
  private:\
  void resolveError();\
  void resolved(const char *host, unsigned short port, unsigned short txtlen,\
  const char* txtRecord);\
  RemoteServicePrivate *d;\
  \
  friend KDNSSD_EXPORT QDataStream & operator<< (QDataStream & s, const RemoteService & a);\
  friend KDNSSD_EXPORT QDataStream & operator>> (QDataStream & s, RemoteService & a);\
  \
};\
\
}\
\
#endif";

  StyleExample->clear();

  switch(m_context) {
    case GLOBAL:
      StyleExample->setText(m_part->formatSource(example_text, this, m_part->getGlobalOptions()));
      break;
    case PROJECT:
      StyleExample->setText(m_part->formatSource(example_text, this, m_part->getProjectOptions()));
      break;
    default: break;
  }

// 	if(Style_Global->isChecked()) {
// 		if(!globalOptions) {
// 			GeneralExtension->setEnabled(false);
// 			GeneralExtension
// 			globalOptions=!globalOptions;
// 		}
// 	} else {
// 		if ( globalOptions){
// 			GeneralExtension->setEnabled ( true );
// 			GeneralExtension->setText ( m_lastExt );
// 			globalOptions=!globalOptions;
// 		}
// 	}
}

void AStyleWidget::enable_controls() {
  qcmb_style->setEnabled(true);
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
  qcmb_style->setEnabled(false);
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
