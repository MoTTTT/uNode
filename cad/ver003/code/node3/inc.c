/************************************************************************/
/*		INC.C : 552 include file				*/
/*		M.J.Colley '92						*/
/************************************************************************/
#pragma		ROM (COMPACT)
#pragma		SMALL
#pragma		CODE
#pragma		SYMBOLS
#pragma		PAGELENGTH(64)
#include	<stdio.h>
#include	<intrins.h>
#include	<reg552.h>

#define		KPAD	(( char *) 0x28000L)
#define		RS232		2
#define		LCD_DATA	0
#define		LCD_COMMAND	1

#define		CLEAR		0x01
#define		LINE_1		0x80
#define		LINE_2		0xC0


char		io_stream = 0x00;
char	idata	MTD[8];
char	idata	MRD[8];
char	idata	K_BUF[9];
char	idata	x1_flag	=0;

#include	<sio1.c>

	init		( )
{
	TMOD	= 0x21;				/* SET TIMER 1 & 2 MODE	*/
	TH1	= 0xFD;				/* SET TIMER 1 PRELOAD	*/
	TR1	= 1;				/* START TIMER 1	*/
/*	ET1	= 1;*/				/* ENABLE T1 INTERRUPT	*/
	ET0	= 1;				/* ENABLE T0 INTERRUPT	*/
	ES0	= 1;				/* ENABLE S0 INTERRUPT	*/
	EX1	= 1;				/* ENABLE XINT1		*/
	IT1	= 1;				/* XINT1 -> edge int	*/
	EA	= 1;				/* ENABLE GLOBAL INT	*/
	PX1	= 0;				/* EX1 -> LOW PRIORITY	*/
	PT0	= 1;				/* T0  -> HIGH PRIORITY	*/
	PT1	= 1;				/* T1  -> HIGH PRIORITY	*/
	PS0	= 0;				/* SIO -> HIGH PRIORITY	*/
	PCON	= 0x80;				/* SET SMOD=1		*/
	S0CON	= 0x70;				/* SET SERIAL PORT MODE	*/
	io_stream= LCD_COMMAND;
	putchar	( 0x38 );			/* SET LCD FOR 8 BIT	*/
	putchar	( 0x0C );			/* DISP= ON, CURSOR= OFF*/
	putchar	( 0x06 );			/* AUTO INC, FREEZE	*/
	putchar	( CLEAR );			/* CLEAR SCREEN		*/
	io_stream= LCD_DATA;
}

	set_lcd		( char func )
{	io_stream= LCD_COMMAND;	putchar	( func ); io_stream= LCD_DATA; }

	w_dog		( ) 			/* T1 Interrupt routine	*/
{	PCON|=	0x10;	T3=	0x01;	}

	X1_int	 	( ) interrupt 2		/* X1 Interrupt routine	*/
{	x1_flag= 1;	}

char	getkey		( )
{
	while	( !x1_flag )	w_dog	( );
	x1_flag=0;
	return	( (*KPAD)- 240 );
}

char	range_getkey	( char in_key, char cursor_pos )
{	switch ( cursor_pos )
	{
	case ( 0 ) : if (in_key <= 2 ) return ( 1 ); break;
	case ( 1 ) : if (in_key <= 9 ) return ( 1 ); break;
	case ( 2 ) :
	case ( 4 ) : if (in_key <= 5 ) return ( 1 ); break;
	case ( 3 ) :
  	case ( 5 ) : if (in_key <= 9 ) return ( 1 ); break;
	}
	return	( 0 );
}

	get_time	( )			/* Assume no alarm int	*/
{
char	idata	in, busy= 1, cursor= 0;
	set_lcd	( LINE_2 );
	printf	( " Time: HH:MM:SS" );
	set_lcd	( LINE_2+ 7 );
	set_lcd	( 0x0F );
	while	( cursor< 6 )
	{
		in=	getkey	( );
		if	( range_getkey( in, cursor ))
		{
			K_BUF[ cursor ]= in;
			printf	( "%bX", in );
			if(( cursor== 1 )|| ( cursor== 3 ))
				printf	( ":" );
			cursor++;
		}
	}
	MTD[0]=	2;
	MTD[3]= 16* K_BUF[0]+ K_BUF[1];
	MTD[2]= 16* K_BUF[2]+ K_BUF[3];
	MTD[1]= 16* K_BUF[4]+ K_BUF[5];
	if	((MTD[3] <= 0x23) && (MTD[2] <= 0x59) && (MTD[1] <= 0x59))
	{
		start_sio1	( RTC_W, 4, MTD );
		set_lcd		( CLEAR );
		MTD[0]	= 0;
		set_lcd	( 0x0C );
		return	( 1 );
	}
	return	( 0 );
}

	show_time	( )
{
	MTD[0]=	0x0;
	start_sio1	( RTC_W, 1, MTD );
	start_sio1	( RTC_R, 6, MRD );
	set_lcd	( LINE_1+ 7 );
	printf	( "%02bX:%02bX:%02bX", MRD[4], MRD[3], MRD[2]);
}

char	test_x1		( )
{
char	idata	test=0;
	start_sio1	( RTC_W, 1, &test );
	start_sio1	( RTC_R, 1, &test );
	if	( !(test & 0x02) ) return ( 1 );
	return	( 0 );
}
