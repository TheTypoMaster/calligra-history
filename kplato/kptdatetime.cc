/* This file is part of the KDE project
   Copyright (C) 2003 Dag Andersen <danders@get2net.dk>

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

#include "kptdatetime.h"

#include <kdebug.h>

KPTDateTime::KPTDateTime() : QDateTime() {
}

KPTDateTime::KPTDateTime(const QDateTime &dt) : QDateTime(dt.date(), dt.time()) {
}

KPTDateTime::KPTDateTime(const QDate &date, const QTime &time) : QDateTime(date, time) {
}

void KPTDateTime::add(const KPTDuration &duration) {
    int days = KPTDuration::zeroDuration.dateTime().daysTo(duration.dateTime());
    QTime t; //00:00:00
    int secs = t.secsTo(duration.dateTime().time());
    *this = addDays(days);
    *this = addSecs(secs);
    //kdDebug()<<k_funcinfo<<"days,secs: "<<days<<","<<secs<<" gives: "<<toString()<<endl;
}

void KPTDateTime::subtract(const KPTDuration &duration) {
    int days =  KPTDuration::zeroDuration.dateTime().daysTo(duration.dateTime());
    QTime t; //00:00:00
    int secs = t.secsTo(duration.dateTime().time());
    *this = addDays(-days);
    *this = addSecs(-secs);
    //kdDebug()<<k_funcinfo<<"days,secs: "<<days<<","<<secs<<" gives: "<<toString()<<endl;
}

KPTDuration KPTDateTime::duration(const KPTDateTime &dt) const {
    KPTDuration dur;
    if (*this > dt) {
        dur.addDays(dt.daysTo(*this));
        dur.addSecs(dt.time().secsTo(time()));
    } else {
        dur.addDays(daysTo(dt));
        dur.addSecs(time().secsTo(dt.time()));
    }
    //kdDebug()<<k_funcinfo<<"this="<<this->toString()<<" - dt="<<dt.toString()<<" gives: "<<dur.toString()<<endl;
    return dur;
}

KPTDateTime KPTDateTime::operator+(const KPTDuration &duration) const {
    KPTDateTime d(*this);
    d.add(duration);
    return d;
}

KPTDateTime& KPTDateTime::operator+=(const KPTDuration &duration) {
    add(duration);
    return *this;
}

KPTDateTime KPTDateTime::operator-(const KPTDuration &duration) const {
    KPTDateTime d(*this);
    d.subtract(duration);
    return d;
}

KPTDateTime& KPTDateTime::operator-=(const KPTDuration &duration) {
    subtract(duration);
    return *this;
}
