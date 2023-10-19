/************************************************************************/
/*		N3INC.H : Header file for Node3				*/
/*		M.J.Colley '92						*/
/************************************************************************/

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

extern	bit	x1_flag;			/* set by x1_int	*/
extern	char	io_stream;			/* used by putchar	*/

void	w_dog	( char period );		/* T3 Refresh routine	*/
void	set_lcd	( char command );		/* Sets up Command reg	*/
void	init_lcd( );				/* Initialise LCD displ	*/
bit	start_sio1( unsigned char addr, 
		    unsigned char length, 
		    char idata *d_ptr);
void	init_sio0( );				/* Init RS232 port	*/
bit	init_RTC( );				/* 1 if RTC exists	*/
void	read_time	( time *tout );		/* Read RTC time	*/
void	read_date	( date *din );		/* Read RTC date	*/
void	display_time	( time *tout,char pos );/* Display time at pos	*/
void	display_date	( date *dout,char pos );/* Display date a pos	*/
void	show_time( char pos );			/* Read & print time	*/
void	show_date( char pos );			/* Read & print date 	*/
bit	range_getkey	( char in_key, char cursor_pos );
char	getkey	( );				/* Waits for x1_flag	*/
bit	get_time( );				/* Assumes no alarm int	*/
bit	test_x1	( );				/* 1 if kpad int	*/
