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
/* Module: Image Collection                                       */
/******************************************************************/

#include "imagecollection.h"
#include "kword_doc.h"

/******************************************************************/
/* Class: KWImageCollection                                       */
/******************************************************************/

/*================================================================*/
KWImageCollection::KWImageCollection(KWordDocument_impl *_doc)
  : images(1999,true,true)
{
  images.setAutoDelete(true);
  doc = _doc;
}

/*================================================================*/
KWImageCollection::~KWImageCollection()
{
  images.clear();
}

/*================================================================*/
KWImage *KWImageCollection::getImage(KWImage &_image,QString &key)
{
  key = "";

  key = generateKey(_image);

  KWImage *image = findImage(key);
  if (image)
    {
      image->incRef();
      
      return image;
    }
  else
    return insertImage(key,_image);
}

/*================================================================*/
void KWImageCollection::removeImage(KWImage *_image)
{
  QString key = generateKey(*_image);

  images.remove(key);
  
  //if (images.remove(key))
  //debug("remove: %s",key.data());
}

/*================================================================*/
QString KWImageCollection::generateKey(KWImage &_image)
{
  QString key;

  // Key: filename-width-height
  // e.g. /home/reggie/pics/kde.gif-40-36
  key.sprintf("%s--%d-%d",_image.getFilename().data(),
	      _image.width(),_image.height());
  return key;
}

/*================================================================*/
KWImage *KWImageCollection::findImage(QString _key)
{
  return images.find(_key.data());
}

/*================================================================*/
KWImage *KWImageCollection::insertImage(QString _key,KWImage &_image)
{
  KWImage *image = new KWImage(doc,_image);
  
  images.insert(_key.data(),image);
  //debug("insert: %s",_key.data());
  image->incRef();

  return image;
}


