#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "util.h"

#define LF 10
#define CR 13

void getword (char *word, char *line, char stop)
{
    int x = 0, y;

    for (x = 0; ((line[x]) && (line[x] != stop)); x++)
	word[x] = line[x];

    word[x] = '\0';
    if (line[x])
	++x;
    y = 0;

    while ((line[y++] = line[x++]));
}

char *
makeword (char *line, char stop)
{
    int x = 0, y;
    char *word = (char *) malloc (sizeof (char) * (strlen (line) + 1));

    for (x = 0; ((line[x]) && (line[x] != stop)); x++)
	word[x] = line[x];

    word[x] = '\0';
    if (line[x])
	++x;
    y = 0;

    while ((line[y++] = line[x++]));
    return word;
}

char *
fmakeword (FILE * f, char stop, int *cl)
{
    int wsize;
    char *word;
    int ll;

    wsize = 102400;
    ll = 0;
    word = (char *) malloc (sizeof (char) * (wsize + 1));

    while (1) {
	word[ll] = (char) fgetc (f);
	if (ll == wsize) {
	    word[ll + 1] = '\0';
	    wsize += 102400;
	    word = (char *) realloc (word, sizeof (char) * (wsize + 1));
	}
	--(*cl);
	if ((word[ll] == stop) || (feof (f)) || (!(*cl))) {
	    if (word[ll] != stop)
		ll++;
	    word[ll] = '\0';
	    return word;
	}
	++ll;
    }
}

#ifndef ESCGRK
void escape_url (char *url)
{
    register int x, y, l;
    char c;
    char code[10];
    char new[4096];

    l = strlen (url);
    for (y = 0, x = 0; x < l; ++x) {
	if (isalnum(c = url[x]) || c=='.' || c=='/' || c=='-' || c=='_') {
	    new[y] = url[x];
	    y++;
	}
	else {
	    sprintf (code, "%02X", c);
	    sprintf (&new[y], "%%%s", &code[strlen (code) - 2]);
	    /*
	       sprintf(code,"%%%02X",c);
	       strncpy(&new[y],code,3);
	     */
	    y += 3;
	}
    }
    new[y] = '\0';
    strcpy (url, new);
}

#else
void escape_url (unsigned char *url)
{
    register int x, y, l;
    int c;
    char code[10];
    char new[2048];

    l = strlen (url);
    for (y = 0, x = 0; x < l; ++x) {
        c=url[x];
	if (isdigit(c) || 
	    ((c>=65)&&(c<=90)) || ((c>=97)&&(c<=122))   //alnum
	    || ((c>182)&&(c<=254)) //greek
	    || c=='.' || c=='/' || c=='-' || c=='_') {
	    new[y] = url[x];
	    y++;
	}
	else {
	    sprintf (code, "%02X", c);
	    sprintf (&new[y], "%%%s", &code[strlen (code) - 2]);
	    y += 3;
	}
    }
    new[y] = '\0';
    strcpy (url, new);
}
#endif



char x2c (char *what)
{
    register char digit;

    digit =
	(what[0] >= 'A' ? ((what[0] & 0xdf) - 'A') + 10 : (what[0] - '0'));
    digit *= 16;
    digit +=
	(what[1] >= 'A' ? ((what[1] & 0xdf) - 'A') + 10 : (what[1] - '0'));
    return (digit);
}

void unescape_url (char *url)
{
    register int x, y;

    for (x = 0, y = 0; url[y]; ++x, ++y) {
	if ((url[x] = url[y]) == '%') {
	    url[x] = x2c (&url[y + 1]);
	    y += 2;
	}
    }
    url[x] = '\0';
}

void plustospace (char *str)
{
    register int x;

    for (x = 0; str[x]; x++)
	if (str[x] == '+')
	    str[x] = ' ';
}

int rind (char *s, char c)
{
    register int x;
    for (x = strlen (s) - 1; x != -1; x--)
	if (s[x] == c)
	    return x;
    return -1;
}

void tobasename (char *s)
{
    register int i;
    int l;
    char buf[1024];

    l = strlen (s) - 1;

    for (i = l; i > 0 && s[i] != '/'; i--);
    strcpy (buf, &s[i + 1]);
    strcpy (s, buf);
}


/* Return NAME with any leading path stripped off. (faster)*/
char * basename (const char * name)
{ 
  const char *base = name;
      
  while (*name) {
    if (*name == '/')
      base = name + 1;
    ++name;
  }
  return (char *) base;
}



void
cutlast (char *s)
{
    register int x, i;
    int l;

    l = strlen (s) - 1;

    for (i = l; i > 0 && s[i] == '/'; i--);
    for (x = i; x > 0 && s[x] != '/'; x--) {
	if (s[x - 1] == '/') {
	    s[x] = '\0';
	    break;
	}
    }
}

