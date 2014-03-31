METASOURCES = AUTO
INCLUDES = -I$(top_srcdir)/buildtools/vstudio \
	-I$(top_srcdir)/lib/interfaces \
	-I$(top_srcdir)/lib/interfaces/extensions \
	-I$(top_srcdir)/lib/interfaces/external \
	-I$(top_srcdir)/lib/interfaces/extras \
	-I$(top_srcdir)/lib/util \
	-I$(top_srcdir)/lib/widgets \
	-I$(top_srcdir)/buildtools/lib \
	-I$(top_srcdir)/buildtools/lib/base \
	-I$(top_srcdir)/buildtools/lib/widgets \
	-I$(top_builddir)/buildtools/lib/widgets \
	$(all_includes)

noinst_HEADERS = vs_common.h vs_explorer.h vs_explorer_widget.h vs_model.h \
	vs_part.h
kde_module_LTLIBRARIES = libkdevvs.la
libkdevvs_la_SOURCES = vs_common.cpp vs_explorer.cpp vs_explorer_widget.ui \
	vs_model.cpp vs_part.cpp
libkdevvs_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkdevvs_la_LIBADD = $(top_builddir)/lib/libkdevelop.la \
	$(top_builddir)/buildtools/lib/base/libkdevbuildbase.la \
	$(top_builddir)/buildtools/lib/widgets/libkdevbuildtoolswidgets.la \
	$(LIB_KHTML)
service_DATA = kdevpart_vs.desktop
servicedir = $(kde_servicesdir)
# $(kdedir)/share/services/kdevpart_vs.desktop
rc_DATA = kdevpart_vs.rc
rcdir = $(kde_datadir)/kdevpart_vs
# $(kdedir)/share/apps/kdevvs/kdevpart_vs.rc
