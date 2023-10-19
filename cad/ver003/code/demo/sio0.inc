/************************************************************************/
/*		sio0.C : RS232 include file				*/
/*		M.J.Colley '92						*/
/************************************************************************/

	init_sio		( )
{
	TMOD	= 0x21;				/* SET TIMER 1 & 2 MODE	*/
	TH1	= 0xFD;				/* SET TIMER 1 PRELOAD	*/
	TR1	= 1;				/* START TIMER 1	*/
	PCON	= 0x80;				/* SET SMOD=1		*/
	S0CON	= 0x70;				/* SET SERIAL PORT MODE	*/
}
