/******************************************************************/ 
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Page (header)                                          */
/******************************************************************/

#ifndef kword_page_h
#define kword_page_h

#include "defs.h"
#include "fc.h"
#include "format.h"
#include "paraglayout.h"
#include "char.h"

#include <qevent.h>
#include <math.h>
#include <qwidget.h>
#include <qkeycode.h>
#include <qpixmap.h>

#include <X11/Xlib.h>
#include <kapp.h>

class KWordGUI;
class KWordDocument_impl;

/******************************************************************/
/* Class: KWPage                                                  */
/******************************************************************/

class KWPage : public QWidget
{
  Q_OBJECT
  
public:
  KWPage(QWidget *parent,KWordDocument_impl *_doc,KWordGUI *_gui);

  void setDocument(KWordDocument_impl *_doc)
    { doc = _doc; }

  void mousePressEvent(QMouseEvent* e) {}
  void mouseMoveEvent(QMouseEvent* e) {}
  void mouseReleaseEvent(QMouseEvent* e) {}
  void paintEvent(QPaintEvent* e);
  void keyPressEvent(QKeyEvent * e);
  void resizeEvent(QResizeEvent *e);

  /**
   * @return 0 if the cursor is visible with respect to the 
   *         y-coordinate. Mention that the cursor may be never the less
   *         invisible because of its x-coordinate. 1 is returned if the cursor
   *         is under the visible area and -1 if the cursor is above.
   *
   * @see #isCursorXVisible
   */
  int isCursorYVisible(KWFormatContext &_fc);
  /**
   * @see #isCursorYVisible
   */
  int isCursorXVisible(KWFormatContext &_fc);
  
  void scrollToCursor(KWFormatContext &_fc);
  void scrollToOffset(int _x,int _y,KWFormatContext &_fc);
 
  void setXOffset(int _x)
    { xOffset = _x; calcVisiblePages(); }
  void setYOffset(int _y)
    { yOffset = _y; calcVisiblePages(); }

  void scroll(int dx,int dy);

  void formatChanged(KWFormat &_format);
  void setFlow(KWParagLayout::Flow _flow)
    { fc->getParag()->getParagLayout()->setFlow(_flow); recalcCursor(); }
  void setLeftIndent(float _left)
    { fc->getParag()->getParagLayout()->setptLeftIndent(static_cast<unsigned int>(MM_TO_POINT(_left))); recalcCursor(); }
  void setFirstLineIndent(float _first)
    { fc->getParag()->getParagLayout()->setPTFirstLineLeftIndent(static_cast<unsigned int>(MM_TO_POINT(_first))); recalcCursor(); }
  void setSpaceBeforeParag(float _before)
    { fc->getParag()->getParagLayout()->setptParagHeadOffset(static_cast<unsigned int>(MM_TO_POINT(_before))); recalcCursor(); }
  void setSpaceAfterParag(float _after)
    { fc->getParag()->getParagLayout()->setptParagFootOffset(static_cast<unsigned int>(MM_TO_POINT(_after))); recalcCursor(); }
  void setLineSpacing(unsigned int _spacing)
    { fc->getParag()->getParagLayout()->setPTLineSpacing(_spacing); recalcCursor(); }

  float getLeftIndent()
    { return POINT_TO_MM(static_cast<float>(fc->getParag()->getParagLayout()->getPTLeftIndent())); }
  float getFirstLineIndent()
    { return POINT_TO_MM(static_cast<float>(fc->getParag()->getParagLayout()->getPTFirstLineLeftIndent())); }
  float getSpaceBeforeParag()
    { return POINT_TO_MM(static_cast<float>(fc->getParag()->getParagLayout()->getPTParagHeadOffset())); }
  float getSpaceAfterParag()
    { return POINT_TO_MM(static_cast<float>(fc->getParag()->getParagLayout()->getPTParagFootOffset())); }
  unsigned int getLineSpacing()
    { return fc->getParag()->getParagLayout()->getPTLineSpacing(); }


  void recalcCursor();

  int getVertRulerPos();

  void insertPictureAsChar(QString _filename);

public slots:
  void newLeftIndent(int _left)
    { setLeftIndent(static_cast<float>(_left)); }
  void newFirstIndent(int _first)
    { setFirstLineIndent(static_cast<float>(_first)); }

protected:
  unsigned int ptLeftBorder();
  unsigned int ptRightBorder();
  unsigned int ptTopBorder();
  unsigned int ptBottomBorder();
  unsigned int ptPaperWidth();
  unsigned int ptPaperHeight();
  unsigned int ptColumnWidth();
  unsigned int ptColumnSpacing();

  void enterEvent(QEvent *)
    { setFocus(); }
  void focusInEvent(QFocusEvent *) {}
  void focusOutEvent(QFocusEvent *) {}

  /**
   * Looks at 'yOffset' and 'paperHeight' and calculates the first and
   * last visible pages. The values are stored in 'firstVisiblePage' and
   * 'lastVisiblePages'.
   *
   * @see #yOffset
   * @see #paperHeight
   * @see #firstVisiblePage
   * @see #lastVisiblePage
   */
  void calcVisiblePages();

  void drawBuffer();
  void copyBuffer();
  
  KWordDocument_impl *doc;
  bool markerIsVisible;
  bool paint_directly,has_to_copy;

  /**
   * The xOffset in zoomed pixels.
   */
  unsigned int xOffset;
  /**
   * The yOffset in zoomed pixels.
   */
  unsigned int yOffset;

  /**
   * The first (partial?) visible page.
   *
   * @see #calcVisiblePages
   */
  unsigned int firstVisiblePage;
  /**
   * The last (partial?) visible page.
   *
   * @see #calcVisiblePages
   */
  unsigned int lastVisiblePage;
  
  KWFormatContext *fc;

  KWordGUI *gui;
  QPixmap buffer;
  KWFormat format;

};

#endif





