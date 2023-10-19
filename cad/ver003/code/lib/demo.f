/************************************************************************/
/*		DEMO.F : Demonstration Code for uNode3			*/
/*		M.J.Colley '92						*/
/************************************************************************/

#include	<inc.c>

bit	RTC_test;

	init		( )
{
	ET1	= 1;				/* ENABLE T1 INTERRUPT	*/
	ET0	= 1;				/* ENABLE T0 INTERRUPT	*/
	EX1	= 1;				/* ENABLE XINT1		*/
	IT1	= 1;				/* XINT1 -> edge int	*/
	EA	= 1;				/* ENABLE GLOBAL INT	*/
	PX1	= 0;				/* EX1 -> LOW PRIORITY	*/
	init_sio0( );
	init_lcd( );
	init_sio1( 0x31 );
	RTC_test= 	init_RTC ( );
/*	set_date( );*/
}

	service_pad	( char in )
{
	x1_flag=0;
	switch	( in )	
	{
	case 0x0B:	break;
	case 0x0C:	while	( !get_time( ) );	break;
	case 0x0D:	break;
	}
}

	main		( )
{
	init	( );
	set_lcd	( LINE_1 );
	printf	( "Iic Test" );
	while	( 1 )
	{
		if	( RTC_test )
		{
			show_time( LINE_2 + 4 );
			show_date( LINE_1 + 11 );
		}
		if	( x1_flag && test_x1 ())
			service_pad	((*KPAD)-240 );
	}
}