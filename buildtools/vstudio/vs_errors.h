/*
*kate: space-indent on; tab-width 2; indent-width 2; indent-mode cstyle; encoding UTF-8;
*
*  C/C++ Implementation: vs_errors
*
* Description:
*
*
* Author: iegor <rmtdev@gmail.com>, (C) 2014
*
* Copyright: See COPYING file that comes with this distribution
*/

#ifndef __KDEVPART_VSTUDIOPART_ERRORS_H__
#define __KDEVPART_VSTUDIOPART_ERRORS_H__

#ifdef DEBUG
#define VSPART_ERROR "Error!!! "
#define VSPART_WARNING "Warning!!! "
#endif /* DEBUG */

namespace VStudio {
#define VSPART_ERR_ENTITYLIST_CORRUPTED VSPART_ERROR \
  "Part's entities list is corrupted.\n"
#define VSPART_WRN_CONFIG_NAMEORPLATFORM_UNDEFINED \
  "Warning! either name of platform is undefined.\n"

#ifdef DEBUG
extern const QString g_err_ent_notfound;
extern const QString g_msg_configapply;
#endif /* DEBUG */

extern const QString g_err_slnactivate;
extern const QString g_err_unsupportedtyp;
extern const QString g_wrn_unsupportedtyp;
}; /* VStudio namespace */

#endif /* __KDEVPART_VSTUDIOPART_ERRORS_H__ */