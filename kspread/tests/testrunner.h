/* This file is part of the KDE project
   Copyright 2004 Ariya Hidayat <ariya@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KSPREAD_TEST_RUNNER
#define KSPREAD_TEST_RUNNER

#include <kdialogbase.h>

class KLocale;
class KSpreadSheet;

namespace KSpread
{

class Tester;

class TestRunner : public KDialogBase
{
Q_OBJECT

public:
  TestRunner(KLocale *locale, KSpreadSheet *sheet);
  ~TestRunner();
  
  void addTester( Tester* tester );
  
private slots:
  void runTest();
  
private:
  class Private;
  Private* d;
};

}

#endif // KSPREAD_TEST_RUNNER

