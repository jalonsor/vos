#include <stdio.h>
#include <curses.h>

#define CTRL(c) ((c) & 037)

/* Global value of current cursor position */
int row, col;
WINDOW *w;

/* http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/index.html */

int main(int argc,char **argv)
{
	extern void perror(), exit();

	int i, n, l;
	int c;
	int line = 0;
	FILE *fd;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s file\n", argv[0]);
		exit(1);
	}

	fd = fopen(argv[1], "r");
	if (fd == NULL)
	{
		perror(argv[1]);
		exit(2);
	}


	w=initscr();
	cbreak();
	nonl();
	noecho();
	idlok(stdscr, TRUE);
	keypad(stdscr, TRUE);
	//box(w, 0 , 0);
	refresh();

	
	initColores();

	/* Read in the file */
	while ((c = getc(fd)) != EOF)
	{
		if (c == '\n')
			line++;
		if (line > LINES - 2)
			break;
		addch(c);
	}
	fclose(fd);

	move(0,0);
	//wrefresh(w);
	refresh();
	edit();

	/* Write out the file */
	fd = fopen(argv[1], "w");
	for (l = 0; l < LINES - 1; l++)
	{
		n = len(l);
		for (i = 0; i < n; i++)
		putc(mvinch(l, i) & A_CHARTEXT, fd);
		putc('\n', fd);
	}
	fclose(fd);

	endwin();
	return 0;
}



ddQuery(char *pathFich)
{
	/*
		TODO.
	*/
}

initColores()
{
	if (has_colors())
	{
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

len(int lineno)
{
   int linelen = COLS - 1;

   while (linelen >= 0 && mvinch(lineno, linelen) == ' ')
       linelen--;
   return linelen + 1;
}

edit()
{
   int c;

   while(1)
   {
       move(row, col);
       wrefresh(w);//refresh();
       c = getch();

       /* Editor commands */
       switch (c)
       {

       /* hjkl and arrow keys: move cursor
        * in direction indicated */
       case 'h':
       case KEY_LEFT:
           if (col > 0)
               col--;
           else
               flash();
           break;

       case 'j':
       case KEY_DOWN:
           if (row < LINES - 1)
               row++;
           else
               flash();
           break;

       case 'k':
       case KEY_UP:
           if (row > 0)
               row--;
           else
               flash();
           break;

       case 'l':
       case KEY_RIGHT:
           if (col < COLS - 1)
               col++;
           else
               flash();
           break;
       /* i: enter input mode */
       case KEY_IC:
       case 'i':
           input();
           break;

       /* x: delete current character */
       case KEY_DC:
       case 'x':
           delch();
           break;

       /* o: open up a new line and enter input mode */
       case KEY_IL:
       case 'o':
           move(++row, col = 0);
           insertln();
           input();
           break;

       /* d: delete current line */
       case KEY_DL:
       case 'd':
           deleteln();
           break;

       /* ^L: redraw screen */
       case KEY_CLEAR:
       case CTRL('L'):
           wrefresh(curscr);
           break;

       /* w: write and quit */
       case 'w':
           return;
       /* q: quit without writing */
       case 'q':
           endwin();
           exit(2);
       default:
           flash();
           break;
       }
   }
}

/*
* Insert mode: accept characters and insert them.
*  End with ^D or EIC
*/
input()
{
   int c,i=0;

   standout();
   mvaddstr(LINES - 1, COLS - 20, "INPUT MODE");
   standend();
   move(row, col);
   wrefresh(w);//refresh();
   for (;;)
   {
       c = getch();
       if (c == CTRL('D') || c == KEY_EIC)
           break;
       insch(c);
       move(row, ++col);
       wrefresh(w);//refresh();
   }
   move(LINES - 1, COLS - 20);
   clrtoeol();
   move(row, col);
   wrefresh(w);//refresh();
}
