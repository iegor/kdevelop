INCLUDES = -I$(top_srcdir)/lib/interfaces -I$(top_srcdir)/lib/util \
	$(all_includes)
METASOURCES = AUTO
partincludedirdir = $(includedir)/kdevelop/parts/documentation
lib_LTLIBRARIES = libdocumentation_interfaces.la
libdocumentation_interfaces_la_LDFLAGS = $(all_libraries)
libdocumentation_interfaces_la_LIBADD = $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_KIO)
libdocumentation_interfaces_la_SOURCES = kdevdocumentationplugin.cpp
partincludedir_HEADERS = kdevdocumentationplugin.h
servicetypedir = $(kde_servicetypesdir)
servicetype_DATA = kdevelopdocumentationplugins.desktop

DOXYGEN_REFERENCES = dcop interfaces kdecore kdefx kdeui khtml kmdi kio kjs kparts kutils kdevutil kdevinterfaces
DOXYGEN_PROJECTNAME = KDevelop Documentation Part Interfaces Library
DOXYGEN_DOCDIRPREFIX = kdevdoc
include ../../../Doxyfile.am
