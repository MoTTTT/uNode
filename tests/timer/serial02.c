
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>

#define IN_IDLE 0
#define IN_SYNC 1

int fdSer;
int fdCon;
struct termios t, tOrg;
struct timeval timeout;
fd_set readset;
unsigned char buf[256];
int in_stat= IN_IDLE;
int in_ptr=  0;

int init	( void )
{
    fclose(stdin);
    fclose(stdout);
    if ((fdSer = open("/dev/cua1", O_RDWR )) < 0) {
        perror("open of /dev/cua1 failed");
        return -1;
    }
    if ((fdCon = open("/dev/tty", O_RDWR )) < 0) {
        perror("open of /dev/tty failed");
        return -1;
    }
    t.c_iflag = IGNBRK | INPCK;
    t.c_oflag = 0;
    t.c_cflag = CS8 | CREAD | CLOCAL | CSTOPB;
    t.c_lflag = 0;
    cfsetispeed(&t,B4800);
    cfsetospeed(&t,B4800);
    tcsetattr(fdSer,TCSANOW,&t);

    tcgetattr(fdCon,&tOrg);
    t = tOrg;
    t.c_iflag = 0;
    t.c_lflag = 0;
    t.c_cc[VTIME] = t.c_cc[VMIN] = 0;
    tcsetattr(fdCon,TCSANOW,&t);
    return  0;
}

int	proc_serial ( char in )
{
//	write(fdCon,buf,in);
	switch ( in_stat )
	{
	case IN_IDLE:
		if ( in == '$' )
		{
			in_ptr= 0;
			in_stat= IN_SYNC;
		}
        	write(fdCon,buf,in);
		break;
	case IN_SYNC: 
		break;
	default: write(fdCon,buf,in);
	}
}

int main() {
int c;
    if	( init()< 0 )
	return -1;
    while (1) {
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        FD_ZERO(&readset);
        FD_SET(fdCon,&readset);
        FD_SET(fdSer,&readset);
        if (select(fdCon+1,&readset,0,0,&timeout) == -1)
            break;
        if (FD_ISSET(fdCon,&readset)) {
            if (c = read(fdCon,buf,sizeof(buf))) {
                static unsigned char prev = 0;
                if ( buf[0] == 'q')
                    break;
                prev = buf[0];
                write(fdSer,buf,c);
            }
        }
        if (FD_ISSET(fdSer,&readset)) {
            if (c = read(fdSer,buf,sizeof(buf))) 
		proc_serial( c );
        }
    }
    tcsetattr(fdCon,TCSANOW,&tOrg);
    return 0;
}


