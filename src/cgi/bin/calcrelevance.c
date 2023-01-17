#include <stdio.h>
#include <string.h>

/*to idio se awk (panargo):
 * cat stylebins | awk 'BEGIN{FS="";getline< "refbin.tmp";split($0,B,"");} {split($0,b,""); rs
 * um=0; for (i=0;i<NF;i++) rsum=rsum+(B[i]*b[i]); printf "%d:%d\n",rsum,NR;}' | sort -rn | head 
 * */

main(int argc,char **argv)
{
  FILE *fpr,*fp;
  char B[1024],b[1024];
  int len,i,sum,line;

  if (argc!=3) {
    printf("sivann 03\nUsage calcrelevance <reference bin file> <stylebins>\n\n");exit(3);}

  fpr=fopen(argv[1],"r");
  if (!fpr) {perror(argv[1]);exit(1);}

  fp=fopen(argv[2],"r");
  if (!fp) {perror(argv[2]);exit(2);}

  fgets(B,1024,fpr);
  fclose(fpr);
  len=strlen(B);
  line=0;

  while (!feof(fp)) {
    fgets(b,1024,fp);
    line++;
    for(sum=0,i=0;i<len-1;i++) {
      sum=sum+((B[i]-'0')*(b[i]-'0'));
      //printf("%d,%d,%d ",sum,B[i],b[i]);
    }
    if (sum) printf("%d:%d\n",sum,line);
  }

}
