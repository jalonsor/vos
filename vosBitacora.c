#include <string.h>
#include <stdlib.h>
#include "vosBitacora.h"
#include "vosUtils.h"

void vosBitacoraInit(VOSBITACORA *pb, const char *strFich)
{
	strcat(pb->nomF, strFich);
	strcat(pb->nomF, "_Vos_Edition_Journal");
	pb->swDelAdd=0;
	pb->nElems=0;
	pb->arrElems=NULL;	
}

void vosBitacoraKill(VOSBITACORA *pb)
{
	long i=0;
	char *pAux=NULL;
	//fclose(pb->pf);
	
	for(i=0; i<pb->nElems; i++)
	{
		pb->arrElems[i];
		pAux=(pb->arrElems[i])->bufDat;
		free(pAux);
		free(pb->arrElems[i]);
	}
	
	free(pb->arrElems);
	
	pb->nomF[0]=0;
	pb->nElems=0;
}

long vosBitacoraAdd(VOSBITACORA *pb, char *bufer,  unsigned long long nReg, unsigned long long buffLen)
{
	VOSBITACORAELEM *pNew;
	long id=-1;
	
	if((id=vosBitacoraIsIn(pb, nReg))==-1) // Si no está insertado en Bitacora ...
	{
		//Reserva de mem para la estructura.
		pNew=(VOSBITACORAELEM*)malloc(sizeof(VOSBITACORAELEM));
		
		//Reserva de mem para los datos y copia de datos.
		pNew->bufDat=(char*)malloc(buffLen*sizeof(char));
	}
	else // Si ya está insertado en Bitacora ...
	{
		pNew=pb->arrElems[id];
	}
	
	strBinCpy(pNew->bufDat, bufer, buffLen);
	pNew->buffLen=buffLen;
	pNew->nReg=nReg;
	
	if(id==-1)
	{
		//Reserva de mem para el array.
		pb->arrElems=(VOSBITACORAELEM**)realloc((VOSBITACORAELEM**)pb->arrElems, 
										sizeof(VOSBITACORAELEM*)*(pb->nElems+1));
		pb->arrElems[pb->nElems]=pNew;
		pb->nElems++;
	}
	else
	{
		pb->arrElems[id]=pNew;
	}
	
	return pb->nElems;
}

unsigned long long vosBitacoraGet(VOSBITACORA *pb, char *bufer, long id)
{
	if(id<pb->nElems)
	{
		strBinCpy(bufer, (pb->arrElems[id])->bufDat, (pb->arrElems[id])->buffLen);
		return pb->arrElems[id]->buffLen;
	}
	
	return 0;
}

unsigned long long vosBitacoraGetReg(VOSBITACORA *pb, char *bufer, unsigned long long reg)
{
	long id=0;
	if((id=vosBitacoraIsIn(pb, reg))!=-1)
	{
		return vosBitacoraGet(pb, bufer, id);
	}
	
	return 0;
}

unsigned long long vosBitacoraDel(VOSBITACORA *pb, long id)
{
	unsigned long long auxPage=0;
	VOSBITACORAELEM *pAux;
	long i=id;
	
	if(id<pb->nElems)
	{
		pAux=pb->arrElems[id];
		auxPage=pAux->nPage;
		
		for(i=id; i<pb->nElems-1; i++)
		{
			pb->arrElems[i]=pb->arrElems[i+1];
		}
		pb->arrElems=(VOSBITACORAELEM**)realloc((VOSBITACORAELEM**)pb->arrElems, 
									sizeof(VOSBITACORAELEM*)*(pb->nElems-1));
		pb->nElems--;
		
		free(pAux->bufDat);
		free(pAux);
	}
	return auxPage;
}

long vosBitacoraIsIn(VOSBITACORA *pb, unsigned long long nReg)
{
	long i=-1;
	
	if(pb->nElems>0)
		for(i=0; i<pb->nElems; i++)
			if((pb->arrElems[i])->nReg==nReg)
			{
				return i;
			}
	return -1;
}

void vosBitacoraFreeAll(VOSBITACORA *pb)
{
	long i=0;
	
	for(i=0; i<pb->nElems; i++)
	{
		free((pb->arrElems[i])->bufDat);
		free(pb->arrElems[i]);
	}
	//free(pb->arrElems);
	pb->nElems=0;
	pb->swDelAdd=0;
}

void vosBitacoraSaveToFile(VOSBITACORA *pb)
{
	
}

void vosBitacoraReadFromFile(VOSBITACORA *pb)
{
	
}

/* Operaciones sobre la bitacora */
char vosBitacoraGetOP(VOSBITACORA *pb, unsigned long long nReg)
{
	long id=-1;
	if((id=vosBitacoraIsIn(pb, nReg))!=-1)
		return (pb->arrElems[id])->op;
	
	return -1;
}

void vosBitacoraOPAdd(VOSBITACORA *pb)
{
	pb->swDelAdd=1;
}

void vosBitacoraOPDel(VOSBITACORA *pb, char *bufer, unsigned long long buffLen, unsigned long long nReg, unsigned long long nPage)
{
	long nElem=vosBitacoraAdd(pb, bufer, nReg, buffLen);
	(pb->arrElems[nElem-1])->op='D';
	(pb->arrElems[nElem-1])->nPage=nPage;
	pb->swDelAdd=1;
}

//&bra, getPointerToReg(&buf,nRegEdit), buf.regSize, nRegEdit
void vosBitacoraOPModif(VOSBITACORA *pb, char *bufer, unsigned long long buffLen, unsigned long long nReg, unsigned long long nPage)
{
	long nElem=vosBitacoraAdd(pb, bufer, nReg, buffLen);
	(pb->arrElems[nElem-1])->op='M';
	(pb->arrElems[nElem-1])->nPage=nPage;
}

int vosBitacoraCompareReg(const void *x, const void *y) 
{
	VOSBITACORAELEM *v1=*(VOSBITACORAELEM **)x;
	VOSBITACORAELEM *v2=*(VOSBITACORAELEM **)y;
	//return (*(int*)x - *(int*)y);
	
	//return (int)(   (unsigned long long)(((VOSBITACORAELEM*)x)->nReg) - (unsigned long long)(((VOSBITACORAELEM*)y)->nReg)    );
	
	//printf("\n SORT: %ld, %ld", v1->nReg, v2->nReg);
	
	if( v1->nReg < v2->nReg )
		return -1;
	
	if( v1->nReg > v2->nReg )
		return 1;
		
	return 0;
}

void vosBitacoraSortByRegNumber(VOSBITACORA *pb)
{
	qsort(pb->arrElems, pb->nElems, sizeof(VOSBITACORAELEM*), vosBitacoraCompareReg);
	
	/* EJEMPLO:
		#include <stdio.h>
		#include <conio.h>
		#include <stdlib.h>
		int array[] = { 90, 3, 33, 28, 80, 49, 8, 30, 36, 25 };
		int sort(const void *x, const void *y) {
		  return (*(int*)x - *(int*)y);
		}
		void main() {
		  clrscr();
		  int i;
		  qsort(array, 10, sizeof(int), sort);
		  for (i=0; i<10; i++) {
			printf("%d ", array[i]);
		  }
		  getch();
		}
	*/
	
}

unsigned long long vosBitacoraOPUndoLast(VOSBITACORA *pb)
{
	unsigned long long auxPage;
	if(pb->nElems>0)
	{
		auxPage=vosBitacoraDel(pb, pb->nElems-1);
	}
	return auxPage;
}

int vosBitacoraIsChangeLength(VOSBITACORA *pb)
{
	return (int)pb->swDelAdd;
}


