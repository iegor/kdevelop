#include "qlistview.h" /* defines QListViewItem */
/*
*kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;
*
*  C/C++ Implementation: vs
*
* Description:
*  ! Parts were inherited from autoprojectpart src
*
* Author: iegor <rmtdev@gmail.com>, (C) 2013
*
* Copyright: See COPYING file that comes with this distribution
*/
#ifndef __VSMANAGER_WIDGET_H__
#define __VSMANAGER_WIDGET_H__

#include <qmap.h>
#include <qtooltip.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qlistview.h>

// #include <codemodel.h>
// #include <fancylistviewitem.h>

#include "vs_explorer_widget.h"

class KSelectAction;
class KToggleAction;
class TargetItem;
class FileItem;
class AutoProjectPart;

//BEGIN //VStudio namespace
namespace VStudio {
  class VSPart;
  //TODO: A template would look nicer
  // struct FindOp { const FunctionDom& m_dom; };
  // struct FindOp2 { const FunctionDefinitionDom& m_dom; };

  class VSExplorer : public VsExplorerWidget {
    Q_OBJECT
  public:
    VSExplorer(VSPart *part, QWidget *parent=0, const char *name=0);
    virtual ~VSExplorer();
  //   bool selectItem(ItemDom item);
    bool addSolutionNode(const QString &name);
    bool addProjectNode(const QString &name);
  protected:
    void contentsContextMenuEvent(QContextMenuEvent*);
    void maybeTip(QPoint const &);

  private slots:
    void slotProjectOpened();
    void slotProjectClosed();

  private:
    VSPart* m_part;
    QString m_projectDirectory;
    int m_projectDirectoryLength;
  //   TextPaintStyleStore m_paintStyles;
  };

  /**
  * Base class for all items
  */
  class VSExplorerEntity : public QListViewItem {
  public:
    enum Type { Solution, Project, File };
  public:
    VSExplorerEntity(Type type, QListView *parent, const QString &text);
    VSExplorerEntity(Type type, VSExplorerEntity *parent, const QString &text);
    void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment);
    void setBold(bool b) { bld = b; }
    bool isBold() const { return bld; }
    Type type() { return typ; }
  private:
    Type typ;
    bool bld;
  };

  /**
  * Solution node for vs widget
  */
  class VSSlnNode : public VSExplorerEntity {
  public:
    VSSlnNode(QListView *lv, const QString &text);
  private:
    QString name;
    QString uiFileLink;
  };
};
//END // VStudio namespace
#endif /* __VSMANAGER_WIDGET_H__ */
