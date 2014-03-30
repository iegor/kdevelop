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

#include <klistview.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qtooltip.h>
#include <codemodel.h>
#include <fancylistviewitem.h>
// #include <navigator.h>

#include "vs_manager_widget.h"

class KDevProject;
class VStudioPart;
// class ClassViewItem;
// class FolderBrowserItem;
// class NamespaceDomBrowserItem;
// class ClassDomBrowserItem;
// class TypeAliasDomBrowserItem;
// class FunctionDomBrowserItem;
// class VariableDomBrowserItem;
// class VSManagerItem;
class KSelectAction;
class KToggleAction;

class VSManagerWidget : public KListView, public QToolTip {
  Q_OBJECT
public:
//   enum ViewMode
//   {
//     KDevelop3ViewMode = 0,
//     KDevelop2ViewMode,
//     JavaLikeViewMode
//   };

public:
  VSManagerWidget(VStudioPart *part);
  virtual ~VSManagerWidget();

//   int viewMode() const;
//   void setViewMode( int mode );

  void clear();

  bool selectItem(ItemDom item);
  bool doFollowEditor();

  inline TextPaintStyleStore& paintStyles() {
    return m_paintStyles;
  }

// private slots:
//   void slotNewClass();
//   void slotAddMethod();
//   void slotAddAttribute();
//   void slotOpenDeclaration();
//   void slotOpenImplementation();
//   void slotCreateAccessMethods();
  void slotFollowEditor();

protected:
  void contentsContextMenuEvent(QContextMenuEvent*);
  void maybeTip(QPoint const &);

private slots:
  void slotProjectOpened();
  void slotProjectClosed();
  void refresh();
  void insertFile(const QString& fileName);
  void removeFile(const QString& fileName);
  void slotExecuted(QListViewItem* item);

private:
  VStudioPart* m_part;
  QStringList removedText;
  QString m_projectDirectory;
  int m_projectDirectoryLength;
//   FolderBrowserItem* m_projectItem;
  KSelectAction* m_actionViewMode;
//   KAction* m_actionNewClass;
//   KAction* m_actionAddMethod;
//   KAction* m_actionAddAttribute;
//   KAction* m_actionOpenDeclaration;
//   KAction* m_actionOpenImplementation;
//   KAction* m_actionCreateAccessMethods;
  KToggleAction * m_actionFollowEditor;
  bool m_doFollowEditor;

//   friend class ClassViewItem;
//   friend class FolderBrowserItem;
//   friend class NamespaceDomBrowserItem;
//   friend class ClassDomBrowserItem;
//   friend class TypeAliasDomBrowserItem;
//   friend class FunctionDomBrowserItem;
//   friend class VariableDomBrowserItem;

  TextPaintStyleStore m_paintStyles;
};

typedef VSManagerWidget vsw;

class VSManagerItem: public FancyListViewItem {
private:
public:
  VSManagerItem(QListView* parent, const QString& text=QString::null)
    : FancyListViewItem(static_cast<vsw*>(parent)->paintStyles(), parent, text) {
  }

  VSManagerItem(QListViewItem* parent, const QString& text=QString::null)
    : FancyListViewItem(static_cast<vsw*>(parent->listView())->paintStyles(), parent, text) {
  }

//   virtual const CodeModelItem* model() const                       {return 0;}

//   virtual bool isFolder() const                                {return false;}
//   virtual bool isFile() const                                  {return false;}
//   virtual bool isNamespace() const                             {return false;}
//   virtual bool isClass() const                                 {return false;}
//   virtual bool isFunction() const                              {return false;}
//   virtual bool isVariable() const                              {return false;}
//   virtual bool isTypeAlias() const                             {return false;}

//   virtual bool hasDeclaration() const                          {return false;}
//   virtual bool hasImplementation() const                       {return false;}

//   virtual void openDeclaration()                                              {}
//   virtual void openImplementation()                                           {}

//   void select();

//   virtual QString comment();

  vsw* listView() {
    return static_cast<vsw*>(QListViewItem::listView());
  }

  const vsw* listView() const {
    return static_cast<vsw*>(QListViewItem::listView());
  }
};

