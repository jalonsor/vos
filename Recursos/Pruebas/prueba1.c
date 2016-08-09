#include <stdio.h>

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

int main(void)
{

	unsigned int nbr1, nbr2, nbr3;
	char string1 [] = "  FFF";
	char string2 [] = "\t\t  7Fxx";
	char string3 [] = "Aa55Bb66 1234";

    	nbr1 = htoi (string1);      
    	nbr2 = htoi (string2);     
    	nbr3 = htoi (string3);    
    	//nbr3 = htoi (&string3[8]);

	printf("\nstring1: %u. string2:  %u. string3: %u.\n", nbr1, nbr2, nbr3);

	return 0;
}
