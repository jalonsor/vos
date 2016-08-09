#include <errno.h>
#include <sys/statvfs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include "regex.h" // Ojo!! es la que viene con el TRE
#include <curses.h>
#include "vosKeyCodes.h"
#include "vos.h"
#include "vosDialog.h"
#include "vosUtils.h"
#include "vosBitacora.h"

#ifndef log(...)
#define log(...) {FILE *plog;if(NULL!=(plog=fopen(fLog,"a"))){fprintf(plog, __VA_ARGS__);fflush(plog);fclose(plog);}}
#endif

char fLog[512]={0};
STATUS stat;
BUFFER buf;
CFG cfg;
VOSBITACORA bra;

unsigned char swFileReadOnly=0;

char pS[2048]={0};
char cadenaBuscada[SEARCH_STRING_SIZE+1]={0};
char cadenaBuscadaTraduc[SEARCH_STRING_SIZE+1]={0};
char aux[255]={0};
int lenCadenaBuscadaTraduc=0;
unsigned long long ultimaPagConincidente=0;
unsigned int offsetInicioSearch=0;
SEARCHRESULT sRet[MAX_SEARCH_RESULT]={{-1},{-1}};
BUFFER bufSearch;

POSITIONING pos;
int nSRet=0;
long nSearchBuffs=0;

char *vosYESNO[2]={"Yes","No"};
char *vosOK[1]={"Ok"};

int row=1, col=0;

char *strHelp="\n Syntax: vos [-l regLength] File\n\tDefault reg length is 80."
"\n More info in man page. Section 1. Type 'man vos' in your terminal command line.\n"
"\n\n ***IMPORTANT NOTE*** Never change your terminal window size while running this editor. File Corruption is sure.\n"
"\n Editor Keys:"
"\n\t ee ->\t\t\tEdit actual record."
"\n\t ii ->\t\t\tInsert new record after."
"\n\t rr ->\t\t\tDuplicate actual record after."
"\n\t dd ->\t\t\tDelete actual record."
"\n\t uu ->\t\t\tUndo last edit operation."
"\n\t CTRL+x \t\tInput HEX character or string when you are in input mode (e or i)."
"\n\n\t l or ->\t\tLeft."
"\n\t h or <-\t\tRight."
"\n\t k or ^\t\t\tUp."
"\n\t j or v\t\t\tDown."
"\n\t Av Pag\t\t\tNext Page."
"\n\t Re Pag\t\t\tPrevious Page."
"\n\t F10\t\t\tPage Scroll Leftt."
"\n\t F11\t\t\tPage Scroll Right."
"\n\t H or Home\t\tInitial column in the record you are."
"\n\t L or End\t\tLast column in the record you are."
"\n\t g\t\t\tGo to the Start of the file."
"\n\t G\t\t\tGo to the End of the file."
"\n\t nnnn[G|g]\t\tGo to the register pointed by number nnnn."
"\n\t x\t\t\tShow current row in Hex. mode."
"\n\t s\t\t\tSearch in Ascii mode. (From the page you are)."
"\n\t S\t\t\tSearch in Hex mode. (From the page you are). Example: 48656c6c6f or 48 65 6c 6c 6f=Hello"
"\n\t R\t\t\tSearch in Regular Expresion mode. (From the page you are)."
"\n\t n\t\t\tRepeat Search forward. (From the page with last match)."
"\n\t N\t\t\tRepeat Search backward. (From the page with last match)."
"\n\t w\t\t\tWrite changes to file. (Save)"
"\n\t Q or q\t\t\tQuit."
"\n Foot Displays:"
"\n\t ROW\t\t\tRow in the screen in wich you are."
"\n\t COL\t\t\tColumn in the screen in wich you are."
"\n\t REG\t\t\tRecord number in the file according to the given record length."
"\n\t COLREG\t\t\tColumn in the file in wich you are."
"\n\t FILE OFFSET\t\tByte position in the file."
"\n\t RO\t\t\tRead Only File."
"\n Minor Notes:"
"\n\t 1. Some special characters like control chars, tabs cr, lf, ff and so on are displayed like '·'";
			
int main(int argc,char **argv)
{
	
    extern void perror(), Exit();
    int i, n, l, c, line = 0, kk=0;

	sprintf(aux, "%c%c%c%c%c%c%c%c%c%c%c%c",COLOREDPLUS);
	
    stat.swMode='V';
    stat.swModifFile=0;
    stat.swModifSrcreen=0;
	stat.percentSaved=-1; //Siempre inicializar a -1. (ver pintaPantalla())
	stat.percentSearch=-1; //Siempre inicializar a -1. (ver pintaPantalla())
	
    cfg.minCol=0; cfg.minRow=1;
    cfg.maxCol=0; cfg.maxRow=0;

	strcat(pS, _VOS_LITERAL_);
	
	pos.nreg=0; pos.colreg=0; pos.offset=0; pos.row=&row; pos.col=&col;

	(void) signal(SIGINT, capturaSignals); //Captura de señales. Ctrl+c
	(void) signal(SIGTSTP, capturaSignals); //Captura de señales. Ctrl+z

	//pruebas(); return 0;

    initCfg(argc, argv);
    // Init Curses.
    keypad(NULL, TRUE);
    initscr(); cbreak(); nonl(); noecho();
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_WHITE);
    init_pair(3, COLOR_RED, COLOR_WHITE);


    idlok(stdscr, TRUE); keypad(stdscr, TRUE);
    cfg.maxCol=COLS-1, cfg.maxRow=LINES -2;

    //Init Color Palete.
    //initColores();

    //Inicializaciones de Buffer y la bitacora de cambios en el fichero
    //	- Siempre despues de las ini de las pantallas.
    log("\n\nESPACIO LIBRE en EL FS: %lld\n\n", getFsFreeSpace(buf.nomFich));
    initBuffer(&buf, "r+");
    //initBuffer(&bufSearch, "rm");	//Buffer de Busqueda. Accesos map en vez de IO calls
	vosBitacoraInit(&bra, buf.nomFich);
	
    //Leer del fichero al buffer - Siempre despues de initBuffer.
    scrollDown();

    pintaPantalla();
	move(0,0);
	pintaPantalla();

	commandsView();

   /* Write out the file */
   /*for (l = 0; l < cfg.maxRow; l++)
   {
       n = len(l);
       for (i = 0; i < n; i++)
           putc(mvinch(l, i) & A_CHARTEXT, buf.pf);
       putc('\n', buf.pf);
   }*/
	
	pruebas();
	
	log("\nBITACORA: Numreg: %ld\n",(bra.nElems));
	vosBitacoraKill(&bra);
	freeBuffer(&buf);
	//freeBuffer(&bufSearch);
	
	endwin();
	return 0;
}

void capturaSignals(int signal)
{
	switch(signal)
	{
		case SIGTSTP:
		case SIGSTOP:
		case SIGINT: //Ctrl+C
			break;
	}
}

void pruebas(void)
{
	char *aux[2048]={0};
	unsigned long long reg=0;
	
	int i=0, j=0;
	
	for(i=0; i< bra.nElems; i++)
	{
		//vosBitacoraGet(VOSBITACORA *pb, char *bufer, long id);
		reg=vosBitacoraGet(&bra, aux, i);
		log("\nBITACORA: Numreg: %ld\n",(bra.arrElems[i])->nReg);
		log("\nBITACORA: OP: %c\n",(bra.arrElems[i])->op);
		//log("\nBITACORA: %s\n",aux);
		for(j=0; j<2048; j++)
			aux[j]=0;
	}
	
}

