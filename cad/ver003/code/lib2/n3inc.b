/************************************************************************/
/*		N3INC.C : Include file for Node3			*/
/*		M.J.Colley '92						*/
/************************************************************************/
/*#include	<reg552.h>
#include	<stdio.h>*/

#define		KPAD	(( char *) 0x28000L)	/* Address of keypad	*/

#define		RS232		2		/* Values of io_stream	*/
#define		LCD_DATA	0		/*			*/
#define		LCD_COMMAND	1		/*			*/

#define		CLEAR		0x01		/* LCD control commands	*/
#define		LINE_1		0x80		/*			*/
#define		LINE_2		0xC0		/*			*/

#define		OWN_SLAVE_ADR	0x31		/* Conrtoller slave adr	*/

#define		NA_O_NI_AA_CRO		0xD5	/* S1CON values		*/
#define		NA_NO_NI_AA_CRO		0xC5	/* used in SIO1_int	*/
#define		NA_NO_NI_NAA_CRO	0xC1	/*			*/
#define		A_NO_NI_AA_CRO		0xE5	/*			*/

#define		RTC_R		0xA1		/* PCF8583 read		*/
#define		RTC_W		0xA0		/* PCF8583 write	*/


bit	x1_flag	=0;				/* set by x1_int	*/

char		io_stream = 0x00;		/* used by putchar	*/

char	idata	numbyte_mst;			/* Global variables for	*/
char	idata	sla;				/* SIO1_int		*/
char	idata	*td_ptr;			/*			*/
char	idata	*rd_ptr;			/*			*/
char	idata	l_bak;				/*			*/
char	idata	*t_ptr;				/*			*/
char	idata	*r_ptr;				/*			*/	

	w_dog	( char period )			/* T1 Interrupt routine	*/
{	
	PCON|=	0x10;
	T3=	period;
}

	X1_int	 	( ) interrupt 2	using 2	/* X1 Interrupt routine	*/
{	
	x1_flag= 1;
}

	set_lcd		( char command )	/* Sets up Command reg	*/
{
	io_stream= LCD_COMMAND;	
	putchar	( command ); 
	io_stream= LCD_DATA;
}

	init_lcd		( )		/* Initialise LCD dis	*/
{
	io_stream= LCD_COMMAND;
	putchar	( 0x38 );			/* SET LCD FOR 8 BIT	*/
	putchar	( 0x0C );			/* DISP= ON, CURSOR= OFF*/
	putchar	( 0x06 );			/* AUTO INC, FREEZE	*/
	putchar	( CLEAR );			/* CLEAR SCREEN		*/
	io_stream= LCD_DATA;
}

	SIO1_int	( ) interrupt 5 using 1	/* IIC Bus Interrupt	*/
{	
	switch	( S1STA )
	{
	case(0x00):				/* Bus Error		*/
		S1CON=	NA_O_NI_AA_CRO;		/* Clear SI, Gen STOP	*/
		break;
	case(0x08):				/* START cond TX	*/
	case(0x10):				/* Repeated START TX	*/ 
		S1DAT=	sla;			/* Store Slave Addr+ R/W*/
		S1CON=	NA_NO_NI_AA_CRO;	/* Clear SI		*/
		l_bak=	numbyte_mst;		/* Save Data Length	*/
		t_ptr=	td_ptr;			/* Save TX base ptr	*/
		r_ptr=	rd_ptr;			/* Save Rx base ptr	*/
		break;
	case(0x18):
		S1DAT=	*t_ptr;			/* Store first char	*/
		S1CON=	NA_NO_NI_AA_CRO;	/* Clear SI, Set AA	*/
		t_ptr++;
		break;
	case(0x20):
		S1CON=	NA_O_NI_AA_CRO;		/* Set STOP, Clear SI	*/
		break;
	case(0x28):
		numbyte_mst--;
		if	( numbyte_mst )
		{
			S1DAT=	*t_ptr;
			S1CON=	NA_NO_NI_AA_CRO;/* Clear SI, Set AA	*/
			t_ptr++;
		}
		else	S1CON=	NA_O_NI_AA_CRO;	/* Set STOP, Clear SI	*/
		break;
	case(0x30):
		S1CON=	NA_O_NI_AA_CRO;		/* Set STOP, Clear SI	*/
		break;
	case(0x38):
		S1CON=	A_NO_NI_AA_CRO;		/* Set START		*/
		numbyte_mst=	l_bak;
		break;
	case(0x40):
		S1CON=	NA_NO_NI_AA_CRO;	/* Clear SI		*/
		break;
	case(0x48):
		S1CON=	NA_O_NI_AA_CRO;		/* Clear SI, gen STOP	*/
		break;
	case(0x50):
		*r_ptr=	S1DAT;			/* Get RX Character	*/
		numbyte_mst--;
		if	( numbyte_mst )
		{
			S1CON=	NA_NO_NI_AA_CRO;/* Clear SI, Set AA	*/
			r_ptr++;
		}
		else	S1CON=	NA_NO_NI_NAA_CRO;/* Clear SI, AA	*/
		break;
	case(0x58):
		S1CON=	NA_O_NI_AA_CRO;		/* Clear SI, gen STOP	*/
		break;
	}
}

