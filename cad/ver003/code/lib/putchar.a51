;-------------------------------------------------------;
;		    PUTCHAR.a51				;
;							;
;		int putchar (int val);			;
;							;
; For 80C552.  Prints on serial port 0 or to LCD.	;
; Only for text output.	 Can be modified for other	;
; hardware configurations.				;
;-------------------------------------------------------;
	NAME	putchar
	EXTRN	DATA	( io_stream, lcd_addr )
	PUBLIC	_putchar
PUT_C	SEGMENT	CODE

	RSEG	PUT_C
	USING	0
_putchar:
	CJNE	R7,#0AH,PLAIN		; LF (\n)?
	MOV	A,#0DH			; Yes, first CR
	MOV	R7,A
	LCALL	SEND_IT
	SJMP	P_OUT
PLAIN:  LCALL	SEND_IT
P_OUT:	RET

SEND_IT:
	PUSH	AR0
	PUSH	AR1
	PUSH	DPL
	PUSH	DPH
	MOV	R1,#io_stream
	MOV	A,@R1
	CJNE	A,#02H,lcd
rs232:	MOV	A,R7
	MOV	153,A			; Adress of S0BUF
	JNB	TI,$			; Wait for transmission
	CLR	TI
	SJMP	s_out			; Jump out
lcd:	MOV	DPH,lcd_addr		; Address H of LCD display
	MOV	DPL,#02H		; Address L of status register
wait:	MOVX	A,@DPTR			; Read status
	ANL	A,#80H			; Bitmask 'busy' bit
	JNZ	wait			; Repeat if busy
	MOV	A,@R1			; Test io_stream
	JNZ	command
	MOV	DPL,#01H		; io_stream= LCD_DATA
	MOV	A,R7
	MOVX	@DPTR,A			; Send byte
	SJMP	s_out			; Jump out
command:MOV	DPL,#00H		; io_stream= LCD_COMMAND
	MOV	A,R7
	MOVX	@DPTR,A			; Send byte
s_out:	POP	DPH			; Clean up
	POP	DPL
	POP	AR1
	POP	AR0
	RET
	END