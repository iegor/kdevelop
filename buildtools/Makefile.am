##
## Do NOT remove the comments that start with "kdevelop:"
## They are actually directives to the kdevelop plugin system
##
## The include_xxxx variables are controlled by configure.in.in
##

#kdevelop: ADA_BUILDTOOL = ada
if include_ada
ADA_BUILDTOOL = ada
endif

#kdevelop: ANT_BUILDTOOL = ant
if include_ant
ANT_BUILDTOOL = ant
endif

#kdevelop: AUTOTOOLS_BUILDTOOL = autotools
if include_autoproject
AUTOTOOLS_BUILDTOOL = autotools
endif

#kdevelop: CUSTOMMAKEFILES_BUILDTOOL = custommakefiles
if include_customproject
CUSTOMMAKEFILES_BUILDTOOL = custommakefiles
endif

#kdevelop: PASCAL_BUILDTOOL = pascal
if include_pascal
PASCAL_BUILDTOOL = pascal
endif

#kdevelop: QMAKE_BUILDTOOL = qmake
if include_trollproject
QMAKE_BUILDTOOL = qmake
endif

#kdevelop: SCRIPT_BUILDTOOL = script
if include_scriptproject
SCRIPT_BUILDTOOL = script
endif

#kdevelop: VSTUDIO_BUILDTOOL = vstudio
if include_vstudio
VSTUDIO_BUILDTOOL = vstudio
endif

SUBDIRS = lib $(ADA_BUILDTOOL) $(ANT_BUILDTOOL) $(AUTOTOOLS_BUILDTOOL) \
	$(CUSTOMMAKEFILES_BUILDTOOL) $(PASCAL_BUILDTOOL) $(QMAKE_BUILDTOOL) $(SCRIPT_BUILDTOOL) \
	$(VSTUDIO_BUILDTOOL)

DOXYGEN_EMPTY = YES
include ../Doxyfile.am
