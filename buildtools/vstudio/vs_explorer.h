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
#include "vs_model.h"

class KSelectAction;
class KToggleAction;
class TargetItem;
class FileItem;
class AutoProjectPart;

//BEGIN //VStudio namespace
namespace VStudio {
  class VSPart;
  class VSSlnNode;
  class VSPrjNode;
  //TODO: A template would look nicer
  // struct FindOp { const FunctionDom& m_dom; };
  // struct FindOp2 { const FunctionDefinitionDom& m_dom; };

  class VSExplorer : public VsExplorerWidget {
    Q_OBJECT
  public:
    VSExplorer(VSPart *part, QWidget *parent=0, const char *name=0);
    virtual ~VSExplorer();
  //   bool selectItem(ItemDom item);
    VSSlnNode* addSolutionNode(VSSolution *sln);
    VSPrjNode* addProjectNode(VSSlnNode *sln, VSProject *prj);
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
    VSExplorerEntity(e_VSEntityType type, QListView *parent, const QString &text);
    VSExplorerEntity(e_VSEntityType type, VSExplorerEntity *parent, const QString &text);
    void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment);
    e_VSEntityType type() { return typ; }
  protected:
    e_VSEntityType typ;
  };

  /**
  * Solution node for vs widget
  */
  class VSSlnNode : public VSExplorerEntity {
  public:
    VSSlnNode(QListView *lv, VSSolution *sln);
  protected:
    QString name;
//     QString uiFileLink;
    VSSolution *sln;
  };

  /**
  * Project node for vs widget
  */
  class VSPrjNode : public VSExplorerEntity {
  public:
    VSPrjNode(VSSlnNode *sln, VSProject *prj);
  protected:
    QString name;
    VSSlnNode *sln; //parent solution
    VSProject *prj;
  };
};
//END // VStudio namespace
#endif /* __VSMANAGER_WIDGET_H__ */
