/*
** A program to convert the XML rendered by KWord into LATEX.
**
** Copyright (C) 2000 Robert JACOLIN
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** To receive a copy of the GNU Library General Public License, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
**
*/

#include <stdlib.h>		/* for atoi function    */
#include <kdebug.h>		/* for kdDebug() stream */
#include "para.h"
#include "texte.h"		/* father class.        */
#include "format.h"		/* children classes.    */
#include "textzone.h"
#include "footnote.h"

/*******************************************/
/* Constructor                             */
/*******************************************/
Para::Para(Texte* texte)
{
	_element   = texte;
	_lines     = 0;
	_next      = 0;
	_previous  = 0;
	_name      = 0;
	_info      = EP_NONE;	/* the parag is not a footnote */
	_hardbrk   = EP_FLOW;	/* and it's not a new page */
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Para::~Para()
{
	kdDebug() << "Destruction of a parag." << endl;
	if(_lines != 0)
		delete _lines;
}

/*******************************************/
/* IsColored                               */
/*******************************************/
/* Return TRUE if there is at least one    */
/* text zone which use color.              */
/*******************************************/
/* DEPRECATED.                             */
/*******************************************/
bool Para::isColored() const
{
	bool color;
	FormatIter iter;

	color = false;
	iter.setList(_lines);
	while(!iter.isTerminate() && !color)
	{
		if(iter.getCourant()->getFormatType()== EF_TEXTZONE)
			color = ((TextZone*) iter.getCourant())->isColor();
		iter.next();
	}
	return color;
}

/*******************************************/
/* IsUnderlined                            */
/*******************************************/
/* Return TRUE if there is at least one    */
/* text zone which use uline.              */
/*******************************************/
/* DEPRECATED.                             */
/*******************************************/
bool Para::isUlined() const
{
	bool uline;
	FormatIter iter;

	uline = false;
	iter.setList(_lines);
	while(!iter.isTerminate() && !uline)
	{
		if(iter.getCourant()->getFormatType()== EF_TEXTZONE)
			uline = ((TextZone*) iter.getCourant())->isUnderlined();
		iter.next();
	}
	return uline;
}

/*******************************************/
/* GetFrameType                            */
/*******************************************/
/* To know if it's the text or it's a      */
/* header or a footer.                     */
/*******************************************/
SSect Para::getFrameType() const
{
	return _element->getSection();
}

/*******************************************/
/* getTypeFormat                           */
/*******************************************/
/* To know if the zone is a textzone, a    */
/* footnote, a picture, a variable.        */
/*******************************************/
EFormat Para::getTypeFormat(const Markup* balise) const
{
	Arg *arg = 0;

	//<FORMAT id="1" ...>
	for(arg= balise->pArg; arg!= 0; arg= arg->pNext)
	{
		kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "ID")== 0)
		{
			return (EFormat) atoi(arg->zValue);
		}
	}
	return EF_ERROR;
}

/*******************************************/
/* Analyse                                 */
/*******************************************/
void Para::analyse(const Markup * balise_initiale)
{
	Token* savedToken = 0;
	Markup* balise    = 0;

	/* MARKUP TYPE :  PARAGRAPH */

	/* Analyse of the parameters */
	kdDebug() << "ANALYSE A PARAGRAPH" << endl;
	
	/* Analyse of the children markups */
	savedToken = enterTokenChild(balise_initiale);
	while((balise = getNextMarkup()) != 0)
	{
		if(strcmp(balise->token.zText, "TEXT")== 0)
		{
			for(Token *p = balise->pContent; p!= 0; p = p->pNext)
			{
				if(p->zText!= 0)
				{
					_texte += p->zText;
				}
			}
		}
		else if(strcmp(balise->token.zText, "NAME")== 0)
		{
			analyseName(balise);
		}
		else if(strcmp(balise->token.zText, "INFO")== 0)
		{
			analyseInfo(balise);
		}
		else if(strcmp(balise->token.zText, "HARDBRK")== 0)
		{
			analyseBrk(balise);
		}
		/*else if(strcmp(balise->token.zText, "FORMATS")== 0)
		{
			// IMPORTANT ==> police + style
			kdDebug() << "FORMATS" << endl;
			analyseFormats(balise);
			
		}*/
		else if(strcmp(balise->token.zText, "LAYOUT")== 0)
		{
			kdDebug() << "LAYOUT" << endl;
			analyseLayoutPara(balise);
		}
	}
	kdDebug() << "END OF PARAGRAPH" << endl;
}

