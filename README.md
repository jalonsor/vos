# vos
(Very early stage, but functional). This  (vos) editor  is  intended  to edit very large files in only one line, formed by fixed positions and which are imported from  IBM  Main-frames to Linux. Since  most  of the editors in the market or in Linux distributions try to load the full file in memory, and this is  a  problem  when  working with large files, vos is the best option to cover this scene.  Some keys are similar to IBM ISPF.

# vos man page
VOS(Editor)              ZOS Records Editor for Linux.             VOS(Editor)

NAME
       vos - editor

DESCRIPTION
       This (vos) editor is intended to edit very large files in only one line, formed by fixed positions and which are imported from IBM Mainframes to Linux.

       Since most of the editors in the market or in Linux distributions try to load the full file in memory, and this is a problem when working with large files,
       vos is the best option to cover this scene.

       *** IT IS VERY IMPORTANT *** that you work with a fixed window terminal size and you do not change it while running vos.

       *** IT IS VERY IMPORTANT *** that you know that CTRL+S/CTRL+Q are special combinations for your serial terminal handler. These combined keys  are  software
       flow  control signals same as XOFF/XON hardware control. If you press CTRL+S (XOFF=scroll-lock) your terminal does not refresh. Then, you must press CTRL+Q
       (XON=scroll-lock off) to restart the refresh of your terminal. If you want to inhibit these combined keys you  must  reconfigure  your  terminal  (see  man stty).

COMMAND LINE
       vos [-l RecordLength] /SomePath/FILE

       -l N (Lower L). Record Length. N is the number of characters that compose one record. Default is 80.

EXAMPLE
       vos -l 219 /Files/FILE.XXXXX

RUNNING vos
       Vos runs in two modes. View Mode and Edit Mode. After running the example above, you are seeing the file formatted in view mode. Most of  the  commands  of
       vos works from View Mode.

       Whenever you want to go to View Mode you can press ESC key and you will see on the Top Left corner the caption " MODE: VIEW."

       All the Dialog Boxes with one or more buttons will be closed by selecting one of the buttons. You can select one button by pressing the first letter of the
       button label.

       Dialog Boxes that does not have buttons will be closed by pressing ESC key.

       For a modified file, vos will display its name in red color in the Bottom Right corner before it will be saved.

       All kind character that cannot be printed without breaking the formatted display of the file will be replaced with the character · You cwill know its value
       in HEX Mode pressing x key from View Mode.

VIEW MODE
       Caption on the Top Left corner: " MODE: VIEW."

          NAVIGATION Keys:
             l or ->    Left.
             h or <-    Right.
             k or ^     Up.
             j or v     Down.
             Av Pag     Next Page.
             Re Pag     Previous Page.
             F10        Page Scroll Leftt.
             F11        Page Scroll Right.
             H or Home  Initial column in the record you are.
             L or End   Last column in the record you are.
             g          Go to the Start of the file.
             G          Go to the End of the file.
             nnnn[G|g]  Go to the register pointed by number nnnn.

          SEARCH Keys:
             s     Search in Ascii mode. (From the page you are).
             S     Search in Hex mode. (From the page you are).
                   Example: 48656c6c6f or 48 65 6c 6c 6f=Hello
             R     Search in Regular Expression mode. (From the page you are). (**)NOT IMPLEMENTED
             n     Repeat Search forward. (From the page with last match).
             N     Repeat Search backward. (From the page with last match). (**)NOT IMPLEMENTED

          SHOW Keys:
             x     Show current row in Hex. mode.

          MODIFIYING CONTENT:
             ee    Enters in Edit Mode. Edit actual record. To exit from record edition press ESC.
             ii    Insert new record after. (**)NOT IMPLEMENTED
             rr    Duplicate actual record after. (**)NOT IMPLEMENTED
             dd    Delete actual record.
             uu    Undo last edit operation.

          END WORK Keys:
             w     Write changes to file. (Save). This operation may take some
                   time if the file was modified in size and its size is very
                   large.
             [Q|q] Quit.

EDIT MODE
       Caption on the Top Left corner: " MODE: EDIT."

       You enter in this mode (Edit Mode) from View Mode by typing ee or ii commands.

          COMBINED Keys:
             CTRL+x  Imput HEX character or HEX string when you are in
                     Edit Mode (ee or ii). Each character must be
                     formed by two HEX digits.

FOOT DISPLAYS
             ROW       Row in the screen in wich you are.
             COL       Column in the screen in wich you are.
             REG       Record number in the file.
             COLREG    Column in the file in wich you are.
             FILE OFFSET  Byte position in the file.
             RO        Read Only File.

BUGS
       As vos editor is an early software, there are operations not yet implemented and can contain bugs not yet reported. So, use it carefully and report any bug
       you encounter to the author. Reporting bugs in time will do vos better for you and your needs. Thank you very much.

AUTHOR
                       Juan Manuel Alonso Robles. (jalonsor)

                                  June, 2010                       VOS(Editor)


