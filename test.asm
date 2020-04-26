BOOT START	0
	 CLEAR A
	 LDX   #128
LOOP JSUB  GETC
	 RMO   A,S
	 SHIFTL S,4
	 JSUB  GETC
	 ADDR   S,A
	 STCH   0,X
	 TIXR   X,X
	 J      LOOP
GETC TD     INPUT
	 JEQ    GETC
	 RD     INPUT
	 COMP   #4
	 JEQ    80
	 COMP   #48
	 JLT    GETC
	 SUB    #48
	 COMP   #10
	 JLT    GETC
	 SUB    #7
RETURN	RSUB   
INPUT BYTE  X'F1'
	 END LOOP
