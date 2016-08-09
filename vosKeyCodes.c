#include <stdio.h>
#include "vosKeyCodes.h"

int vosIsPrint(int c)
{
	switch(c)
	{
		case '\f':
		case '\n':
		case '\r':
		case '\t':
		case '\v':
			return 1;	//False
			break;
			
/*		case VOSKEY_enie:
		case VOSKEY_ENIE:
			return 0;	//True
			break;
*/		
		default:
			//if(!isprint(c) || iscntrl(c))
			if(iscntrl(c))
				return 1;	//False
	}
		
	return 0;	//True
}