// class SolutionItem : public FancyListViewItem {
// public:
//   SolutionItem(QListView* parent, const QString& text=QString::null)
//       :FancyListViewItem(static_cast<PVSEWidget>(parent)->m_paintStyles, parent, text) {
//   }
//   SolutionItem(QListViewItem* parent, const QString& text=QString::null)
//       :FancyListViewItem(static_cast<PVSEWidget>(parent->listView())->m_paintStyles, parent, text) {
//   }
// };

// class FolderBrowserItem: public ClassViewItem
// {
// public:
//   FolderBrowserItem( VSSlnExplorerWidget* widget, QListView* parent, const QString& name=QString::null )
//       : ClassViewItem( parent, name ), m_widget(widget) {}
//   FolderBrowserItem( VSSlnExplorerWidget* widget, QListViewItem* parent, const QString& name=QString::null )
//       : ClassViewItem( parent, name ), m_widget(widget) {}
//
//   virtual bool isFolder() const { return true; }
//
//   void setup();
//   QString key( int, bool ) const;
//
//   void processFile( FileDom file, QStringList& path, bool remove=false );
//   void processNamespace( NamespaceDom ns, bool remove=false );
//   void processClass( ClassDom klass, bool remove=false );
//   void processTypeAlias( TypeAliasDom typeAlias, bool remove=false );
//   void processFunction( FunctionDom fun, bool remove=false );
//   void processVariable( VariableDom var, bool remove=false );
//   bool selectItem(ItemDom item);
//
// private:
//   QMap<QString, FolderBrowserItem*> m_folders;
//   QMap<QString, NamespaceDomBrowserItem*> m_namespaces;
//   QMap<ClassDom, ClassDomBrowserItem*> m_classes;
//   QMap<TypeAliasDom, TypeAliasDomBrowserItem*> m_typeAliases;
//   QMap<FunctionDom, FunctionDomBrowserItem*> m_functions;
//   QMap<VariableDom, VariableDomBrowserItem*> m_variables;
//
//   VSSlnExplorerWidget* m_widget;
// };

// class NamespaceDomBrowserItem: public ClassViewItem
// {
// public:
//   NamespaceDomBrowserItem( QListView* parent, NamespaceDom dom )
//       : ClassViewItem( parent, dom->name() ), m_dom( dom ) {}
//   NamespaceDomBrowserItem( QListViewItem* parent, NamespaceDom dom )
//       : ClassViewItem( parent, dom->name() ), m_dom( dom ) {}
//
//   const CodeModelItem* model() const { return m_dom; }
//
//   virtual bool isNamespace() const { return true; }
//
//   void setup();
//   QString key( int, bool ) const;
//
//   void processNamespace( NamespaceDom ns, bool remove=false );
//   void processClass( ClassDom klass, bool remove=false );
//   void processTypeAlias( TypeAliasDom typeAlias, bool remove=false );
//   void processFunction( FunctionDom fun, bool remove=false );
//   void processVariable( VariableDom var, bool remove=false );
//   bool selectItem( ItemDom item );
//   virtual QString comment();
//
//   NamespaceDom dom() { return m_dom; }
//
// private:
//   NamespaceDom m_dom;
//   QMap<QString, NamespaceDomBrowserItem*> m_namespaces;
//   QMap<ClassDom, ClassDomBrowserItem*> m_classes;
//   QMap<TypeAliasDom, TypeAliasDomBrowserItem*> m_typeAliases;
//   QMap<FunctionDom, FunctionDomBrowserItem*> m_functions;
//   QMap<VariableDom, VariableDomBrowserItem*> m_variables;
// };