void initCfg(int argc, char **argv)
{
    //TODO: Falta comprobar rangos y tipos de dato de los argumentos recogidos.
    int i=0;
    cfg.arg0[0]=0; cfg.nonFich[0]=0; cfg.lreg=-1; cfg.copy[0]=0; cfg.fCopy[0]=0;
    strcpy(cfg.arg0, argv[0]);
    strcpy(fLog, getenv("HOME"));
    strcat(fLog, "/vos.log");
    strcat(pS, aux);
    if (argc < 2)
    {
        help();	Exit(1);
    }
    else if(argc == 2)
    {
        strcpy(cfg.nonFich, argv[1]);
    }
    else if(argc == 2 || argc == 4 || argc == 6 )
    {
        int i=0;
		for(i=0; i<argc; i++)
		{
            if(strcmp(argv[i], "-l")==0)
            {
                cfg.lreg=atol(argv[i+1]);
            }
            else if(strcmp(argv[i], "-c")==0)
            {
                strcpy(cfg.copy, argv[i+1]);
            }
        }
        strcpy(cfg.nonFich, argv[argc -1]);
    }
    else
    {
		help();	Exit(1);
    }

	if(cfg.lreg==-1)
	{
		cfg.lreg=80;
		//fprintf(stderr, "\nWARNING! No Reg Length. Default is %d", cfg.lreg);
		fflush(stderr);
	}
}

unsigned char isFileReadOnly(const char *strFileName)
{
	unsigned char swWrite=1; 
	unsigned char swRead=1;
	
	FILE *pF=NULL;
	
	if((pF=fopen(cfg.nonFich, "r+"))==NULL)
		swWrite=0;
	else if((pF=fopen(cfg.nonFich, "r"))==NULL)
		swRead=0;
		
	if(pF!=NULL)
		fclose(pF);
		
	if(swWrite==0 && swRead==0)
	{
		fprintf(stderr, "\nERROR. Can't open file '%s' for Reading or Writing\n", cfg.nonFich);
		Exit(10);
	}
	
	if(swWrite==0)
		return 1;
		
	//1= Read Only. 0=Read y Write.
	return 0;
}

void initBuffer(BUFFER *b, const char *mode)
{
    /*
    -    char nomFich[256];		//Path al fichero
    -    unsigned long long buffLen;	//Longitud del Buffer.
    -    unsigned long long regSize;	//Longitud del registro
    -    unsigned int nPagVert;		//Numero de paginas verticales.
    -    unsigned int nPagHoriz;		//Numero de paginas horizontales.
    -    unsigned int pagVActiva;	//Pagina Vertical Activa.
    -    unsigned int pagHActiva;	//Pagina Horizontal Activa.
    -    unsigned long long fPos;	//Ultimo Car leido del fichero.
    -    unsigned long long pos;		//Posicion del cursor de pantalla respecto albuffer.
        int winLineLength;		//Tama
        FILE *pf;
        char copy[1000];		//Copy del fichero.
    -    char *buff;			//Buffer
    */
    
    b->pf=NULL;
    
    if((swFileReadOnly=isFileReadOnly(cfg.nonFich))==1) //Si es Read Only
    {
		b->pf = fopen(cfg.nonFich, "r");	//Solo para lectura
		if (b->pf == NULL)
		{
			perror(cfg.nonFich);
			printf("\n");
			Exit(2);
		}
	}
	else
	{
		b->pf = fopen(cfg.nonFich, mode);
		if (b->pf == NULL)
		{
			perror(cfg.nonFich);
			printf("\n");
			Exit(2);
		}
	}
    //Inicializacion de la Cache de lectura/Escritura. DESPUES DE ABRIR EL FILE
    //b->ioCache=(char*)malloc(VOSSIZEIOCACHE);
    //setvbuf(b->pf, b->ioCache, _IOFBF, VOSSIZEIOCACHE);
    
	fseek(b->pf, 0, SEEK_END);
	b->nBytesFich=ftell(b->pf);
	rewind(b->pf);

    b->regSize=cfg.lreg;
    strcpy(b->nomFich, cfg.nonFich);

    //Calculo Tamanio del buffer.
    b->buffLen=((b->regSize*cfg.maxRow*1)); //*sizeof(char)
    b->bRealLen=0;

    //Calculo del nº de pags verticales.
    b->nPagVert=b->nBytesFich/b->buffLen;
    if((b->nBytesFich%b->buffLen)>0)
        b->nPagVert+=1;

    log("Nº BYTES FICHERO: %ld\n", b->nBytesFich);
    log("Nº PAGS VERTICALES: %ld\n", b->nPagVert);
    log("Nº PAGS VERTICALES -1: %ld\n", b->nPagVert-1);

    //Calculo del scroll Horizontal.
    b->nPagHoriz=b->regSize/cfg.maxCol;
    if((b->regSize%cfg.maxCol)>0)
        b->nPagHoriz+=1;

    b->pagVActiva=0; b->pagHActiva=0; b->fPos=0;
    b->buff=malloc(b->buffLen * sizeof(char));

    //fprintf(stderr,"\nLREG: %ld\n\n", b->buffLen);fflush(stderr);
}

void freeBuffer(BUFFER *b)
{
	if(b->pf!=NULL)
	{
		fflush(b->pf);
		fclose(b->pf);
		free(b->buff);
	}
	//free(b->ioCache);
}

void replaceBitacoraInBuffer(BUFFER *b, void *pb, unsigned long long nPage, int numRegPerPag)
{
	char *p;
	int i=0;
	unsigned long long startReg=nPage*numRegPerPag;
	
	p=b->buff;
	
	//log("\nBITACORA: numRegPerPag: %d\n", numRegPerPag);
	for(i=0; i<numRegPerPag; i++)
	{
		//log("\nBITACORA: Registro Pantalla: %ld\n", (startReg+i));
		//log("\nBITACORA: Puntero a Buffer:: %p\n", (p+(i*b->regSize)));
		switch(vosBitacoraGetOP(pb, startReg+i))
		{
			case 'D':
				strBinCpy((p+(i*b->regSize)), RECORDDELMSG, b->regSize);
				break;
				
			case 'M':
				vosBitacoraGetReg(pb, (p+(i*b->regSize)), startReg+i);
				break;
				
			case 'A':
				break;
		}
	}
}

int loadPageToBuffer(BUFFER *b, unsigned long long nPage)
{
	//TODO: Controlar y retornar errores.
	if(nPage<0)
		return -1;

	fseek(b->pf, nPage*b->buffLen, SEEK_SET);
	b->bRealLen=fread(b->buff, sizeof(char), b->buffLen, b->pf);
	
	//¿TODO: Sustituir los registros modificados en la Bitacora.?
	if(bra.nElems>0)
		replaceBitacoraInBuffer(b, (VOSBITACORA*)&bra, nPage, cfg.maxRow);
}

int readToBuffer(BUFFER *b, unsigned long long fromPos)
{
	//TODO: Controlar y retornar errores.

	if(fromPos == INICIO)
	{
		fseek(b->pf, 0, SEEK_SET);
		b->bRealLen=fread(b->buff, sizeof(char), b->buffLen, b->pf);
	}
	else if(fromPos == FIN)
	{
		fseek(b->pf, b->buffLen*(b->nPagVert-1), SEEK_SET);
		b->bRealLen=fread(b->buff, sizeof(char), b->buffLen, b->pf);
	}
	else if(fromPos == ADELANTE)
	{
		b->bRealLen=fread(b->buff, sizeof(char), b->buffLen, b->pf);
	}
	else if(fromPos == ATRAS)
	{
		/*if(labs(b->buffLen * sizeof(char) * 2) <= labs(buf.nBytesFich))
		{
			log("----------->FSEEK - ABS RET: %d\n", fseek(b->pf, -((b->buffLen * sizeof(char))*2), SEEK_CUR));
		}
		else
		{
			log("----------->FSEEK - NO ABS RET: %d\n", fseek(b->pf, 0, SEEK_SET));
		}

		log("----------->TAMANIO FICHERO: %ld\n",buf.nBytesFich);
		log("----------->FSEEK - OFFSET: %d\n", -b->buffLen * sizeof(char)*2);
		*/
		b->bRealLen=fread(b->buff, sizeof(char), b->buffLen, b->pf);
	}
	log("Calculados: %ld\n", b->buffLen);
	log("Leidos: %ld\n", b->bRealLen);
}

