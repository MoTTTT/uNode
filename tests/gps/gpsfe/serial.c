From - Mon May 19 23:46:47 1997
Return-Path: <colleymj@telkom.co.za>
Received: from qit-ul-0102.telkom.co.za ([165.143.240.133])
          by cheetah.intekom.co.za (Netscape Mail Server v2.02) with SMTP
          id AAA18968 for <qsolution@intekom.co.za>;
          Mon, 19 May 1997 15:54:09 +0200
Received: from wbif-colley.telkom.co.za (wbif-colley.telkom.co.za [165.148.12.73]) by qit-ul-0102.telkom.co.za (8.6.11/1997042006) with ESMTP id PAA23489 for <qsolution@intekom.co.za>; Mon, 19 May 1997 15:50:54 +0200
Message-ID: <33805A64.9B1A9BBC@telkom.co.za>
Date: Mon, 19 May 1997 15:49:26 +0200
From: Martin Colley <colleymj@telkom.co.za>
X-Mailer: Mozilla 4.0b4 [en] (Win95; I)
MIME-Version: 1.0
To: qsolution@intekom.co.za
Subject: [Fwd: I need some solid 'C' routines to read from a serial port in 'LINUX']
X-Priority: 3 (Normal)
Content-Type: multipart/mixed; boundary="------------CE68AB55E2DE5E554007DFBB"
X-Mozilla-Status: 0001
Content-Length: 6087

This is a multi-part message in MIME format.
--------------CE68AB55E2DE5E554007DFBB
Content-Type: text/plain; charset=us-ascii
Content-Transfer-Encoding: 7bit

Mark Hahn wrote:

> : >I need some 'c' routines that can read the serial port on a LINUX
> : >box.  It will be running either Slackware Linux 2.3 or Red Hat 4.1.
>
> : >Let me know the most common and most reliable methods.
>
> read/write is the only way to do IO on serial ports, and it's
> completely reliable.  it can be layered under buffered stdio
> if you like, but that sometimes complicates things.
>
> : FILE *serial=fopen("/dev/ttya","r+");
> : getchar(serial);
>
> indeed.  here's a bit more useful example, a fully-functional program
> I use to talk to my modem when I don't want to bother cranking up
> pppd:
>
> #include <unistd.h>
> #include <sys/time.h>
> #include <fcntl.h>
> #include <stdio.h>
> #include <termios.h>
>
> int main() {
>     int fdSer;
>     int fdCon;
>     struct termios t, tOrg;
>
>     fclose(stdin);
>     fclose(stdout);
>
>     if ((fdSer = open("/dev/modem", O_RDWR )) < 0) {
>         perror("open of /dev/modem failed");
>         return -1;
>     }
>     if ((fdCon = open("/dev/tty", O_RDWR )) < 0) {
>         perror("open of /dev/tty failed");
>         return -1;
>     }
>
>     t.c_iflag = IGNBRK | INPCK;
>     t.c_oflag = 0;
>     t.c_cflag = CS8 | CREAD | CLOCAL | CSTOPB;
>     t.c_lflag = 0;
>     cfsetispeed(&t,B38400);
>     cfsetospeed(&t,B38400);
>     tcsetattr(fdSer,TCSANOW,&t);
>
>     tcgetattr(fdCon,&tOrg);
>     t = tOrg;
>     t.c_iflag = 0;
>     t.c_lflag = 0;
>     t.c_cc[VTIME] = t.c_cc[VMIN] = 0;
>     tcsetattr(fdCon,TCSANOW,&t);
>
>     while (1) {
>         struct timeval timeout;
>         fd_set readset;
>         unsigned char buf[256];
>         int c;
>
>         timeout.tv_sec = 10;
>         timeout.tv_usec = 0;
>
>         FD_ZERO(&readset);
>         FD_SET(fdCon,&readset);
>         FD_SET(fdSer,&readset);
>
>         if (select(fdCon+1,&readset,0,0,&timeout) == -1)
>             break;
>
>         if (FD_ISSET(fdCon,&readset)) {
>             if (c = read(fdCon,buf,sizeof(buf))) {
>                 static unsigned char prev = 0;
>                 if (prev == 0x1d && buf[0] == 'q')
>                     break;
>                 prev = buf[0];
>                 write(fdSer,buf,c);
>             }
>         }
>         if (FD_ISSET(fdSer,&readset)) {
>             if (c = read(fdSer,buf,sizeof(buf))) {
>                 write(fdCon,buf,c);
>             }
>         }
>     }
>     tcsetattr(fdCon,TCSANOW,&tOrg);
>     return 0;
> }
>
> regards, mark hahn.
> --
> operator may differ from spokesperson.  hahn@neurocog.lrdc.pitt.edu
>
  http://neurocog.lrdc.pitt.edu/~hahn/



--------------CE68AB55E2DE5E554007DFBB
Content-Type: message/rfc822
Content-Transfer-Encoding: 7bit
Content-Disposition: inline

Path: news1.saix.net!uunet!in3.uu.net!207.172.25.142!ohno.news.erols.com!feed1.news.erols.com!howland.erols.net!newsfeed.internetmci.com!in2.uu.net!136.142.185.26!newsfeed.pitt.edu!neurocog.lrdc.pitt.edu!hahn
From: hahn@neurocog.lrdc.pitt.edu (Mark Hahn)
Newsgroups: comp.unix.programmer,comp.os.linux.development.apps,comp.os.linux.development.system,comp.unix.questions,slo.unix
Subject: Re: I need some solid 'C' routines to read from a serial port in 'LINUX'
Followup-To: comp.unix.programmer,comp.os.linux.development.apps,comp.os.linux.development.system,comp.unix.questions,slo.unix
Date: 18 May 1997 03:36:42 GMT
Organization: Learning Research and Development Center at U. of Pittsburgh
Message-ID: <5lltga$p72@usenet.srv.cis.pitt.edu>
References: <337D8287.3B42@earthlink.net> <EABxo0.H1z.B.stealth@bath.ac.uk>
NNTP-Posting-Host: neurocog.lrdc.pitt.edu
Xref: news1.saix.net comp.unix.programmer:56002 comp.os.linux.development.apps:32010 comp.os.linux.development.system:45575 comp.unix.questions:123913 slo.unix:2382

: >I need some 'c' routines that can read the serial port on a LINUX
: >box.  It will be running either Slackware Linux 2.3 or Red Hat 4.1.
: >Let me know the most common and most reliable methods.

read/write is the only way to do IO on serial ports, and it's
completely reliable.  it can be layered under buffered stdio
if you like, but that sometimes complicates things.

: FILE *serial=fopen("/dev/ttya","r+");
: getchar(serial);

indeed.  here's a bit more useful example, a fully-functional program
I use to talk to my modem when I don't want to bother cranking up pppd:

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
		if (prev == 0x1d && buf[0] == 'q')
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

regards, mark hahn.
--
operator may differ from spokesperson.	hahn@neurocog.lrdc.pitt.edu
					http://neurocog.lrdc.pitt.edu/~hahn/

--------------CE68AB55E2DE5E554007DFBB--

