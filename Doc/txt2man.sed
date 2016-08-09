1s/^\(.*\)$/\.TH \1/
/NAME/s/^\(.*\)$/\.SH \1/
/DESCRIPTION/s/^\(.*\)$/\.SH \1/
/COMMAND LINE/s/^\(.*\)$/\.SH \1/
/VIEW MODE/s/^\(.*\)$/\.SH \1/
/EDIT MODE/s/^\(.*\)$/\.SH \1/
/FOOT DISPLAYS/s/^\(.*\)$/\.SH \1/
/RUNNING vos/s/^\(.*\)$/\.SH \1/
/BUGS/s/^\(.*\)$/\.SH \1/
/\<EXAMPLE\>/s/^\(.*\)$/\.SH \1/
/AUTHOR/s/^\(.*\)$/\.SH \1/

s/vos/\\fIvos\\fR/g
s/View Mode/\\fIView Mode\\fR/g
s/Edit Mode/\\fIEdit Mode\\fR/g
s/Dialog Boxes/\\fIDialog Boxes\\fR/g
/11111/,/22222/{
a\
.br
}
/EXAMPLEMARK/,/EXAMPLEMARK/{
a\
.br
}
/EXAMPLEMARK/s///
/11111/s///
/22222/s///

/SEE ALSO/,/AUTHOR/{
/^[a-z]/s/^\(.*\)$/\.BR \1/
s/(\([1-9]\))/ "(\1), "/g
s/(C++)/ "(C++), "/g
s/(C++)$/ "(C++)"/
}
/AUTHOR/{
i\

a\
   			  
}