char *getPointerToReg(BUFFER *b,int fila)
{
	unsigned long long pos=(fila-1)*b->regSize;
	log("\nPOSI PRIMER ELEM BUFF=%ld\n",pos);
	return b->buff+pos;
}

void ddQuery(char *pathFich)
{
	/*
		TODO:	
	*/
}

void initColores(void)
{
	if (has_colors())
	{
		//log("\nInicializo colores ...\n");
		start_color();
		/*
		* Asignación de colores simples, todos los necesarios normalmente.
		*/
		init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);
		init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
		init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
		init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
		init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
		init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
		init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
		init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
	}
}

int len(int lineno)
{
   int linelen = cfg.maxCol;

   while (linelen >= 0 && mvinch(lineno, linelen) == ' ')
       linelen--;
   return linelen + 1;
}

void printRegistro(unsigned long r, unsigned long len)
{
	int i=0, ini=0, fin=0, offset;
	char *aux=buf.buff;
	offset=(r*buf.regSize);
	//log("OFFSET: %d\n", offset);
	aux+=offset;

	ini=buf.pagHActiva*cfg.maxCol;
	fin=ini + cfg.maxCol;
	if(fin>len)
		fin=len;

	//log("INI: %d\n", ini);
	//log("FIN: %d\n", fin);
	//log("PACTIVA: %d\n", buf.pagHActiva);
	//log("MAXPAG: %d\n", buf.nPagHoriz);

	for(i=ini; i<fin; i++)
		printC((chtype)aux[i]);
	addch('\n');
	//color_set(short pair, void *opt);
}

void printRegistroHEX(WINDOW *w, unsigned long r, unsigned long len)
{
	int i=0, ini=0, fin=0, offset, winCol=0, naux=0;
	char xByte[16]={0};
	char *aux=buf.buff;
	offset=(r*buf.regSize);
	//log("\nOFFSET: %d\n", offset);
	aux+=offset;

	ini=buf.pagHActiva*cfg.maxCol;
	fin=ini + cfg.maxCol;
	if(fin>len)
		fin=len;

	//log("INI: %d\n", ini);
	//log("FIN: %d\n", fin);
	//log("PACTIVA: %d\n", buf.pagHActiva);
	//log("MAXPAG: %d\n", buf.nPagHoriz);

	for(i=ini, winCol=0; i<fin; i++, winCol++)
	{
		//log("\nCar: %d", i);
		naux=sprintf(xByte, "%0.2X", aux[i]);
		///log("\nNun Cars Format: %d. Str=%s",naux, xByte);
		//waddch(w, (chtype)'R');
		if(naux>2)
		{
			mvwaddch(w, 1, winCol, (chtype)xByte[naux-2]);
			mvwaddch(w, 2, winCol, (chtype)xByte[naux-1]);
		}
		else
		{
			mvwaddch(w, 1, winCol, (chtype)xByte[0]);
			mvwaddch(w, 2, winCol, (chtype)xByte[1]);
		}
		wrefresh(w);
		//log("\nDespues de waddch ");
		//printC((chtype)aux[i]);
		xByte[0]=0; xByte[1]=0; xByte[2]=0; xByte[3]=0; xByte[4]=0; xByte[5]=0; xByte[6]=0;
		xByte[7]=0; xByte[8]=0; xByte[9]=0; xByte[10]=0; xByte[11]=0; xByte[12]=0; xByte[13]=0;
		xByte[14]=0; xByte[15]=0;
	}
	//color_set(short pair, void *opt);
}

void printBuffer(BUFFER *b)
{
	unsigned int i=0, k=1, nReg=0, resto=0;
	int auxRow=row, auxCol=col;

	nReg=(int)(b->bRealLen/b->regSize);
	resto=(int)(b->bRealLen%b->regSize);
	//log("NREG: %d\n", nReg);
	//log("RESTO: %d\n", nReg);
	//log("RLEN: %d\n",b->regSize);
	
	if(b->pagVActiva==(b->nPagVert-1))	//Si es la ultima pagina se borra toda la pantalla para evitar
		clearScreen();					//restos de ventanas.
	
	move(row=1, col=0); refresh();

	for(i=0; i<nReg;i++)
	{
		printRegistro(i,b->regSize);
	}

	if(resto!=0)
		printRegistro(i,resto);
	
	move(row=auxRow, col=auxCol); refresh();
}

void clearScreen(void)
{
	unsigned int i=0, j=0, nReg=0, resto=0;
	int auxRow=row, auxCol=col;

	nReg=(int)(buf.buffLen/buf.regSize);
	
	move(row=1, col=0); refresh();

	for(i=0; i<nReg;i++)
	{
		for(j=0; j<buf.regSize; j++)
			printC((chtype)' ');
		addch('\n');
	}

	move(row=auxRow, col=auxCol); refresh();
}

void changeBuffer(BUFFER *b, int c, unsigned long long fileOffset)
{
	unsigned long long pantallaOffset=0;
	
	pantallaOffset=fileOffset%b->buffLen;
	b->buff[pantallaOffset]=(char)c;
}

void printC(int c)
{
	if(vosIsPrint(c))
		addch(VOSNOOTPRINTCHAR);
	else
		addch(c);
}

void putC(BUFFER *b, int fila, int columna, int c, unsigned long long fileOffset)
{
	if(c==VOSENTER)
		c=VOSENTERREPLACEMENT;
		
	changeBuffer(b, c, fileOffset-1);
	if(vosIsPrint(c))
		mvaddch(fila, columna, VOSNOOTPRINTCHAR);
	else
		mvaddch(fila, columna, c);
}

void delC(BUFFER *b, int fila, int columna, int c, unsigned long long fileOffset)
{
	if(c==VOSENTER)
		c=VOSENTERREPLACEMENT;
		
	changeBuffer(b, c, fileOffset-2);
	if(vosIsPrint(c))
		mvaddch(fila, columna, VOSNOOTPRINTCHAR);
	else
		mvaddch(fila, columna, c);
}

void showHex(void)
{
	//Backup de la posicion del cursor.
	int c, row_back=row, col_back=col, winY=row+1;
	WINDOW *wHex;

	if(winY>(cfg.maxRow-5))
		winY=row-4;
	wHex=createWin(wHex, 4, cfg.maxCol, winY, 0);
	//log("\nVentana Creada.");

	//color_set(SHOWHEX, NULL);
	//wattron(wHex, COLOR_YELLOW);

	//log("\nComienzo pintado HEX");
	printRegistroHEX(wHex, row-1, buf.regSize);
	/*mvwaddch(wHex, 1, 1, (chtype)'R');
	wrefresh(wHex);
	mvwaddch(wHex, 1, 2, (chtype)'R');
	wrefresh(wHex);
	*/
	//log("\nFin pintado HEX");

    while(1)
    {
		if((c = getch())==ESCAPE_KEY)
		{
			//log("\nESC pulsado");
			break;
		}
	}
	//color_set(NORMAL, NULL);
	//wattroff(wHex, COLOR_YELLOW);

	killWin(wHex);
	//log("\nVentana Matada");

	printBuffer(&buf);
	//log("\nBuffer repintado.");

	//Restore de la posicion del cursor.
	col=col_back; row=row_back;move(row, col);
	//log("Cursor restaurado en %d, %d", row, col);
	pintaPantalla();
	//log("\nPinta Pantalla");
}

char *traduceString(const char *str, char *strRet, int nCars)
{
	char *ret=NULL;

	strcpy(strRet, str);

	return strRet;
}

