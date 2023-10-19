
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>

int main() {
    int fdSer;
    int fdCon;
    struct termios t, tOrg;

    fclose(stdin);
    fclose(stdout);

    if ((fdSer = open("/dev/modem", O_RDWR )) < 0) {
        perror("open of /dev/modem failed");
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
    cfsetispeed(&t,B38400);
    cfsetospeed(&t,B38400);
    tcsetattr(fdSer,TCSANOW,&t);

    tcgetattr(fdCon,&tOrg);
    t = tOrg;
    t.c_iflag = 0;
    t.c_lflag = 0;
    t.c_cc[VTIME] = t.c_cc[VMIN] = 0;
    tcsetattr(fdCon,TCSANOW,&t);

    while (1) {
        struct timeval timeout;
        fd_set readset;
        unsigned char buf[256];
        int c;

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
                if (buf[0] == 'q')
                    break;
                prev = buf[0];
                write(fdSer,buf,c);
            }
        }
        if (FD_ISSET(fdSer,&readset)) {
            if (c = read(fdSer,buf,sizeof(buf))) {
                write(fdCon,buf,c);
            }
        }
    }
    tcsetattr(fdCon,TCSANOW,&tOrg);
    return 0;
}


