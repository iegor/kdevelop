/***************************************************************************
 *   Copyright (C) 2002-2004 by Alexander Dymo                             *
 *   cloudtemple@mskat.net                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "pspinbox.h"

#include <limits.h>

#include <qspinbox.h>
#include <qlayout.h>

namespace PropertyLib{

PSpinBox::PSpinBox(MultiProperty *property, QWidget *parent, const char *name)
    :PropertyWidget(property, parent, name)
{
    QHBoxLayout *l = new QHBoxLayout(this, 0, 0);
    m_edit = new QSpinBox(INT_MIN, INT_MAX, 1, this);
    m_edit->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    l->addWidget(m_edit);
    
    connect(m_edit, SIGNAL(valueChanged(int)), this, SLOT(updateProperty(int)));
}

PSpinBox::PSpinBox(MultiProperty *property, int minValue, int maxValue, int step, QWidget *parent, const char *name)
    :PropertyWidget(property, parent, name)
{
    QHBoxLayout *l = new QHBoxLayout(this, 0, 0);
    m_edit = new QSpinBox(minValue, maxValue, step, this);
    l->addWidget(m_edit);
    
    connect(m_edit, SIGNAL(valueChanged(int)), this, SLOT(updateProperty(int)));
}

QVariant PSpinBox::value() const
{
    return QVariant(m_edit->cleanText().toInt());
}

void PSpinBox::setValue(const QVariant &value, bool emitChange)
{
    disconnect(m_edit, SIGNAL(valueChanged(int)), this, SLOT(updateProperty(int)));
    m_edit->setValue(value.toInt());
    connect(m_edit, SIGNAL(valueChanged(int)), this, SLOT(updateProperty(int)));
    if (emitChange)
        emit propertyChanged(m_property, value);
}

void PSpinBox::updateProperty(int val)
{
    emit propertyChanged(m_property, QVariant(val));
}

}

#ifndef PURE_QT
#include "pspinbox.moc"
#endif