void search(char mode, char swOption)
{
	//BUFFER bufSearch;
	unsigned long long nReg=0, nPag=0, resto=0, colEnReg=0;
	int regEnPag=0;
	unsigned int c=0, j=0, k=0, strTam=0;
	char strPairHex[3]={0};
	char strLabelDialog[256]=" String Search:";
	
	//SEARCHANY -1
	//SEARCHASC 0
	//SEARCHHEX 1
	//SEARREGEX 2
	//REPESEARNO
	//REPESEARCHFORWARD
	//REPESEARCHBACKWARD

	regex_t preg;
	regmatch_t resulSearch;
	// La he pasado a global. SEARCHRESULT sRes[MAX_SEARCH_RESULT];

	long nBuffers=-1;
	//char cadenaBuscada[SEARCH_STRING_SIZE+1]={0};
	//char cadenaBuscadaTraduc[SEARCH_STRING_SIZE+1]={0};
	char *pSBuff=NULL;
	long nCars=0;
	unsigned long long i=0, nPagIni=0, nPagFin=0;
	int step=1;

	if(mode==SEARREGEX)
	{
		//unsigned int htoi (const char *ptr)
		msgBox("Option not yet available.", "Ok");
		printBuffer(&buf);
		return;
	}
	
	if(swOption==REPESEARNO)	//Busqueda Nueva ... Se inicializan las variables necesarias.
	{
		cadenaBuscada[0]=0;
		cadenaBuscadaTraduc[0]=0;
		ultimaPagConincidente=0;
		offsetInicioSearch=0;
		lenCadenaBuscadaTraduc=0;
		
		if(mode==SEARCHHEX)
			strcpy(strLabelDialog, "HEX search: ");
		else if(mode==SEARCHASC)
			strcpy(strLabelDialog, "ASCII search: ");			
		else if(mode==SEARREGEX)
			strcpy(strLabelDialog, "REGEX search: ");
			
		if(NULL==getStrBox(strLabelDialog, cadenaBuscada, SEARCH_STRING_SIZE))
		{
			msgBox("ERROR. No blank string to search is permited.", "Ok");
			printBuffer(&buf);
			return;
		}
	}

	if(mode==SEARCHHEX)
	{
		//log("\nSEARCHHEX: Modo SEARCHHEX");
		strTam=strlen(cadenaBuscada);
		for(i=0, j=0; i<strTam; i++)
		{
			if(isalpha(cadenaBuscada[i]) || isdigit(cadenaBuscada[i]))
			{
				strPairHex[j++]=cadenaBuscada[i];
				strPairHex[j]=0;// Poner la terminacion de cadena siempre en el siguiente car para pa porsi.
				//log("\nSEARCHHEX: HEX: strPairHex: %s", strPairHex);
			}
			if(j==2 || i==(strTam-1))
			{
				c=htoi(strPairHex);
				cadenaBuscadaTraduc[k++]=(char) c;
				cadenaBuscadaTraduc[k]=0;// Poner la terminacion de cadena siempre en el siguiente car para pa porsi.
				j=0;
				lenCadenaBuscadaTraduc++;
			}
		}
		//log("\nSEARCHHEX: (%s).\n", cadenaBuscadaTraduc);
		//log("\nSEARCHHEX: Len: %d", lenCadenaBuscadaTraduc);
	}

	if(mode==SEARCHASC) //No se usa buscando expresiones regulares.
	{
		//Binary Search: http://www.conestogac.on.ca/~ikolenko/itct/search.html
		//http://laurikari.net/tre/about/
/*
		http://laurikari.net/tre/documentation/regcomp/
		int tre_regcomp(regex_t *preg, const char *regex, int cflags);
		int tre_regncomp(regex_t *preg, const char *regex, size_t len, int cflags);
		int tre_regwcomp(regex_t *preg, const wchar_t *regex, int cflags);
		int tre_regwncomp(regex_t *preg, const wchar_t *regex, size_t len, int cflags);
		void tre_regfree(regex_t *preg);


		http://laurikari.net/tre/documentation/regexec/
		int tre_regexec(const regex_t *preg, const char *string, size_t nmatch,
                regmatch_t pmatch[], int eflags);
		int tre_regnexec(const regex_t *preg, const char *string, size_t len,
						 size_t nmatch, regmatch_t pmatch[], int eflags);
		int tre_regwexec(const regex_t *preg, const wchar_t *string, size_t nmatch,
						 regmatch_t pmatch[], int eflags);
		int tre_regwnexec(const regex_t *preg, const wchar_t *string, size_t len,
						  size_t nmatch, regmatch_t pmatch[], int eflags);
*/
		//log("\nSEARCH: Modo SEARCHASC");
		if(NULL==traduceString(cadenaBuscada, cadenaBuscadaTraduc, SEARCH_STRING_SIZE))
		{
			msgBox("ERROR. Search string could not be translated.", "Ok");
			printBuffer(&buf);
			return;
		}
		lenCadenaBuscadaTraduc=strlen(cadenaBuscadaTraduc);
	}
	
	
	/* *** INICIO DE PROCEDIMIENTO DE BUSQUEDA COMUN PARA TODOS LOS MODOS. *** */
	if(tre_regncomp(&preg, cadenaBuscadaTraduc, lenCadenaBuscadaTraduc, REG_LITERAL | REG_ICASE)!=0)
	{
		msgBox("ERROR. Invalid chars in search string.", "Ok");
		printBuffer(&buf);
		return;
	}
	initBuffer(&bufSearch, "rm");	//accesos map en vez de IO calls
	
	step=1;
	//nPagIni=0;
	nPagFin=buf.nPagVert;
	
	if(swOption==REPESEARCHFORWARD)
	{
		log("\nSEARCH: REPESEARCHFORWARD");
		i=ultimaPagConincidente;
	}
	if(swOption==REPESEARCHBACKWARD)
	{
		log("\nSEARCH: REPESEARCHBACKWARD");
		i=ultimaPagConincidente;
	}
	else if(swOption==REPESEARNO)
	{
		log("\nSEARCH: REPESEARNO");
		i=buf.pagVActiva;
	}

	//log("\nSEARCH: Inicio i=%ld", i);
	//for(; i<buf.nPagVert; i++)
	for(; i<buf.nPagVert; i+=step)
	{
		//log("\nSEARCH: for i: %ld", i);
		loadPageToBuffer(&bufSearch, i);
		if(bufSearch.bRealLen>0)
		{
			//log("\nSEARCH: Dentro if(bufSearch.bRealLen>0)");
			/*
			 * resulSearch.regoff_t rm_so --> Offset from start of string to start of substring.
			 * resulSearch.regoff_t rm_eo --> Offset from start of string to the first character
			 * 								  after the substring.
			 */
			if(tre_regnexec(&preg, bufSearch.buff+offsetInicioSearch, bufSearch.bRealLen-offsetInicioSearch, 
				1, &resulSearch, REG_NOTBOL|REG_NOTEOL)==0) //Si hay coincidencia ...
			{
				log("\nSEARCH: ENCONTRADO EN %ld\n", resulSearch.rm_so);
				sRet[nSRet].fOffset=resulSearch.rm_so;
				sRet[nSRet].length=lenCadenaBuscadaTraduc;
				
				loadPageToBuffer(&buf, i);
				clear();
				printBuffer(&buf);
				pintaPantalla();
				log ("\nSEARCH: ---->REG: SEARCH OFFSET: %d\n", (sRet[nSRet].fOffset + offsetInicioSearch));
				regEnPag=(sRet[nSRet].fOffset + offsetInicioSearch) / buf.regSize;
				log("\nSEARCH: regEnPag %d", regEnPag);
				colEnReg=(sRet[nSRet].fOffset + offsetInicioSearch) % buf.regSize;
				log("\nSEARCH: colEnReg %ld", colEnReg);
				
				cursorToRegCol(regEnPag, colEnReg);
				resalta(regEnPag, 1, 10);
				
				buf.pagVActiva=i;
				offsetInicioSearch+=(sRet[nSRet].fOffset + lenCadenaBuscadaTraduc);
				ultimaPagConincidente=buf.pagVActiva;
				
				break;
			}
		}
		offsetInicioSearch=0;	//Inicio de Offset cuando se avanza Pagina.
	}
	
	if(i==buf.nPagVert)
		printBuffer(&buf);
	free(bufSearch.buff);
	tre_regfree(&preg);
}

