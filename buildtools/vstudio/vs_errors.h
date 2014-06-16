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
#define VSPART_ERR_ENTITYLIST_CORRUPTED \
    VSPART_ERROR"Part's entities list is corrupted.\n"
#define VSPART_WRN_CONFIG_NAMEORPLATFORM_UNDEFINED \
    VSPART_WARNING"either name of platform is undefined.\n"
#define VSPART_ERR_REFCOUNT_NONZERODELETE \
    VSPART_ERROR"Destructing referenced object.\n"
#define VSPART_ERR_REFCOUNT_WARPAROUND \
    VSPART_ERROR"Can't increase ref counter, it warps around.\n"

#ifdef DEBUG
extern const QString g_msg_configapply;
extern const QString g_msg_slnselect;
extern const QString g_msg_entselected;
#endif /* DEBUG */

// extern const QString g_err_refcount_nonzeroremoval;
extern const QString g_err_emptypath;
extern const QString g_err_unsupportedplatform;
extern const QString g_err_notenoughmem;
extern const QString g_err_list_corrupted;
extern const QString g_err_nullptr;
extern const QString g_err_slnactivate;
extern const QString g_err_slnload;
extern const QString g_err_prjload;
extern const QString g_err_ent_notfound;
extern const QString g_err_unsupportedtyp;
extern const QString g_wrn_unsupportedtyp;
}; /* VStudio namespace */

#endif /* __KDEVPART_VSTUDIOPART_ERRORS_H__ */