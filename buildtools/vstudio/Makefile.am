# Here resides the class view part.

INCLUDES = -I$(top_srcdir)/buildtools/lib -I$(top_srcdir)/buildtools/lib/base \
	-I$(top_srcdir)/buildtools/lib/widgets -I$(top_srcdir)/lib/interfaces \
	-I$(top_srcdir)/lib/interfaces/extensions -I$(top_srcdir)/lib/interfaces/external \
	-I$(top_srcdir)/lib/interfaces/extras -I$(top_srcdir)/lib/util -I$(top_srcdir)/lib/widgets \
	-I$(top_builddir)/buildtools/lib/widgets $(all_includes)

kde_module_LTLIBRARIES = libvstudio.la
libvstudio_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libvstudio_la_LIBADD = \
	$(top_builddir)/lib/interfaces/extensions/libkdevextensions.la $(top_builddir)/lib/libkdevelop.la \
	$(top_builddir)/lib/widgets/libkdevwidgets.la $(top_builddir)/buildtools/lib/widgets/libkdevbuildtoolswidgets.la \
	$(top_builddir)/buildtools/lib/base/libkdevbuildbase.la $(LIB_KHTML)

libvstudio_la_SOURCES = vstudiopart.cpp vsmanagerwidget.cpp \
	digraphview.cpp hierarchydlg.cpp vs_manager_widget.ui
# navigator.cpp viewcombos.cpp

METASOURCES = AUTO

servicedir = $(kde_servicesdir)
service_DATA = vstudio.kdesktop

rcdir = $(kde_datadir)/kdevvstudio
rc_DATA = kdevvstudio.rc

QTDIR = /home/bernd/kdesrc/qt-copy

#foo: digraphview.cpp digraphview.h
#	$(QTDIR)/bin/moc digraphview.h > digraphview.moc ; \
#	g++ digraphview.cpp -I$(QTDIR)/include -L$(QTDIR)/lib -L/usr/X11R6/lib -lqt -lX11 -lXext -o foo

noinst_HEADERS = viewcombos.h navigator.h
