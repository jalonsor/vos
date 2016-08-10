#!/bin/bash

#~ if [ $# -ne 2 ]
#~ then
	#~ echo "ERROR. numero incorrecto de argumaentos."
	#~ echo "Sintaxis: $0 fichero.txt N"
	#~ echo -e "\t N Numero de seccion del man."
	#~ exit 1
#~ fi

#~ FICH="$1"
#~ SECC=$2
#~ TMP="$$.tmp"
#~ NOM="`echo $FICH|cut -d'.' -f1`"

#~ if [ ! -f "$FICH" ]
#~ then
	#~ cp txt2man.txt "$FICH" 
#~ fi

#~ vim "$FICH"

FICH="vos.txt"
SECC="1"
TMP="$$.tmp"
NOM="`echo $FICH|cut -d'.' -f1`"

sed -f "vosman.sed" "$FICH" > "$TMP"

nroff -e -mandoc "$TMP" | less -s

mv "$TMP" "$NOM.$SECC"

gzip -f "$NOM.$SECC"
