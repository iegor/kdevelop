INCLUDES = -I$(top_srcdir)/lib/compat -I$(top_srcdir)/lib/interfaces $(all_includes)

METASOURCES = AUTO

lib_LTLIBRARIES = libkdevwidgets.la

kdevwidgetsincludedir = $(includedir)/kdevelop/widgets

libkdevwidgets_la_SOURCES = fancylistviewitem.cpp flagboxes.cpp kcomboview.cpp \
	kdevhtmlpart.cpp kfilednddetailview.cpp kfiledndiconview.cpp kimporticonview.cpp \
	klistviewaction.cpp ksavealldialog.cpp processlinemaker.cpp processwidget.cpp \
	qcomboview.cpp resizablecombo.cpp


kdevwidgetsinclude_HEADERS = fancylistviewitem.h flagboxes.h kcomboview.h \
	kdevhtmlpart.h kfilednddetailview.h kfiledndiconview.h kimporticonview.h \
	klistviewaction.h ksavealldialog.h processlinemaker.h processwidget.h qcomboview.h \
	resizablecombo.h
	

SUBDIRS = propeditor

libkdevwidgets_la_LIBADD = $(top_builddir)/lib/interfaces/libkdevinterfaces.la
libkdevwidgets_la_LDFLAGS = -no-undefined $(all_libraries)
rcdir = $(kde_datadir)/kdevelop

rc_DATA =  kdevhtml_partui.rc

DOXYGEN_REFERENCES = dcop interfaces kdecore kdefx kdeui khtml kmdi kio kjs kparts kutils kdevutil kdevinterfaces
DOXYGEN_PROJECTNAME = KDevelop Widgets Library
DOXYGEN_DOCDIRPREFIX = kdev
include ../../Doxyfile.am
