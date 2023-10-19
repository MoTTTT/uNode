/*			Copyright Q Solutions				*/
/*	File:		test.c						*/
/*	Programmer:	MoT						*/
/*	Module:		Library routine test program			*/
/*									*/
/*			History						*/
/* 05:19am 06-07-1997  	Written from scratch				*/
/*									*/

#pragma		ROM (COMPACT)
#pragma		LARGE

/* Library files							*/
#include	<stdio.h>
#include	<reg552.h>

typedef	unsigned int uint;		/* Optimise for size & speed:	*/
typedef	unsigned char byte;		/* Use unsigned char and int	*/

#include	<nodelcd.h>		/* Local LCD module routines	*/
#include	<nkey.h>		/* Local Keypad module routines	*/
#include	<wdog.h>		/* Watchdog refresh routine	*/
#include	<serial.h>		/* Serial port routines		*/

#define		IO_NLCD	1		/* Output to uNode LCD		*/
#define		IO_SER	2		/* Output to Serial Port	*/
#define		B19200	0xFD		/* Serial Port Baud rate	*/
#define		B4800	0xFA		/* Serial Port Baud rate	*/

const char *signon1= "    Magellan    ";/* Line 1 of sign on screen	*/
const char *signon2= "    Simulator   ";/* Line 2 of signon screen	*/

char	out_stream;			/* Output port			*/
uint	counter;			/* Routine cycle counter	*/
char	cnt;

void	signon		( )		/* Splash Screen		*/
{
	out_stream= IO_NLCD;		/* Output port: LCD		*/
	paint_nlcd ( signon1, signon2 );
	out_stream= IO_SER;		/* Output port: Serial		*/
	printf	( "Magellan GPS 3000 Simulator.\n" );
	printf	( "Sending GGA Frames...\n" );
}

void	initialise	( )		/* Initialise Hardware		*/
{
	EA=		1;		/* Enable Global Interrupt	*/
	init_nlcd	( );		/* Initialise local LCD		*/
	init_nkey	( );		/* Initialise local keypad	*/
	init_serial	( B4800 );	/* Initialise Serial Port	*/
	signon		( );		/* Splash screen		*/
}

char	putchar		( char out )
{
char	result;
	switch	( out_stream )
	{
	case IO_NLCD:			/* Write to local LCD		*/
		result= nputchar ( out );
		break;
	case IO_SER:			/* Write to serial Port		*/
		result= sputchar ( out );
		break;
	default:
		return	( -1 );
	}
	return	( result );
}

void	send_GGA	( void )
{
	printf	( "$GPGGA,hhmmss.ss,111.11,a," );
	printf	( "yyyyy.yy,a,x,xx,x.x,x.x," );
	printf	( "M,x.x,M,x.x,xxxx*hh\n" );
}

void	service_keypad	( )		/* Read and process keypad	*/
{
byte	in;
	if	( x1_flag )
	{
		in= ngetkey	( );
		switch	( in )
		{
		case 0:	putchar (0x0A);
			break;
		default:send_GGA( );
			break;
		}
	}
}

void	main	( void )		/* Main Loop			*/
{
int	i;
	initialise	( );		/* Set up drivers, sign on	*/
	while	( 1 )
	{
		wdog	( 10 );		/* Refresh watchdog		*/
		service_keypad	( );	/* Process input		*/
		send_GGA( );
		while	( s_run );
		counter++;		/* Increment loop counter	*/
	}
}