char compruebaScreen(void)
{
	if((cfg.maxCol!=COLS-1) || (cfg.maxRow!=LINES -2))
		return 1;
	if(stat.swModifSrcreen!=0)
		return 1;
	return 0;
}

void commandsView(void)
{
	char goNReg[1000]={0};
	int cntGoNReg=0;
    int c, swSalir=0;
    unsigned long long nPag=0, resto=0, rowAux=0; 
    char swEE=0, swII=0, swRR=0, swDD=0, swUU=0;

    while(swSalir==0)
    {
        move(row, col);
        pintaPantalla();
        if((stat.swModifSrcreen=compruebaScreen())!=0)
        {
			msgBox("ATENTION!. Your window size has been modified. File corruption in edition mode is sure. You could not save the modified file. Restart VOS and do not change your terminal window size.","OK");
			printBuffer(&buf);
		}
        c = getch();

		switch (c)
        {
			case KEY_NPAGE:	//AV Pag
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swEE=0, swII=0, swRR=0, swDD=0, swUU=0;
			    //log("-Nº pagVActiva: %ld\n", buf.pagVActiva);
			    //log("-Nº PAGS VERTICALES: %ld\n", buf.nPagVert);
				//log("-Nº PAGS VERTICALES -1: %d\n", buf.nPagVert-1);

				if(buf.pagVActiva<buf.nPagVert-1)
				{
					buf.pagVActiva++;
					scrollDown();

				}
				else
				{
					flash(); beep();
				}
				break;

			case KEY_PPAGE:	//Re Pag
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swEE=0, swII=0, swRR=0, swDD=0, swUU=0;
				if(buf.pagVActiva>0)
				{
					buf.pagVActiva--;
					scrollUp();
				}
				else
				{
					flash(); beep();
				}
				break;

			case KEY_F(11):	//Scroll Right
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swEE=0, swII=0, swRR=0, swDD=0, swUU=0;
				if(buf.pagHActiva<buf.nPagHoriz-1)
				{
					buf.pagHActiva++;
					scrollRight();
				}
				else
				{
					flash(); beep();
				}

				break;

			case KEY_F(10):	//Scroll Left
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swEE=0, swII=0, swRR=0, swDD=0, swUU=0;
				if(buf.pagHActiva>0)
				{
					buf.pagHActiva--;
					scrollLeft();
				}
				else
				{
					flash(); beep();
				}
				break;

			case KEY_HOME:	//Inicio
			case KEY_BEG:
			case 'H':
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swEE=0, swII=0, swRR=0, swDD=0, swUU=0;
				//col=0;
				buf.pagHActiva=0;
				rowAux=row;
				scrollLeft();
				move(row=rowAux, col = 0);
				break;

			case KEY_END:	//Fin
			case 'L':
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swEE=0, swII=0, swRR=0, swDD=0, swUU=0;
				//nCol=0, resto=0
				nPag=buf.regSize / cfg.maxCol;
				resto=buf.regSize % cfg.maxCol;
				buf.pagHActiva=nPag;
				rowAux=row;
				scrollRight();
				//move(row, col = cfg.maxCol-1);
				move(row=rowAux, col = resto-1);
				break;

			case 'h':
			case KEY_LEFT:
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swEE=0, swII=0, swRR=0, swDD=0, swUU=0;
				if (col > cfg.minCol)
					col--;
				else
				{
					flash(); beep();
				}
				break;

		   case 'j':
		   case KEY_DOWN:
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swEE=0, swII=0, swRR=0, swDD=0, swUU=0;
				if (row < cfg.maxRow)
					row++;
				else
				{
					flash();
					beep();
				}
				break;

		   case 'k':
		   case KEY_UP:
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swEE=0, swII=0, swRR=0, swDD=0, swUU=0;
				if (row > cfg.minRow)
					row--;
				else
				{
					flash();
					beep();
				}
				break;

			case 'l':
			case KEY_RIGHT:
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swEE=0, swII=0, swRR=0, swDD=0, swUU=0;
				if (col < cfg.maxCol && pos.colreg < cfg.lreg)
				{
					col++;
				}
				else
				{
					flash();
					beep();
				}
			   break;

			case 'g':	// Principio de Fichero
				swEE=0, swII=0, swRR=0, swDD=0, swUU=0;
				if(cntGoNReg>0)
				{
					goNReg[cntGoNReg]=0;
					goReg(goNReg);
					cntGoNReg=0;
					break;
				}
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				buf.pagVActiva=0;
				goIni();
				break;

			case 'G':	// Fin de fichero
				swEE=0, swII=0, swRR=0, swDD=0, swUU=0;
				if(cntGoNReg>0)
				{
					goNReg[cntGoNReg]=0;
					goReg(goNReg);
					cntGoNReg=0;
					break;
				}
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				buf.pagVActiva=buf.nPagVert-1;
				goEnd();
				break;

			case 'x':
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				showHex();
				break;

			case 's':
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				search(SEARCHASC, REPESEARNO);
				break;

			case 'S':
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swEE=0, swII=0, swRR=0, swDD=0, swUU=0;
				search(SEARCHHEX, REPESEARNO);
				break;

			case 'R':
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swEE=0, swII=0, swRR=0, swDD=0, swUU=0;
				search(SEARREGEX, REPESEARNO);
				break;

			case 'n':	//Repetir busqueda atrás
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swEE=0, swII=0, swRR=0, swDD=0, swUU=0;
				search(SEARCHANY, REPESEARCHFORWARD);
				break;

			case 'N':	//Repetir busqueda adelante
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swEE=0, swII=0, swRR=0, swDD=0, swUU=0;
				msgBox("Option not yet available.", "Ok");
				printBuffer(&buf);
				//search(SEARCHANY, REPESEARCHBACKWARD);
				break;

			/* ^L: redraw screen */
			case KEY_CLEAR:
			case CTRL('L'):
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swEE=0, swII=0, swRR=0, swDD=0, swUU=0;
				wrefresh(curscr);
				break;

		   case 'M':
		   case 'm':
				swEE=0, swII=0, swRR=0, swDD=0, swUU=0;
				wHelp();
				printBuffer(&buf);
				break;

		   /* o: open up a new line and enter input mode */
//			case KEY_IL:
//			case 'o':
//				move(++row, col = 0);
//				insertln();
//				input();
//				break;

/* i: Insertar Registro a continuacion*/
			case KEY_IC:
				swII++;
			case 'i':
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swEE=0, swRR=0, swDD=0, swUU=0;
				swII++;
				if(swII==2)
				{
					swII=0;
					commandsInsertRecord();
				}
				break;				

/* r: Duplicar registro */
			case 'r':
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swEE=0, swII=0, swDD=0, swUU=0;
				swRR++;
				if(swRR==2)
				{
					swRR=0;
					commandsDuplicateRecord();
				}
				break;				

/* e: Editar Registro */
			case 'e':
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swII=0, swRR=0, swDD=0, swUU=0;
				swEE++;
				if(swEE==2)
				{
					swEE=0;
					commandsEdit();
				}
				break;				

/* d: Borrar Registro */
			case 'd':
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swEE=0, swII=0, swRR=0, swUU=0;
				swDD++;
				if(swDD==2)
				{
					swDD=0;
					commandsDeleteRecord();
					
				}
				break;				

/* u: UNDO */
			case 'u':
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swEE=0, swII=0, swRR=0, swDD=0;
				swUU++;
				if(swUU==2)
				{
					swUU=0;
					if(bra.nElems>0)
					{
						if(swFileReadOnly)
						{
							msgBox("Read Only File", "OK");
							printBuffer(&buf);
						}
						//log("\nUNDO\n");
						loadPageToBuffer(&buf, vosBitacoraOPUndoLast(&bra));
						printBuffer(&buf);
					}
				}
				break;				

/* W: Save As */
			case 'W':
				break;
/* w: Save */
			case 'w':
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swEE=0, swII=0, swRR=0, swDD=0, swUU=0;
				if((stat.swModifSrcreen=compruebaScreen())==0)
				{
					if(stat.swModifFile==1 && bra.nElems>0)
					{
						if(msgConfirm("Do you really want to save the file?", vosYESNO, 2)=='n')
						{
							printBuffer(&buf);
							break;
						}
						printBuffer(&buf);
						switch(saveFile(NULL))
						{
							case 0:
								msgBox("File Saved", "Ok");
								stat.percentSaved=-1;
								printBuffer(&buf);
								pintaPantalla();
								stat.swModifFile=0;
								break;
							
							case -1:
								msgBox("ERROR. Not enough space left to save the file by creating a TMP file.", "Ok");
								printBuffer(&buf);
								break;
								
							case -2:
								msgBox("ERROR. Can not query File System properties.", "Ok");
								printBuffer(&buf);
								break;
								
							case -5:
								msgBox("ERROR. Can not write in the file.", "Ok");
								printBuffer(&buf);
								break;
															
							case -6:
								msgBox("ERROR. Can not create TMP file.", "Ok");
								printBuffer(&buf);
								break;
															
							case -7:
								msgBox("ERROR. Can not write in TMP file.", "Ok");
								printBuffer(&buf);
								break;
															
							default:
								msgBox("ERROR. Unknown error saving file.", "Ok");
								printBuffer(&buf);
								break;
						}
					}
				}
				else
				{
					msgBox("YOU CAN NOT SAVE THE FILE. Your window size has been modified. File corruption in edition mode is sure. Restart VOS and do not change your terminal window size.","OK");
					printBuffer(&buf);
				}
				pintaPantalla();
				break;

/* q: Quit */
		   case 'Q':
		   case 'q':
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swEE=0, swII=0, swRR=0, swDD=0, swUU=0;
				//if(msgConfirm("There is some modified content. Do you really want to quit without save?", vosYESNO, 2)=='y')
				if(stat.swModifFile==1 && bra.nElems>0)
				{
					if(msgConfirm("There are unsaved data. Do you really want to quit without save?", vosYESNO, 2)=='y')
					{
						swSalir=1;
						break;
					}
				}
				else
				{
					swSalir=1;
					break;
				}

				printBuffer(&buf);
				break;

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				swEE=0, swII=0, swRR=0, swDD=0, swUU=0;
				goNReg[cntGoNReg++]=(char) c;
				goNReg[cntGoNReg]=0;// Poner la terminacion de cadena siempre en el siguiente car para pa porsi.
				//log("\n\n %c %s\n", c, goNReg);
				break;

			default:
				goNReg[0]=0; cntGoNReg=0;//Reinicio el Nº de linea.
				swEE=0, swII=0, swRR=0, swDD=0, swUU=0;

				//log("%c",c);
				flash();
				beep();
				break;
       }
   }
}

