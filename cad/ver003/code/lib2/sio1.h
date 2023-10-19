/************************************************************************/
/*		SIO1.H : Header file for Node3				*/
/*		M.J.Colley '92						*/
/************************************************************************/

extern	char	NUMBYTMST;
void	start_master	( unsigned char addr, 
			  unsigned char length, 
			  char idata *d_ptr );
void	sio1_init	( char own_slave_addr );

