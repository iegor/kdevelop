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

/* Qt */
#include <qpushbutton.h>
#include <qptrlist.h>
#include <qmap.h>
#include <qtooltip.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qlistview.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qscrollview.h>
#include <qlabel.h>

// #include <codemodel.h>
// #include <fancylistviewitem.h>

/* VStudio */
#include "vs_model.h"
#include "vs_explorer_widget.h" // Base widget class

class QPushButton;
class QGridLayout;

class KSelectAction;
class KToggleAction;
class TargetItem;
class FileItem;
class AutoProjectPart;

//BEGIN //VStudio namespace
namespace VStudio {
  predeclare_vs_typ(ListWidgetItem, lwi);
  predeclare_vs_typ(ListWidget, lw);

  class ListWidgetItem : public QHBox {
    friend class ListWidget;

    Q_OBJECT
    public:
      ListWidgetItem(QWidget *parent, const char *name=0, WFlags fl=0);
      // ListWidgetItem(lw_p parent_list, lwi_p parent, const char *name=0, WFlags f=0);
      virtual ~ListWidgetItem();

      int totalHeight();
      virtual void invalidateHeight();

    protected:
      lwi_p parent;
      lwi_p sibling;
      lwi_p child;

      QHBoxLayout *hbl_main;

      lw_p list;
      int maybeTotalHeight;
      uint lvl;
  };

  class ListWidget : public QScrollView {
    Q_OBJECT
    Q_PROPERTY( int childCount READ childCount )
    Q_PROPERTY( int itemMargin READ itemMargin WRITE setItemMargin )
    Q_PROPERTY( int levelShift READ lvlShift WRITE setLvlShift )

    public:
      ListWidget(QWidget* parent=0, const char* name=0, WFlags f=0);
      virtual ~ListWidget();

      int lvlShift() const;
      virtual void setLvlShift(int shift);

      virtual bool insertItem(lwi_p pItem, lwi_p pParent=0);

    protected:
      // virtual void drawContents(QPainter *p, int cx, int cy, int cw, int ch);

    private:
      pv_lwi items;
      // pv_lwi drawqueue;

      int lvl_shift;
  };

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
      // bool selectItem(ItemDom item);
      uivss_p addSolutionNode(vss_p sln);
      uivsp_p addProjectNode(uivse_p sln, vsp_p prj);
      uivsf_p addFilterNode(uivse_p parent, vsf_p filter);
      uivsfl_p addFileNode(uivse_p parent, vsfl_p file);

      uivse_p getByUID(const QUuid &uid) const;

      void emitAddedFiles(const QStringList& list);

    protected:
      void contentsContextMenuEvent(QContextMenuEvent*);
      void maybeTip(QPoint const &);

    private slots:
      void slotSelectItem(QListViewItem *item);
      // void slotContextMenu(KListView *lv, QListViewItem *item, const QPoint &p);
      void slotEntityRenamed(QListViewItem *item, const QString &str, int col);
      void slotProjectOpened();
      void slotProjectClosed();
      void slotSetEntityRltPath(); //TODO: Remove that later, that will be part of test only
      void slotConfigureEntity();
      void slotRenameEntity();
      void slotActivateEntity();
      void slotHighlightContextMenuItem(int id);
      void slotSaveEntity();
      void slotSaveEntityAs();

    public slots:
      void slotRefreshUI();

    private:
      VSPart *m_part;
      KActionCollection *actions;
      KAction *actSetEntityRltPath; //TODO: Remove that later, that will be part of test only
      KAction *actCfgEntity;
      KAction *actRenameEntity;
      QString m_projectDirectory;
      int m_projectDirectoryLength;
      // TextPaintStyleStore m_paintStyles;
      pv_uivse uients; // UI entities
      ListWidget *lw_explorer;
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
  class VSExplorerEntity : public ListWidgetItem {
  public:
    VSExplorerEntity(e_VSEntityType type, QWidget *parent=0, const char *name=0, WFlags fl=0);
    // VSExplorerEntity(e_VSEntityType type, lw_p parent_list=0, lwi_p parent=0, const char *name=0, WFlags fl=0);
    virtual ~VSExplorerEntity();

    virtual vse_p getModel() const = 0;
    virtual const QUuid& getUID() const = 0;

    // void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int alignment);
    e_VSEntityType getType() { return typ; }

  // private slots:
    virtual void slotRefreshText() = 0;

  protected:
    e_VSEntityType typ;
  };

  /**
  * Solution node for vs widget
  */
  class VSSlnNode : public VSExplorerEntity {
  public:
    VSSlnNode(vss_p sln, QWidget *parent=0, const char *name=0, WFlags fl=0);
    virtual ~VSSlnNode();

    //virtual void paintCell(QPainter *p, const QColorGroup &cg, int column, int  width, int alignment);
    //virtual int width(const QFontMetrics &fnt_mtrx, const QListView *listview, int column) const;

  // VSExplorerEntity interface
    virtual vse_p getModel() const;
    virtual const QUuid& getUID() const;

  // private slots:
    virtual void slotRefreshText();

  public:
    // VSSlnNode interface
    void setState(const QString &state);

  private:
    vss_p sln;
    pv_uivsp projects;
    pv_uivsf filters;
    // QString uiFileLink;

    QLabel *lbl_icon;
    QLabel *lbl_name;
    QPushButton *btn_config;
  };

  /**
  * Project node for vs widget
  */
  class VSPrjNode : public VSExplorerEntity {
  public:
    VSPrjNode(vsp_p prj, QWidget *parent=0, const char *name=0, WFlags fl=0);
    virtual ~VSPrjNode();

  // VSExplorerEntity interface
    virtual vse_p getModel() const;
    virtual const QUuid& getUID() const;

  // private slots:
    virtual void slotRefreshText();

  public:
  // VSPrjNode interface
    uivss_p getSolution() const { return sln; }

  private:
    vsp_p prj;
    uivss_p sln; // Parent solution
    pv_uivsfl files;
    pv_uivsf filters;

    QLabel *lbl_icon;
    QLabel *lbl_name;
    QPushButton *btn_config;
  };

  /**
  * Filter node for vs widget
  */
  class VSFltNode : public VSExplorerEntity {
  public:
    VSFltNode(vsf_p filter, QWidget *parent=0, const char *name=0, WFlags fl=0);
    virtual ~VSFltNode();

  // VSExplorerEntity interface
    virtual vse_p getModel() const;
    virtual const QUuid& getUID() const;

  // private slots:
    virtual void slotRefreshText();

  public:
  // VSFltNode interface
    uivse_p getUIContainer() const;

  private:
    vsf_p flt;  // VSFilter, model representation
    uivse_p container; // Parent container, solution or project UI

    QLabel *lbl_icon;
    QLabel *lbl_name;
    pv_uivse contents;
  };

  /**
  * File node for vs widget
  */
  class VSFilNode : public VSExplorerEntity {
  public:
    VSFilNode(vsfl_p file, QWidget *parent=0, const char *name=0, WFlags fl=0);
    virtual ~VSFilNode();

  // VSExplorerEntity interface
    virtual vse_p getModel() const;
    virtual const QUuid& getUID() const;

  // private slots:
    virtual void slotRefreshText();

  public:
  // VSFilNode interface
    uivse_p getUIContainer() const;
    void setState(const QString &state);

  private:
    vsfl_p file;
    uivse_p container; // Parent filter|project

    QLabel *lbl_icon;
    QLabel *lbl_name;
    QPushButton *btn_config;
  };
};
//END // VStudio namespace
#endif /* __VSMANAGER_WIDGET_H__ */
