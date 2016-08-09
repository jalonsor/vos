Imports Microsoft.VisualBasic

Module Module1

'Sample COMP-3 conversion code
'Adapted from http://support.microsoft.com/kb/65323
'This code has not been tested

Sub Main()

    Dim Digits%(15)       'Holds the digits for each number (max = 16).
    Dim Basiceqv#(1000)   'Holds the Basic equivalent of each COMP-3 number.

    'Added to make code compile
    Dim MyByte As Char, HighPower%, HighNibble%
    Dim LowNibble%, Digit%, E%, Decimal%, FileName$


    'Clear the screen, get the filename and the amount of decimal places
    'desired for each number, and open the file for sequential input:
    FileName$ = InputBox("Enter the COBOL data file name: ")
    Decimal% = InputBox("Enter the number of decimal places desired: ")

    FileOpen(1, FileName$, OpenMode.Binary)

    Do Until EOF(1)   'Loop until the end of the file is reached.
        Input(1, MyByte)
        If MyByte = Chr(0) Then     'Check if byte is 0 (ASC won't work on 0).
            Digits%(HighPower%) = 0       'Make next two digits 0. Increment
            Digits%(HighPower% + 1) = 0   'the high power to reflect the
            HighPower% = HighPower% + 2   'number of digits in the number
            'plus 1.
        Else
            HighNibble% = Asc(MyByte) \ 16      'Extract the high and low
            LowNibble% = Asc(MyByte) And &HF    'nibbles from the byte. The
            Digits%(HighPower%) = HighNibble%  'high nibble will always be a
            'digit.
            If LowNibble% <= 9 Then                   'If low nibble is a
                'digit, assign it and
                Digits%(HighPower% + 1) = LowNibble%   'increment the high
                HighPower% = HighPower% + 2            'power accordingly.
            Else
                HighPower% = HighPower% + 1 'Low nibble was not a digit but a
                Digit% = 0                  '+ or - signals end of number.

                'Start at the highest power of 10 for the number and multiply
                'each digit by the power of 10 place it occupies.
                For Power% = (HighPower% - 1) To 0 Step -1
                    Basiceqv#(E%) = Basiceqv#(E%) + (Digits%(Digit%) * (10 ^ Power%))
                    Digit% = Digit% + 1
                Next

                'If the sign read was negative, make the number negative.
                If LowNibble% = 13 Then
                    Basiceqv#(E%) = Basiceqv#(E%) - (2 * Basiceqv#(E%))
                End If

                'Give the number the desired amount of decimal places, print
                'the number, increment E% to point to the next number to be
                'converted, and reinitialize the highest power.
                Basiceqv#(E%) = Basiceqv#(E%) / (10 ^ Decimal%)
                Print(Basiceqv#(E%))
                E% = E% + 1
                HighPower% = 0
            End If
        End If
    Loop

    FileClose()   'Close the COBOL data file, and end.
End Sub

End Module