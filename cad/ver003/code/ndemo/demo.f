/************************************************************************/
/*		DEMO.F : Demonstration Code for uNode3			*/
/*		M.J.Colley '92						*/
/************************************************************************/
#include	<inc.c>

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

	set_date	( )
{
	MTD[0]=	0x05;
	MTD[1]=	0x27;
	MTD[2]=	0x67;
	start_sio1	( RTC_W, 3, MTD );
}

	main		( )
{
idata	char	test;
	init	( );
	init_iic( );
/*	set_date( );*/
	set_lcd	( LINE_1 );
	printf	( "\xe4Node Demo" );
	while	( 1 )
	{
		show_time( );
		if	( x1_flag && test_x1 ())
			service_pad	((*KPAD)-240 );
	}
}