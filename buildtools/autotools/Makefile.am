# Here resides the automake project part.

INCLUDES = -I$(top_srcdir)/buildtools/lib -I$(top_srcdir)/buildtools/lib/base \
	-I$(top_srcdir)/buildtools/lib/parsers/autotools -I$(top_srcdir)/buildtools/lib/widgets -I$(top_srcdir)/lib/interfaces \
	-I$(top_srcdir)/lib/interfaces/extensions -I$(top_srcdir)/lib/interfaces/external \
	-I$(top_srcdir)/lib/interfaces/extras -I$(top_srcdir)/lib/util -I$(top_srcdir)/lib/widgets $(all_includes) \
	-I$(top_builddir)/buildtools/lib/widgets


kde_module_LTLIBRARIES = libkdevautoproject.la
libkdevautoproject_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkdevautoproject_la_LIBADD = \
	$(top_builddir)/buildtools/lib/base/libkdevbuildbase.la \
	$(top_builddir)/buildtools/lib/parsers/autotools/libkdevautotoolsparser.la $(top_builddir)/buildtools/lib/widgets/libkdevbuildtoolswidgets.la \
	$(top_builddir)/lib/interfaces/extras/libkdevextras.la $(top_builddir)/lib/libkdevelop.la \
	$(top_builddir)/lib/widgets/libkdevwidgets.la

libkdevautoproject_la_SOURCES = addapplicationdlg.cpp addapplicationdlgbase.ui \
	addexistingdirectoriesdlg.cpp addexistingdlgbase.ui addexistingfilesdlg.cpp addfiledlg.cpp \
	addfiledlgbase.ui addicondlg.cpp addicondlgbase.ui addprefixdlg.cpp addservicedlg.cpp \
	addservicedlgbase.ui addsubprojectdlg.cpp addsubprojectdlgbase.ui addtargetdlg.cpp \
	addtargetdlgbase.ui addtranslationdlg.cpp autodetailsview.cpp autolistviewitems.cpp \
	autoprojectpart.cpp autoprojectviewbase.ui autoprojectwidget.cpp autosubprojectview.cpp \
	autotoolsaction.cpp choosetargetdialog.cpp choosetargetdlgbase.ui \
	configureoptionswidget.cpp configureoptionswidgetbase.ui fileselectorwidget.cpp \
	makefilehandler.cpp managecustomcommand.cpp managecustomcommandsbase.ui misc.cpp \
	removefiledlg.cpp removefiledlgbase.ui removetargetdlg.cpp removetargetdlgbase.ui \
	subprojectoptionsdlg.cpp subprojectoptionsdlgbase.ui targetoptionsdlg.cpp \
	targetoptionsdlgbase.ui

METASOURCES = AUTO

servicedir = $(kde_servicesdir)
service_DATA = kdevautoproject.desktop kdevkdeautoproject.desktop

rcdir = $(kde_datadir)/kdevautoproject
rc_DATA = kdevautoproject.rc

noinst_HEADERS = managecustomcommand.h autotoolsaction.h makefilehandler.h
