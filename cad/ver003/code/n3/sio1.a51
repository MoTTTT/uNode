;-------------------------------------------------------;
;		    SIO1.a51				;
;-------------------------------------------------------;
	PUBLIC	_start_master
	PUBLIC	_init_sio1 
	PUBLIC	NUMBYTMST
;-------------------SIO1 SFR's--------------------------;
S1CON		EQU	0D8H
S1STA		EQU	0D9H
S1DAT		EQU	0DAH
S1ADR		EQU	0DBH
IEN0		EQU	0A8H
IP0		EQU	0B8H
;-------------------BIT LOCATIONS-----------------------;
STA		EQU	0DDH
SIO1HP		EQU	0BDH
;-------------------S1CON DATA--------------------------;
NA_O_NI_A	EQU	0D5H
NA_NO_NI_A	EQU	0C5H
NA_NO_NI_NA	EQU	0C1H
A_NO_NI_A	EQU	0E5H
;-------------------GENERAL DATA------------------------;
OWNSLA		EQU	031H
ENSIO1		EQU	0A0H
PAG1		EQU	001H
SLAW		EQU	0A1H
SLAR		EQU	0A0H
SELRB3		EQU	018H

IICDATA	SEGMENT	DATA
	RSEG	IICDATA
MD:		DS	1
SRD:		DS	1
STD:		DS	1
SLEN:		DS	1
BACKUP:		DS	1
NUMBYTMST:	DS	1
SLA:		DS	1
HADD:		DS	1

IICCODE	SEGMENT	CODE
	RSEG	IICCODE
	USING	0
;-------------------INITIALIZATION ROUTINE--------------;
;-------------------R7 contains OWNSLA------------------;
_init_sio1:
	MOV	S1ADR,R7
	SETB	P1.6
	SETB	P1.7
	MOV	HADD,#PAG1
	ORL	IEN0,#ENSIO1
	CLR	SIO1HP
	MOV	S1CON,#NA_NO_NI_A
	RET
;--------------------START TX/RX------------------------;
;------- R7: addr, R5: length, R3: d_ptr----------------;
_start_master:
	MOV	NUMBYTMST,R5
	MOV	SLA,R7
	SETB	STA
	MOV	MD,R3
	RET
;--------------------SIO1 INTERRUPT ROUTINE-------------;
	CSEG	AT	002BH
	PUSH	PSW
	PUSH	S1STA
	PUSH	HADD
	RET
;--------------------STATE ROUTINES---------------------;
;-------STATE 00, Bus Error-----------------------------;
	CSEG	AT	0100H 
	MOV	S1CON,#NA_O_NI_A
	POP	PSW
	RETI
;-------MASTER STATE SERVICE ROUTINES-------------------;
;-------STATE 08, A start condition has been Tx---------;
	CSEG	AT	0108H
	MOV	S1DAT,SLA
	MOV	S1CON,#NA_NO_NI_A
	AJMP	INITBASE1
;-------STATE 10, A repeat start condition has been Tx--;
	CSEG	AT	0110H
	MOV	S1DAT,SLA
	MOV	S1CON,#NA_NO_NI_A
	AJMP	INITBASE1

	CSEG	AT	00A0H
	USING	3
INITBASE1:	
	MOV	PSW,#SELRB3
	MOV	R0,MD
	MOV	R1,MD
	MOV	BACKUP,NUMBYTMST
	POP	PSW
	RETI
;-------STATE 18, ACK recieved--------------------------;
	CSEG	AT	0118H
	MOV	PSW,#SELRB3
	MOV	S1DAT,@R1
	AJMP	CON
;-------STATE 20, NACK recieved-------------------------;
	CSEG	AT	0120H
	MOV	S1CON,#NA_O_NI_A
	POP	PSW
	RETI
;-------STATE 28, DATA Tx, ACK Rx-----------------------;
	CSEG	AT	0128H
	DJNZ	NUMBYTMST,NOTLDAT1
	MOV	S1CON,#NA_O_NI_A
	AJMP	RETmt

	CSEG	AT	00B0H
NOTLDAT1:
	MOV	PSW,#SELRB3
	MOV	S1DAT,@R1
CON:
	MOV	S1CON,#NA_NO_NI_A
	INC	R1
RETmt:
	POP	PSW
	RETI
;-------STATE 30, DATA Tx, NACK Rx----------------------;
	CSEG	AT	0130H
	MOV	S1CON,#NA_O_NI_A
	POP	PSW
	RETI
;-------STATE 38, Arbitration lost----------------------;
	CSEG	AT	0138H
	MOV	S1CON,#A_NO_NI_A
	MOV	NUMBYTMST,BACKUP
	AJMP	RETmt
;-------------------MASTER RECIEVER ROUTINES------------;
;-------STATE 40, SLA+R Tx, ACK Rx----------------------;
	CSEG	AT	0140H
	MOV	S1CON,#NA_NO_NI_A
	AJMP	RETmr
;-------STATE 48, SLA+R Tx, NACK Rx---------------------;
	CSEG	AT	0148H
	MOV	S1CON,#NA_O_NI_A
	POP	PSW
	RETI
;-------STATE 50, DATA Rx, ACK Tx-----------------------;
	CSEG	AT	0150H
	MOV	PSW,#SELRB3
	MOV	@R0,S1DAT
	AJMP	REC1

	CSEG	AT	00C0H
REC1:
	DJNZ	NUMBYTMST,NOTLDAT2
	MOV	S1CON,#NA_NO_NI_NA
	SJMP	RETmr
NOTLDAT2:
	MOV	S1CON,#NA_NO_NI_A
	INC	R0
RETmr:
	POP	PSW
	RETI
;-------STATE 58, DATA Rx, NACK Tx----------------------;
	CSEG	AT	0158H
	MOV	S1CON,#NA_O_NI_A
	POP	PSW
	RETI
	END