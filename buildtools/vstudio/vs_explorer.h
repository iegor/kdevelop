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

#include <qpushbutton.h>
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

class QPushButton;
class QGridLayout;

class KSelectAction;
class KToggleAction;
class TargetItem;
class FileItem;
class AutoProjectPart;

//BEGIN //VStudio namespace
namespace VStudio {
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
  * Dialog for setting paths
  */
  class SetPathWidget : public QWidget {
    Q_OBJECT
  public:
    SetPathWidget(QWidget *parent = 0, const char *name = 0, WFlags fl = 0);
    virtual ~SetPathWidget();

    QPushButton* btn_change;
  public slots:
    virtual void widgetDestroyed(QObject*);
  protected:
    QGridLayout* layout;
  protected slots:
    virtual void languageChange();
  };

  /**
  * Base class for all items
  */
  class VSExplorerEntity : public QListViewItem {
  public:
    VSExplorerEntity(e_VSEntityType type, QListView *parent, const QString &text);
    VSExplorerEntity(e_VSEntityType type, VSExplorerEntity *parent, const QString &text);
    virtual ~VSExplorerEntity();
    void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment);
    e_VSEntityType type() { return typ; }
    virtual const VSEntity* getModelRepresentation() const = 0;
    virtual VSEntity* getModelRepresentation() = 0;
  protected:
    e_VSEntityType typ;
    QString name;
  };

  /**
  * Solution node for vs widget
  */
  class VSSlnNode : public VSExplorerEntity {
  public:
    VSSlnNode(QListView *parent, VSSolution *sln);
    virtual ~VSSlnNode();

    virtual const VSSolution* getModelRepresentation() const { return sln; }
    virtual VSSolution* getModelRepresentation() { return sln; }
  private:
//     QString uiFileLink;
    VSSolution *sln;
#ifdef USE_BOOST
    boost::container::vector<uivsp_p> projects;
    boost::container::vector<uivsf_p> filters;
#else
#endif
  };

  /**
  * Project node for vs widget
  */
  class VSPrjNode : public VSExplorerEntity {
  public:
    VSPrjNode(VSSlnNode *parent, VSProject *prj);
    virtual ~VSPrjNode();

    virtual const VSProject* getModelRepresentation() const { return prj; }
    virtual VSProject* getModelRepresentation() { return prj; }
  private:
#ifdef USE_BOOST
    boost::container::vector<uivsfl_p> files;
    boost::container::vector<uivsf_p> filters;
#else
#endif
    uivss_p sln; //parent solution
    vsp_p prj;
  };

  /**
  * Filter node for vs widget
  */
  class VSFltNode : public VSExplorerEntity {
  public:
    VSFltNode(uivse_p parent);
    virtual ~VSFltNode();
  private:
#ifdef USE_BOOST
    boost::container::vector<uivse_p> contents;
#else
#endif
    uivse_p parent; //parent solution or project
    vsf_p filter;
  };

  /**
  * File node for vs widget
  */
  class VSFilNode : public VSExplorerEntity {
  public:
    VSFilNode(uivse_p parent, vsfl_p file);
    virtual ~VSFilNode();
  private:
    uivsp_p parent;
    vsfl_p file;
  };
};
//END // VStudio namespace
#endif /* __VSMANAGER_WIDGET_H__ */
