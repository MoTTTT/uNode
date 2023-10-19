/************************************************************************/
/*		SIO1.H : Header file for Node3				*/
/*		M.J.Colley '92						*/
/************************************************************************/

extern	char	NUMBYTMST;
void	start_master	( unsigned char addr, 
			  unsigned char length, 
			  char idata *d_ptr );
void	init_sio1	( char own_slave_addr );
void	sio1_interrupt	( );

