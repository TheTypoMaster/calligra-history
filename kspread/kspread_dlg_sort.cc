/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999,2000,2001 Montel Laurent <lmontel@mandrakesoft.com>

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


#include "kspread_dlg_sort.h"
#include "kspread_doc.h"
#include "kspread_map.h"
#include "kspread_table.h"
#include "kspread_view.h"
#include "kspread_util.h"

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qtabwidget.h>
#include <qwidget.h>

KSpreadSortDlg::KSpreadSortDlg( KSpreadView * parent,  const char * name,
                                bool modal, WFlags fl )
  : QDialog( parent, name, modal, fl ),
    m_pView( parent )
{
  if ( !name )
    setName( "KSpreadSortDlg" );

  resize( 528, 316 );
  setCaption( i18n( "Sort - attention: this is still work in progress!!" ) );
  setSizeGripEnabled( true );

  QVBoxLayout * KSpreadSortDlgLayout
    = new QVBoxLayout( this, 11, 6, "KSpreadSortDlgLayout");

  m_tabWidget = new QTabWidget( this, "m_tabWidget" );

  m_page1 = new QWidget( m_tabWidget, "m_page1" );
  QGridLayout * page1Layout
    = new QGridLayout( m_page1, 1, 1, 11, 6, "page1Layout");

  QGroupBox * sort1Box = new QGroupBox( m_page1, "sort1Box" );
  sort1Box->setTitle( i18n( "Sort by" ) );
  sort1Box->setColumnLayout(0, Qt::Vertical );
  sort1Box->layout()->setSpacing( 6 );
  sort1Box->layout()->setMargin( 11 );
  QHBoxLayout * sort1BoxLayout = new QHBoxLayout( sort1Box->layout() );
  sort1BoxLayout->setAlignment( Qt::AlignTop );

  m_sortKey1 = new QComboBox( false, sort1Box, "m_sortKey1" );
  sort1BoxLayout->addWidget( m_sortKey1 );

  m_sortOrder1 = new QComboBox( false, sort1Box, "m_sortOrder1" );
  m_sortOrder1->insertItem( i18n( "Ascending" ) );
  m_sortOrder1->insertItem( i18n( "Descending" ) );
  sort1BoxLayout->addWidget( m_sortOrder1 );

  page1Layout->addWidget( sort1Box, 0, 0 );

  QGroupBox * sort2Box = new QGroupBox( m_page1, "sort2Box" );
  sort2Box->setTitle( i18n( "Then by - still working on implementation" ) );
  sort2Box->setColumnLayout(0, Qt::Vertical );
  sort2Box->layout()->setSpacing( 6 );
  sort2Box->layout()->setMargin( 11 );
  QHBoxLayout * sort2BoxLayout = new QHBoxLayout( sort2Box->layout() );
  sort2BoxLayout->setAlignment( Qt::AlignTop );

  m_sortKey2 = new QComboBox( false, sort2Box, "m_sortKey2" );
  m_sortKey2->insertItem( i18n( "None" ) );
  m_sortKey2->setEnabled( false );
  sort2BoxLayout->addWidget( m_sortKey2 );

  m_sortOrder2 = new QComboBox( false, sort2Box, "m_sortOrder2" );
  m_sortOrder2->insertItem( i18n( "Ascending" ) );
  m_sortOrder2->insertItem( i18n( "Descending" ) );
  m_sortOrder2->setEnabled ( false );
  sort2BoxLayout->addWidget( m_sortOrder2 );

  page1Layout->addWidget( sort2Box, 1, 0 );

  QGroupBox * sort3Box = new QGroupBox( m_page1, "sort3Box" );
  sort3Box->setTitle( i18n( "Then by" ) );
  sort3Box->setColumnLayout(0, Qt::Vertical );
  sort3Box->layout()->setSpacing( 6 );
  sort3Box->layout()->setMargin( 11 );
  QHBoxLayout * sort3BoxLayout = new QHBoxLayout( sort3Box->layout() );
  sort3BoxLayout->setAlignment( Qt::AlignTop );

  m_sortKey3 = new QComboBox( false, sort3Box, "m_sortKey3" );
  m_sortKey3->insertItem( i18n( "None" ) );
  m_sortKey3->setEnabled( false );
  sort3BoxLayout->addWidget( m_sortKey3 );

  m_sortOrder3 = new QComboBox( false, sort3Box, "m_sortOrder3" );
  m_sortOrder3->insertItem( i18n( "Ascending" ) );
  m_sortOrder3->insertItem( i18n( "Descending" ) );
  m_sortOrder3->setEnabled( false );
  sort3BoxLayout->addWidget( m_sortOrder3 );

  page1Layout->addWidget( sort3Box, 2, 0 );
  m_tabWidget->insertTab( m_page1, i18n( "Sort criterions" ) );


  // options page

  m_page2 = new QWidget( m_tabWidget, "m_page2" );
  QGridLayout * page2Layout = new QGridLayout( m_page2, 1, 1, 11, 6, "page2Layout");

  QGroupBox * firstKeyBox = new QGroupBox( m_page2, "firstKeyBox" );
  firstKeyBox->setTitle( i18n( "First key" ) );
  firstKeyBox->setColumnLayout(0, Qt::Vertical );
  firstKeyBox->layout()->setSpacing( 6 );
  firstKeyBox->layout()->setMargin( 11 );
  QVBoxLayout * firstKeyBoxLayout = new QVBoxLayout( firstKeyBox->layout() );
  firstKeyBoxLayout->setAlignment( Qt::AlignTop );

  m_useCustomLists = new QCheckBox( firstKeyBox, "m_useCustomLists_2" );
  m_useCustomLists->setText( i18n( "&Use custom list" ) );
  firstKeyBoxLayout->addWidget( m_useCustomLists );

  m_customList = new QComboBox( false, firstKeyBox, "m_customList" );
  m_customList->setEnabled( false );
  m_customList->setMaximumSize( 230, 30 );
  firstKeyBoxLayout->addWidget( m_customList );

  page2Layout->addWidget( firstKeyBox, 0, 1 );

  QButtonGroup * orientationGroup = new QButtonGroup( m_page2, "orientationGroup" );
  orientationGroup->setTitle( i18n( "Orientation" ) );
  orientationGroup->setColumnLayout(0, Qt::Vertical );
  orientationGroup->layout()->setSpacing( 6 );
  orientationGroup->layout()->setMargin( 11 );
  QGridLayout * orientationGroupLayout = new QGridLayout( orientationGroup->layout() );
  orientationGroupLayout->setAlignment( Qt::AlignTop );

  m_sortColumn = new QRadioButton( orientationGroup, "m_sortColumn" );
  m_sortColumn->setText( i18n( "&Column" ) );
  m_sortColumn->setChecked( true );

  orientationGroupLayout->addWidget( m_sortColumn, 0, 0 );

  m_sortRow = new QRadioButton( orientationGroup, "m_sortRow" );
  m_sortRow->setText( i18n( "&Row" ) );

  orientationGroupLayout->addWidget( m_sortRow, 1, 0 );

  page2Layout->addWidget( orientationGroup, 0, 0 );

  m_copyLayout = new QCheckBox( m_page2, "m_copyLayout" );
  m_copyLayout->setText( i18n( "Copy &layout" ) );

  page2Layout->addMultiCellWidget( m_copyLayout, 2, 2, 0, 1 );

  QGroupBox * resultToBox = new QGroupBox( m_page2, "resultToBox" );
  resultToBox->setTitle( i18n( "Put results to" ) );
  resultToBox->setColumnLayout(0, Qt::Vertical );
  resultToBox->layout()->setSpacing( 6 );
  resultToBox->layout()->setMargin( 11 );
  resultToBox->setEnabled( false ); // TODO: remove this later
  QHBoxLayout * resultToBoxLayout = new QHBoxLayout( resultToBox->layout() );
  resultToBoxLayout->setAlignment( Qt::AlignTop );

  m_outputTable = new QComboBox( false, resultToBox, "m_outputTable" );
  resultToBoxLayout->addWidget( m_outputTable );
  QSpacerItem * spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  resultToBoxLayout->addItem( spacer );

  QLabel * startingCellLabel = new QLabel( resultToBox, "startingCellLabel" );
  startingCellLabel->setText( i18n( "Starting cell:" ) );
  resultToBoxLayout->addWidget( startingCellLabel );

  m_outputCell = new QLineEdit( resultToBox, "m_outputCell" );
  m_outputCell->setMaximumSize( QSize( 60, 32767 ) );
  m_outputCell->setEnabled( false ); // TODO: remove this later
  resultToBoxLayout->addWidget( m_outputCell );

  page2Layout->addMultiCellWidget( resultToBox, 1, 1, 0, 1 );
  m_tabWidget->insertTab( m_page2, i18n( "Options" ) );
  KSpreadSortDlgLayout->addWidget( m_tabWidget );

  QHBoxLayout * Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1");
  QSpacerItem * spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding,
                                            QSizePolicy::Minimum );
  Layout1->addItem( spacer_2 );

  m_buttonOk = new QPushButton( this, "m_buttonOk" );
  m_buttonOk->setText( i18n( "&OK" ) );
  m_buttonOk->setAccel( 0 );
  m_buttonOk->setAutoDefault( TRUE );
  m_buttonOk->setDefault( TRUE );
  Layout1->addWidget( m_buttonOk );

  m_buttonCancel = new QPushButton( this, "buttonCancel" );
  m_buttonCancel->setText( i18n( "Cancel" ) );
  m_buttonCancel->setAccel( 0 );
  m_buttonCancel->setAutoDefault( TRUE );
  Layout1->addWidget( m_buttonCancel );
  KSpreadSortDlgLayout->addLayout( Layout1 );

  connect( m_buttonOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

  connect( m_sortKey2, SIGNAL( activated( int ) ), this,
           SLOT( sortKey2textChanged( int ) ) );
  connect( m_useCustomLists, SIGNAL( stateChanged(int) ), this,
           SLOT( useCustomListsStateChanged(int) ) );
  connect( orientationGroup, SIGNAL( pressed(int) ), this,
           SLOT( slotOrientationChanged(int) ) );

  init();
}

