#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TOTLINES 6000
#define TOTCOLUMNS 4000

main(int ac,char **av)
{
  /* char lines[TOTLINES][TOTCOLUMNS]; */
  char *lines[TOTLINES];
  unsigned long ln;
  unsigned long i=0;
  FILE *fp;
  char s[256];

  if (ac!=2) {
    printf("USage printlines <filename> < l1 l2 l3 ...\n");
    exit(1);
  }

  fp=fopen(av[1],"r");
  if (!fp) {perror(av[1]);exit(1);}

  while ((!feof(fp)) && (i<TOTLINES)) {
    lines[i] = (char *) malloc (TOTCOLUMNS);
    fgets(lines[i],TOTCOLUMNS,fp);
    i++;
  }

  while (fgets(s,255,stdin)) {
    ln=atoi(s);
    if ((ln<i) && lines[ln-1]) printf("%s",lines[ln-1]);
  }

}
