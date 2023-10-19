/************************************************************************/
/*		NODE3.F : CODE FOR uCONTROLLER NODE3			*/
/*		M.J.Colley '92						*/
/************************************************************************/
#include	<inc.c>

char	idata	RX_busy;
char	idata	RX_complete;
char	xdata	emucode[16384];
unsigned int	ram_index= 0;
char	lcd_addr= 0x82;
bit	RTC_test= 0;

	init		( )
{
	EX1	= 1;				/* ENABLE XINT1		*/
	ES0	= 1;				/* ENABLE RS232 int	*/
	IT1	= 1;				/* XINT1 -> edge int	*/
	EA	= 1;				/* ENABLE GLOBAL INT	*/
	PX1	= 0;				/* EX1 -> LOW PRIORITY	*/
	PT0	= 1;				/* T0  -> HIGH PRIORITY	*/
	PS0	= 0;				/* SIO -> HIGH PRIORITY	*/
	init_sio0( );				/* INIT RS232 PORT	*/
	init_lcd( );
	init_sio1( OWN_SLAVE_ADR );
	RTC_test= init_RTC ( );
}

	timeout		( ) interrupt 1		/* T0 Interrupt routine	*/
{
	TR0=		0;
	RX_complete=	1;
	RX_busy=	0;
}

	write_bytes	( )
{
	set_lcd	( LINE_2 );
	printf	( " Bytes RX: %d ", ram_index );
	ram_index=	0;
	RX_complete=	0;
}

	RX_code		( ) interrupt 4 using 3	/* SIO1 Interrupt 	*/
{
	RX_busy= 1;
	IEN0=	0x82;				/* ENABLE ONLY ET0	*/
	TR0=	1;				/* START TIMEOUT CLOCK	*/
	RI=	0;				/* RESET RI		*/
	while	( RX_busy )			/* LOOP FOREVER...	*/
	{	
		TH0=	0x00;			/* RESET T0		*/
		emucode[ ram_index ]= S0BUF;
		ram_index++;
		w_dog	( 0xD8 );
		while	( (!_testbit_(RI)) & RX_busy );
	}
	IEN0=	0xB6;				/* RESTORE INTERRUPTS	*/
}

	toggle_BR		( )
{
	set_lcd	( LINE_2 );
	if	( PCON & 0x80 )
	{
		PCON= 0;
		printf	( " Baud Rate: 96K " );
	}
	else
	{
		PCON= 0x80;
		printf	( " Baud Rate:192K " );
	}
}

	service_pad	( char in )
{
	x1_flag=0;
	switch	( in )
	{
	case 0x0B:
		toggle_BR	( );
		break;
	case 0x0C:	
		while	( !get_time( ) );
		break;
	}
}

	main		( )
{
char	idata	test;
	init	( );
	printf	( " \xe4Node  NO RTC  " );
	while	( 1 )
	{
		w_dog	( 0x90 );
		if	( RTC_test )	show_time( LINE_1 + 7 );
		if	( x1_flag && test_x1 ())
			service_pad	((*KPAD)-240 );
		if	( RX_complete )	write_bytes	( );
	}
}