void resalta(int reg, int colIni, int colFin)
{
/*
	WA_STANDOUT    characters are displayed in standout mode
	WA_UNDERLINE   characters are displayed underlined
	WA_REVERSE     characters are displayed in inverse video
	WA_BLINK       characters blink
	WA_DIM         characters are displayed at a lower intensity
	WA_BOLD        characters are displayed at a higher intensity
	WA_INVIS       characters are added invisibly
	WA_PROTECT     characters are protected from modification
	WA_ALTCHARSET  characters are displayed using the alternate charac-
					ter set (ACS)
	WA_LOW         characters are displayed with low highlight
	WA_TOP         characters are displayed with top highlight
	WA_HORIZONTAL  characters are displayed with horizontal highlight
	WA_VERTICAL    characters are displayed with vertical highlight
	WA_LEFT        characters are displayed with left highlight
	WA_RIGHT       characters are displayed with right highlight
*/

	//log("\n--------------------->REG: %d\n", reg);
	mvchgat(reg+1, 0, -1, WA_STANDOUT, 1, NULL);
	refresh();
}

void cursorToReg(unsigned long long nReg)
{
	row=nReg;
	move(row, col);
}

void cursorToRegCol(unsigned long long nReg, unsigned long long nCol)
{
	int pagHoriz=0, resto=0;
	//log("\nSEARCH: Maxcol: %d", cfg.maxCol);
	pagHoriz=nCol/cfg.maxCol;
	resto=nCol % cfg.maxCol;
	
	buf.pagHActiva=pagHoriz;
	scrollRight();
	
	row=nReg+1;
	col=resto;
	move(row, col);
}

void goReg(char *strNReg)
{
	unsigned long long nReg=0, nPag=0, resto=0, regEnPag=0;
	//log("\ngoReg: %s \n", strNReg);
	nReg=atoll(strNReg);
	//log("\ngoReg Num: %lld \n", nReg);
	nPag=(unsigned long long)(buf.regSize*nReg/buf.buffLen);
	//log("\nPag del registro: %lld \n", nPag);
	resto=(unsigned long long)(buf.regSize*nReg)%buf.buffLen;
	//log("\nResto de la Pag: %lld \n", resto);
	regEnPag=(unsigned long long)resto/buf.regSize;
	//log("\nRegistro en la Pag: %lld \n", regEnPag);

	if(nPag>buf.nPagVert)
	{
		msgBox("ERROR. The file has less records than you typed.", "OK");
		printBuffer(&buf);
		return;
	}

	buf.pagVActiva=nPag;
	loadPageToBuffer(&buf, buf.pagVActiva);
	clear();
	printBuffer(&buf);
	cursorToReg(regEnPag);
}

void goIni(void)
{
	loadPageToBuffer(&buf, buf.pagVActiva);
	clear();
	printBuffer(&buf);

}

void goEnd(void)
{
	loadPageToBuffer(&buf, buf.pagVActiva);
	clear();
	printBuffer(&buf);
}

void scrollUp(void)
{
	loadPageToBuffer(&buf, buf.pagVActiva);
	clear();
	printBuffer(&buf);
}

void scrollDown(void)
{
	loadPageToBuffer(&buf, buf.pagVActiva);
	clear();
	printBuffer(&buf);
}

void scrollLeft(void)
{
	clear();
	printBuffer(&buf);
}

void scrollRight(void)
{
	clear();
	printBuffer(&buf);
}

void calculaPOSITIONING(void)
{
    pos.nreg=(unsigned long)(buf.pagVActiva*(buf.buffLen/buf.regSize)) + *(pos.row);
    pos.offset=((pos.nreg-1) * buf.regSize) + *(pos.col) + (buf.pagHActiva*cfg.maxCol)+1;
    pos.colreg=pos.offset - ((pos.nreg - 1) * buf.regSize );
}

void pintaPantalla(void)
{
    int auxTama=0, auxTama1=0;
    char str[2048]={0};
    char str0[255]={0};
    char str1[255]={0};
    char str2[255]={0};
    char str3[269]={0};
    char percentSave[31]={0};

	if(stat.swMode=='E')
	{
		mvaddstr(0, 0, STRHEADEDIT);	//E
		mvaddstr(0, 55, _VOS_VERISON_);
		mvchgat(0, 0, 12, WA_STANDOUT, 2, NULL);
		mvchgat(0, 12, -1, WA_STANDOUT, 0, NULL);
	}
	else
	{
		mvaddstr(0, 0, STRHEADVIEW);	//V
		mvaddstr(0, 55, _VOS_VERISON_);
		mvchgat(0, 0, -1, WA_STANDOUT, 0, NULL);
	}	

    calculaPOSITIONING();

	sprintf(str, " PAGE: %ld/%ld, %d/%d. ", (buf.pagVActiva+1), buf.nPagVert, (buf.pagHActiva+1), buf.nPagHoriz);
	sprintf(str0, " ROW: %d. COL: %d. REG: %ld ", *(pos.row), (*(pos.col)+1), pos.nreg);
	sprintf(str1, "COLREG: %ld ", pos.colreg);
	sprintf(str2, "FILE OFFSET: %ld  ", pos.offset);
	if(swFileReadOnly==1)
		sprintf(str2, "[RO] ");
	sprintf(str3, "[%s]", cfg.nonFich);

	strcat(str, str0);
	strcat(str, str1);
	strcat(str, str2);
	auxTama=strlen(str);	//Para saber donde poner en rojo el nombre de fichero.
	auxTama1=strlen(str3);	//Para saber donde poner en rojo el nombre de fichero.
	strcat(str, str3);
	if(stat.percentSaved!=-1)
    {
		sprintf(percentSave, "  [Saved: %.2f%%]", stat.percentSaved);
		strcat(str, percentSave);
	}

    mvaddstr(LINES - 1, 0, str);
    mvchgat(LINES - 1, 0, -1, WA_STANDOUT, 0, NULL);
    
    if(stat.swModifFile==1 && bra.nElems>0) //Si el fichero no esta salvado ...
	{
		mvchgat(LINES - 1, auxTama-1, auxTama1+2, WA_STANDOUT, 3, NULL);
	}
    

    
    move(row, col);
    refresh();
}

