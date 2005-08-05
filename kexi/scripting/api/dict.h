/***************************************************************************
 * dict.h
 * This file is part of the KDE project
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#ifndef KROSS_API_DICT_H
#define KROSS_API_DICT_H

#include <qstring.h>
#include <qmap.h>

#include "object.h"
#include "value.h"

namespace Kross { namespace Api {

    /**
     * The Dict class implementates \a Value to handle
     * key=value base dictonaries/maps.
     */
    class Dict : public Value< List, QMap<QString, Object::Ptr> >
    {
            friend class Value< List, QMap<QString, Object::Ptr> >;
        public:

            /**
            * Constructor.
            */
            Dict(const QMap<QString, Object::Ptr> value, const QString& name = "dict");

            /**
            * Destructor.
            */
            virtual ~Dict();

            /// \see Kross::Api::Object::getClassName()
            virtual const QString getClassName() const;

            /// \see Kross::Api::Object::getDescription()
            virtual const QString getDescription() const;

            /**
             * \return a string representation of the whole dictonary.
             *
             * \see Kross::Api::Object::toString()
             */
            virtual const QString toString();

    };

}}

#endif