KSpreadSortDlg::~KSpreadSortDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

void KSpreadSortDlg::init()
{
  QString month;
  month += i18n("January") + ", ";
  month += i18n("February") + ", ";
  month += i18n("March") + ", ";
  month += i18n("April") + ", ";
  month += i18n("May") + ", ";
  month += i18n("June") + ", ";
  month += i18n("July") + ", ";
  month += i18n("August") + ", ";
  month += i18n("September") + ", ";
  month += i18n("October") + ", ";
  month += i18n("November") + ", ";
  month += i18n("December");
  QStringList lst;
  lst.append(month);

  QString day( i18n("Monday") + ", " );
  day += i18n("Tuesday") + ", ";
  day += i18n("Wednesday") + ", ";
  day += i18n("Thursday") + ", ";
  day += i18n("Friday") + ", ";
  day += i18n("Saturday") + ", ";
  day += i18n("Sunday");
  lst.append(day);

  KConfig * config = KSpreadFactory::global()->config();
  config->setGroup( "Parameters" );
  QStringList other = config->readListEntry("Other list");
  QString tmp;
  for ( QStringList::Iterator it = other.begin(); it != other.end(); ++it )
  {
    if((*it) != "\\")
      tmp += (*it) + ", ";
    else if( it != other.begin())
    {
      tmp = tmp.left(tmp.length() - 2);
      lst.append(tmp);
      tmp = "";
    }
  }
  m_customList->insertStringList(lst);

  QPtrList<KSpreadTable> tableList = m_pView->doc()->map()->tableList();
  for (unsigned int c = 0; c < tableList.count(); ++c)
  {
    KSpreadTable * t = tableList.at(c);
    if (!t)
      continue;
    m_outputTable->insertItem( t->tableName() );
  }
  m_outputTable->setCurrentText( m_pView->activeTable()->tableName() );

  QRect r = m_pView->activeTable()-> selectionRect();
  QString cellArea;
  cellArea += util_encodeColumnLabelText(r.left());
  cellArea += QString::number( r.top() );
  m_outputCell->setText( cellArea );

  // Entire columns selected ?
  if ( m_pView->activeTable()->isColumnSelected() )
  {
    m_sortRow->setEnabled(false);
    m_sortColumn->setChecked(true);

    int right = r.right();
    for (int i = r.left(); i <= right; ++i)
      m_listColumn += i18n("Column %1").arg(util_encodeColumnLabelText(i));
  }
  // Entire rows selected ?
  else if ( m_pView->activeTable()->isRowSelected() )
  {
    m_sortColumn->setEnabled(false);
    m_sortRow->setChecked(true);

    int bottom = r.bottom();
    for (int i = r.top(); i <= bottom; ++i)
      m_listRow += i18n("Row %1").arg(i);
  }
  else
  {
    // Selection is only one row
    if ( r.top() == r.bottom() )
    {
      m_sortColumn->setEnabled(false);
      m_sortRow->setChecked(true);
    }
    // only one column
    else if (r.left() == r.right())
    {
      m_sortRow->setEnabled(false);
      m_sortColumn->setChecked(true);
    }
    else
    {
      m_sortColumn->setChecked(true);
    }

    int right  = r.right();
    int bottom = r.bottom();
    for (int i = r.left(); i <= right; ++i)
      m_listColumn += i18n("Column %1").arg(util_encodeColumnLabelText(i));

    for (int i = r.top(); i <= bottom; ++i)
      m_listRow += i18n("Row %1").arg(i);
  }

  // Initialize the combo box
  if ( m_sortRow->isChecked() )
    slotOrientationChanged(1);
  else
    slotOrientationChanged(0);
}