long calculaPagVActiva(unsigned long long offset)
{
	buf.pagVActiva=offset/buf.buffLen;
	//if((buf.pagVActiva%buf.buffLen)>0)
    //    buf.pagVActiva+=1;
	//log("----------------> buf.pagVActiva: %ld. buf.nPagVert: %ld\n", buf.pagVActiva, buf.nPagVert);
	//log("----------------> buf.len: %ld.\n", buf.buffLen);
	return buf.pagVActiva;
}

/*
* Insert mode: accept characters and insert them.
*  End with ^D or EIC
*/
void commandsEdit(void)
{
	int c, iHex=0, swRegistroEditado=0;
	char strHexImput[IMPUTHEX_STRING_SIZE+1]={0};
	char strAsciiImput[IMPUTHEX_STRING_SIZE+1]={0};
	int asciiLen=0;
	unsigned long long nRegEdit=0;
	
	if(swFileReadOnly)
	{
		msgBox("Read Only File", "OK");
		printBuffer(&buf);
		return;
	}
	
	stat.swMode='E';
	//standout();
	/*mvaddstr(LINES - 1, COLS - 20, "INPUT MODE");*/
	//mvaddstr(LINES - 1, COLS - 12, " Input ON ");
	//standend();
	move(row, col);
	pintaPantalla();
	
	nRegEdit=pos.nreg-1;
	
	while(1)
    {
		move(row, col);
		refresh();
		pintaPantalla();
		
		if((c = getch())==ESCAPE_KEY)
			break;
		
		log("\nTyped Char: %d\n",c);
		
		switch (c)
        {
			case KEY_NPAGE:
			case KEY_PPAGE:
			case KEY_HOME:
			case KEY_BEG:
			case KEY_END:
			case KEY_CLEAR:
			case VOSKEY_INS:
			case KEY_DOWN:
			case KEY_UP:
				flash(); beep();
				break;
				
				//msgBox("SUP", "OK");
			case VOSKEY_SUP:
				break;

			case KEY_BACKSPACE:
				if(col > cfg.minCol)
				{
					swRegistroEditado=1;
					c=(int)' ';
					delC(&buf, row, --col, c, pos.offset);
				}
				break;

			case KEY_LEFT:
				if (col > cfg.minCol)
					col--;
				else
				{
					flash(); beep();
				}
				break;

			case KEY_RIGHT:
				if (col < cfg.maxCol && pos.colreg < cfg.lreg)
				{
					col++;
				}
				else
				{
					flash();
					beep();
				}
				break;

			case CTRL('x'):	//Show HEX input mode.
				if(NULL==getStrBox("HBX Input:", strHexImput, IMPUTHEX_STRING_SIZE))
					break;

				printBuffer(&buf);
				
				asciiLen=getAsciiFromHEX(strHexImput, strAsciiImput);
				for(iHex=0; iHex<asciiLen; iHex++)
				{
					//swRegistroEditado=1;
					move(row, col);
					refresh();
					pintaPantalla();
					
					c=strAsciiImput[iHex];
					if (col <= cfg.maxCol && pos.colreg <=cfg.lreg)
					{
						swRegistroEditado=1;
						putC(&buf, row, col++, c, pos.offset);
					}
					else
					{
						flash();
						beep();
					}
					
					
					//if(!isprint(c) || iscntrl(c) || c=='\f' || c=='\n' || c=='\r' || c=='\t' || c=='\v' )
					//	mvaddch(row, col++, '·');
					//else
					//	mvaddch(row, col++, c);
				}	
				
				break;
				
			default:
				if (col <= cfg.maxCol && pos.colreg <=cfg.lreg)
				{
					swRegistroEditado=1;
					putC(&buf, row, col++, c, pos.offset);
				}
				else
				{
					flash();
					beep();
				}
				
				//if(isprint(c))	//Recoger la cadena.
				//{
				//	swRegistroEditado=1;
				//	putC(&buf, row, col++, c, pos.offset);
				//}
				//else
				//{
				//	flash();
				//	beep();
				//}
		}
	}
	
	if(swRegistroEditado==1)
	{
		stat.swModifFile=1;
		vosBitacoraOPModif(&bra, getPointerToReg(&buf,row), buf.regSize, nRegEdit, buf.pagVActiva);
	}
		
	stat.swMode='V';
	move(LINES - 1, COLS - 20);
	//clrtoeol();
	move(row, col);
	pintaPantalla();
}

void commandsInsertRecord(void)
{
	if(swFileReadOnly)
	{
		msgBox("Read Only File", "OK");
		printBuffer(&buf);
		return;
	}
	msgBox("Option not yet available.", "OK");
	printBuffer(&buf);
}

void commandsDuplicateRecord(void)
{
	if(swFileReadOnly)
	{
		msgBox("Read Only File", "OK");
		printBuffer(&buf);
		return;
	}

	msgBox("Option not yet available.", "OK");
	printBuffer(&buf);
}

void commandsDeleteRecord(void)
{
	if(swFileReadOnly)
	{
		msgBox("Read Only File", "OK");
		printBuffer(&buf);
		return;
	}
	
	log("\nMarcado para Borrar registro: %ld\n", pos.nreg-1);
	stat.swModifFile=1;
	vosBitacoraOPDel(&bra, getPointerToReg(&buf,row), buf.regSize, pos.nreg-1, buf.pagVActiva);
	strBinCpy(getPointerToReg(&buf,row), RECORDDELMSG, buf.regSize);
	printBuffer(&buf);
}

WINDOW *createWin(WINDOW *w, int height, int width, int starty, int startx)
{
	w = newwin(height, width, starty, startx);
	box(w, 0 , 0);		// 0, 0 gives default characters for the vertical and horizontal lines
	wrefresh(w);		// Show that box

	return w;
}

