dataFiles = src-Makefile.am kdevpart_part.cpp kdevpart_part.h \
 	kdevpart_widget.h kdevpart_widget.cpp kdevpart_part.rc \
	kdevpart.desktop app.kdevelop README.dox globalconfigbase.ui globalconfig.h \
	globalconfig.cpp projectconfigbase.ui projectconfig.h projectconfig.cpp
templateName = kdevpart

### no need to change below:
template_DATA = $(templateName).kdevtemplate
templatedir = ${appwizarddatadir}/templates

appwizarddatadir = ${kde_datadir}/kdevappwizard
$(templateName).tar.gz: ${dataFiles}
	$(TAR) -cf $(templateName).tar -C $(srcdir) ${dataFiles}
	$(GZIP_COMMAND) -f9 $(templateName).tar

archivedir = ${appwizarddatadir}
archive_DATA = $(templateName).tar.gz ${templateName}.png

CLEANFILES = *.tar.gz