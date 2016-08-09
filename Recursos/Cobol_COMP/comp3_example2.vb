DIM Digits%(15)       'Holds the digits for each number (max = 16).
DIM Basiceqv#(1000)   'Holds the Basic equivalent of each COMP-3 number.

'Clear the screen, get the filename and the amount of decimal places
'desired for each number, and open the file for sequential input:
CLS
INPUT "Enter the COBOL data file name: ", FileName$
INPUT "Enter the number of decimal places desired: ", Decimal%
OPEN FileName$ FOR INPUT AS #1

DO UNTIL EOF(1)   'Loop until the end of the file is reached.
   Byte$ = INPUT$(1, 1)   'Read in a byte.
   IF Byte$ = CHR$(0) THEN  'Check if byte is 0 (ASC won't work on 0).
      Digits%(HighPower%) = 0       'Make next two digits 0. Increment
      Digits%(HighPower% + 1) = 0   'the high power to reflect the
      HighPower% = HighPower% + 2   'number of digits in the number
                                    'plus 1.
   ELSE
      HighNibble% = ASC(Byte$) \ 16      'Extract the high and low
      LowNibble% = ASC(Byte$) AND &HF    'nibbles from the byte. The
      Digits%(HighPower%) = HighNibble%  'high nibble will always be a
                                         'digit.
      IF LowNibble% <= 9 THEN                   'If low nibble is a
                                                'digit, assign it and
         Digits%(HighPower% + 1) = LowNibble%   'increment the high
         HighPower% = HighPower% + 2            'power accordingly.
      ELSE
         HighPower% = HighPower% + 1 'Low nibble was not a digit but a
         Digit% = 0                  '+ or - signals end of number.

         'Start at the highest power of 10 for the number and multiply
         'each digit by the power of 10 place it occupies.
         FOR Power% = (HighPower% - 1) TO 0 STEP -1
         Basiceqv#(E%)=Basiceqv#(E%) + (Digits%(Digit%) * (10^Power%))
         Digit% = Digit% + 1
         NEXT

         'If the sign read was negative, make the number negative.
         IF LowNibble% = 13 THEN
            Basiceqv#(E%) = Basiceqv#(E%) - (2 * Basiceqv#(E%))
         END IF

         'Give the number the desired amount of decimal places, print
         'the number, increment E% to point to the next number to be
         'converted, and reinitialize the highest power.
         Basiceqv#(E%) = Basiceqv#(E%) / (10 ^ Decimal%)
         PRINT Basiceqv#(E%)
         E% = E% + 1
         HighPower% = 0
      END IF
   END IF
LOOP
CLOSE   'Close the COBOL data file, and end.