void killWin(WINDOW *w)
{
	wborder(w, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(w);
	delwin(w);
}

void wHelp(void)
{
	int c=-1;
	WINDOW *w;
	
	w = newwin(LINES -8 , COLS -10 , 1, 5);
	wmove(w, 1, 1);
	wrefresh(w);


	waddstr(w, pS);
	waddstr(w, _VOS_VERISON_);
	waddstr(w, strHelp);
	box(w, 0 , 0);
	wrefresh(w);

	while(1)
    {
		if((c = tolower(getch()))==ESCAPE_KEY || c==KEY_ENTER || c=='\r' || c=='\n')
		{
			break;
		}
		else
			flash();
	}

	wborder(w, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(w);
	delwin(w);
}

void help(void)
{
    fprintf(stderr, pS);
    fprintf(stderr, _VOS_VERISON_);
    fprintf(stderr, "%s\n",strHelp);    
}

//		struct statvfs {
//           unsigned long  f_bsize;    /* file system block size */
//           unsigned long  f_frsize;   /* fragment size */
//           fsblkcnt_t     f_blocks;   /* size of fs in f_frsize units */
//           fsblkcnt_t     f_bfree;    /* # free blocks */
//           fsblkcnt_t     f_bavail;   /* # free blocks for non-root */
//           fsfilcnt_t     f_files;    /* # inodes */
//           fsfilcnt_t     f_ffree;    /* # free inodes */
//           fsfilcnt_t     f_favail;   /* # free inodes for non-root */
//           unsigned long  f_fsid;     /* file system ID */
//           unsigned long  f_flag;     /* mount flags */
//           unsigned long  f_namemax;  /* maximum filename length */
//         };

int getFsCanWrite(const char *fich)
{
	return 0;
}

long long getFsFreeSpace(const char *fich)
{
	int codErr=0;
	struct statvfs info;
	long long ret=0;
	char bDir[2048]={0};
	
	log("\n\nFS fichero getFsFreeSpace: %s\n\n", fich);
	
	//man statvfs
	baseDir(fich, bDir);
	ret=statvfs(bDir, &info);
	log("\n\nFS PATH getFsFreeSpace: %s\n\n", bDir);
	if(ret!=0)
	{
		codErr=errno;
		log("\nERROR getFsFreeSpace: Cod. %d, MSG: %s\n",codErr, strerror(codErr));
		
		return -1;
	}
	
	log("\n\nFS TAMANIO BLOQUE getFsFreeSpace: %ld\n\n", info.f_bsize);
	log("\n\nFS BLOQUES LIBRES getFsFreeSpace: %ld\n\n", info.f_bavail);
	
	return (long long)((long long)info.f_bsize*(long long)info.f_bavail);
}

int vosCopy(char *pBuf, unsigned long long bufLen, FILE *pIn, FILE *pOut, unsigned long long regIni, long long regEnd)
{
	long long i, nBloques=0, resto=0, nBytesWrite=0, ret=0;
	int percent=0;
	
	
	if(regEnd<0) //Si es negativo se copia el resto del fichero.
	{
		nBytesWrite=buf.nBytesFich-(regIni*buf.regSize);
	}
	else
	{
		nBytesWrite=(regEnd-regIni)*buf.regSize;
	}
	
	if(nBytesWrite>0)
	{
		log("\nVOSCOPY BuffLen: %ld \n",bufLen);
		log("\nVOSCOPY nBytesWrite: %ld\n",nBytesWrite);

		nBloques=(nBytesWrite/bufLen);
		resto=(unsigned long long)(nBytesWrite%bufLen);
		
		log("\nVOSCOPY RegIni: %ld \n",regIni);
		log("\nVOSCOPY RegFin: %ld\n",regEnd);
		
		log("\nVOSCOPY Bloques: %ld \n",nBloques);
		log("\nVOSCOPY Resto: %ld\n",resto);
		
		fseek(pIn,regIni*buf.regSize, SEEK_SET);
	
		log("\nVOSCOPY Porcentaje Salvado: %.2f\n", (stat.percentSaved=getPorcent(buf.nBytesFich, ftell(pIn))));
		pintaPantalla();
		for(i=0; i<nBloques; i++)
		{
			log("\nVOSCOPY BLOQUE: %ld\n",i);
			fread(pBuf,bufLen,1,pIn);
			ret=fwrite(pBuf,bufLen,1,pOut);
			if(ret!=1)
			{
				log("\nVOSCOPY ERROR. No se ha podico escribir el bloque %ld en el fichero TMP.\n", i);
				return -1;
			}
			log("\nVOSCOPY Porcentaje Salvado: %.2f\n", (stat.percentSaved=getPorcent(buf.nBytesFich, ftell(pIn))));
			pintaPantalla();
		}
		
		if(resto>0)
		{
			fread(pBuf,resto,1,pIn);
			ret=fwrite(pBuf,resto,1,pOut);
			if(ret!=1)
			{
				log("\nVOSCOPY ERROR. No se ha podico escribir el RESTO en el fichero TMP.\n", i);
				return -1;
			}
			log("\nVOSCOPY Porcentaje Salvado: %.2f\n", (stat.percentSaved=getPorcent(buf.nBytesFich, ftell(pIn))));
			pintaPantalla();
		}
	}
	
	return 0;
}

int saveFile(const char *strNewName)
{
	FILE *pOut;
	char nomFichTMP[512]={0};
	char nomFichBACK[512]={0};
	char *pBufWrite=NULL;
	long long ret=0;
	char op=0;
	unsigned long long fileOffset=0, startReg=0;
	long i, pagVActivaAUX;
	VOSBITACORAELEM *pEl;
	
	if(swFileReadOnly)
	{
		msgBox("Read Only File", "OK");
		printBuffer(&buf);
		return 0;
	}
	
	pagVActivaAUX=buf.pagVActiva;
	
	strcat(nomFichBACK,buf.nomFich);
	strcat(nomFichBACK,".vosSAVED.back");
	strcat(nomFichTMP,buf.nomFich);
	strcat(nomFichTMP,".vosSAVED.tmp");
		
	if(vosBitacoraIsChangeLength(&bra)!=0) //Si cambia el tamanio
	{
		if((ret=getFsFreeSpace(nomFichTMP))<(buf.nBytesFich*2)) //si no hay el doble del tamanio del fichero
			return -1;											//para salvar el TMP ...
		
		if(ret<0)
			return -2;
		
		ret=0;
	}
	
	for(i=0; i<bra.nElems; i++)
	{
		if((bra.arrElems[i])->op=='M')
		{
			pEl=bra.arrElems[i];
			fileOffset=pEl->nReg*buf.regSize;
				
			fseek(buf.pf, fileOffset, SEEK_SET);
			if(fwrite((char*)pEl->bufDat, pEl->buffLen,1,buf.pf)!=1)
				return -5;
		}
	}

	if(vosBitacoraIsChangeLength(&bra)!=0) //Si hay cambio de tamanio del fichero ...
	{
		for(i=0; i<bra.nElems; i++)
			log("\nvosBitacora Antes Sort %ld\n",(bra.arrElems[i])->nReg);
			
		vosBitacoraSortByRegNumber(&bra);
		
		for(i=0; i<bra.nElems; i++)
			log("\nvosBitacora Despues Sort %ld\n",(bra.arrElems[i])->nReg);
		
		
		
		if((pOut=fopen(nomFichTMP, "w"))!=NULL)
		{
			pBufWrite=(char*)malloc(VOSSIZEBUFWRITE);
			for(i=0; i<bra.nElems; i++)
			{
				op=(bra.arrElems[i])->op;
				if(op!='M')
				{
					log("\nVOSCOPY INI: %ld\n", startReg);
					log("\nVOSCOPY FIN: %ld\n", (bra.arrElems[i])->nReg);
					ret=vosCopy(pBufWrite, VOSSIZEBUFWRITE, buf.pf, pOut, startReg, (bra.arrElems[i])->nReg);
					if(ret!=0)
						return -7;
						
					if(op=='A')
					{
						ret=fwrite((char*)(bra.arrElems[i])->bufDat, (bra.arrElems[i])->buffLen, 1, pOut);
						if(ret!=0)
							return -7;
					}
					
					startReg=(bra.arrElems[i])->nReg+1;
				}
			}
			ret=vosCopy(pBufWrite, VOSSIZEBUFWRITE, buf.pf, pOut, startReg, -1); //Copiar hasta el final.
			if(ret!=0)
				return -7;			
				
			free(pBufWrite);
			fflush(pOut);
			fclose(pOut);
		}
		else
			return -6;
			
	}
	
	if(ret==0)
	{
		vosBitacoraFreeAll(&bra);
		if(op!=0)
		{
			freeBuffer(&buf);
			rename(buf.nomFich, nomFichBACK);
			rename(nomFichTMP, buf.nomFich);
			remove(nomFichBACK);
			initBuffer(&buf, "r+");
			buf.pagVActiva=pagVActivaAUX;
			scrollDown();
			pintaPantalla();	
			stat.percentSaved=-1;
		}
		
	}
	
	return (int)ret;
}

void Exit(int ret)
{
	vosBitacoraKill(&bra);
	freeBuffer(&buf);
	//freeBuffer(&bufSearch);
	
	endwin();
	printf("\n");
	exit(ret);
}


