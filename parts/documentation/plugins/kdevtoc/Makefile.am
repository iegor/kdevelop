INCLUDES = -I$(top_srcdir)/lib/interfaces -I$(top_srcdir)/lib/util \
	-I$(top_srcdir)/parts/documentation/interfaces $(all_includes)
METASOURCES = AUTO
kde_module_LTLIBRARIES =  libdockdevtocplugin.la
libdockdevtocplugin_la_LDFLAGS = -module -avoid-version -no-undefined $(all_libraries)
kde_services_DATA =  dockdevtocplugin.desktop
noinst_HEADERS =  dockdevtocplugin.h
libdockdevtocplugin_la_SOURCES =  dockdevtocplugin.cpp
libdockdevtocplugin_la_LIBADD = $(top_builddir)/lib/libkdevelop.la \
	$(top_builddir)/parts/documentation/interfaces/libdocumentation_interfaces.la
