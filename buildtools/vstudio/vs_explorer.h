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
#include <qcheckbox.h>
#include <qptrlist.h>
#include <qmap.h>
#include <qtooltip.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qlistview.h>
#include <qframe.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qscrollview.h>
#include <qlabel.h>

/* KDE */
//#include <kpushbutton.h>

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

  class ListWidgetItem : public QFrame {
    friend class ListWidget;

    Q_OBJECT
    public:
      enum e_flags {
        IS_ROOT     = 0x00000001,
        IS_EXPANDED = 0x00000002,
        IS_LMB_DOWN = 0x00000010,
        IS_PRESSED  = 0x00000020,
      };
    public:
      ListWidgetItem(QWidget *parent, const char *name=0, WFlags fl=0);
      virtual ~ListWidgetItem();

    // QWidget's methods:
      virtual void mousePressEvent(QMouseEvent *event);
      virtual void mouseReleaseEvent(QMouseEvent *event);
      virtual void mouseDoubleClickEvent(QMouseEvent *event);
      virtual void enterEvent(QEvent *event);
      virtual void leaveEvent(QEvent *event);

      int totalHeight();
      virtual void invalidateHeight();

      lwi_p parent() const;
      lwi_p sibling() const;
      lwi_p child() const;
      void setParent(lwi_cp parent);
      void setChild(lwi_cp child);
      void setSibling(lwi_cp sibling);

      int level() const;
      void setLevel(int lvl);

      bool isRoot() const;
      bool isExpanded() const;
      bool canExpand() const;

      virtual void expand();
      virtual void collapse();

      virtual void addChild(lwi_p child);
      virtual void sort();
      virtual void updateArrangement();

      virtual bool hit_item(const QPoint &point) const;

    signals:
      void released();
      void clicked();

    protected:
      lwi_p pnt; // Parent node
      lwi_p sbl; // Sibling node
      lwi_p chd; // Child node

      lw_p list;
      int maybeTotalHeight;
      int lvl;
      uint flags;
      uint numChildren;
  };

  class ListWidget : public QScrollView {
    Q_OBJECT
    Q_PROPERTY( int childCount READ childCount )
    Q_PROPERTY( int itemMargin READ itemMargin WRITE setItemMargin )
    Q_PROPERTY( int levelShift READ lvlShift WRITE setLvlShift )

    public:
      /** \class ListWidget::LayoutMapper
       * \brief Maps list widget items thus creates layout for widget
       */
      class LayoutMapper {
        public:
          LayoutMapper(lw_p host);
          virtual ~LayoutMapper();

          virtual void map();

          const lw_p host;
      };

      class Tree_LM : public LayoutMapper {
        public:
          Tree_LM(lw_p host);
          virtual ~Tree_LM();

          virtual void map();

          int lvl_shift;
      };

    private:
      class RootItem : public ListWidgetItem {
        public:
          RootItem(QWidget *parent=0, const char *name=0, WFlags fl=0);
          virtual ~RootItem();
      };

    public:
      ListWidget(QWidget *parent=0, const char* name=0, WFlags f=0);
      virtual ~ListWidget();

    // QWidget's methods:
      virtual void paintEvent(QPaintEvent *event);

    // QScrollView's methods:
      virtual void drawContents(QPainter *painter, int cx, int cy, int cw, int ch);

      // int lvlShift() const;
      // virtual void setLvlShift(int shift);
      virtual bool insertItem(lwi_p pItem, lwi_p pParent=0);
      void setFocused(lwi_p item);
      void select(lwi_p item);

    public slots:
      void slotRemapItems();

    signals:
      void selectionChanged(lwi_p);

    protected:
      lwi_p focusItem;
      lwi_p selectedItem;
      lwi_p prevFocusItem;
      lwi_p prevSelectedItem;

    private:
      RootItem *root;
      pv_lwi items;
      pv_lwi dqueue;
      // int lvl_shift;
      LayoutMapper *imapper;
  };

  /** \class VSExplorerEntity
   * \brief Base class for all VS UI entities
   */
  class VSExplorerEntity : public ListWidgetItem {
    Q_OBJECT
    public:
      enum e_flags {
        IS_OPEN = 0x00000001,
        IS_HOVERED_ON = 0x00000002,
        CONTROLS_VISIBLE = 0x00000004,
      };
    public:
      VSExplorerEntity(QWidget *parent=0, const char *name=0, WFlags fl=0);
      virtual ~VSExplorerEntity();

    // QTWidget's methods:
      virtual void enterEvent(QEvent *event);
      virtual void leaveEvent(QEvent *event);

    // ListWidgetItem's methods:
      virtual void invalidateHeight();
      virtual void expand();
      virtual void collapse();
      virtual void addChild(lwi_p child);
      virtual bool hit_item(const QPoint &point) const;

    // VStudio UI entity methods:
      virtual vse_p getModel() const = 0;
      virtual const QUuid& getUID() const = 0;
      virtual e_VSEntityType getType() const = 0;

      virtual void showControls();
      virtual void hideControls();
      bool controlsVisible() const;

      bool isSelected() const;

  // private slots:
      virtual void slotRefreshText() = 0;

    protected slots:
      void slotToggleExpand(bool toggle_on);

    protected:
      QHBoxLayout *hbl_main;
      QVBox *vb_ctrl; // UI controls vbox, expand, select, etc.
      QSpacerItem *spc_vbctrl; // UI controls vbox spacer, to keek controls up
      QVBoxLayout *vbl_elem;
      QHBox *hb_top;
      QCheckBox *chb_select;
      QLabel *lbl_icon;
      QLabel *lbl_name;
      uint enflg;
  };

  /** \class VSExplorerListWidget
   * \brief Lists solutions, projects and files in form of widgets
   */
  class VSExplorerListWidget : public ListWidget {
    Q_OBJECT
    public:
      VSExplorerListWidget(QWidget *parent=0, const char *name=0, WFlags fl=0);
      virtual ~VSExplorerListWidget();

      // QObject methods:
      virtual bool eventFilter(QObject *object, QEvent *event);

      // QWidget methods:
      // virtual void hide();
      // virtual void show();
      // virtual void setEnabled(bool enable);
      // virtual void setDisabled(bool disable);
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
      void slotSelectItem(lwi_p item);
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
      VSExplorerListWidget *explorer;
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
  * Solution node for vs widget
  */
  class VSSlnNode : public VSExplorerEntity {
    Q_OBJECT
    public:
      VSSlnNode(vss_p sln, QWidget *parent=0, const char *name=0, WFlags fl=0);
      virtual ~VSSlnNode();

    // QTWidget's methods:
      virtual void enterEvent(QEvent *event);
      virtual void leaveEvent(QEvent *event);

    // ListWidgetItem's methods:
      virtual void expand();
      virtual void collapse();

    // VSExplorerEntity interface
      virtual vse_p getModel() const;
      virtual const QUuid& getUID() const;
      virtual e_VSEntityType getType() const;
      virtual void showControls();
      virtual void hideControls();

    public:
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

      QHBox *hb_tools;
      QPushButton *btn_cfg;
      QPushButton *btn_bld;
      QPushButton *btn_clr;
      QPushButton *btn_expand;
  };

  /**
  * Project node for vs widget
  */
  class VSPrjNode : public VSExplorerEntity {
    Q_OBJECT
  public:
    VSPrjNode(vsp_p prj, QWidget *parent=0, const char *name=0, WFlags fl=0);
    virtual ~VSPrjNode();

  // VSExplorerEntity interface
    virtual vse_p getModel() const;
    virtual const QUuid& getUID() const;
    virtual e_VSEntityType getType() const;

  public:
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

    QPushButton *btn_cfg;
    QPushButton *btn_bld;
    QPushButton *btn_clr;
    QPushButton *btn_expand;
  };

  /**
  * Filter node for vs widget
  */
  class VSFltNode : public VSExplorerEntity {
    Q_OBJECT
  public:
    VSFltNode(vsf_p filter, QWidget *parent=0, const char *name=0, WFlags fl=0);
    virtual ~VSFltNode();

  // VSExplorerEntity interface
    virtual vse_p getModel() const;
    virtual const QUuid& getUID() const;
    virtual e_VSEntityType getType() const;

  // private slots:
    virtual void slotRefreshText();

  public:
  // VSFltNode interface
    uivse_p getUIContainer() const;

  private:
    vsf_p flt;  // VSFilter, model representation
    uivse_p container; // Parent container, solution or project UI

    QPushButton *btn_bld;
    QPushButton *btn_clr;
    QPushButton *btn_expand;
  };

  /**
  * File node for vs widget
  */
  class VSFilNode : public VSExplorerEntity {
    Q_OBJECT
  public:
    VSFilNode(vsfl_p file, QWidget *parent=0, const char *name=0, WFlags fl=0);
    virtual ~VSFilNode();

  // VSExplorerEntity interface
    virtual vse_p getModel() const;
    virtual const QUuid& getUID() const;
    virtual e_VSEntityType getType() const;

  // private slots:
    virtual void slotRefreshText();

  public:
  // VSFilNode interface
    uivse_p getUIContainer() const;
    void setState(const QString &state);

  private:
    vsfl_p file;
    uivse_p container; // Parent filter|project

    QPushButton *btn_cfg;
    QPushButton *btn_bld; //NOTE: meaning is "compile file"
    QPushButton *btn_clr;
  };
};
//END // VStudio namespace
#endif /* __VSMANAGER_WIDGET_H__ */
