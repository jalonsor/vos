#include <string.h>

unsigned int htoi (const char *ptr)
{
	unsigned int value = 0;
    char ch = *ptr;
    
    while (ch == ' ' || ch == '\t')
		ch = *(++ptr);

	for (;;)
	{
		if (ch >= '0' && ch <= '9')
			value = (value << 4) + (ch - '0');
		else if (ch >= 'A' && ch <= 'F')
			value = (value << 4) + (ch - 'A' + 10);
		else if (ch >= 'a' && ch <= 'f')
			value = (value << 4) + (ch - 'a' + 10);
		else
			return value;
		ch = *(++ptr);
	}
}

int getAsciiFromHEX(const char *strHex, char *strAscii)
{
	unsigned int c=0, i=0, j=0, k=0, strTam=0, asciiLen=0;
	char strPairHex[3]={0};
	
	strTam=strlen(strHex);
	for(i=0, j=0; i<strTam; i++)
	{
		if(isalpha(strHex[i]) || isdigit(strHex[i]))
		{
			strPairHex[j++]=strHex[i];
			strPairHex[j]=0;// Poner la terminacion de cadena siempre en el siguiente car para pa porsi.
		}
		if(j==2 || i==(strTam-1))
		{
			c=htoi(strPairHex);
			strAscii[k++]=(char) c;
			strAscii[k]=0;// Poner la terminacion de cadena siempre en el siguiente car para pa porsi.
			j=0;
			asciiLen++;
		}
	}
	
	return asciiLen;
}

float getPorcent(long long total, long long valor)
{
	return (float)valor*100.00/total;
}

char *baseDir(const char *filePath, char *baseDir)
{
	int i=0, pos=-1, strTam=0;
	
	strTam=strlen(filePath);
	
	for(i=0; i<strTam; i++)
		if(filePath[i]=='/')
			pos=i;
			
	if(pos==-1)
		strcpy(baseDir,"./");
	else
		strncpy(baseDir, filePath, pos+1);
	
	return baseDir;
}

int vosCompare(const void *x, const void *y) 
{
	return (*(int*)x - *(int*)y);
}

char *strBinCpy(char *dest, const char *src, long n)
{
	long i=0;
	for(i=0; i<n; i++)
		dest[i]=src[i];
		
	return dest;
}