/*******************************************/
/* AnalyseName                             */
/*******************************************/
/* If a footnote have a name : it's a      */
/* footnote/endnote.                       */
/*******************************************/
void Para::analyseName(const Markup* balise)
{
	Arg *arg = 0;

	//<NAME name="Footnote/Endnote_1">
	for(arg= balise->pArg; arg!= 0; arg= arg->pNext)
	{
		kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "NAME")== 0)
		{
			_name = new QString(arg->zValue);
		}
	}
}

/*******************************************/
/* AnalyseInfo                             */
/*******************************************/
/* Type of the parag. : if info is 1, it's */
/* a footnote/endnote (so it have a name). */
/*******************************************/
void Para::analyseInfo(const Markup* balise)
{
	Arg *arg = 0;

	//<INFO info="1">
	for(arg= balise->pArg; arg!= 0; arg= arg->pNext)
	{
		kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "INFO")== 0)
		{
			_info = (EP_INFO) atoi(arg->zValue);
		}
	}
}

/*******************************************/
/* AnalyseBrk                              */
/*******************************************/
/* There is a new page before this         */
/* paragraph.                              */
/*******************************************/
void Para::analyseBrk(const Markup* balise)
{
	Arg *arg = 0;

	//<NAME name="Footnote/Endnote_1">
	for(arg= balise->pArg; arg!= 0; arg= arg->pNext)
	{
		kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "FRAME")== 0)
		{
			_hardbrk = (EP_HARDBRK) atoi(arg->zValue);
		}
	}
}

/*******************************************/
/* AnalyseLayoutPara                       */
/*******************************************/
/* Analyse the layout of a para.           */
/* For each format, keep the type (picture,*/
/* text, variable, footnote) and put the   */
/* zone in a list.                         */
/*******************************************/
void Para::analyseLayoutPara(const Markup *balise_initiale)
{
	Token* savedToken = 0;
	Markup* balise    = 0;

	savedToken = enterTokenChild(balise_initiale);
	analyseLayout(balise_initiale);
	while((balise = getNextMarkup()) != NULL)
	{
		kdDebug() << balise << endl;
		kdDebug() << balise->token.zText << endl;
		if(strcmp(balise->token.zText, "FORMAT")== 0)
		{
			analyseFormat(balise);
		}
		else
			kdDebug() << " FORMAT FIELD UNKNOWN" << endl;
	}
	setTokenCurrent(savedToken);

}

/*******************************************/
/* AnalyseFormat                           */
/*******************************************/
/* Analyse one format.                     */
/*  keep the type (picture, text, variable,*/
/* footnote) and put the zone in a list.   */
/*******************************************/
void Para::analyseFormat(const Markup *balise)
{
	Format *zone = 0;
	kdDebug() << "ANALYSE FORMAT BODY" << endl;
	switch(getTypeFormat(balise))
	{
		case EF_ERROR: kdDebug() << "Id format error" << endl;
			break;
		case EF_TEXTZONE: /* It's a text line */
				zone = new TextZone(_texte, this);
			break;
		case EF_FOOTNOTE: /* It's a footnote */
				zone = new Footnote(this);
			break;
		case EF_PICTURE: /* It's a picture */
		//		zone = new PictureZone(this);
			break;
		case EF_VARIABLE: /* It's a variable */
		//		zone = new VariableZone(this);
			break;
		default: /* Unknown */
				kdDebug() << "Format not yet supported" << endl;
		}
	if(zone != 0)
	{
		zone->analyse(balise);
		if(_lines == 0)
			_lines = new ListeFormat;

		/* add the text */
		_lines->addLast(zone);
	}
}

/*******************************************/
/* Generate                                */
/*******************************************/
void Para::generate(QTextStream &out)
{

	kdDebug() << "  GENERATION PARA" << endl;
	
	if(getInfo() != EP_FOOTNOTE && getFrameType() != SS_HEADERS &&
	   getFrameType() != SS_FOOTERS)
	{	
		/* We generate center, itemize tag and new page only for
		 * parag not for footnote
		 * If a parag. have a special format (begining)
		 */
		if(_hardbrk == EP_NEXT)
			out << "\\newpage" << endl;
		generateDebut(out);
	}
	/*setLastName();
	setLastCounter();*/
	/* If a parag. have text :))) */
	if(_lines != 0)
	{
		FormatIter iter;
		kdDebug() << "  NB ZONE : " << _lines->getSize() << endl;
		iter.setList(_lines);
		while(!iter.isTerminate())
		{
			iter.getCourant()->generate(out);
			iter.next();
		}
	}
	
	if(getInfo() != EP_FOOTNOTE && getFrameType() != SS_HEADERS &&
	   getFrameType() != SS_FOOTERS)
	{
		/* id than above : a parag. have a special format. (end) 
		 * only it's not a header, nor a footer nor a footnote/endnote
		 */
		generateFin(out);
	}
	kdDebug() << "PARA GENERATED" << endl;
}

