/************************************************************************/
/*		DEMO.F : Demonstration Code for uNode3			*/
/*		M.J.Colley '92						*/
/************************************************************************/

#include	<inc.c>

#define		ADCI_MASK	0xEF
#define		ADCS_MASK	0x08
char		adc[8];
unsigned char	i,j;
char	lcd_addr;
char	par_addr;

void	init_adc	( )
{
	EAD=	1;
	ADCON=	0x08;
	j=	0;
}

	init		( )
{
	EX1	= 1;				/* ENABLE XINT1		*/
	IT1	= 1;				/* XINT1 -> edge int	*/
	EA	= 1;				/* ENABLE GLOBAL INT	*/
	PX1	= 0;				/* EX1 -> LOW PRIORITY	*/
	lcd_addr= 0x82;
	init_lcd( );
	init_adc( );
}

	service_pad	( char in )
{
	x1_flag=0;
	switch	( in )	
	{
	case 0x0C:	break;
	}
}

void	adc_int		( ) interrupt 10
{
	adc[j]=ADCH;
	ADCON&=	ADCI_MASK;
	j= ( j== 7 ) ? 0: j+1;
	ADCON=	0x08+ j;
}

	main		( )
{
	init	( );
	set_lcd	( LINE_1 );
	printf	( "    ADC Test    " );
	while	( 1 )
	{
		set_lcd	( LINE_1 );
		for	( i= 0; i<= 3; i++ )	printf	( " %02bX ",adc[i] );
		set_lcd	( LINE_2 );
		for	( i= 4; i<= 7; i++ )	printf	( " %02bX ",adc[i] );
	}
}
