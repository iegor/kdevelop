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
#ifndef AUTOLISTVIEWITEMS_H
#define AUTOLISTVIEWITEMS_H

#include <qptrlist.h>

#include <qlistview.h>

class TargetItem;
class FileItem;
class AutoProjectPart;

/**
* Base class for all items
*/
class ProjectItem : public QListViewItem {
public:
  enum Type { Subproject, Target, File };

  ProjectItem(Type type, QListView *parent, const QString &text);
  ProjectItem(Type type, ProjectItem *parent, const QString &text);

  void paintCell(QPainter *p, const QColorGroup &cg,
                 int column, int width, int alignment);
  void setBold(bool b) {
    bld = b;
  }
  bool isBold() const {
    return bld;
  }
  Type type() {
    return typ;
  }

private:
  Type typ;
  bool bld;

};

// Not sure if this complexity is really necessary...
class FileItem : public ProjectItem {
public:
  FileItem(QListView *lv, const QString &text, bool set_is_subst = false);
  void changeSubstitution();
  void changeMakefileEntry(const QString&);

  QString name;
  QString uiFileLink;
  const bool is_subst;
  QString m_sRelativeToProjName;
};

#endif
