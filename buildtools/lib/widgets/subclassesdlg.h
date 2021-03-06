/* This file is part of the KDE project
   Copyright (C) 2003 Alexander Dymo <cloudtemple@mksat.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef SUBCLASSESDLG_H
#define SUBCLASSESDLG_H
#include "domutil.h"
#include "subclassesdlgbase.h"

/**
Subclass creation dialog.
*/
class SubclassesDlg : public SubclassesDlgBase
{
  Q_OBJECT

public:
    SubclassesDlg(QString form, DomUtil::PairList &config, QString projectDir, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SubclassesDlg();
  /*$PUBLIC_FUNCTIONS$*/

public slots:
  /*$PUBLIC_SLOTS$*/
    virtual void accept();

protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/
  virtual void          newRelation();
  virtual void          removeRelation();
  virtual void          changeCurrentURL(const QString &str);
  virtual void          currentRelationChanged(QListBoxItem *item);

private:
    QString m_form;
    DomUtil::PairList &m_config;
    QString m_projectDir;
};

#endif

