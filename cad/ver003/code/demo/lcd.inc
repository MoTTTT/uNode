/************************************************************************/
/*		INC.C : LCD include file				*/
/*		M.J.Colley '92						*/
/************************************************************************/
#define		RS232		2
#define		LCD_DATA	0
#define		LCD_COMMAND	1

#define		CLEAR		0x01
#define		LINE_1		0x80
#define		LINE_2		0xC0


char		io_stream = 0x00;


	set_lcd		( char func )
{	io_stream= LCD_COMMAND;	putchar	( func ); io_stream= LCD_DATA; }


	init_lcd		( )
{
	io_stream= LCD_COMMAND;
	putchar	( 0x38 );			/* SET LCD FOR 8 BIT	*/
	putchar	( 0x0C );			/* DISP= ON, CURSOR= OFF*/
	putchar	( 0x06 );			/* AUTO INC, FREEZE	*/
	putchar	( CLEAR );			/* CLEAR SCREEN		*/
	io_stream= LCD_DATA;
}