// class ClassDomBrowserItem: public ClassViewItem
// {
// public:
//   ClassDomBrowserItem( QListView* parent, ClassDom dom )
//       : ClassViewItem( parent, dom->name() ), m_dom( dom ) {}
//   ClassDomBrowserItem( QListViewItem* parent, ClassDom dom )
//       : ClassViewItem( parent, dom->name() ), m_dom( dom ) {}
//
//   const CodeModelItem* model() const { return m_dom; }
//   virtual bool isClass() const { return true; }
//
//   virtual bool hasDeclaration() const { return true; }
//   virtual void openDeclaration();
//
//   void setup();
//   QString key( int, bool ) const;
//
//   void processClass( ClassDom klass, bool remove=false );
//   void processTypeAlias( TypeAliasDom typeAlias, bool remove=false );
//   void processFunction( FunctionDom fun, bool remove=false );
//   void processVariable( VariableDom var, bool remove=false );
//
//   virtual QString comment() {
//     if ( !m_dom ) return "";
//     return m_dom->comment();
//   }
//
//   bool selectItem(ItemDom item);
//   ClassDom dom() { return m_dom; }
//
// private:
//   ClassDom m_dom;
//   QMap<ClassDom, ClassDomBrowserItem*> m_classes;
//   QMap<TypeAliasDom, TypeAliasDomBrowserItem*> m_typeAliases;
//   QMap<FunctionDom, FunctionDomBrowserItem*> m_functions;
//   QMap<VariableDom, VariableDomBrowserItem*> m_variables;
// };

// class TypeAliasDomBrowserItem: public ClassViewItem
// {
// public:
//   TypeAliasDomBrowserItem( QListView* parent, TypeAliasDom dom )
//       : ClassViewItem( parent, dom->name() ), m_dom( dom ) {}
//   TypeAliasDomBrowserItem( QListViewItem* parent, TypeAliasDom dom )
//       : ClassViewItem( parent, dom->name() ), m_dom( dom ) {}
//
//   const CodeModelItem* model() const { return m_dom; }
//   virtual bool isTypeAlias() const { return true; }
//
//   virtual bool hasDeclaration() const { return true; }
//   virtual void openDeclaration();
//
//   virtual QString comment() {
//     if ( !m_dom ) return "";
//     return m_dom->comment();
//   }
//
//   void setup();
//   QString key( int, bool ) const;
//
//   TypeAliasDom dom() { return m_dom; }
//
// private:
//   TypeAliasDom m_dom;
// };

// class FunctionDomBrowserItem: public ClassViewItem
// {
// public:
//   FunctionDomBrowserItem( QListView* parent, FunctionDom dom )
//       : ClassViewItem( parent, dom->name() ), m_dom( dom ) {}
//   FunctionDomBrowserItem( QListViewItem* parent, FunctionDom dom )
//       : ClassViewItem( parent, dom->name() ), m_dom( dom ) {}
//
//   const CodeModelItem* model() const { return m_dom; }
//   virtual bool isFunction() const { return true; }
//
//   virtual bool hasDeclaration() const { return true; }
//   virtual bool hasImplementation() const;
//
//   virtual void openDeclaration();
//   virtual void openImplementation();
//
//   virtual QString comment() {
//     if ( !m_dom ) return "";
//     return m_dom->comment();
//   }
//
//   void setup();
//   QString key( int, bool ) const;
//
//   FunctionDom dom() { return m_dom; }
//
// private:
//   FunctionDom m_dom;
// };

// class VariableDomBrowserItem: public ClassViewItem
// {
// public:
//   VariableDomBrowserItem( QListView* parent, VariableDom dom )
//       : ClassViewItem( parent, dom->name() ), m_dom( dom ) {}
//   VariableDomBrowserItem( QListViewItem* parent, VariableDom dom )
//       : ClassViewItem( parent, dom->name() ), m_dom( dom ) {}
//
//   const CodeModelItem* model() const { return m_dom; }
//   virtual bool isVariable() const { return true; }
//
//   virtual bool hasDeclaration() const { return true; }
//   virtual bool hasImplementation() const { return false; }
//
//   virtual QString comment() {
//     if ( !m_dom ) return "";
//     return m_dom->comment();
//   }
//
//   virtual void openDeclaration();
//   virtual void openImplementation();
//
//   void setup();
//   QString key( int, bool ) const;
//
//   VariableDom dom() { return m_dom; }
//
// private:
//   VariableDom m_dom;
// };

struct FindOp2 { ///a template would look nicer
  const FunctionDefinitionDom& m_dom;
};

struct FindOp {
  const FunctionDom& m_dom;
};

#endif /* __VSMANAGER_WIDGET_H__ */