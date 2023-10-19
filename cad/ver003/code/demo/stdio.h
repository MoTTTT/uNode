/* (c) Copyright FRANKLIN SOFTWARE, INC.  1989, 1990 All rights reserved. */
/* STDIO.H: prototypes for standard i/o functions, ver. 1.2 */

#ifndef EOF
#define EOF -1
#endif

extern char _getkey ();
extern char getchar ();
extern char ungetchar (char);
extern int putchar (char);
extern int printf (const char *, ...);
extern int sprintf (char *, const char *, ...);
extern char *gets (char *, int n);
extern int scanf (const char *, ...);
extern int sscanf (char *, const char *, ...);
extern int puts (const char *);
