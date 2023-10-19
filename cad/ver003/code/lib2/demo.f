/************************************************************************/
/*		DEMO.F : Demonstration Code for uNode3			*/
/*		M.J.Colley '92						*/
/************************************************************************/

#include	<inc.c>

bit	RTC_test= 0;

date	d1;
time	t1;

	init		( )
{
	EX1	= 1;				/* ENABLE XINT1		*/
	IT1	= 1;				/* XINT1 -> edge int	*/
	EA	= 1;				/* ENABLE GLOBAL INT	*/
	PX1	= 0;				/* EX1 -> LOW PRIORITY	*/
	init_sio0( );
	init_lcd( );
	sio1_init( 0x31 );
	RTC_test= 	init_RTC ( );
/*	set_date( );*/
}

	service_pad	( char in )
{
	x1_flag=0;
	switch	( in )	
	{
	case 0x0C:	while	( !get_time( ) );	break;
	}
}

	main		( )
{
	init	( );
	set_lcd	( LINE_1 );
	printf	( "\xe4Node3" );
	set_lcd	( LINE_2 );
	printf	( "Demo" );
	while	( 1 )
	{
		if	( RTC_test )
		{
			show_time( LINE_2+ 8 );
			show_date( LINE_1+ 11); 
		}
		if	( x1_flag && test_x1 ())
			service_pad	((*KPAD)-240 );
	}
}