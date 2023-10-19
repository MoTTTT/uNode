/************************************************************************/
/*		N3INC.C : Include file for Node3			*/
/*		M.J.Colley '92						*/
/************************************************************************/
#include	<reg552.h>
#include	<stdio.h>
#include	<sio1.h>

#define		KPAD	(( char *) 0x28000L)	/* Address of keypad	*/

#define		RS232		2		/* Values of io_stream	*/
#define		LCD_DATA	0		/*			*/
#define		LCD_COMMAND	1		/*			*/
#define		CLEAR		0x01		/* LCD control commands	*/
#define		LINE_1		0x80		/*			*/
#define		LINE_2		0xC0		/*			*/

#define		OWN_SLAVE_ADR	0x31		/* Conrtoller slave adr	*/
#define		RTC_R		0xA1		/* PCF8583 read		*/
#define		RTC_W		0xA0		/* PCF8583 write	*/

typedef	struct	{
		char	second;
		char	minute;
		char	hour;
		} time;

typedef	struct	{
		char	day;
		char	month;
		} date;

bit		x1_flag	=0;			/* set by x1_int	*/
char		io_stream = 0x00;		/* used by putchar	*/

	w_dog	( char period )			/* T1 Interrupt routine	*/
{	PCON|=	0x10;	T3=	period;	}

	X1_int	 	( ) interrupt 2		/* X1 Interrupt routine	*/
{	x1_flag= 1;	}

	set_lcd		( char command )	/* Sets up Command reg	*/
{	io_stream= LCD_COMMAND;	putchar	( command ); io_stream= LCD_DATA;}

	init_lcd		( )		/* Initialise LCD dis	*/
{
	io_stream= LCD_COMMAND;
	putchar	( 0x38 );			/* SET LCD FOR 8 BIT	*/
	putchar	( 0x0C );			/* DISP= ON, CURSOR= OFF*/
	putchar	( 0x06 );			/* AUTO INC, FREEZE	*/
	putchar	( CLEAR );			/* CLEAR SCREEN		*/
	io_stream= LCD_DATA;
}

bit	start_sio1		( char addr, 
				  char length,
				  char idata *d_ptr)
{
int	idata	num_tries= 0;
	start_master	( addr, length, d_ptr );
	while	( NUMBYTMST )
	{
		if ( ++num_tries > 100 )	/* Wait for end of TX/RX*/
			return ( 0 );
	}
	return	( 1 );
}

	init_sio0		( )		/* Init RS232 port	*/
{
	TMOD	= 0x21;				/* SET TIMER 1 & 2 MODE	*/
	TH1	= 0xFD;				/* SET TIMER 1 PRELOAD	*/
	TR1	= 1;				/* START TIMER 1	*/
	PCON	= 0x80;				/* SET SMOD=1		*/
	S0CON	= 0x70;				/* SET SERIAL PORT MODE	*/
}

bit	init_RTC	( )			/* 1 if RTC exists	*/
{
char	idata	set[2];
	set[0]=	0x0;				/* Word Address		*/
	set[1]=	0x0C;				/* Status Control Reg	*/
	return	( start_sio1( RTC_W, 2, set ));	/* Send Control Reg	*/
}

void	read_time	( time *tout )
{
char	idata	tm[5];
	tm[0]=0x0;
	start_sio1	( RTC_W, 1, tm );
	start_sio1	( RTC_R, 5, tm );
	tout->second=	tm[2];
	tout->minute=	tm[3];
	tout->hour=	tm[4];
}

void	read_date	( date *din )
{
char	idata	dt[7];
	dt[0]=	0;
	start_sio1	( RTC_W, 1, dt );
	start_sio1	( RTC_R, 8, dt );
	din->day=	dt[5];
	din->month=	dt[6];
}

void	display_time	( time *tout, char pos ) 
{
	set_lcd	( pos );
	printf	( "%02bX:%02bX:%02bX", 
		tout->hour, tout->minute, tout->second );
}

void	display_date	( date *dout, char pos )
{
	set_lcd	( pos );
	printf	( "%02bX/%02bX", dout->month, dout->day ); 
}

	show_time	( char pos )		/* Prints time at pos	*/
{
time	idata	tm;
	read_time	( &tm );
	display_time	( &tm, pos );
}

	show_date	( char pos )		/* Prints date at pos	*/
{
date	idata	dt;
	read_date	( &dt );
	display_date	( &dt, pos );
}

bit	range_getkey	( char in_key, char cursor_pos )
{	
	switch ( cursor_pos )
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

char	getkey		( )
{	while	( !x1_flag );	x1_flag=0;	return	( (*KPAD)- 240 );}

bit	get_time	( )			/* Assume no alarm int	*/
{
char	idata	in, cursor= 0;
char	idata	tm[4];
char	K_BUF[6];
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
	tm[0]=	2;
	tm[3]= 16* K_BUF[0]+ K_BUF[1];
	tm[2]= 16* K_BUF[2]+ K_BUF[3];
	tm[1]= 16* K_BUF[4]+ K_BUF[5];
	if	((tm[3] <= 0x23) && (tm[2] <= 0x59) && (tm[1] <= 0x59))
	{
		start_sio1	( RTC_W, 4, tm );
		set_lcd		( CLEAR );
		tm[0]	= 0;
		set_lcd	( 0x0C );
		return	( 1 );
	}
	return	( 0 );
}

bit	test_x1		( )
{
char	idata	test=0;
	start_sio1	( RTC_W, 1, &test );
	start_sio1	( RTC_R, 1, &test );
	if	( !(test & 0x02) ) return ( 1 );
	return	( 0 );
}