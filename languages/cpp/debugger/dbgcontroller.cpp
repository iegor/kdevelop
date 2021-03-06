/***************************************************************************
    begin                : Sun Aug 8 1999
    copyright            : (C) 1999 by John Birch
    email                : jbb@kdevelop.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dbgcontroller.h"
#include <kprocess.h>

/***************************************************************************/

namespace GDBDebugger
{

DbgController::DbgController()
    : dbgProcess_(0)
{
}

/***************************************************************************/

DbgController::~DbgController()
{
    delete dbgProcess_;
}

/***************************************************************************/

}

#include "dbgcontroller.moc"
