/************************************************************************/
/*		SIO1.C : IIC bus include file				*/
/*		M.J.Colley '92						*/
/************************************************************************/

#define		RTC_R		0xA1		/* PCF8583 read		*/
#define		RTC_W		0xA0		/* PCF8583 write	*/

#define		NA_O_NI_AA_CRO		0xD5
#define		NA_NO_NI_AA_CRO		0xC5
#define		NA_NO_NI_NAA_CRO	0xC1
#define		A_NO_NI_AA_CRO		0xE5

char	idata	set[2];
char	idata	numbyte_mst;
char	idata	sla;
char	idata	*td_ptr;
char	idata	*rd_ptr;
char	idata	l_bak;
char	idata	*t_ptr;
char	idata	*r_ptr;

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

	start_sio1		( unsigned char addr, 
				  unsigned char length,
				  char idata *d_ptr)
{
	td_ptr= rd_ptr= d_ptr;			/* Set up data pointers	*/
	numbyte_mst=	length;			/* Set up length	*/
	sla=	addr;
	STA=	1;				/* Generate START cond	*/
	while	( numbyte_mst );		/* Wait for end of TX/RX*/
}

	init_iic	( )
{
	P1_6=	1;
	P1_7=	1;
	IEN0|=	0xA0;				/* Enable IIC Interrupt	*/
	PS1=	1;				/* IIC Int Low Priority	*/
	S1CON=	NA_NO_NI_AA_CRO;		/* Release Bus And Ack	*/
	set[0]=	0x0;				/* Word Address		*/
	set[1]=	0x0C;				/* Status Control Reg	*/
	start_sio1	( RTC_W, 2, set );	/* Send Control Reg	*/
}
