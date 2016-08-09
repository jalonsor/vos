#include <stdio.h>

#ifndef _BITACORA_
#define _VOSBITACORA_ VOSBITACORA

typedef struct
{
	unsigned long long nPage;
	unsigned long long nReg;
	char op;	//Operaciones: M=Modificar, D=Delete, A=Add
	unsigned long long buffLen;
	char *bufDat;
}VOSBITACORAELEM;

typedef struct
{
	char nomF[2048];
	FILE *pf;
	char swDelAdd;
	long nElems;
	VOSBITACORAELEM **arrElems;	//Array de Punteros a VOSBITACORAELEM;
}VOSBITACORA;

void vosBitacoraInit(VOSBITACORA *pb, const char *strFich);
void vosBitacoraKill(VOSBITACORA *pb);
long vosBitacoraAdd(VOSBITACORA *pb, char *bufer,  unsigned long long nReg, unsigned long long buffLen);
unsigned long long vosBitacoraGet(VOSBITACORA *pb, char *bufer, long id);
unsigned long long vosBitacoraGetReg(VOSBITACORA *pb, char *bufer, unsigned long long reg);
unsigned long long vosBitacoraDel(VOSBITACORA *pb, long id);
long vosBitacoraIsIn(VOSBITACORA *pb, unsigned long long nReg);
void vosBitacoraFreeAll(VOSBITACORA *pb);
void vosBitacoraSaveToFile(VOSBITACORA *pb);
void vosBitacoraReadFromFile(VOSBITACORA *pb);
char vosBitacoraGetOP(VOSBITACORA *pb, unsigned long long nReg);
void vosBitacoraOPAdd(VOSBITACORA *pb);
void vosBitacoraOPDel(VOSBITACORA *pb, char *bufer, unsigned long long buffLen, unsigned long long nReg, unsigned long long nPage);
void vosBitacoraOPModif(VOSBITACORA *pb, char *bufer, unsigned long long buffLen, unsigned long long nReg, unsigned long long nPage);
int vosBitacoraCompareReg(const void *x, const void *y);
void vosBitacoraSortByRegNumber(VOSBITACORA *pb);
unsigned long long vosBitacoraOPUndoLast(VOSBITACORA *pb);
int vosBitacoraIsChangeLength(VOSBITACORA *pb);
#endif