int
getline1 (char *s, int n, FILE * f)
{
    register int i = 0;

    while (1) {
	s[i] = (char) fgetc (f);

	if (s[i] == CR)
	    s[i] = fgetc (f);

	if ((s[i] == 0x4) || (s[i] == LF) || (i == (n - 1)) || s[i] == EOF) {
	    s[i] = '\0';
	    return (feof (f) ? 1 : 0);
	}
	++i;
    }
}

void
send_fd (FILE * f, FILE * fd)
{
    char c;

    while (1) {
	c = fgetc (f);
	if (feof (f))
	    return;
	fputc (c, fd);
    }
}

void
spacetoplus (char *str)
{
    register int x;
    for (x = 0; str[x]; x++)
	if (str[x] == ' ')
	    str[x] = '+';
}

void
spacetounderscore (char *str)
{
    register int x;
    for (x = 0; str[x]; x++)
	if (str[x] == ' ')
	    str[x] = '_';
}

/* Replace all occurences of needle with string in haystack */
void
str_replace(char *needle, char *string, char *haystack)
{
  char tmp[80000],*p1,*p2;
  register int i=0,j=0;
  int nlen,slen;

  nlen=strlen(needle);
  slen=strlen(string);
  tmp[0]=0;

  p1=haystack;

  while ((p2=strstr(p1,needle))) {
    for (i=0;i<p2-p1;i++)
      tmp[j+i]=*(p1+i);
    tmp[j+i]=0;
    memcpy(&tmp[j+i],string,slen);
    j+=i+slen;
    tmp[j]=0;
    p1=p2+nlen;
  }
  strcat(tmp,p1);
  strcpy(haystack,tmp);
}


int min3(int a,int b,int c)
/*Gets the minimum of three values*/
{
  int min=a;
  if(b<min)
    min=b;
  if(c<min)
    min=c;
  return min;
}



int ldist(char *s,char*t)
/*Compute levenshtein distance between s and t*/
{
  //Step 1
  int k,i,j,n,m,cost,*d,distance;
  n=strlen(s); 
  m=strlen(t);
  if(n!=0&&m!=0)
  {
    d=malloc((sizeof(int))*(m+1)*(n+1));
    m++;
    n++;
    //Step 2	
    for(k=0;k<n;k++)
	d[k]=k;
    for(k=0;k<m;k++)
      d[k*n]=k;
    //Step 3 and 4	
    for(i=1;i<n;i++)
      for(j=1;j<m;j++)
	{
        //Step 5
        if(s[i-1]==t[j-1])
          cost=0;
        else
          cost=1;
        //Step 6			 
        d[j*n+i]=min3(d[(j-1)*n+i]+1,d[j*n+i-1]+1,d[(j-1)*n+i-1]+cost);
      }
    distance=d[n*m-1];
    free(d);
    return distance;
  }
  else 
    return -1; //negative return value means that one or both strings are empty.
}

int ldisti(char *s,char*t)
/*Compute case insensitive levenshtein distance between s and t*/
{
  //Step 1
  int k,i,j,n,m,cost,*d,distance;
  n=strlen(s); 
  m=strlen(t);
  if(n!=0&&m!=0)
  {
    d=malloc((sizeof(int))*(m+1)*(n+1));
    m++;
    n++;
    //Step 2	
    for(k=0;k<n;k++)
	d[k]=k;
    for(k=0;k<m;k++)
      d[k*n]=k;
    //Step 3 and 4	
    for(i=1;i<n;i++)
      for(j=1;j<m;j++)
	{
        //Step 5
        if(tolower(s[i-1])==tolower(t[j-1]))
          cost=0;
        else
          cost=1;
        //Step 6			 
        d[j*n+i]=min3(d[(j-1)*n+i]+1,d[j*n+i-1]+1,d[(j-1)*n+i-1]+cost);
      }
    distance=d[n*m-1];
    free(d);
    return distance;
  }
  else 
    return -1; //negative return value means that one or both strings are empty.
}

void getenvar(char *var, char *str, unsigned int len)
{
  char * s;
    if ((s = getenv (str))) 
      strncpy (var, s,len-1);
}

//checks if src contains str, and it copies src's second part in dest
//returns 1 if dest gets filled
int varckfill(char *src, char *dest, char *str,int len)
{
  unsigned int ret,strl=0;

  strl=strlen(str);

  if (!(ret=strncmp (src, str, strl)))
    strncpy (dest, &src[strl+1],len-1);

  return !ret;

}


