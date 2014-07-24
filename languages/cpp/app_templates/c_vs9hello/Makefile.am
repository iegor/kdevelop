templateName = c_vs9hello
dataFiles = hello.c app.sln app.vcproj c_vs9hello.png $(templateName).kdevelop

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

# proj
# - proj.kdevelop
# -- src
# --- app
# ---- app.vcproj
# ---- app.c
# -- sln
# --- app.sln
