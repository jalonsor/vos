#!/bin/bash
#gcc -g vos.c vosDialog.c -o vos -lcurses

LISTA="vos.c vosDialog.c regcomp.c  regerror.c  regexec.c  tre-ast.c  tre-compile.c  tre-match-approx.c  tre-match-backtrack.c  tre-match-parallel.c  tre-mem.c tre-parse.c tre-stack.c  xmalloc.c vosUtils.c vosBitacora.c vosKeyCodes.c"

echo -e "\n\n\n ***** INICIO [`date`] *****"
if [ "$1" == "zip" ]
then
	rm -f vos.zip >/dev/null 2>&1
	zip -9 vos.zip $LISTA  *.h compilaV.sh despliega.sh despliegaDA.sh txt2man.sh txt2man.txt txt2man.sed vos.txt vos.1.gz
elif [ "$1" == "static" ]
then
	gcc -ggdb -static-libstdc++ -static-libgcc -static -g $LISTA -o vos -lcurses && strip vos
elif  [ "$1" == "debug" ]
then
	gcc -ggdb -g $LISTA -o vos -lcurses  -O0 
	if [ $? -eq 0 ]
	then 
		(valgrind --leak-check=full --show-reachable=yes --track-origins=yes ./vos -l $2 "$3") 2> vosValgrid.log
	fi
else
	gcc $LISTA -o vos -lcurses  && strip vos
fi 
exit $?


#gcc -ggdb -g vos.c vosDialog.c regcomp.c  regerror.c  regexec.c  tre-ast.c  tre-compile.c  tre-match-approx.c  tre-match-backtrack.c  tre-match-parallel.c  tre-mem.c  tre-parse.c  tre-stack.c  xmalloc.c  -o vos -lcurses




export LD_DEBUG=files	---> Se muestra info de las librerias que se cargan al ejecutar un programa.

Sacado de la pagina http://www.faqs.org/docs/Linux-HOWTO/Program-Library-HOWTO.html


