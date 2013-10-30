# Here resides the class view part.

INCLUDES = -I$(top_srcdir)/buildtools/lib -I$(top_srcdir)/buildtools/lib/base \
	-I$(top_srcdir)/buildtools/lib/widgets -I$(top_srcdir)/lib/interfaces \
	-I$(top_srcdir)/lib/interfaces/extensions -I$(top_srcdir)/lib/interfaces/external \
	-I$(top_srcdir)/lib/interfaces/extras -I$(top_srcdir)/lib/util -I$(top_srcdir)/lib/widgets \
	-I$(top_builddir)/buildtools/lib/widgets $(all_includes)

kde_module_LTLIBRARIES = libkdevvstudioproject.la
libkdevvstudioproject_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkdevvstudioproject_la_LIBADD = \
	$(top_builddir)/buildtools/lib/base/libkdevbuildbase.la $(top_builddir)/buildtools/lib/widgets/libkdevbuildtoolswidgets.la \
	$(top_builddir)/lib/interfaces/extras/libkdevextras.la $(top_builddir)/lib/libkdevelop.la $(LIB_KHTML)

libkdevvstudioproject_la_SOURCES = vs_manager_widget.ui vs_solution.cpp \
	vsmanagerwidget.cpp vstudiopart.cpp
# navigator.cpp viewcombos.cpp

METASOURCES = AUTO

servicedir = $(kde_servicesdir)
service_DATA = kdevvstudioproject.desktop

rcdir = $(kde_datadir)/kdevvstudioproject
rc_DATA = kdevvstudioproject.rc

noinst_HEADERS = vs_solution.h
