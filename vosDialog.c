/* Funtions file for vos Dialogs */

#include <curses.h>
#include <string.h>
#include <ctype.h>
#include "vos.h"
#include "vosKeyCodes.h"

//cfg.maxCol=COLS-1, cfg.maxRow=LINES -2;

int msgConfirm(const char *str, char **opcs, int nOpcs)
{
	int c=-1, i=0, j=0, lStr=0, lOps=0, maxCars=0, ret=-1 , maxLines=0;
	WINDOW *w;
	
	maxCars=COLS-4;
	lStr=strlen(str);
	maxLines=(lStr/maxCars)+4;
	for(i=0; i<nOpcs; i++)
		lOps+=strlen(opcs[i]);
	lOps+=nOpcs;
	
	if((lStr+4)<maxCars)
		maxCars=lStr+4;
	
	w = newwin(maxLines, maxCars, (LINES/2)-maxLines, (COLS-maxCars)/2);
	box(w, 0 , 0);
	wmove(w, 1, 1);
	wrefresh(w);

	for(i=0, j=0; i<lStr; i++, j++)
	{
		if(j==maxCars)
		{
			waddch(w, '\n');
			j=0;
		}
		waddch(w, (chtype)str[i]);
	}
	
	wmove(w, maxLines -2, (maxCars-lOps)/2);
	for(i=0; i<nOpcs; i++)
	{
		waddstr(w, (char*)opcs[i]);
		if(i<nOpcs-1)
			waddch(w, (chtype)' ');
	}
	
	wrefresh(w);
	
	while(ret==-1)
    {
		c = tolower(getch());
		for(i=0; i<nOpcs; i++)
		{
			if(c==tolower(opcs[i][0]))
			{
				ret=c;
				break;
			}
		}
		if(ret==-1)
			flash();
	}
	
	wborder(w, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(w);
	delwin(w);
	return ret;
}

void msgBox(const char *str, const char *strBtn)
{
	int c=-1, i=0, j=0, lStr=0, maxCars=0, maxLines=0;
	WINDOW *w;
	
	maxCars=COLS-4;
	lStr=strlen(str);
	maxLines=(lStr/maxCars)+4;
	
	if((lStr+4)<maxCars)
		maxCars=lStr+4;
	
	w = newwin(maxLines, maxCars, (LINES/2)-maxLines, (COLS-maxCars)/2);
	box(w, 0 , 0);		// 0, 0 gives default characters for the vertical and horizontal lines
	wmove(w, 1, 1);
	wrefresh(w);		// Show that box

	for(i=0, j=0; i<lStr; i++, j++)
	{
		if(j==maxCars)
		{
			waddch(w, '\n');
			j=0;
		}
		waddch(w, (chtype)str[i]);
	}
	mvwaddstr(w, maxLines -2 , (maxCars-strlen(strBtn))/2, strBtn);
	wrefresh(w);
	
	while(1)
    {
		if((c = tolower(getch()))==ESCAPE_KEY || c==KEY_ENTER || c=='\r' || c=='\n' || c==tolower(strBtn[0]))
		{
			//log("\nESC pulsado");
			break;
		}
		else
			flash();
	}
	
	wborder(w, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(w);
	delwin(w);
}

void progress(const char *str, int *progress)
{
	if(*progress==-1)
		return;
		
}

char *getStrBox(const char *str, char *ret, int nCars)
{
	int c=-1, i=0, wRow=1, wCol=1, lStr=0, maxCars=0, maxLines=0;
	char strAux[11]={0};
	WINDOW *w;
	
	maxCars=COLS-4;
	lStr=strlen(str)+nCars+1;
	maxLines=(lStr/maxCars)+4;
	
	if((lStr+4)<maxCars)
		maxCars=lStr+4;
	
	w = newwin(maxLines, maxCars, (LINES/2)-maxLines, (COLS-maxCars)/2);
	box(w, 0 , 0);		// 0, 0 gives default characters for the vertical and horizontal lines
	mvwaddstr(w, wRow, wCol, str);
	//mvwaddstr(w, wRow, (wCol+=strlen(str)), str);
	wrefresh(w);		// Show that box

	wCol+=strlen(str);
	//mvwaddstr(w, maxLines -2 , (maxCars-strlen(strBtn))/2, strBtn);
	//wrefresh(w);
	
	while(((c = (unsigned char)getch())!='\r' && c!='\n' && c!=KEY_ENTER) && i <nCars)
    {
		if(isalnum(c) || isblank(c) || isprint(c))	//Recoger la cadena.
		{
			ret[i++]=c;
			ret[i]=0;	//Fin de cadena para pa porsi
			waddch(w,c);			
		}
		else if(c==ESCAPE_KEY)	//Salir si recoger la cadena
		{
			return NULL;
		}
		else if(c==KEY_DC || c==7 || c==8 || c==127 || c==KEY_BACKSPACE)	//Borrar (BackSpace)
		{
			if(i>0)
			{
				ret[--i]=0;
				mvwaddch(w, getcury(w), getcurx(w)-1, ' ');
				wmove(w, getcury(w), getcurx(w)-1);
			}
		}
/*		else
		{
			sprintf(strAux, "%d", c);
			msgBox(strAux, "OK");
		}*/
		wrefresh(w);
	}
	ret[i]=0;
	
	wborder(w, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(w);
	delwin(w);
	return ret;
}
