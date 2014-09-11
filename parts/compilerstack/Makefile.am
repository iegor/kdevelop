INCLUDES = -I$(top_srcdir)/lib/interfaces \
	-I$(top_srcdir)/lib/interfaces/extensions \
	-I$(top_srcdir)/lib/util \
	$(all_includes)

METASOURCES = AUTO

kde_module_LTLIBRARIES = libcompilerstack.la
libcompilerstack_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libcompilerstack_la_SOURCES = part.cpp settings_dlg.cpp
libcompilerstack_la_LIBADD = \
	$(top_builddir)/lib/interfaces/extensions/libkdevextensions.la \
	$(top_builddir)/lib/libkdevelop.la \
	$(top_builddir)/lib/widgets/libkdevwidgets.la \
	$(LIB_KHTML)

noinst_HEADERS = settings_dlg.h part.h

servicedir = $(kde_servicesdir)
service_DATA = compilerstack.desktop

rc_DATA = compilerstack.rc
rcdir = $(kde_datadir)/compilerstack
