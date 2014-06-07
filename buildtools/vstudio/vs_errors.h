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

#ifdef DEBUG
#define VSPART_ERROR_ENTITYLIST_CORRUPTED VSPART_ERROR "Part's entities list is corrupted.\n"
extern const QString g_err_ent_notfound;
#endif /* DEBUG */

extern const QString g_err_slnactivate;

#endif /* __KDEVPART_VSTUDIOPART_ERRORS_H__ */