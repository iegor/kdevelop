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

#include <qptrlist.h>
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
  class VSExplorerEntity;
  class VSSlnNode;
  class VSPrjNode;
  //TODO: A template would look nicer
  // struct FindOp { const FunctionDom& m_dom; };
  // struct FindOp2 { const FunctionDefinitionDom& m_dom; };

  class VSExplorer : public VsExplorerWidget {
    Q_OBJECT
  public:
    typedef QPtrList<QListViewItem*>::const_iterator pitems_ci;
    typedef QPtrList<QListViewItem*>::iterator pitems_i;
    typedef QPtrList<QListViewItem>::const_iterator items_ci;
    typedef QPtrList<QListViewItem>::iterator items_i;
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
    void slotContextMenu(KListView *lv, QListViewItem *item, const QPoint &p);
    void slotEntityRenamed(QListViewItem *item, const QString &str, int col);
    void slotProjectOpened();
    void slotProjectClosed();
    void slotSetEntityRltPath(); //TODO: Remove that later, that will be part of test only
    void slotConfigureEntity();
    void slotRenameEntity();

  private:
    KAction *actSetEntityRltPath; //TODO: Remove that later, that will be part of test only
    KAction *actCfgEntity;
    KAction *actRenameEntity;
    VSPart *m_part;
    KActionCollection *actions;
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
    virtual const VSEntity* getModelRepresentation() const = 0;
    virtual VSEntity* getModelRepresentation() = 0;
  protected:
    e_VSEntityType typ;
  };

  /**
  * Solution node for vs widget
  */
  class VSSlnNode : public VSExplorerEntity {
  public:
    VSSlnNode(QListView *lv, VSSolution *sln);

    virtual const VSSolution* getModelRepresentation() const { return sln; }
    virtual VSSolution* getModelRepresentation() { return sln; }
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

    virtual const VSProject* getModelRepresentation() const { return prj; }
    virtual VSProject* getModelRepresentation() { return prj; }
  protected:
    QString name;
    VSSlnNode *sln; //parent solution
    VSProject *prj;
  };
};
//END // VStudio namespace
#endif /* __VSMANAGER_WIDGET_H__ */
