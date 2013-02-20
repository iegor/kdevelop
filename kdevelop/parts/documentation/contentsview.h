/***************************************************************************
 *   Copyright (C) 2004 by Alexander Dymo                                  *
 *   cloudtemple@mksat.net                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef CONTENTSVIEW_H
#define CONTENTSVIEW_H

#include <qwidget.h>

class FindDocumentation;
class DocumentationWidget;
class QListViewItem;
class KListView;

class ContentsView : public QWidget
{
    Q_OBJECT
public:
    ContentsView(DocumentationWidget *parent, const char *name = 0);
    ~ContentsView();
    
    KListView *view() const { return m_view; }

protected slots:
    void itemExecuted(QListViewItem *item, const QPoint &p, int col);
    void itemMouseButtonPressed(int button, QListViewItem *item, const QPoint &pos, int c);

protected:
    virtual void focusInEvent(QFocusEvent *e);
    
private:
    DocumentationWidget *m_widget;
    KListView *m_view;
    
friend class FindDocumentation;
};

#endif
