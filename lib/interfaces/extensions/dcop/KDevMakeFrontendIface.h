/* This file is part of the KDE project
   Copyright (C) 2001 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
   Copyright (C) 2001 Bernd Gehrmann <bernd@kdevelop.org>
   Copyright (C) 2002 Roberto Raggi <roberto@kdevelop.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#ifndef _KDEVMAKEFRONTENDIFACE_H_
#define _KDEVMAKEFRONTENDIFACE_H_

#include <dcopobject.h>

class KDevMakeFrontend;

class KDevMakeFrontendIface : public DCOPObject
{
    K_DCOP
    
public:
    
    KDevMakeFrontendIface( KDevMakeFrontend *makeFrontend );
    ~KDevMakeFrontendIface();

k_dcop:
    void queueCommand(const QString &dir, const QString &command);
    bool isRunning();

private:
    KDevMakeFrontend *m_makeFrontend;
};

#endif
