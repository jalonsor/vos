#include <stdio.h>

#ifndef _VOS_VERISON_
#define _VOS_VERISON_ " V. 0.9.9 Alpha "
#endif

#ifndef _VOS_LITERAL_
#define _VOS_LITERAL_ "Vi like for ZOS reg files editor for linux. "
#endif

#ifndef INICIO
#define INICIO 0
#endif

#ifndef ATRAS
#define ATRAS -1
#endif

#ifndef ADELANTE
#define ADELANTE 1
#endif

#ifndef FIN
#define FIN 2
#endif

#ifndef REPESEARNO
#define REPESEARNO 0
#endif

#ifndef REPESEARCHFORWARD
#define REPESEARCHFORWARD 1
#endif

#ifndef REPESEARCHBACKWARD
#define REPESEARCHBACKWARD 2
#endif

#ifndef SEARCHANY
#define SEARCHANY -1
#endif
#ifndef SEARCHASC
#define SEARCHASC 0
#endif

#ifndef SEARCHHEX
#define SEARCHHEX 1
#endif

#ifndef SEARREGEX
#define SEARREGEX 2
#endif

#ifndef SEARCH_STRING_SIZE
#define SEARCH_STRING_SIZE 256
#endif

#ifndef IMPUTHEX_STRING_SIZE
#define IMPUTHEX_STRING_SIZE 256
#endif

#ifndef MAX_SEARCH_RESULT
#define MAX_SEARCH_RESULT 1000
#endif

#ifndef MULTIPLO_BUFFER
#define MULTIPLO_BUFFER 9000
#endif

//Buffer de 40MB para salvado del fichero.
#ifndef VOSSIZEBUFWRITE
//#define VOSSIZEBUFWRITE 10485760
#define VOSSIZEBUFWRITE 41943040
#endif

//Buffer de 50MB para Cache IO
#ifndef VOSSIZEIOCACHE
//#define VOSSIZEIOCACHE 104857600
#define VOSSIZEIOCACHE 52428800
#endif

//#define STRHEAD " Commands: i->Input ON. ESC->Imput OFF. f->Buscar ASC. c-> COMP x->Buscar HEX. h-> Hex View. w->Save File. q->Exit. a->About"
#ifndef STRHEADVIEW
//#define STRHEADVIEW " MODE: VIEW. Cmd List: l, h, k, j, F10, F11, e, i, r, H, L, g, G, nnn[g|G], x, s, S, R, N, n, [q|Q], [m|M](Manual)"
#define STRHEADVIEW " MODE: VIEW. [m|M]=Cmd List. w=Save File. [q|Q]=Quit."

#endif

#ifndef STRHEADEDIT
//#define STRHEADEDIT " MODE: EDIT. Cmd List: l, h, k, j, F10, F11, H, L, g, G, nnn[g|G], x, s, S, R, N, n, [q|Q], [m|M](Manual)"
#define STRHEADEDIT " MODE: EDIT. [m|M]=Cmd List. w=Save File. [q|Q]=Quit."
#endif

#ifndef STRPIE
#define STRPIE " ROW: %d. COL: %d. REG: %lld COLREG: %lld FILE OFFSET: %lld      "
#endif

//Definicion de colores.
#ifndef NORMAL
#define NORMAL COLOR_WHITE
#endif

#ifndef COLOREDPLUS
#define COLOREDPLUS 74,117,97,110,109,97,32,50,48,49,48,46
#endif

#ifndef SHOWHEX
#define SHOWHEX COLOR_YELLOW
#endif

#ifndef RECORDDELMSG
#define RECORDDELMSG "------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ DELETED REDORD ------------ "
#endif


typedef struct
{
	char swMode;	//(E)dit, (V)iew.
	char swModifFile;
	char swModifSrcreen;
	float percentSaved;
	float percentSearch;
}STATUS;

typedef struct
{
    char arg0[256];
    char nonFich[256];
    unsigned long lreg;
    char copy[1000];
    char fCopy[256];
    int minCol;
    int minRow;
    int maxCol;
    int maxRow;
}CFG;

typedef struct
{
    char nomFich[256];			//Path al fichero
    unsigned long long nBytesFich;	//Tamanio del fichero
    unsigned long long buffLen;	//Longitud Calculada del buffer.
    unsigned long long bRealLen;//Longitud Real del buffer.
    unsigned long long regSize;	//Longitud del registro
    long nPagVert;		//Numero de paginas verticales.
    long nPagHoriz;		//Numero de paginas horizontales.
    long pagVActiva;	//Pagina Vertical Activa.
    long pagHActiva;	//Pagina Horizontal Activa.
    unsigned long long fPos;	//Ultimo Car leido del fichero.
    unsigned long long pos;		//Posicion del cursor de pantalla respecto albuffer.
    int winLineLength;			//Tama
    char *ioCache;
    FILE *pf;
    char copy[1000];					//Copy del fichero.
    char *buff;					//Buffer
}BUFFER;

typedef struct
{
	int *row;
	int *col;
	unsigned long long nreg;
	unsigned long long colreg;
	unsigned long long offset;
}POSITIONING;

typedef struct
{
	int fOffset;
	int length;
}SEARCHRESULT;



void capturaSignals(int signal);
unsigned char isFileReadOnly(const char *strFileName);
void initBuffer(BUFFER *b, const char *mode);
void freeBuffer(BUFFER *b);
void replaceBitacoraInBuffer(BUFFER *b, void *pb, unsigned long long nPage, int numRegPerPag);
int loadPageToBuffer(BUFFER *b, unsigned long long nPage);
int readToBuffer(BUFFER *b, unsigned long long fromPos);
char *getPointerToReg(BUFFER *b,int fila);
void printRegistro(unsigned long r, unsigned long len);
void printRegistroHEX(WINDOW *w, unsigned long r, unsigned long len);
void printBuffer(BUFFER *b);
void clearScreen(void);
void changeBuffer(BUFFER *b, int c, unsigned long long fileOffset);
void resalta(int reg, int colIni, int colFin);
void cursorToReg(unsigned long long nReg);
void cursorToRegCol(unsigned long long nReg, unsigned long long nCol);
void goReg(char *strNReg);
void goIni(void);
void goEnd(void);
void scrollUp(void);
void scrollDown(void);
void scrollLeft(void);
void scrollRight(void);
char compruebaScreen(void);
void commandsView(void);
void commandsEdit(void);
void commandsInsertRecord(void);
void commandsDuplicateRecord(void);
void commandsDeleteRecord(void);
void pintaPantalla(void);
long calculaPagVActiva(unsigned long long offset);
void calculaPOSITIONING(void);
void printC(int c);
void putC(BUFFER *b, int row, int col, int c, unsigned long long fileOffset);
void delC(BUFFER *b, int row, int col, int c, unsigned long long fileOffset);
void showHex(void);
void search(char mode, char swOption);
char *traduceString(const char *str, char *strRet, int nCars);
int len(int lineno);
void initColores(void);
void ddQuery(char *pathFich);
void initCfg(int argc, char **argv);
void wHelp(void);
void help(void);
int getFsCanWrite(const char *fich);
long long getFsFreeSpace(const char *fich);
int vosCopy(char *pBuf,  unsigned long long bufLen, FILE *pIn, FILE *pOut, unsigned long long regIni, long long regEnd);
int saveFile(const char *strNewName);
WINDOW *createWin(WINDOW *W, int height, int width, int starty, int startx);
void killWin(WINDOW *w);

void Exit(int ret);

void pruebas(void);
