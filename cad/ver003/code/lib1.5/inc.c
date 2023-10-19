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

#include	<sio1.h>
#include	<n3inc.h>

	set_date	( )
{
char	idata	date[3];
	date[0]=0x05;
	date[1]=0x23;
	date[2]=0x69;
	start_sio1	( RTC_W, 3, date );
}