bit	start_sio1		( unsigned char addr, 
				  unsigned char length,
				  char idata *d_ptr)
{
int	idata	num_tries;
	td_ptr= rd_ptr= d_ptr;			/* Set up data pointers	*/
	numbyte_mst=	length;			/* Set up length	*/
	sla=	addr;
	STA=	1;				/* Generate START cond	*/
	num_tries= 0;
	while	( numbyte_mst );
	{ 
		if ( ++num_tries > 100 )	/* Wait for end of TX/RX*/
			return ( 0 );
	}
	return	( 1 );
}

	init_sio1( char own_slave_address )	/* Init IIC bus		*/
{
	S1ADR=	own_slave_address;
	P1_6=	1;
	P1_7=	1;
	IEN0|=	0xA0;				/* Enable IIC Interrupt	*/
	PS1=	1;				/* IIC Int Low Priority	*/
	S1CON=	NA_NO_NI_AA_CRO;		/* Release Bus And Ack	*/
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
	start_sio1( RTC_W, 2, set );		/* Send Control Reg	*/
	return	( 1 );
}

	show_time	( char pos )		/* Prints time at pos	*/
{
char	idata	time[5];
	time[0]=0x0;
	start_sio1	( RTC_W, 1, time );
	start_sio1	( RTC_R, 5, time );
	io_stream=	LCD_COMMAND;
	putchar	( pos );
	io_stream=	LCD_DATA;
	printf	( "%02bX:%02bX:%02bX", 
		time[4], time[3], time[2] );
}

	show_date	( char pos )		/* Prints date at pos	*/
{
char	idata	date[2];
	date[0]=0x00;
	start_sio1	( RTC_W, 1, date );
	start_sio1	( RTC_R, 8, date );
	io_stream=	LCD_COMMAND;
	putchar	( pos );
	io_stream=	LCD_DATA;
	printf	( "%02bX/%02bX", date[6], date[5] );
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
{	
	while	( !x1_flag );
	x1_flag=0;
	return	( (*KPAD)- 240 );
}

bit	get_time	( )			/* Assume no alarm int	*/
{
char	idata	in, busy= 1, cursor= 0;
char	idata	time[4];
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
	time[0]=	2;
	time[3]= 16* K_BUF[0]+ K_BUF[1];
	time[2]= 16* K_BUF[2]+ K_BUF[3];
	time[1]= 16* K_BUF[4]+ K_BUF[5];
	if	((time[3] <= 0x23) && (time[2] <= 0x59) && (time[1] <= 0x59))
	{
		start_sio1	( RTC_W, 4, time );
		set_lcd		( CLEAR );
		time[0]	= 0;
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
}