/*******************************************/
/* GenerateDebut                           */
/*******************************************/
void Para::generateDebut(QTextStream &out)
{
	/* if it's a chapter */
	if(isChapter())
	{
		/* switch the type, the depth do*/
		generateTitle(out);
	}
	else
	{
		if(_previous == 0 || _previous->getEnv() != getEnv() || getInfo() == EP_FOOTNOTE)
		{
			/* It's a parag. */
			switch(getEnv())
			{
				case ENV_LEFT: out << "\\begin{flushleft}" << endl;
					break;
				case ENV_RIGHT: out << "\\begin{flushright}" << endl;
					break;
				case ENV_CENTER: out << "\\begin{center}" << endl;
					break;
				case ENV_NONE: out << endl;
					break;
			}
		}
		if(isList())
		{
			/* if it's a list */
			if(_previous == 0 || !_previous->isList() ||
				(_previous->isList() && _previous->getCounterDepth() < getCounterDepth()) ||
				(_previous->isList() && _previous->getCounterType() != getCounterType()))
			{
				switch(getCounterType())
				{
					case TL_STANDARD:
						break;
					case TL_ARABIC:
						   out << "\\begin{enumerate}" << endl;
						break;
					case TL_LLETTER:	/* a, b, ... */
						out << "\\begin{enumerate}[a]" << endl;
						break;
					case TL_CLETTER:	/* A, B, ... */
						out << "\\begin{enumerate}[A]" << endl;
						break;
					case TL_LLNUMBER:	/* i, ii, ... */
						out << "\\begin{enumerate}[i]" << endl;
						break;
					case TL_CLNUMBER: /* I, II, ... */
						out << "\\begin{enumerate}[I]" << endl;
						break;
					case TL_BULLET:
						     out << "\\begin{itemize}" << endl;
				}
			}
			out << "\\item ";
		}
	}
}

/*******************************************/
/* GenerateFin                             */
/*******************************************/
void Para::generateFin(QTextStream &out)
{
	/* Close a title of chapter */
	if(isChapter())
		out << "}" << endl;
	if(isList())
	{
		/* It's a list */
		if(_next == 0 || !_next->isList() ||
			(_next->isList() && _next->getCounterDepth() < getCounterDepth()) ||
			(_next->isList() && _next->getCounterType() != getCounterType()))
		{
			out << endl;
			/* but the next parag is not a same list */
			switch(getCounterType())
			{
				case TL_STANDARD: out << endl;
					break;
				case TL_ARABIC:
				case TL_LLETTER:  /* a, b, ... */
				case TL_CLETTER:  /* A, B, ... P. 250*/
				case TL_LLNUMBER: /* i, ii, ... */
				case TL_CLNUMBER: /* I, II, ... */
					       out << endl << "\\end{enumerate}" << endl;
					break;
				case TL_BULLET:
					     out << endl << "\\end{itemize}" << endl;
			}
		}
	}
	if((_previous && !_previous->isChapter()) || !_previous)
	{
		if((_next == 0 || _next->getEnv() != getEnv()) && !isChapter())
		{
			switch(getEnv())
			{
				case ENV_LEFT: out << endl << "\\end{flushleft}" << endl;
					break;
				case ENV_RIGHT: out << endl << "\\end{flushright}" << endl;
					break;
				case ENV_CENTER: out << endl << "\\end{center}" << endl;
					break;
				case ENV_NONE: out << endl;
					break;
			}
		}
		if(_next == 0 || _next->getEnv() == getEnv())
			out << endl;
	}
}

/*******************************************/
/* GenerateTitle                           */
/*******************************************/
void Para::generateTitle(QTextStream &out)
{
	switch(getCounterDepth())
	{
		case 0:
			out << "\\section{";
			break;
		case 1:
			out << "\\subsection{";
			break;
		case 2:
			out << "\\subsubsection{";
			break;
		case 3:
			out << "\\paragraph{";
			break;
		case 4:
			out << "\\subparagraph{";
			break;
		default:
			out << "% section too deep" << endl;
	}
}
