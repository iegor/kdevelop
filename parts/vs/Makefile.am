METASOURCES = AUTO
INCLUDES = -I$(top_srcdir)/lib/interfaces \
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

noinst_HEADERS =
kde_module_LTLIBRARIES = libkdevvs.la
libkdevvs_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkdevvs_la_LIBADD = $(top_builddir)/buildtools/lib/base/libkdevbuildbase.la \
	$(top_builddir)/buildtools/lib/widgets/libkdevbuildtoolswidgets.la \
	$(top_builddir)/lib/interfaces/extras/libkdevextras.la \
	$(top_builddir)/lib/libkdevelop.la \
	$(LIB_KHTML)

libkdevvs_la_SOURCES = vs_part.cpp
service_DATA = kdevvstudio.desktop
servicedir = $(kde_servicesdir)
rc_DATA = kdevpart_vs.rc
rcdir = $(kde_datadir)/kdevvs