void KSpreadSortDlg::slotOrientationChanged(int id)
{
  switch( id )
  {
   case 0 :
    m_sortKey1->clear();
    m_sortKey2->clear();
    m_sortKey3->clear();
    m_sortKey1->insertStringList(m_listColumn);
    m_sortKey2->insertItem( i18n("None") );
    m_sortKey2->insertStringList(m_listColumn);
    m_sortKey3->insertItem( i18n("None") );
    m_sortKey3->insertStringList(m_listColumn);
    break;

   case 1 :
    m_sortKey1->clear();
    m_sortKey2->clear();
    m_sortKey3->clear();
    m_sortKey1->insertStringList(m_listRow);
    m_sortKey2->insertItem( i18n("None") );
    m_sortKey2->insertStringList(m_listRow);
    m_sortKey3->insertItem( i18n("None") );
    m_sortKey3->insertStringList(m_listRow);
    break;

   default :
    kdDebug(36001) << "Error in signal : pressed(int id)" << endl;
    break;
  }
}

void KSpreadSortDlg::slotOk()
{
  KSpreadTable * table = m_pView->doc()->map()->findTable( m_outputTable->currentText() );
  if ( !table )
  {
    KMessageBox::error( this, i18n("The selected output table doesn't exist!") );
    m_outputTable->setFocus();
    m_tabWidget->setTabEnabled(m_page2, true);
    return;
  }

  KSpreadPoint outputPoint( m_outputCell->text() );
  if ( !outputPoint.isValid() || outputPoint.isTableKnown() )
  {
    KMessageBox::error( this, i18n("The output cell is invalid!") );
    m_outputCell->setFocus();
    m_tabWidget->setTabEnabled(m_page2, true);
    return;
  }
  outputPoint.table = table;

  QRect r = m_pView->activeTable()-> selectionRect();
  if ( r.topLeft() != outputPoint.pos )
  {
    int t = outputPoint.pos.y() + r.height();
    int l = outputPoint.pos.x() + r.width();
    if ( r.contains(outputPoint.pos)
         || ( t >= r.left() && t <= r.right() )
         || ( l >= r.top()  && l <= r.bottom() ) )
    {
      KMessageBox::error( this, i18n("The output region must not overlapp with the source region!") );
      m_outputCell->setFocus();
      // TODO: set right tab
      return;
    }
  }

  int key1 = 1;
  int key2 = 0;
  int key3 = 0;
  QStringList * firstKey = 0L;
  KSpreadTable::SortingOrder order1;
  KSpreadTable::SortingOrder order2;
  KSpreadTable::SortingOrder order3;

  order1 = ( m_sortOrder1->currentItem() == 0 ? KSpreadTable::Increase
             : KSpreadTable::Decrease );
  order2 = ( m_sortOrder2->currentItem() == 0 ? KSpreadTable::Increase
             : KSpreadTable::Decrease );
  order3 = ( m_sortOrder3->currentItem() == 0 ? KSpreadTable::Increase
             : KSpreadTable::Decrease );

  if ( m_sortRow->isChecked() )
  {
    key1 = m_sortKey1->currentItem() + r.top();
    if (m_sortKey2->currentItem() > 0)
      key2 = m_sortKey2->currentItem() + r.top() + 1; // cause there is "None"
    if (m_sortKey3->currentItem() > 0)
      key3 = m_sortKey3->currentItem() + r.top() + 1; // cause there is "None"
  }
  else
  {
    key1 = m_sortKey1->currentItem() + r.left();
    if (m_sortKey2->currentItem() > 0)
      key2 = m_sortKey2->currentItem() + r.left() + 1; // cause there is "None"
    if (m_sortKey3->currentItem() > 0)
      key3 = m_sortKey3->currentItem() + r.left() + 1; // cause there is "None"
  }

  if ( m_useCustomLists->isChecked() )
  {
    QString list = m_customList->currentText();
    QString tmp;
    int l = list.length();
    for ( int i = 0; i < l; ++i )
    {
      if ( list[i] == ',' )
      {
        firstKey->append( tmp.stripWhiteSpace() );
        tmp = "";
      }
      else
        tmp += list[i];
    }
  }

  if (key1 == key2)
    key2 = 0;

  if (key1 == key3)
    key3 = 0;

  if (key2 == 0 && key3 > 0)
  {
    key2 = key3;
    key3 = 0;
  }

  kdDebug() << key1 << ", " << key2 << ", " << key3 << ", " << order1 << ", " << order2 << ", " << order3 << ", " << m_copyLayout->isChecked()
            << ", " << outputPoint.tableName << ", " << outputPoint.pos.x() << ", " << outputPoint.pos.y() << endl;

  if ( m_sortRow->isChecked() )
  {
    m_pView->activeTable()->sortByRow( key1, key2, key3,
                                       order1, order2, order3,
                                       firstKey, m_copyLayout->isChecked(),
                                       outputPoint );
  }
  else if (m_sortColumn->isChecked())
  {
    m_pView->activeTable()->sortByColumn( key1, key2, key3,
                                          order1, order2, order3,
                                          firstKey, m_copyLayout->isChecked(),
                                          outputPoint );
  }
  else
  {
    kdDebug(36001) << "Err in radiobutton" << endl;
  }

  if ( firstKey )
  {
    delete firstKey;
    firstKey = 0L;
  }

  accept();
}

void KSpreadSortDlg::sortKey2textChanged( int )
{
  m_sortKey3->setEnabled( true );
  m_sortOrder3->setEnabled( true );
}

void KSpreadSortDlg::useCustomListsStateChanged( int state )
{
  if (state == 1)
    m_customList->setEnabled(true);
  else
    m_customList->setEnabled(false);
}

#include <kspread_dlg_sort.moc>
