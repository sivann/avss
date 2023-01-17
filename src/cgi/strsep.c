#include <string.h>
#include <stdio.h>

#if defined(LIBC_SCCS) && !defined(lint)
#if 0
static char sccsid[] = "@(#)strsep.c	8.1 (Berkeley) 6/4/93";

#else	/*  */
static char *rcsid =
    "$OpenBSD: strsep.c,v 1.3 1997/08/20 04:28:14 millert Exp $"; 
#endif	/*  */
#endif	/* LIBC_SCCS and not lint */
    
/*
 * Get next token from string *stringp, where tokens are possibly-empty
 * strings separated by characters from delim.  
 *
 * Writes NULs into the string at *stringp to end tokens.
 * delim need not remain constant from call to call.
 * On return, *stringp points past the last NUL written (if there might
 * be further tokens), or is NULL (if there are definitely no more tokens).
 *
 * If *stringp is NULL, strsep returns NULL.
 */ 
char *
     strsep (stringp, delim)  register char **
	 stringp;
     register const char *delim;

{
    register char *s;
    register const char *spanp;
    register int c, sc;
    char *tok;
    if ((s = *stringp) == NULL)
	return (NULL);
    for (tok = s;;) {
	c = *s++;
	spanp = delim;
	
	do {
	    if ((sc = *spanp++) == c) {
		if (c == 0)
		    s = NULL;
		
		else
		    s[-1] = 0;
		*stringp = s;
		return (tok);
	    }
	} while (sc != 0);
    }
    
	/* NOTREACHED */ 
}


