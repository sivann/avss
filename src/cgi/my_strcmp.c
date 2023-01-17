/*
 * Compare strings:  s1>s2: >0  s1==s2: 0  s1<s2: <0
 */

#include <string.h>
#include <stdio.h>

int
my_strcmp (s1, s2)
     char *s1, *s2;
{
    int ln = 0, rn = 0;
    char *c1, *c2;

    c1 = s1;
    c2 = s2;

    while (*s1 == *s2++)
	if (*s1++ == '\0')
	    return (0);

    if ((c1 = strrchr (c1, '-')) && (c2 = strrchr (c2, '-'))) {
	sscanf (c1 + 1, "%d", &ln);
	sscanf (c2 + 1, "%d", &rn);
    }

    /*
       if (c1) printf("c1:%s\n",c1);
       if (c2) printf("c2:%s\n",c2);
       printf("ln:%d rn:%d\n",ln,rn);
     */

    if (ln > rn)
	return 1;
    else if (ln < rn)
	return -1;

    return (*s1 - *--s2);
}
