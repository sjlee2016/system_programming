SUM START 0
FIRST LDX #0
LOOP LDA #100
+LDB #TABLE2
BASE TABLE2
ADD TABLE1,X
ADD TABLE2,X
TABLE2 RESW   2
TIX  CONT
JLT LOOP
+STA TOTAL
RSUB
CONT    BYTE C'SGU'
TABLE1 WORD  1
TOTAL RESW  1
END   FIRST

