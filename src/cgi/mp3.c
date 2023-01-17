/* Programmer:
 *
 * Spiros Ioannou (c) 1998-2006
 * sivann at softlab.ntua.gr
 */

/* Credits: See below
 */
//#define MAINTENANCE 

#define CREDITS "\n<!--\n\
 ____  __.__.__  .__             ________    \n\
|    |/ _|__|  | |  |           /   __   \\  \n\
|      < |  |  | |  |    ______ \\____    /  \n\
|    |  \\|  |  |_|  |__ /_____/    /    /   \n\
|____|__ \\__|____/____/           /____/    \n\
        \\/                                  \n\
\n\
 Programmer: \n\
 Spiros Ioannou (c) 1998-2008 \n\
 sivann@ at \n\
 \n\
 Credits:\n\
 Vassilis Papadimos for the original perl implementation\n\
 Costas Tavernarakis for various programming and layout tips\n\
 Kostas Karpouzis for various programming and layout tips\n\
 Vassilis Boulogiorgos for his excelent layout tips\n\
 Panagiotis Christias for setting competition to a higher level ;-)\n\
 Johnny Avrithis for recreating the competition\n\
 Nasos Drosopoulos for his pepper steak\n\
 All the members of kill-9 for their contributions and support\
 (φτιάχτε κανα filename ρε ρεμάλια)\n\
\n\
-->\n\
"
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>  /*bzero */
#include <ctype.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <utime.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <sys/param.h>
#include <syslog.h>
#include <regex.h>
#include <sqlite3.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "mp3info.h"
#include "id3.h"
#include "mp3.h"
#include "util.h"
#include "protos.h"
#include "mp3defs.h"


//Globals - skin
char TDSEARCH_BGCOLOR[32]="#EEEEEE";
char TD_FNTCOLOR[32]="#000000";
char BIO_FNTCOLOR[32]="#000000";
char BIO_BGCOLOR[32]="#EEFAF6";
char HDR_BGCOLOR[32]="#314984";
char HDR_FNTCOLOR[32]="#FFFFFF";
char TBL_BGCOLOR[32]="#6699CC";
char INFO_BGCOLOR[32]="#EEFAF6";
char DROW1_BGCOLOR[32]="#dddddd";
char DROW2_BGCOLOR[32]="#f9f9f9";
char FROW1_BGCOLOR[32]="#eeeeee";
char FROW2_BGCOLOR[32]="#F4FAFF";
char PHOTO_BGCOLOR[32]="#F7F3CE";
char BODY_BGCOLOR[32]="#6699cc";
char BODY_BACKGROUND[64]="";
char BODY_TEXT[32]="#000000";
char BODY_LINK[32]="#0065CE";
char BODY_VLINK[32]="#00309C";
char BODY_ALINK[32]="#6699cc";
char A_HOVER[32]="#FF00FF";
char BODY_TAG[256];
char INFOICO[32]="info.gif";
char skin[128]="none";

/* Globals */
char script_name[64];
char http_host[64];
char ident[64];
artistinfo_s artistinfo;
discinfo_s di[256];
int disize=0;
char frm_myurl[128]="myuria";
char tmp[4096];
char tmp2[2048];
char tmp3[2048];
char tmp4[2048];
char path[2048];

/* Globals - authentication */
char myurl[384];
char username[64], passwd[64], challenge[128];
char user[128], pass[128], gecos[128], maker[128], acl[128];
char tmp[4096], tmp2[2048];
char authorized, expired;
char remote_addr[32], remote_port[32];
char logout[8];
char cookie_avss[64];
char sess[64];

char http_x_forwarded_for[64],http_client_ip[64],http_cookie[1024];
char http_user_agent[128];
char http_range[128];
char query_string[4096];
time_t tim;

//Globals - search
char whatsnew[8];
char script_name_mp3[512];
char newperm[8];
char newdirs[8];
char encoding[16];
char searchitem[128];

int isanmp3 = 0,isanogg=0;


void readcookie_avss()
{
  char *s;
  int l=0;
  cookie_avss[0]=0;

  s=strstr(http_cookie,"avss=");
  if (s) l=strlen(s);
  if (l>=5)
    sscanf(s+5,"%[^;]",cookie_avss);
    
}


int getident(char *host,char *port, char *remport) {
  char tmp[80];
  int sd,r;
  struct sockaddr_in sa;
  struct hostent *chost;
  //int arg;

  /*Open socket and connect to host*/
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
    printf ("Content-type: text/html\n\n");
    printf("socket:%s",strerror(errno));
    return 0;
  }
  sa.sin_port = htons ((u_short) atoi (port));
  chost = gethostbyname (host);
  sa.sin_family = AF_INET;
  memcpy (&sa.sin_addr, chost->h_addr, chost->h_length);


  //set non-blocking
  /*
  if((arg = fcntl(sd, F_GETFL, NULL)) <0) { 
      printf ("Content-type: text/html\n\n");
      printf("fcntl:%s",strerror(errno));
     return 0;
  } 
  arg |= O_NONBLOCK; 
  if (fcntl(sd, F_SETFL, arg) < 0) { 
      printf ("Content-type: text/html\n\n");
      printf("fcntl:%s",strerror(errno));
     return 0; 
  } 
  */


  if (connect (sd,(struct sockaddr *) &sa, sizeof (sa)) < 0) {
    printf ("Content-type: text/html\n\n");
    printf("connect:%s",strerror(errno));
    return 0;
  }

  sprintf(tmp,"%s, 80\r\n",remport);
  r=write (sd, tmp, strlen(tmp));
  usleep(500000);
  r=read(sd,tmp,sizeof (tmp));tmp[r]=0;
  shutdown (sd, 2);
  close (sd);
  //4907, 80 : USERID : UNIX : sivann 
  sscanf(tmp,"%*s %*s : %*s : %*s : %s",ident); 
  //strcpy (ident,tmp);
  return 1;
}

void highlight(char * text)
{

//#define HIGHLIGHT1 "<A HREF=\"javascript:(function(){var count=0, text, regexp;text=prompt(%22Search regexp:%22, %22%22);"
#define HIGHLIGHT1 "<A HREF=\"javascript:(function(){var count=0, text, regexp;text="
#define HIGHLIGHT2 "if(text==null || text.length==0) return;try{regexp=new RegExp(%22(%22 + text +%22)%22, %22i%22);}catch(er){alert(%22Unable to create regular expression using text '%22+text+%22'.\n\n%22+er);return;}\n\
  function searchWithinNode(node, re){var pos, skip, spannode, middlebit, endbit, middleclone;skip=0;\n\
  if( node.nodeType==3 ){pos=node.data.search(re);\n\
    if(pos>=0){spannode=document.createElement(%22SPAN%22);spannode.style.backgroundColor=%22yellow%22;middlebit=node.splitText(pos);endbit=middlebit.splitText(RegExp.$1.length);middleclone=middlebit.cloneNode(true);spannode.appendChild(middleclone);middlebit.parentNode.replaceChild(spannode,middlebit);++count;skip=1;}}else if( node.nodeType==1 && node.childNodes && node.tagName.toUpperCase()!=%22SCRIPT%22 && node.tagName.toUpperCase!=%22STYLE%22)\n\
    {for (var child=0; child < node.childNodes.length; ++child){child=child+searchWithinNode(node.childNodes[child], re);}}return skip;}\n\
    window.status=%22Searching for %22+regexp+%22...%22;searchWithinNode(document.body, regexp);window.status=%22Found %22+count+%22 match%22+(count==1?%22%22:%22es%22)+%22 for %22+regexp+%22.%22;})();\">highlight</A>"
  puts(HIGHLIGHT1);
  printf(" '%s'; ",text);
  puts(HIGHLIGHT2);
}

int readskin(char *fname)
{
  char line[256];
  char opt[128];
  char sval[128];
  int linenum=0;
  int error=0;
  FILE *fp;
  //long tim;
  
  if (strstr(fname,"none")) return -1;
  fp = fopen (fname, "r");
  if  (!fp) {perror(fname); return -2;};

  //tim=time(0);
	    

  while(fgets(line,256,fp)) {
    opt[0]=sval[0]=0;
    linenum++;
    if (*line==' ' || *line=='#' || *line=='\n' || *line=='\t') 
      continue;

    sscanf(line,"%s%s",opt,sval);

    if (!strlen(sval)) {
      /*
      char tmp[255];
      int j;
      strcpy(tmp,ctime(&tim));
      j=strlen(tmp)-1; if (tmp[j]==10) tmp[j]=0; 
      fprintf(stderr,"[%s] error:in skin file (%s),line %d:no value\n",
	  tmp,fname,linenum);
      */
      error++;
    }

    if (!strcmp(opt,"TDSEARCH_BGCOLOR")) {
      strncpy(TDSEARCH_BGCOLOR,sval,32);
    }
    else if (!strcmp(opt,"TD_FNTCOLOR")) {
      strncpy(TD_FNTCOLOR,sval,32);
    } 
    else if (!strcmp(opt,"BIO_FNTCOLOR")) {
      strncpy(BIO_FNTCOLOR,sval,32);
    } 
    else if (!strcmp(opt,"BIO_BGCOLOR")) {
      strncpy(BIO_BGCOLOR,sval,32);
    } 
    else if (!strcmp(opt,"HDR_BGCOLOR")) {
      strncpy(HDR_BGCOLOR,sval,32);
    } 
    else if (!strcmp(opt,"HDR_FNTCOLOR")) {
      strncpy(HDR_FNTCOLOR,sval,32);
    } 
    else if (!strcmp(opt,"TBL_BGCOLOR")) {
      strncpy(TBL_BGCOLOR,sval,32);
    } 
    else if (!strcmp(opt,"INFO_BGCOLOR")) {
      strncpy(INFO_BGCOLOR,sval,32);
    } 
    else if (!strcmp(opt,"DROW1_BGCOLOR")) {
      strncpy(DROW1_BGCOLOR,sval,32);
    } 
    else if (!strcmp(opt,"DROW2_BGCOLOR")) {
      strncpy(DROW2_BGCOLOR,sval,32);
    } 
    else if (!strcmp(opt,"FROW1_BGCOLOR")) {
      strncpy(FROW1_BGCOLOR,sval,32);
    } 
    else if (!strcmp(opt,"FROW2_BGCOLOR")) {
      strncpy(FROW2_BGCOLOR,sval,32);
    } 
    else if (!strcmp(opt,"PHOTO_BGCOLOR")) {
      strncpy(PHOTO_BGCOLOR,sval,32);
    } 
    else if (!strcmp(opt,"BODY_BGCOLOR")) {
      strncpy(BODY_BGCOLOR,sval,32);
    } 
    else if (!strcmp(opt,"BODY_BACKGROUND")) {
      strncpy(BODY_BACKGROUND,sval,64);
    } 
    else if (!strcmp(opt,"BODY_LINK")) {
      strncpy(BODY_LINK,sval,32);
    } 
    else if (!strcmp(opt,"BODY_VLINK")) {
      strncpy(BODY_VLINK,sval,32);
    } 
    else if (!strcmp(opt,"BODY_ALINK")) {
      strncpy(BODY_ALINK,sval,32);
    } 
    else if (!strcmp(opt,"A_HOVER")) {
      strncpy(A_HOVER,sval,32);
    } 
    else if (!strcmp(opt,"BODY_TEXT")) {
      strncpy(BODY_TEXT,sval,32);
    } 
    else if (!strcmp(opt,"INFOICO")) {
      strncpy(INFOICO,sval,32);
    } 

  }
  
  return error;

}//readskin


/* sqlite callback foreach row */
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {

  char tmp[80480];
  char tmp2[64];
  struct tm *ptr;
  time_t tm;
  //songhits(
  //0 id numeric, 
  //1 time numeric, 
  //2 ip text, 
  //3 ip2 text, 
  //4 user text, 
  //5 path text, 
  //6 song text, 
  //7 agent text, 
  //8 xx text)

  tm=atol(argv[1]);
  ptr = localtime(&tm);
  strftime(tmp2 ,64 , "%H:%M:%S",ptr);

  strcpy(tmp,argv[5]);
  escape_url(tmp);

  printf("\n  <tr><TD  class=tb_sm>%s (%s)&nbsp;</td><TD> "
      "<a class=tb_sm title=\"%s\" "
      "href=\"%s?playpathfile=1&sess=%s&amp;path=%s\">%s</a></td></tr>",
      tmp2,argv[4],tmp, script_name,sess,tmp,argv[6]);

  printf("\n");
  return 0;
}


/* from sqlite */
void showrecent() {
  sqlite3 *db;
  char *zErrMsg=0;
  int rc;
  long tim;
  char tmp[80480];

  tim=time(0);

  rc = sqlite3_open(LOGDB, &db);
  if (!rc) {
    printf("\n  <table cellspacing=0 cellpadding=0>");
    sprintf(tmp,"select * from songhits where time>%ld ORDER BY time DESC "
	"LIMIT 15;", (tim-3600L));
    rc = sqlite3_exec(db, tmp, callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
      fprintf(stderr, "mp3: SQL error: (%s) %s\n",tmp, zErrMsg);
    }
    sqlite3_close(db);
    printf("\n  </table>\n");
  }
  else {
    fprintf(stderr, "Can't open database (%s): %s\n",LOGDB, sqlite3_errmsg(db));
    sqlite3_close(db);
  }
}

void printbio(char *bio,char * path)
{
  FILE *bfp;
  char tmp[80480];

  sprintf (tmp, MP3DIR"%s/%s", path,bio);
  bfp = fopen (tmp, "r");

  if (bfp) {
    while (fgets (tmp, 30000, bfp)) {
      //strcpy(tmp2,tmp);
      str_replace("{", "<",tmp);
      str_replace("}", ">",tmp);
      str_replace("<", "<b",tmp);
      str_replace(">", "</b>",tmp);
      str_replace("<b", "<b>",tmp);
      str_replace("<b>/I</b>", "</i></b>",tmp);
      str_replace("<b>I</b>", "<b><i>",tmp);
      str_replace("<b>P</b>", "<P>",tmp);
      str_replace("<b>/p</b>", "<br>",tmp);
      printf("%s",tmp);
    }
    fclose(bfp);
  }
  else {perror(bio);}


}

void seealso(char * path)
{
  FILE *pp;
  char buf[2048],tmp[2048],tmp2[2048];
  char bn[128],cmd[256];
  char *p,*p1,*p2;
  int sim=0;

  strcpy(bn,path);
  bn[strlen (bn) - 1] = 0;
  strcpy(bn,basename(bn));
  strcpy(cmd,FINDREL);
  strcat(cmd,"  \"");
  strcat(cmd,bn);
  strcat(cmd,"\"");

//  printf("cmd:%s#\n<br>",cmd);

  if ((pp = popen (cmd, "r")) != NULL) {
    while (fgets (buf, 2048, pp)) {
      buf[strlen (buf) - 1] = 0;
      strcpy(tmp2,buf);
      p = &buf[0];
      p1 = strsep (&p, ":");	/*relevance */
      p2 = strsep (&p, ":");	/*path */
      strcpy(tmp,p2);
      escape_url (tmp);
      strcpy(tmp2,basename(tmp2));
      sim=atoi(p1);
      
      if (sim<2) 
	printf ("\n\t<a TITLE=\"Similarity: %s\" "
	    "class=tb_sm href=\"%s?path=%s\">%s</a>, ",
	      p1, script_name, tmp,tmp2);
      else
	printf ("\n\t<a TITLE=\"Similarity: %s\" "
	    "class=tb href=\"%s?path=%s\">%s</a>, ",
	      p1,script_name, tmp,tmp2);
    }
    pclose(pp);
    printf("...");
  }
  else
    printf ("%s:popen:%s", FINDREL,strerror (errno));

}//seealso

void printdisc()
{
  int i;

  for (i=0;i<disize;i++){
    if (di[i].stars>6)
      printf("<b>(%d) %d %s</b><br>",di[i].stars,di[i].year,di[i].album);
    else if (di[i].stars)
      printf("(%d) %d %s<br>",di[i].stars,di[i].year,di[i].album);
    else if (strlen(di[i].album))
      printf("%d %s<br>",di[i].year,di[i].album);
  }
}

void readdisc(discinfo_s *di, char *disc,char * path)
{
  FILE *bfp;
  char tmp[2048],tmp2[2048];
  char *p,*p0,*p1,*p2,*p3,*p4;

  disize=0;
  sprintf (tmp, MP3DIR"%s/%s", path,disc);
  bfp = fopen (tmp, "r");

  if (bfp) {
    while (fgets (tmp, 256, bfp)) {

      di[disize].album[0]=0;
      di[disize].year=0;
      di[disize].stars=0;

      strcpy (tmp2, tmp);
      p = &tmp2[0];
      p0 = strsep (&p, "@");	//stars 
      p1 = strsep (&p, "@");	//album
      p2 = strsep (&p, "@");	//year
      p3 = strsep (&p, "@");	//company 

      if (strlen(p0)>0 && strlen(p0) <2) { //with score
	strcpy(di[disize].album,p2);
	di[disize].year=atoi(p1);
	di[disize].stars=atoi(p0);
      }
      else if (strlen(p0)){
	strcpy(di[disize].album,p1);
	di[disize].year=atoi(p0);
      }
      p4=strrchr(di[disize].album,'[');
      if (p4) *p4=0;


      disize++;
    }//while
    fclose(bfp);
  }
  else {perror(disc);}
}


void readinfo(artistinfo_s * ai, char *infofile,char * path)
{  
  FILE *ifp;
  char tmp[2048],tmp2[2048];
  char *p;

  ai->artist[0]=ai->style[0]=ai->formed[0]=ai->disbanded[0]=0;
  ai->members[0]=ai->decades[0]=0;
  ai->aka[0]=ai->born[0]=ai->died[0]=0;
  ai->followers[0]=ai->influencedby[0]=ai->similar[0]=ai->seealso[0]=0;

  sprintf (tmp, MP3DIR"%s/%s", path,infofile);
  ifp = fopen (tmp, "r");

  if (ifp) {
    while (fgets (tmp, 2048, ifp)) {
      strcpy (tmp2, tmp);
      p = &tmp2[0];
      if (!strncmp(tmp,"Artist",4)) {
	strsep(&p,":");
	strcpy(ai->artist,strsep(&p,":"));
      }
      else if (!strncmp(tmp,"Style",4)) {
	strsep(&p,":");
	strcpy(ai->style,strsep(&p,":"));
      }
      else if (!strncmp(tmp,"Formed",4)) {
	strsep(&p,":");
	strcpy(ai->formed,strsep(&p,":"));
      }
      else if (!strncmp(tmp,"Disbanded",4)) {
	strsep(&p,":");
	strcpy(ai->disbanded,strsep(&p,":"));
      }
      else if (!strncmp(tmp,"Born",3)) {
	strsep(&p,":");
	strcpy(ai->born,strsep(&p,":"));
      }
      else if (!strncmp(tmp,"Died",3)) {
	strsep(&p,":");
	strcpy(ai->died,strsep(&p,":"));
      }
      else if (!strncmp(tmp,"Members",4)) {
	strsep(&p,":");
	strcpy(ai->members,strsep(&p,":"));
	str_replace(",", ", ",ai->members);
      }
      else if (!strncmp(tmp,"Decades",4)) {
	strsep(&p,":");
	strcpy(ai->decades,strsep(&p,":"));
      }
      else if (!strncmp(tmp,"AKA:",3)) {
	strsep(&p,":");
	strcpy(ai->aka,strsep(&p,":"));
      }
      else if (!strncmp(tmp,"InfluencedBy:",10)) {
	strsep(&p,":");
	strcpy(ai->influencedby,strsep(&p,":"));
	str_replace(",", ", ",ai->influencedby);
      }
      else if (!strncmp(tmp,"Similar:",7)) {
	strsep(&p,":");
	strcpy(ai->similar,strsep(&p,":"));
	str_replace(",", ", ",ai->similar);
      }
      else if (!strncmp(tmp,"SeeAlso:",7)) {
	strsep(&p,":");
	strcpy(ai->seealso,strsep(&p,":"));
	str_replace(",", ", ",ai->seealso);
      }
      else if (!strncmp(tmp,"Followers:",9)) {
	strsep(&p,":");
	strcpy(ai->followers,strsep(&p,":"));
	str_replace(",", ", ",ai->followers);
      }

    } //while
    fclose(ifp);
  } //if ifp
  else {perror(infofile);}
}

void printinfo(artistinfo_s * ai)
{
  char tmp3[256];
  char *pd,*p1;
  if (!ai->artist[0]) return; //no info

  printf("<b>Artist:</b> %s<br>",ai->artist);
  printf("<b>Styles:</b>");
  printStyleAsLinks(ai->style);
  printf("<br>");
  if (strlen(ai->aka)>3) printf("<b>AKA: </b>%s<br>",ai->aka);
  if (strlen(ai->formed)>3) printf("<b>Formed: </b>%s<br>",ai->formed);
  if (strlen(ai->disbanded)>3) printf("<b>Disbanded: </b>%s<br>",
      ai->disbanded);
  if (strlen(ai->born)>3) printf("<b>Born: </b>%s<br>",ai->born);
  if (strlen(ai->died)>3) printf("<b>Died: </b>%s<br>",ai->died);
  if (strlen(ai->members)>3) printf("<b>Members: </b>%s<br>",ai->members);
  if (strlen(ai->seealso)>3) printf("<b>SeeAlso: </b>%s<br>",ai->seealso);
  if (strlen(ai->followers)>3) printf("<b>Followers: </b>%s<br>",ai->followers);
  if (strlen(ai->influencedby)>3) printf("<b>InfluencedBy: </b>%s<br>",ai->influencedby);
  if (strlen(ai->similar)>3) printf("<b>Similar(AMG): </b>%s<br>",ai->similar);
  printf("<b>Decades: </b>");

  strcpy (tmp3, ai->decades);
  p1 = &tmp3[0];
  pd=strsep(&p1,"@");if (pd[0]=='1') printf("10's ");
  pd=strsep(&p1,"@");if (pd[0]=='1') printf("20's ");
  pd=strsep(&p1,"@");if (pd[0]=='1') printf("30's ");
  pd=strsep(&p1,"@");if (pd[0]=='1') printf("40's ");
  pd=strsep(&p1,"@");if (pd[0]=='1') printf("50's ");
  pd=strsep(&p1,"@");if (pd[0]=='1') printf("60's ");
  pd=strsep(&p1,"@");if (pd[0]=='1') printf("70's ");
  pd=strsep(&p1,"@");if (pd[0]=='1') printf("80's ");
  pd=strsep(&p1,"@");if (pd[0]=='1') printf("90's ");
  pd=strsep(&p1,"@");if (pd[0]=='1') printf("00's ");
  printf("\n");

}



//convert style list to links
void printStyleAsLinks(char * style)
{
  char *p,*p1,*ps,*ps1;
  char tmp[256],tmp2[256],tmp3[256],tmp4[256];


  strcpy(tmp,style);
  str_replace(", ", ",",tmp);
  strcpy(tmp2,tmp);
  str_replace("&", "&amp;",tmp);
  str_replace(" ", "&nbsp;",tmp);
  p = &tmp[0];
  ps = &tmp2[0];

  while((p1 = strsep (&p, ","))){
    if ((ps1 = strsep (&ps, ","))) {
      int j;
      strcpy(tmp3,ps1);
      strcpy(tmp4,ps1);
      j=strlen(tmp3)-1; if (tmp3[j]==10) tmp3[j]=0; //chop %0A
      
      for (j=strlen(tmp4)-1;isalnum(tmp4[j]) && (j>0);j--) ;
      tmp4[j]=0;
      spacetoplus(tmp4);
      printf("\n<a class=tb href=\"%s?searchitem="
	  "%s&searchtype=styles\">",
	 script_name,tmp3);
      printf("%s</a>",p1);
      printf("&nbsp;<a class=tb href=\"javascript:void(0);\" "
	  "onClick=\"res=window.open('http://www.allmusic.com"
	  "/cg/amg.dll?P=amg&sql=5%s~C');return false;\">"
	  "<img width=13 align=absmiddle src=\""WWW_IMAGEDIR"%s\" "
	  "border=0 Title=\"Style Info\" alt=\"Style Info\"></a>",
	  tmp4,INFOICO);
      if (ps) printf(", ");
    }
    else
      printf("%s<br>",p1);
  }//while

}



void readfile()
{
  char fname[128];
  char buffer[2048];
  int r,w,fd;

  sprintf(fname,"%s%s",ABSDATADIR,MSG);

  fd = open (fname, O_RDONLY);
  if (fd < 0) {
	printf ("open(%s): %s (error %d)\n", fname, strerror (errno), errno);
	return;
  }
  while(((r=read (fd, buffer, 2047)) > 0) && ((w=write (1, buffer, r))>0));

}


void qsort1 (register int l, register int r, char *a[])
{
    register int i, j;

    char *w;
    char x[256];

    i = l;
    j = r;
    strncpy (x, a[(l + r) / 2], 255);
    while (i <= j) {
	while (strcmp (a[i], x) < 0)
	    i++;
	while (strcmp (a[j], x) > 0)
	    j--;
	if (i <= j) {
	    w = a[i];
	    a[i] = a[j];
	    a[j] = w;
	    i++;
	    j--;
	}
    }
    if (l < j)
	qsort1 (l, j, a);
    if (i < r)
	qsort1 (i, r, a);
}

void
qsort2 (register int l, register int r, char *a[], off_t * siz,MPEG_HEAD * inf)
{
    register int i, j;

    char *w;
    char x[256];

    off_t s;
    MPEG_HEAD mh;

    strncpy (x, a[(l + r) / 2], 255);
    i = l;
    j = r;
    while (i <= j) {
	while (my_strcmp (a[i], x) < 0)
	    i++;
	while (my_strcmp (a[j], x) > 0)
	    j--;

	if (i <= j) {
	    w = a[i];
	    a[i] = a[j];
	    a[j] = w;

	    s = siz[i];
	    siz[i] = siz[j];
	    siz[j] = s;

	    if (inf) {
		mh = inf[i];
		inf[i] = inf[j];
		inf[j] = mh;
	    }
	    i++;
	    j--;
	}
    }
    if (l < j)
	qsort2 (l, j, a, siz, inf);
    if (i < r)
	qsort2 (i, r, a, siz, inf);
}

//also swap dirs2di index file
void qsort3 (register int l, register int r, char *a[], int  *dirs2di)
{
    register int i, j,t;
    char *w;
    char x[256];
    i = l;
    j = r;
    strncpy (x, a[(l + r) / 2], 255);
    while (i <= j) {
	while (strcmp (a[i], x) < 0)
	    i++;
	while (strcmp (a[j], x) > 0)
	    j--;
	if (i <= j) {
	    w = a[i];
	    a[i] = a[j];
	    a[j] = w;

	    t=dirs2di[i];
	    dirs2di[i]=dirs2di[j];
	    dirs2di[j]=t;

	    i++;
	    j--;

	}
    }
    if (l < j)
	qsort3 (l, j, a, dirs2di);
    if (i < r)
	qsort3 (i, r, a, dirs2di);
}


///////////////////////// global var dependent funcs below ////


/* check if username&passwd is valid
 * if we have "username=" test authorization*/
void authuser()
{
  int i,r;

  /* AUTH USER HERE */
  openlog ("avss", LOG_CONS | LOG_PID, LOG_LOCAL2);
  FILE *pf;
  char *p1, *p2, *p3, *p4, *p5;	
      /*username:password:gecos:poios ton eftiakse:ACL */
  char *rr, *p;
  char loginfo[128];
  regex_t prbuf[128];

  sprintf (loginfo,
   "Starting authentication, user (%s:%s) from [%s:%s#%s#%s]",
	   username, ident,remote_addr, remote_port,
	   http_x_forwarded_for,http_client_ip);
  syslog (LOG_LOCAL2 | LOG_INFO, "%s", loginfo);


  //getident(remote_addr,"113", remote_port) ;
  pf = fopen (PASSFILE, "r");
  if (!pf)
      exit (-1);

  //find gecos
  while ((rr = fgets (tmp, 2048, pf)) != NULL) {
      if (strlen (tmp) && tmp[0] == '#')
	  continue;
      tmp[strlen (tmp) - 1] = '\0';	/*cut the newline */
      strcpy (tmp2, tmp);
      p = &tmp2[0];
      p1 = strsep (&p, ":");	/*username */
      p2 = strsep (&p, ":");	/*password */
      p3 = strsep (&p, ":");	/*gecos */
      p4 = strsep (&p, ":");	/*who made it */
      p5 = strsep (&p, ":");
       /*ACL*/
      if (!strcmp (p1, username)) {	/*It's him */
		strcpy (user, username);
		if (p2)
			strcpy (pass, p2);
		if (p3)
			strcpy (gecos, p3);
		if (p4)
			strcpy (maker, p4);
		if (p5)
			strcpy (acl, p5);
		break;
      }
  }

  
  if (rr == 0) { /*Username not found in passfile */
      authorized = 0;

      sprintf (loginfo,
       "Unknown user (%s:%s) failed authorization from [%s:%s#%s#%s]",
	       username, ident,remote_addr, remote_port,
	       http_x_forwarded_for,http_client_ip);
      syslog (LOG_LOCAL2 | LOG_INFO, "%s", loginfo);

      printf ("Content-type: text/html\n\n");
      printf ("<HTML><h2><center>\n"
	  "<img align=absmiddle src=\""WWW_IMAGEDIR"daffy-argue.gif\">"
	  "Go away lamer from...%s\n", remote_addr);
      printf (" this page isn't for yer eiz</H2></center>\n");
      printf ("</html>");
      exit (0);
  }

  if (
      (!strncmp(myurl,"http://www.yyy.zzz/cgi-bin/",41)) ||
      (!strncmp(myurl,"http://xxx.yyy.xxx.com/avss/cgi-bin/",40))) {
    strcpy(myurl,"mute");
  }
  strcat(myurl,"#");
  strcat(myurl,http_user_agent);

  //printf ("Content-type: text/html\n\n"); printf("rr:%d, pass:%s, passwd:%s\n",rr,pass,passwd); exit(0);

  if (!strcmp ((char *) crypt (passwd, pass /*"pw" */ ), pass)) {
      /*it's him */
      authorized = 1;

      sprintf (tmp, "%s%s-%s", IPZDIR, username,challenge);
      r=mkdir(tmp,00775); //etsi ki'alliws
      if ((r < 0) && (errno != EEXIST)) {
	printf ("Content-type: text/plain\n\n");
	printf ("mkdir(%s): %s (error %d)\n", tmp, strerror (errno), errno);
	exit (r);
      }

      sprintf (tmp, "%s%s-%s/timecheck", IPZDIR, username,challenge);
      i = open (tmp, O_WRONLY | O_CREAT | O_TRUNC, 00644);
      if (i < 0) {
	  sprintf (loginfo,
	       "User %s:%s denied listening from [%s:%s#%s#%s]",
	       username,ident, remote_addr,remote_port,
	       http_x_forwarded_for,http_client_ip);
	  syslog (LOG_LOCAL2 | LOG_INFO, "%s", loginfo);
	  printf ("Content-type: text/html\n\n");
	  printf ("<html>\n<head>\n");
	  printf ("<meta HTTP-EQUIV=\"Content-Type\""
	      "CONTENT=\"text/html; charset=UTF-8\">");
	  printf ("</head>");
	  printf ("<body>\n<H1><CENTER>Ούστ Κακόγουστε</CENTER>");
          printf ("or (open(%s):%s) ", tmp, strerror (errno));
	  printf ("</H1></body>\n");

	  printf ("</html>");
	  exit (0);
      }
      close (i);
      touchnow(tmp); //authorize him
  }
  else {
      /*Wrong password */
      authorized = 0;

      sprintf (loginfo,
       "User (%s:%s) failed authentication from [%s:%s#%s#%s]",
	       username, ident,remote_addr, remote_port,
	       http_x_forwarded_for,http_client_ip);
      syslog (LOG_LOCAL2 | LOG_INFO, "%s", loginfo);

      printf ("Content-type: text/html\n\n");
      printf ("<HTML>\n<H1><CENTER>No way dude...</CENTER></H1>\n");
      printf ("</html>");
      exit (0);
  }

  if (strlen (acl)) {	/*check acl */
      regcomp (prbuf, acl, REG_EXTENDED | REG_ICASE);
      r = regexec (prbuf, remote_addr, 0, 0, 0);
      if (r) {
	  struct utimbuf tb;

	  tb.actime = tim - IDLE - 10;
	  tb.modtime = tim - IDLE - 10;
	  sprintf (tmp, "%s%s-%s/timecheck", IPZDIR, username,challenge);
	  utime (tmp, &tb);
	  expired = 1;

	  authorized = 0;
	  printf ("Content-type: text/html\n\n");
	  printf ("<HTML>\n<H1><CENTER>Your Ip (%s) is Banned</CENTER>"
	      "</H1>\n", remote_addr);
	  /* printf("Your acl:%s<br>\n",acl);*/
	  printf ("</html>");
	  sprintf (loginfo, 
	       "User %s:%s failed acl (%s) from [%s:%s#%s#%s] <%s>",
	       username, ident, acl, remote_addr, remote_port,
	       http_x_forwarded_for,http_client_ip,myurl);
	  syslog (LOG_LOCAL2 | LOG_INFO, "%s", loginfo);
	  exit (0);
      }
  } /*check acl */

  sprintf (tmp, "User %s:%s authorized from [%s:%s#%s#%s] <ch=%s:%s>",
	   username,ident, remote_addr, remote_port,
	   http_x_forwarded_for, http_client_ip, challenge,myurl);
  syslog (LOG_LOCAL2 | LOG_INFO, "%s", tmp);

  //write username file
  sprintf (tmp, "%s%s-%s/username", IPZDIR, username,challenge);
  i = open (tmp, O_WRONLY | O_CREAT | O_TRUNC, 00644);
  if (i<0) {
      printf ("Content-Type: text/plain\n\n");
      printf ("%s:open:%s", tmp, strerror (errno));
      exit(0);
  }
  sprintf(tmp,"%s\n",username);
  write(i,tmp,strlen(tmp));
  close(i);

  //write ip file
  sprintf (tmp, "%s%s-%s/ip", IPZDIR, username,challenge);
  //i = open (tmp, O_WRONLY | O_CREAT | O_TRUNC, 00644);
  i = open (tmp, O_WRONLY| O_APPEND | O_CREAT , 00644);
  if (i<0) {
      printf ("Content-Type: text/plain\n\n");
      printf ("%s:open:%s", tmp, strerror (errno));
      exit(0);
  }
  sprintf(tmp,"%s:%s:%s\n",remote_addr,remote_port,http_x_forwarded_for);
  write(i,tmp,strlen(tmp));
  close(i);
  closelog ();

  printf("Set-Cookie: avss=%s-%s\n",username,challenge);
  printf ("Content-type: text/html\n\n");
  printf ("<HTML>\n<HEAD>\n<TITLE>MP3 login </TITLE>\n");
  printf ("<META HTTP-EQUIV=\"Refresh\" CONTENT=\"0\">\n" );
  printf ("<meta HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; ");
  exit(0);

} //authuser

//check if ip has expired and fill expired var.
int isexpired()
{
  int r;
  struct stat st_s;

  /*Check if host exists && if it's expired*/
  //sprintf (tmp, "%s/%s/timecheck", IPZDIR, remote_addr);

  sprintf (tmp, "%s%s/timecheck", IPZDIR, cookie_avss);
  r = stat (tmp, &st_s);

  /*ip does not exist or is too old*/
  if ((strlen(cookie_avss)<2) || (r != 0) || (tim - st_s.st_mtime > IDLE)) {
      expired = 1;
  }
  else
      expired = 0;
  return expired;
}

//logout user as requested
void dologout()
{
  struct utimbuf tb;


  tb.actime = tim - IDLE - 10;
  tb.modtime = tim - IDLE - 10;
  
  if (strlen(cookie_avss)>2) {
    sprintf (tmp, "%s%s/timecheck", IPZDIR, cookie_avss);
    utime (tmp, &tb);
  }
  expired = 1;

}

//display login screen 
void disploginscr()
{
  time_t tim;
  char chal[128];

  //challenge
  tim = time (0);
  sprintf(chal,"%lu",tim);

  //printf("Set-Cookie: avss=%s-%s\n",username,chal);

  /*login screen */
  printf ("Content-type: text/html\n\n");
  printf("<!DOCTYPE HTML "
      "PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" "
       "\"http://www.w3.org/TR/html4/loose.dtd\">\n");
  printf("<html>\n <head>\n <title>The kill-3 MP9z V2</title>\n "
	 "<meta http-equiv=\"Content-Type\" content=\"text/html; " 
	 "charset=UTF-8\">\n");
  printf("<META NAME=\"ROBOTS\" CONTENT=\"NOINDEX, NOFOLLOW\">\n");
  printf(LOGINSTYLE);
  printf("<script>\n"
      "function sf(){document.loginfrm.username.focus();}\n"
      "</script>\n");

  printf("<script language=\"javascript\" src=\""AVSSWWW"/md5.js\"></script>\n\n");

  
  printf("</head>\n<body bgcolor=\"#065595\" onLoad='sf()'>\n <p><br></p>\n"
      "<center>\n "
      //"<table cellspacing=5 cellpadding=0 bgcolor=black width=\"70%%\">"
      "<table cellspacing=5 cellpadding=0 bgcolor=black width=\"600\">"
      "<tr><TD>"
      "<table border=0 cellspacing=0 cellpadding=5 "
      "width=\"100%%\" bgcolor=\"#FFCF00\">\n"
      "<tr>"
      "<TD width=\"1%%\">"
      "<img ALT=\"\" src=\""WWW_IMAGEDIR"mp3communism.jpg\"></td>\n"
      "<TD align=center>\n");

  cutlast (path); /*if logged out, go to path after login*/
  escape_url (path);	
  printf ("<FORM NAME=\"loginfrm\" METHOD=\"POST\" "
	  " ENCTYPE=\"application/x-www-form-urlencoded\""
	  " ACTION=\"%s?path=%s\">\n", script_name,path);
  printf("<b>"
  "<font face=\"arial,helvetica\" size=\"+1\">U</font>SERNAME</b><br>\n"
  "<input type=\"text\" name=\"username\" "
  " value=\"\" tabindex=\"1\">\n"
  "<br><b><font face=\"arial,helvetica\" size=\"+1\">P</font>ASSWORD<br>"
  "</b>\n<input type=\"password\" name=\"password\" value=\"\" "
  "tabindex=\"2\"><br>\n");
  printf("<input type=\"submit\" class=\"subm\" name=\"Login\" "
    "value=\"Login\" tabindex=\"3\" autocomplete=\"off\" "
    "accesskey=\"l\">\n "
    "<input type=\"hidden\" name=\"path\" value=\"\">\n");
  printf("<input type=\"hidden\" name=\"%s\">\n",frm_myurl);
  printf("<input type=\"hidden\" name='challenge' value='%s'>\n",chal);
  printf(	  "</form>\n"
    "<p><i>Μα... είναι υπέροχο!&trade;</i>\n"
    " </td>\n</tr>\n"
    "<tr><TD colspan=2 align=center>"
    "<font size=\"-1\">"
    "Οι κωδικοί είναι <b>αυστηρά</b> προσωπικοί. Σε περίπτωση μη"
    " συμμόρφωσης, ο κωδικός θα αφαιρείται δια παντός.<br>\n"
    " <b>Το TCP forwarding επιτρέπεται σπάνια και "
    "μόνο μετά από συνεννόηση.</b><br>"
    "All attempts and transfers are logged."
    "</font>\n"
    "</td></tr></table>\n"
    "</table>\n</center>\n");
    printf("<script>document.loginfrm.%s.value=document.URL;</script>\n"
    " </body>\n</html>\n ",frm_myurl);


  exit (0);
}



//search by style
void dosearchstyles()
{

  char buf[2048];
  char *restab[30720];
  int i,j,rsize = 0, len;
  FILE *ptr;

  printf ("Content-type: text/html\n\n");
  printf ("<HTML>\n<HEAD>\n<TITLE>MP3 Style Search Results</TITLE>\n");
  printf ("<META HTTP-EQUIV=\"Refresh\" CONTENT=\"%d\">\n", IDLE / 4);
  printf ("<meta HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; "
	  "charset=UTF-8\">\n");
  printf ("</HEAD>\n\n");
  printf (SRCH_BODY_TAG);
  printf ("<a href=\"javascript:history.go(-1)\">[Go Back!]</a>\n");

  /*call egrep here */
  unescape_url (searchitem);
  plustospace (searchitem);
  if (!strlen (searchitem) || strstr (searchitem, ";")
      || searchitem[0] == '\'') {
      strcpy (searchitem, "FUGEPALIOLAMERA");
  }
  sprintf (tmp, "cat %s | /bin/egrep -i '%s' |cut -d: -f1|uniq", 
      STYLELIST, searchitem);
  if ((ptr = popen (tmp, "r")) != NULL)
      while (fgets (buf, 2048, ptr) != NULL) {
	  buf[strlen (buf) - 1] = 0;
	  if (!buf[0])
	      continue;
	  restab[rsize] = (char *) malloc (strlen (buf) + 2);
	  strcpy (restab[rsize], buf);
	  rsize++;
      }
  else {
      printf ("Content-Type: text/plain\n\n");
      printf ("%s:popen:%s", STYLELIST, strerror (errno));
      exit (1);
  }
  printf ("<p><font size=+1><b>%d</b> <b>M</b>atches "
	  "<b>F</b>ound for style \"%s\"</font></p>\n",rsize,searchitem);
  printf ("<p><pre>\n");
  for (i = 0; i < rsize; i++) {
      len = strlen (restab[i]);
      //restab[i][len - 9] = 0;
      for (j = 0; j < len && restab[i][j] != ':'; j++);
      strcpy (tmp, restab[i] + 14);
      escape_url (tmp);
      printf ("<a href=\"%s?path=%s\">%s</a>\n",
	      script_name, tmp, restab[i] + 14);
  }

  printf ("</body>\n</html>\n");
  pclose (ptr);
  exit (0);
}//dosearchstyles

//search for files
void dosearchitem()
{	// && strstr(searchtype,"filenames")) 
  FILE *ptr;
  char buf[2048];
  char *restab[30720];
  float totalsize = 0, totalfree = 0;
  int i,rsize,totalfiles = 0;
  FILE *fp;

  rsize = 0;
  printf ("Content-type: text/html\n\n");
  printf ("<HTML>\n<HEAD>\n<TITLE>MP3 Search Results</TITLE>\n");
  printf ("<META HTTP-EQUIV=\"Refresh\" CONTENT=\"%d\">\n", IDLE / 4);
  printf ("<meta HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; "
	  "charset=UTF-8\">\n");
  printf("<META NAME=\"ROBOTS\" CONTENT=\"NOINDEX, NOFOLLOW\">\n");
  printf(JAVASCRIPTCODE);
  printf(STYLE,A_HOVER,TD_FNTCOLOR);
  printf ("</HEAD>\n\n");
  //printf (BODY_TAG);
  printf (SRCH_BODY_TAG);
  /*call egrep here */
  unescape_url (searchitem);
  plustospace (searchitem);
  strcpy(tmp2,searchitem);
  str_replace("'", "'\\''",searchitem);
  if (!strlen (searchitem) || strstr (searchitem, ";")
      || searchitem[0] == '\'') {
      strcpy (searchitem, "FUGELAMERA");
  }
  str_replace(",", "\\,",searchitem); // for agrep
  sprintf (tmp, GREP" '%s' %s", searchitem, DBFILE);
  if ((ptr = popen (tmp, "r")) != NULL)
      while (fgets (buf, 2048, ptr) != NULL) {
	  buf[strlen (buf) - 1] = 0;
	  restab[rsize] = (char *) malloc (strlen (buf) + 2);
	  strcpy (restab[rsize], buf);
	  rsize++;
      }
  else {
      printf ("Content-Type: text/plain\n\n");
      printf ("%s:popen:%s", DBFILE, strerror (errno));
      exit (1);
  }
  strcpy(searchitem,tmp2);

  printf ("<table width=100%% bgcolor=lightskyblue cellpadding=2>");
  printf("<tr><TD class=tb align=left><font style=\"font-size: 16px\">");
  printf ("<b>%d M</b>atches <b>F</b>ound&nbsp;for<br>"
      "\"%s\"</font>\n&nbsp;",rsize,searchitem);


  if (!strcmp (encoding, "streaming")) { /*Streaming*/
    strcpy(tmp2,"<b>");
    strcpy(tmp3,"");
    strcpy(tmp4,"");
  }
  else if (!strcmp (encoding, "nostreaming")) {	/*Not streaming */
    strcpy(tmp2,"");
    strcpy(tmp3,"<b>");
    strcpy(tmp4,"");
  }
  else { /*Save mode*/
    strcpy(tmp2,"");
    strcpy(tmp3,"");
    strcpy(tmp4,"<b>");
  }
  printf("</td>\n <TD align=left>");
  printf ("%s[<a class=tb href=\""AVSSWWW"cgi-bin/"AVSSCGI".m3u?"
      "%s\">Streaming</a>]</b> ",     tmp2,query_string);
  printf ("%s[<a class=tb href=\""AVSSWWW"cgi-bin/"AVSSCGI".mp3?"
      "%s\">Not&nbsp;Streaming</a>]</b> ", tmp3,query_string);
  printf ("%s[<a class=tb href=\""AVSSWWW"cgi-bin/"AVSSCGI".mpitria?"
      "%s\">Save</a>]</b> ",          tmp4,query_string);
  printf("</td>\n");

  printf("<TD align=left>");
  printf ("<FORM method=\"GET\" ACTION=\""AVSSWWW"cgi-bin/"AVSSCGI".");
  if (!strcmp (encoding, "streaming")) printf ("m3u");
  else if (!strcmp (encoding, "nostreaming")) printf("mp3");
  else printf ("mpitria");
  printf ("\">\n");
  printf ("<TD align=right>Search:</td>\n<TD align=left>");
  printf ("<INPUT accesskey=\"s\" type=\"text\" value=\"%s\" size=\"20\" "
	  "name=\"searchitem\" title=\"Search (Alt+S)\">\n"
	  "<INPUT type=hidden name=path value=\"%s\">\n",
	  searchitem,path);
  highlight(searchitem);

  printf("</td><TD align=left>\n"
	  "<INPUT border=0 type=image src=\""WWW_IMAGEDIR"search.gif\" "
	  "value=Search></FORM></td>\n");


  printf("<TD width=8%% align=left>");
  if (!strcmp (encoding, "streaming")) {
    printf ("<a class=td href=\""AVSSWWW"cgi-bin/"AVSSCGI".m3u" );
    printf ("?ipplaylist=1\">");
    printf ("<b>[Play&nbsp;all]</b></a>\n");
  }
  else
    printf ("&nbsp;");
  printf("</td>\n");
  printf("</tr></table><br>\n");

  printf ("<pre>\n");

//  sprintf (tmp, "%s/%s/results", IPZDIR, remote_addr);
  sprintf (tmp, "%s%s/results", IPZDIR, cookie_avss);
  fp = fopen (tmp, "w");

  for (i = 0; i < rsize; i++) {
      strcpy (tmp2, restab[i]);
      strcpy (tmp3, restab[i]);

      if (tmp2[0]=='#') { //grep info
	int M,E,L;
	sscanf(tmp2,"#%d%d%d",&M,&E,&L);
	printf("\n<b>No exact matches found, but found %d "
	    "match(es) with %d error(s) (limit==%d errors)</b><br>\n",
	    M,E,L);
	continue;
      }


      cutlast (tmp3);
      escape_url (tmp3);	/*directory */
      escape_url (tmp2);	/*song */


      sprintf (tmp4, "%s?path=%s",  script_name, tmp3);

      if (!strcmp (encoding, "streaming")) {
	  sprintf (tmp,"%s?playpathfile=1&sess=%s&path=%s", 
		   script_name, sess,tmp2);
	  fprintf (fp, "http://%s%s?mp3data=1&sess=%s&path=%s\n",
		   http_host,script_name_mp3, sess, tmp2);
      }
      else {	/*not streaming */
	  sprintf (tmp,
		   "%s?playpathfile=1&mp3data=1&sess=%s&path=%s", 
		   script_name, sess, tmp2);
      }

      printf ("<a id=\"add%d\" ",i);
      printf("href=\""WWW_PHPDIR"playlist.php?sess=%s&amp;action=addsong&amp;path=%s\" "
	  "onClick='playlist()' "
	  "target=mp3_playlist ><img border=0 "
	  " align=baseline src=\""WWW_IMAGEDIR"add.gif\" "
	  " alt=\"Add\" title=\"Add to current playlist\"></a>&nbsp;",
	  sess,tmp2);


      unescape_url (tmp3);
      printf ("<A HREF=\"%s\"><font color=\"mediumblue\">%s</a> ", tmp4,
	      tmp3);
      printf ("</font>");
      printf ("<A HREF=\"%s\">%s</a>\n", tmp,
	      (restab[i] + strlen (tmp3)));
  }//for
  fclose(fp);


  if ((fp = fopen (STATISTIK, "r")) == NULL) {
      printf ("Content-Type: text/plain\n\n");
      printf ("%s open:%s", STATISTIK, strerror (errno));
      exit (-1);
  }
  else {
      fgets (tmp, 256, fp);
      sscanf (tmp, "%d", &totalfiles);
      fgets (tmp, 256, fp);
      sscanf (tmp, "%f", &totalsize);
      fgets (tmp, 256, fp);
      sscanf (tmp, "%f", &totalfree);
      fclose (fp);
  }

  printf ("</pre><br><p>\n");

  printf ("<table width=100%% border=0 bgcolor=lightskyblue "
      " cellspacing=2 cellpadding=0>\n" );
  printf("<tr><TD>Total Songs:%d</td>", totalfiles);
  printf ("<TD>Used Megabytes:%.1f</td>\n", totalsize);
  printf ("<TD>Free Megabytes:%.1f</td>\n", totalfree);
  printf ("<TD align=right>Usage:</td>");
  i=(int) ((totalsize * 100) / (totalfree + totalsize));
  printf ("<TD bgcolor=white width=100>"
      "<img src=\""WWW_IMAGEDIR"progress_bar.jpg\" TITLE=%d%% alt=%d%% "
      "border=0 height=20 width=%d> </td></tr>\n",i,i,i);
  printf ("</table>\n\n");


  //printf ("\n<br><a href=\"javascript:history.go(-1)\">[Go Back!]</a>");
  //escape_url(path);
  sprintf (tmp4, "%s?path=%s", script_name,path);
  unescape_url(path);
  printf ("\n<br><a href=\"%s\">[Back to %s]</a>\n",tmp4,path);
  printf ("\n</body></html>\n");
  exit (0);
}//dosearchitem

//whatsnew
void dispwhatsnew()
{
  FILE *fp,*fpr;
  char buf[2048];
  int count=0,count2=0,howmany;
  int onlyperm=0,onlydirs=0;


  printf ("Content-type: text/html\n\n");
  printf ("<HTML>\n<HEAD>\n<TITLE>MP3 What's New</TITLE>\n");
  printf ("<META HTTP-EQUIV=\"Refresh\" CONTENT=\"%d\">\n", IDLE / 4);
  printf ("<meta HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; "
	  "charset=UTF-8\">\n");
  printf ("</HEAD>\n\n");
  printf (SRCH_BODY_TAG);

  sscanf(whatsnew,"%d",&howmany);

  if (strlen(newperm)) onlyperm=1;
  if (strlen(newdirs)) onlydirs=1;

  if (onlydirs)
    strcpy(tmp,NEWDIRS);
  else 
    strcpy(tmp,WHATSNEW);

  if ((fp = fopen (tmp, "r")) == NULL) {
      printf ("Content-Type: text/plain\n\n");
      printf ("%s open:%s", tmp, strerror (errno));
      exit (-1);
  }

  printf ("<center><font size=+1><b>W</b>hat's <b>N</b>ew on"
      " <b>K</b>ill-9 <b>S</b>ite</font>");

  if (onlydirs)
    printf("<p>The %d modified directories", howmany);
  else
    printf("<p>The %d newest files", howmany);

  if (!strcmp (encoding, "streaming")) {
    printf ("<br>\n<a class=td href=\""AVSSWWW"cgi-bin/"AVSSCGI".m3u");
    printf ("?ipplaylist=1\">");
    printf ("<b>[Play&nbsp;all]</b></a>\n");
  }
  printf("<p></center>\n<pre>\n");

  //sprintf (tmp, "%s/%s/results", IPZDIR, remote_addr);
  sprintf (tmp, "%s%s/results", IPZDIR, cookie_avss);
  fpr = fopen (tmp, "w");

  while ((count < howmany) && (count2<5000) &&
	(fgets (buf, 2048, fp) != NULL)) {
      count2++;
      if (onlyperm && strnstr(buf,"/Incoming",30)) continue;
      count++;

      buf[strlen (buf) - 1] = 0;	/*chop */
      strcpy (tmp2, &buf[7]);
      strcpy (tmp3, &buf[7]);
      cutlast (tmp3);
      escape_url (tmp2);	/*Song */
      escape_url (tmp3);	/*Directory */
      sprintf (tmp4, "%s?path=%s",script_name, tmp3);

      if (!onlydirs) {
	if (!strcmp (encoding, "streaming")) {
	    sprintf (tmp, "http://%s%s?path=%s&playpathfile=1", 
		     http_host,script_name, &tmp2[0]);
	    fprintf (fpr, "http://%s%s?mp3data=1&sess=%s&path=%s\n", 
		     http_host,script_name_mp3, sess,tmp2);
	}
	else {		/*not streaming */
	    sprintf (tmp, "%s?path=%s&playpathfile=1&mp3data=1&sess=%s", 
		     script_name, &tmp2[0],sess);
	}
	unescape_url (tmp3);
	strcpy (tmp2, buf);
	tmp2[6] = 0;

	printf ("%s <A HREF=\"%s\">"
	    "<font color=\"mediumblue\">%s</font></a> ", tmp2,tmp4,tmp3);
	printf ("<A HREF=\"%s\">%s</a>\n", tmp, &buf[7 + strlen (tmp3)]);
      }
      else {
	strcpy (tmp, buf); tmp[6] = 0; //date
	unescape_url(tmp3);
	printf ("%s <A HREF=\"%s\">"
	    "<font color=\"mediumblue\">%s</font></a> ",tmp,tmp4,tmp3);

	sprintf (tmp, "%s?path=%s",script_name, tmp2);
	printf ("<A HREF=\"%s\">%s</a>\n", tmp, &buf[7 + strlen (tmp3)]);
      }

  }//while
  fclose (fp);
  fclose (fpr);

  printf ("<p>\n");
  printf ("\n</pre></body></html>\n");
  exit (0);
}//whatsnew

void doplaysimilar()
{
  FILE *pp;
  char buf[2048];
  char cmd[2048];

  strcpy(tmp,path);
  tmp[strlen (tmp) - 1] = 0;
  strcpy(tmp,basename(tmp));
  strcpy(cmd,FINDSIMILAR);
  strcat(cmd,"  '");
  strcat(cmd,tmp);
  strcat(cmd,"'");

  //printf ("Content-Type: text/plain\n\n");
  //printf("cmd:%s#\n<br>",cmd);
  if (!strstr(http_user_agent,"MSIE")) {
    printf ("Content-Type: audio/mpeg-url\n\n");
  }
  else {
    printf ("Content-Type: text/plain\n\n");
  }

  if ((pp = popen (cmd, "r")) != NULL) {
    while (fgets (buf, 2048, pp)) {
      buf[strlen (buf) - 1] = 0;
      escape_url(buf);
      sprintf (tmp4, "http://%s/%s?mp3data=1&sess=%s&path=%s\n", 
	  http_host,script_name_mp3, sess,buf);
      puts(tmp4);
    }
    pclose(pp);
  }
  exit (0);
}//doplaysimilar


void returnstreamurl()
{
  /*we want a reference to the file */
  if (isanogg && !strstr(http_user_agent,"MSIE"))
    printf ("Content-Type: audio/ogg-url\n\n");
  else if (isanmp3 && !strstr(http_user_agent,"MSIE"))
    printf ("Content-Type: audio/mpeg-url\n\n");
  else
    printf ("Content-Type: text/plain\n\n");
  //touch timefile
  //sprintf (tmp, "%s%s/timecheck", IPZDIR, remote_addr);
  sprintf (tmp, "%s%s/timecheck", IPZDIR, cookie_avss);
  utime (tmp, 0x0);

  path[strlen(path)-1]=0;
  escape_url(path);
  if (isanogg && strcmp (encoding, "download"))
    printf ("http://%s/"AVSSWWW"cgi-bin/"AVSSCGI".ogg?mp3data=1&sess=%s&path=%s\n", 
       http_host,sess,path);
  else
    printf ("http://%s%s?mp3data=1&sess=%s&path=%s\n",http_host,script_name,sess,path);
  exit (0);
}//returnstreamurl



//log song hit in database
void db_logplay()
{
  int r;
  sqlite3 *db;
  char *zErrMsg=0;
  int rc;

  strcpy (tmp3, path);
  tobasename (tmp3);
  //log hearing into sqlite db
  //songhits(id numeric, time numeric, ip text, ip2 text, 
  //user text, path text, song text, agent text, xx text)
  rc = sqlite3_open(LOGDB, &db);
  if (!rc) {
    int i;
    //sprintf (tmp4, "%s%s/username", IPZDIR, remote_addr);
    sprintf (tmp4, "%s%s/username", IPZDIR, cookie_avss);
    i = open (tmp4, O_RDONLY);

    if (i) {
      tmp4[0]=0;
      r=read(i,tmp4,sizeof(tmp4));
      tmp4[r]=0;
      close(i);
      tmp4[strlen(tmp4)-1]=0;
      strcpy(username,tmp4);
    }

    strcpy(tmp4,path);
    str_replace("'","''",tmp3);
    str_replace("'","''",tmp4);
    sprintf(tmp2,"insert into songhits values("
	"%ld,%ld,'%s','%s','%s','%s','%s','%s','');",
	0L,time(0),remote_addr,http_x_forwarded_for,username,
	tmp4,tmp3,http_user_agent);
    rc = sqlite3_exec(db, tmp2, 0, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
      fprintf(stderr, "mp3: SQL error: (%s) %s\n",tmp2, zErrMsg);
    }
    sqlite3_close(db);
  }
  else {
    fprintf(stderr, "Can't open database (%s): %s\n",
	LOGDB, sqlite3_errmsg(db));
    sqlite3_close(db);
  }
} //end of db log

int touchnow(char *s)
{
  int r;

  r=utime (s, 0x0);
  if (r < 0) {
    printf ("Content-type: text/plain\n\n");
    printf ("utime(%s): %s\n", s, strerror (errno));
    exit (r);
  }
  return r;
}

int main (int argc, char *argv[])
{
    char ip[32];
    register int i, j, r,w=0;

    struct stat st_s;
    int fd;
    char buffer[135000];
    char *s;
    char str[4096];
    char photo[256];
    char photo_real[384];
    char searchtype[64];
    char playsimilar[8];
    char request_method[16];
    char req_uri[4096];
    char my_url[2048];
    char playpathfile[8];
    char mp3data[8];
    char playlist[8];
    char ipplaylist[8];
    char remote_host[64];
    char koko[8];
    char bio[256];
    char disc[256];
    char info[256];
    char skinname[32];
    char dirartist[128];
    char diralbum[128];
    char artistdir[128];
    char *albumphotos[128];
    char *albumlinks[128];
    char *albumnames[128];
    struct tm * tms;
    struct timeval tv,tv2;
    double timeelapsed;

    /*
#include<sys/resource.h>

    struct rlimit rlim;

    rlim.rlim_cur = rlim.rlim_max = RLIM_INFINITY;
    chdir("/tmp");
    setrlimit(RLIMIT_CORE,&rlim);
    */

    gettimeofday (&tv, NULL);

    tim = time (0);
    setvbuf (stdout, (char *) 0, _IONBF, 0);

    tms=localtime(&tim);
    sprintf(tmp,"mu%d",tms->tm_mday+tms->tm_year*tms->tm_mday*tms->tm_wday);
    strcpy(frm_myurl,tmp);



    ip[0] = str[0] = username[0] = passwd[0] = path[0] = request_method[0] = 
      searchitem[0] = http_user_agent[0]= http_range[0]=myurl[0]= tmp[0] = 
      encoding[0] = http_host[0] = req_uri[0] = script_name[0] = ident[0] = 
    playpathfile[0] = mp3data[0] = remote_addr[0] = remote_port[0] = remote_host[0] 
    = http_x_forwarded_for[0] = http_client_ip[0] = http_cookie[0]=sess[0]=
    playlist[0]=logout[0]=whatsnew[0]=newperm[0]=newdirs[0]=ipplaylist[0] = 
    user[0] = pass[0] = gecos[0] = maker[0] = acl[0] = searchtype[0] = 
    photo[0] = disc[0] = bio[0] = info[0] = skinname[0]= koko[0]= challenge[0]
    =photo_real[0] = playsimilar[0] = dirartist[0]=diralbum[0]=artistdir[0]=0;

    bzero (path, sizeof (path));

    getenvar(ip, "REMOTE_ADDR", 31);
    getenvar(http_user_agent, "HTTP_USER_AGENT", 128);
    getenvar(request_method, "REQUEST_METHOD", 16);
    getenvar(req_uri, "REQUEST_URI", 4096);
    getenvar(http_host, "HTTP_HOST", 64);
    getenvar(script_name, "SCRIPT_NAME", 64);
    getenvar(remote_host, "REMOTE_HOST", 64);
    getenvar(http_range, "HTTP_RANGE", 128);
    getenvar(remote_addr, "REMOTE_ADDR", 128);
    getenvar(remote_port, "REMOTE_PORT", 32);
    getenvar(http_x_forwarded_for, "HTTP_X_FORWARDED_FOR", 64);
    getenvar(http_client_ip, "HTTP_CLIENT_IP", 64);
    getenvar(http_cookie, "HTTP_COOKIE", 1024);

    
    if (strstr (script_name, ".m3u")) {
	strcpy (encoding, "streaming");
	strcpy(script_name_mp3,script_name);
	str_replace(".m3u", ".mp3",script_name_mp3);
    }
    else if (strstr (script_name, ".mp3"))
	strcpy (encoding, "nostreaming");
    else if (strstr (script_name, ".ogg"))
	strcpy (encoding, "nostreaming");
    else
	strcpy (encoding, "download");

    sprintf (my_url, "%s%s", http_host, req_uri);

    if (!strcmp (request_method, "POST")) {
	getline1 (str, 4096, stdin);
	/*Parse the POST parameters */
	s=strtok(str,"&");
	while (s) {
	  varckfill(s,username,"username",64);
	  varckfill(s,passwd,"password",64);
	  varckfill(s,path,"path",2048);
	  varckfill(s,myurl,frm_myurl,128);
	  varckfill(s,challenge,"challenge",128);
	  s = strtok (0, "&");
	}//while
	unescape_url (username);
	unescape_url (passwd);
	unescape_url (myurl);
	unescape_url (path);
	plustospace (path);

	if (path[strlen (path) - 1] != '/')
	    strcat (path, "/");
    }
    else {
      int len=0;
	/*Parse GET parameters */
	if ((s = getenv ("QUERY_STRING"))) {
	    strncpy (query_string, s, 4096);
	    strncpy (str, s, 4096);
	}
	s=strtok(str,"&");
	while(s) {
	  varckfill(s,path,"path",2048);
	  varckfill(s,mp3data,"mp3data",8);
	  varckfill(s,sess,"sess",64);
	  varckfill(s,playpathfile,"playpathfile",8);
	  varckfill(s,playlist,"playlist",8);
	  varckfill(s,ipplaylist,"ipplaylist",8);
	  varckfill(s,logout,"logout",8);
	  varckfill(s,whatsnew,"whatsnew",8);
	  varckfill(s,newperm,"newperm",8);
	  varckfill(s,newdirs,"newdirs",8);
	  varckfill(s,playsimilar,"playsimilar",8);
	  varckfill(s,searchitem,"searchitem",128);
	  varckfill(s,searchtype,"searchtype",64);
	  varckfill(s,skinname,"skinname",32);
	  s = strtok (0, "&");
	}//for
	unescape_url (path);
	len=strlen(path);
	if (path[len - 1] != '/') strcat (path, "/");
	if ((len>4) && ( strstr(&path[len-4], ".mp3")|| 
	      strstr(&path[len-4], ".MP3")))
	    isanmp3 = 1;
	else
	    isanmp3 = 0;

	if (!isanmp3 && (len>4) && (strstr(&path[len-4], ".ogg")))
	    isanogg = 1;
	else
	    isanogg = 0;
    }

    if (!strstr (my_url, "?path=")) {
	strcat (my_url, "?path=");
    }

    //security
    if (strstr (path, "..")) { 
      str_replace("/../", ".",path);
      str_replace("../", ".",path);
    }

    //read cookie_avss if exists
    readcookie_avss();

    if (strlen(sess)) strcpy(cookie_avss,sess);
    else strcpy(sess,cookie_avss);

    //authuser if needed
    if (strlen (passwd) && strlen (username) && strlen(challenge)) 
      authuser(); 


    //no cookie==not logged in
    if (!strlen(cookie_avss)) {
      disploginscr();//set cookie
      printf("No cookie\n");
    }

    //logout user if requested
    if (logout[0]) 
      dologout();

    //check if IP has expired
    if (isexpired()) {
	disploginscr();
	printf("expired\n");
    }


    /* search stuff */
    if (strlen (searchitem) && strstr (searchtype, "styles")) 
      dosearchstyles();
    else if (strlen (searchitem)) 				
      dosearchitem();

    //handle whatsnew requests
    if (strlen (whatsnew)) 
      dispwhatsnew();

    //find and play similar songs
    if (strlen (playsimilar)) 
      doplaysimilar();


    //for streaming, return url
    if (strlen (playpathfile) && (isanmp3||isanogg) && !strlen(mp3data)) 
      returnstreamurl();


    if (strlen (mp3data) && isanmp3) {	/* give us the actual data */
      struct stat st;
      long seekto=0;

      if (strlen(http_range)) 
		sscanf(http_range+6,"%ld",&seekto);

      if (path[strlen (path) - 1] == '/')
	  path[strlen (path) - 1] = 0;

      sprintf (tmp, MP3DIR"%s", path);
      fd = open (tmp, O_RDONLY);
      if (fd < 0) {
	  printf ("Content-type: text/plain\n\n");
	  printf ("open(1) %s: %s\n", tmp, strerror (errno));
	  exit (-4);
      }
      fstat(fd,&st);
      sprintf (tmp4, "%s%s/timecheck", IPZDIR, cookie_avss);
      //utime (tmp4, 0x0);
      touchnow(tmp4);
      strcpy (tmp3, path);
      tobasename (tmp3);

      if (!strcmp (encoding, "download")) {
	  printf ("Content-Length: %ld\n",st.st_size);
	  printf ("Content-disposition: attachment; filename=\"%s\"\n", tmp3);
	  printf ("Content-type: application/octet-stream\n\n");
      }
      else {

	if (strlen(http_range)) {
	  printf("Status: 206 Partial Content\n");
	  printf("Content-Length: %ld\n",st.st_size-seekto);
	  printf("Content-Range: bytes %ld-%ld/%ld\n",
	      seekto,st.st_size,st.st_size-seekto);
	}
	else 
	  printf ("Content-Length: %ld\n",st.st_size);

	if (!strstr(http_user_agent,"MSIE")) {
      strcpy (tmp3, path);
      tobasename (tmp3);
	  printf("Connection: close\n");
	  printf("Server: SivannMp3Server\n");
	  printf("icy-name: %s\n", tmp3);
	  printf("Content-type: audio/mpeg\n\n");

	}
	else 
	  printf ("Content-type: application/mp3\n\n");
      }

      //HTTP_RANGE=bytes=115344-
      if (seekto) 
	 lseek(fd,seekto,SEEK_SET);
      else 
	db_logplay();
      
      //send audio data
      while((r=read (fd, buffer, 131072)) > 0 && (w=write (1, buffer, r))>0);

      //auto-generate an ID3 TAG at the end
      if (w>128 && buffer[w-128]!='T' 
	  && buffer[w-127]!='A' && buffer[w-126]!='G') {
	ID3 id3;

	strcpy (tmp, path);
	tobasename (tmp);
	guessID3(tmp,&id3);
	printID3(&id3);
      }
      /*Refresh time file */
      exit (0);
    }

    else if (strlen (mp3data) && isanogg) {	/* give us the actual ogg data*/
      struct stat st;
      long seekto=0;

	if (strlen(http_range)) {
	  sscanf(http_range+6,"%ld",&seekto);
	}

	if (path[strlen (path) - 1] == '/')
	    path[strlen (path) - 1] = 0;

	sprintf (tmp, MP3DIR"%s", path);
	fd = open (tmp, O_RDONLY);
	if (fd < 0) {
	    printf ("Content-type: text/plain\n\n");
	    printf ("open(2) %s: %s\n", tmp, strerror (errno));
	    exit (-4);
	}
	fstat(fd,&st);

	if (!strcmp (encoding, "download")) {
	    strcpy (tmp, path);
	    tobasename (tmp);
	    printf ("Content-Length: %ld\n",st.st_size);
	    printf ("Content-disposition: attachment; filename=\"%s\"\n", tmp);
	    printf ("Content-type: application/octet-stream\n\n");
	}
	else
	  if (!strstr(http_user_agent,"MSIE")) {
	    if (seekto) {
	      printf ("Content-Length: %ld\n",st.st_size-seekto);
	      printf ("Content-Range: bytes %ld-%ld/%ld\n",seekto,st.st_size,st.st_size-seekto);
	    }
	    else
	      printf ("Content-Length: %ld\n",st.st_size);
	    printf ("Content-type: audio/ogg\n\n");
	  }
	  else {
	    if (seekto) {
	      printf ("Content-Length: %ld\n",st.st_size-seekto);
	      printf ("Content-Range: bytes %ld-%ld/%ld\n",seekto,st.st_size,st.st_size-seekto);
	    }
	    else
	      printf ("Content-Length: %ld\n",st.st_size);
	    printf ("Content-type: application/ogg\n\n");
	  }


	//HTTP_RANGE=bytes=115344-
	if (seekto) lseek(fd,seekto,SEEK_SET);

        sprintf (tmp, "%s%s/timecheck", IPZDIR, cookie_avss);
	touchnow(tmp);

	//ogg
	while((r=read (fd, buffer, 131072)) > 0 && (w=write (1, buffer, r))>0);

	exit (0);
    }
    //ogg end

    else if (strlen (playpathfile) && !isanmp3 && !isanogg) {
	struct stat stp;
	/*handle different file types */
	if (path[strlen (path) - 1] == '/')
	    path[strlen (path) - 1] = 0;
	sprintf (tmp, MP3DIR"%s", path);
	fd = open (tmp, O_RDONLY);
	if (fd < 0) {
	    printf ("Content-Type: text/plain\n\n");
	    printf ("open(3) (%s): %s\n", tmp, strerror (errno));
	    printf ("<pre>DEBUG\n\n");
	    printf ("path:(%s)\n",path);
	    printf ("req_uri:(%s)\n",req_uri);
	    exit (-4);
	}
	//print last-modified and content-length
	fstat (fd, &stp);
	printf("Content-Length: %ld\n",stp.st_size);
	strftime(tmp2,100,"%a, %d %b %Y %H:%M:%S %z",gmtime(&stp.st_mtime));
	printf("Last-Modified: %s\n",tmp2);//h ctime exei kai \n
	printf ("Content-Type: ");
	if (strstr (path, ".jpg")) printf ("image/jpeg\n\n");
	else if (strstr (path, ".JPG")) printf ("image/jpeg\n\n");
	else if (strstr (path, ".gif")) printf ("image/gif\n\n");
	else if (strstr (path, ".png")) printf ("image/png\n\n");
	else if (strstr (path, ".html")) printf ("text/html\n\n");
	else if (strstr (path, ".ogg")) printf ("audio/ogg\n\n");
	else printf ("text/plain\n\n");

	//sprintf (tmp, "%s%s/timecheck", IPZDIR, remote_addr);
        sprintf (tmp, "%s%s/timecheck", IPZDIR, cookie_avss);
	utime (tmp, 0x0);

	while ((r = read (fd, buffer, 131072)) > 0
	       && (write (1, buffer, r)) > 0);
	exit (0);

    }

    /*we want a reference to all links in ip file */
    else if (strlen (ipplaylist)) {
	FILE *fp;

//	sprintf (tmp, "%s/%s/results", IPZDIR, remote_addr);
        sprintf (tmp, "%s%s/results", IPZDIR, cookie_avss);
	fp = (fopen (tmp, "r"));
	if (!strstr(http_user_agent,"MSIE"))
	  printf ("Content-Type: audio/mpeg-url\n\n");
	else
	  printf ("Content-Type: text/plain\n\n");
	while (fgets (tmp, 64, fp))
	    printf ("%s", tmp);
	fclose (fp);
    }

    else if (strlen (playlist)) { /*we want a reference to all files in dir */
	struct dirent *dp;
	struct stat stp;
	DIR *dirp;
	char dir[256];
	char *files[2048];
	int ffsize, i;
	off_t fsize[2048];

	strcpy (dir, MP3DIR);
	strcat (dir, path);
	dirp = opendir (dir);
	if (dirp == 0) {
	    printf ("Content-Type: text/plain\n\n");
	    printf ("2:opendir %s: %s\n", dir, strerror (errno));
	    exit (-2);
	}
	ffsize = 0;
	while ((dp = readdir (dirp)) != NULL) {
	    strcpy (dir, MP3DIR);
	    strcat (dir, path);
	    strcat (dir, dp->d_name);

	    stat (dir, &stp);
	    if (!S_ISDIR (stp.st_mode)) {	/*is a file */
		files[ffsize] =
		    (char *) realloc ((char *) files[ffsize],
				      strlen (dp->d_name) + 2);
		strcpy (files[ffsize], dp->d_name);
		fsize[ffsize] = stp.st_size;
		ffsize++;
	    }
	}
	closedir (dirp);
	/*Sort files */
	if (ffsize - 1 > 0)
	    qsort2 (0, ffsize - 1, files, fsize, 0);

	escape_url (path);

	if (!strstr(http_user_agent,"MSIE"))
	  printf ("Content-Type: audio/mpeg-url\n\n");
	else
	  printf ("Content-Type: text/plain\n\n");

	for (i = 0; i < ffsize; i++) {
	    if (strstr (files[i], ".mp")) {
		strcpy (tmp, files[i]);
		escape_url (tmp);
		printf ("http://%s%s?mp3data=1&sess=%s&path=%s/%s\n", 
			http_host,script_name_mp3,sess,path, tmp);
	    }
	    else if (strstr (files[i], ".ogg")) {
		strcpy (tmp, files[i]);
		escape_url (tmp);
		printf ("http://%s"AVSSWWW"cgi-bin/mp3.ogg?mp3data=1&sess=%s&path=%s/%s\n", 
			 http_host,sess, path, tmp);
	    }
	}

    }	/* end of: if strlen(playlist) */
    else if (strlen (koko)) { /* TEST */
      //Accept-Ranges: bytes
      //Content-Length: 204800
      printf("Connection: close\n");
      //printf ("Content-Type: application/x-tar\n\n");
	    printf ("Content-disposition: attachment; filename=files.tar\n");
	    printf ("Content-type: application/octet-stream\n\n");
      system("tar cvf - /tmp /tmp /tmp /tmp /tmp /tmp /tmp /tmp");
    }

/*host authorized*/
    else {
	struct dirent *dp;
	struct stat stp;
	struct stat stp2;
	int ap=0;
	int fd;
	DIR *dirp;
	char dir[384];
	char *files[2048], *dirs[2048], *photos[256];
	int dirs2di[2048]; //directories 2 discography (di table) mapping 
	char rowcolor[32];
	int ffsize, ddsize, photosize, ignorefiles;
	int lcol,rcol,ddsize2;
	long totfsize,totlen;
	off_t fsize[2048];
	MPEG_HEAD finfo[2048];
	int audioenc = 0;
	char *s,*s1;
	FILE *fp;
	int isperm=0,permdepth=0;;

	strcpy (dir, MP3DIR);
	strcat (dir, path);
	dirp = opendir (dir);
	if (dirp == 0) {
	    printf ("Content-Type: text/plain\n\n");
	    printf ("1:opendir %s: %s\n", dir, strerror (errno));
	    exit (-2);
	}

	if (!dirp)
	    exit (-2);
	ddsize = 0;
	ffsize = 0;
	photosize = 0;
	ignorefiles = 0;
	totfsize = 0;
	totlen = 0;

	/* Read skin information */

        sprintf (tmp, "%s%s/username", IPZDIR, cookie_avss);
	fp=fopen(tmp,"r");
	if (!fp) {
	  printf ("Content-Type: text/plain\n\n");
	  printf ("fopen %s: %s\n", tmp, strerror (errno));
	  exit(0);
	}
	fscanf(fp,"%s",username);
	fclose(fp);

	if (strlen(skinname)) { //set new skin
	  unescape_url(skinname);
	  sprintf (tmp, "%s/%s-skin", USERPREFSDIR, username);
	  fp=fopen(tmp,"w");
	  if (!fp) {
	    printf ("Content-Type: text/plain\n\n");
	    printf ("skin:fopen %s: %s\n", tmp, strerror (errno));
	    //perror(tmp);
	    exit(0);
	  }
	  fprintf(fp,"%s",skinname);
	  strcpy(skin,skinname);
	  fclose(fp);
	}

	sprintf (tmp, "%s/%s-skin", USERPREFSDIR, username);
	r = stat (tmp, &stp);

	/*skin does not exist */
	if (r == 0 ) {
	  fp=fopen(tmp,"r");
	  if (fp) {
	    fscanf(fp,"%s",tmp2);
	    fclose(fp);
	    sprintf(tmp,"%s/%s",SKINZDIR,tmp2);
	    r=readskin(tmp);
	    strcpy(skin,tmp2);
	  }
	  else
	    perror(tmp);
	}
	sprintf(BODY_TAG,
	    "<BODY background=\"%s\" bgcolor=\"%s\" "
	    " LINK=\"%s\" VLINK=\"%s\" ALINK=\"%s\" TEXT=\"%s\">\n",
	    BODY_BACKGROUND,BODY_BGCOLOR,
	    BODY_LINK,BODY_VLINK,BODY_ALINK,BODY_TEXT);


	/* Read directory contents and mp3 file infos*/
	while ((dp = readdir (dirp)) != NULL) {
	    //if (dp->d_name[0]== '.') continue;

	    strcpy (dir, MP3DIR);
	    strcat (dir, path);
	    strcat (dir, dp->d_name);

	    if (strstr (dir, ".mp3"))
		audioenc = 1;
	    else if (strstr (dir, ".ogg"))
		audioenc = 2;
	    else
		audioenc = 0;

	    lstat (dir, &stp);
	    finfo[ffsize].fmode = stp.st_mode;
	    if (S_ISLNK (finfo[ffsize].fmode)) {
		stat (dir, &stp);
		if (S_ISDIR (stp.st_mode))
		    finfo[ffsize].fmode = stp.st_mode;
	    }


	    if (!S_ISDIR (stp.st_mode)) {
		files[ffsize] = (char *) realloc ((char *) files[ffsize],
				      strlen (dp->d_name) + 2);
		strcpy (files[ffsize], dp->d_name);
		fsize[ffsize] = stp.st_size;

		//list of photos
		if (strstr(files[ffsize],"photo.jpg")) {
		  strcpy (tmp, path);
		  strcat (tmp, files[ffsize]);
		  escape_url (tmp);
		  photos[photosize]=(char *)realloc((char *)photos[photosize],
			strlen (tmp)+ 2);
		  strcpy(photos[photosize],tmp);
		  photosize++;
		}

		if (audioenc==1) { //mp3
		    if (!S_ISREG (stp.st_mode)) {
			finfo[ffsize].valid = 0;
			fd = -1;
		    }
		    else
			fd = open (dir, O_RDONLY);
		    if (fd < 0) {
			finfo[ffsize].valid = 0;
		    }
		    else if (S_ISREG (stp.st_mode)) {
			get_mp3header (&finfo[ffsize], stp.st_size, fd);
			close (fd);
		    }
		}
		if (audioenc==2) { //ogg
		    if (!S_ISREG (stp.st_mode)) {
			finfo[ffsize].valid = 0;
			fd = -1;
		    }
		    else
			fd = open (dir, O_RDONLY);
		    if (fd < 0) {
			finfo[ffsize].valid = 0;
		    }
		    else if (S_ISREG (stp.st_mode)) {
			oggnfo (&finfo[ffsize], stp.st_size, fd);
			finfo[ffsize].valid = 1;
			finfo[ffsize].enctype = 2; //ogg
			close (fd);
		    }
		}
		else if (audioenc==0) // non-audio file
		    finfo[ffsize].valid = 0;

		ffsize++;
	    }
	    else {
		dirs[ddsize] =
		    (char *) realloc ((char *) dirs[ddsize],
				      strlen (dp->d_name) + 6); //6 for sorting with album year
		strcpy (dirs[ddsize], dp->d_name);
		ddsize++;
	    }
	}//while readdir
	closedir (dirp);



	//read info fields if exists
	for (i = 0; i < ffsize; i++) {
	  int flen;
	  flen=strlen(files[i]);

	  if ((!bio[0]) && (!strcmp (files[i]+flen-4, ".bio"))){
	    strcpy(bio,files[i]);
	  }
	  else if ((!disc[0])&&(!strcmp (files[i]+flen-5, ".disc"))){
	    strcpy(disc,files[i]);
	    readdisc(&di[0],disc,path);
	  }
	  else if ((!info[0])&&(!strcmp (files[i]+flen-5, ".info"))){
	    strcpy(info,files[i]);
	    readinfo(&artistinfo,info,path); 
	  }
	}


	printf ("Content-type: text/html\n\n");
	printf ("<HTML>\n<HEAD>\n<TITLE>MP3 %s</TITLE>\n", path);
	printf ("<META HTTP-EQUIV=\"Refresh\" CONTENT=\"%d\">\n", IDLE / 4);
	printf ("<meta HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; "
		"charset=UTF-8\">\n");
	printf ("<LINK REL=\"SHORTCUT ICON\" href=\""WWW_IMAGEDIR""FAVICON"\">\n");

	printf(CREDITS);
	printf(JAVASCRIPTCODE);



	/*Sort dirs */
	if (ddsize > 2){
	  if (disize) { //discography exists
	    int t,tmin=99,joftmin=0,notfound=1;

	    for (i = 0; (i < ddsize); i++) {
	      for (j=0,notfound=1,tmin=99;(j<disize);j++) {
		char *p;
		strcpy(tmp2,dirs[i]);
		if ((p=strstr(tmp2," - "))) { //for "album - cd1"
		  *p=0;
		}
		t=ldisti(tmp2,di[j].album); //levenshtein distance

		//printf("<br>DBG%d-%d: %s == %s (%d-%d)",i,j,di[j].album,dirs[i],t,tmin);
		//possible match dir to album
		if ((tmin>=t) && (*dirs[i]!='.') && (t>=0) && (t<3)) {
		  tmin=t;
		  joftmin=j;
		  notfound=0;
		}
		else { //not matched
		  dirs2di[i]=-1;
		  //printf("<br>DBG: %s != %s",di[j].album,dirs[i]);
		}
	      }//for (match)
	      if (notfound) {
		strcpy(tmp,"0000 "); //sort unknown to top to also put "." on top
		strcat(tmp,dirs[i]);
		strcpy(dirs[i],tmp);
	      }
	      else { //found
		//printf("<br>DBG%d-%d: %s == %s",i,joftmin,di[joftmin].album,dirs[i]);
		sprintf(tmp,"%d ",di[joftmin].year);
		strcat(tmp,dirs[i]);
		strcpy(dirs[i],tmp);
		dirs2di[i]=joftmin; //dirs[i] <-> di[dirs2di[i]]
	      }
	    }//for

	    //qsort3 (2, ddsize - 1, dirs, dirs2di);
	    qsort3 (0, ddsize - 1, dirs, dirs2di);
	    //remove added bytes on front that were added for sorting
	    for (i = 0; (i < ddsize); i++) {
	      strcpy(tmp,dirs[i]+5);
	      strcpy(dirs[i],tmp);
	    }
	  } //if disize
	  else {
	    qsort1 (0, ddsize - 1, dirs);
	  }
	} //if ddsize


	/*Sort files */
	if (ffsize - 1 > 0)
	    qsort2 (0, ffsize - 1, files, fsize, finfo);

	/*Sort photos */
	if (photosize - 1 > 0)
	    qsort1 (0, photosize - 1, photos);

	//photo array
	printf("<script>\n  Pictures = new Array(");
	for (i=0;i<photosize;i++) {
	  printf("\"%s?path=%s&playpathfile=1\"",script_name,photos[i]);
	  if (i<photosize-1) printf(",\n");
	}
	printf(");\n</script>\n\n");

	printf(STYLE,A_HOVER,TD_FNTCOLOR);

	printf ("</HEAD>\n\n");
	printf (BODY_TAG);


#ifdef MAINTENANCE
	printf ("<h3>Under Maintenance</h3>\n");
#endif

	//top table
	printf("<table cellspacing=2 width=100%% align=center>"
	    "<tr><TD width=75%% align=left>");

	printf("</td>");

	printf("<TD align=right>");

	//My Incoming
	sprintf (tmp4, "%s/Incoming/%s", MP3DIR, username);
	r = stat (tmp4, &st_s);
	if (!r) {
	  sprintf (tmp4, "%s?path=", script_name);
	  printf ("&nbsp;&nbsp;<a class=tb title=\"Incoming/%s\" "
	      "href=\"%s/Incoming/%s\">",username,tmp4,username);
	  printf("<b>My Incoming</b>");
	  printf("</a> | ");
	}

	printf ("<a class=tb href=\""WWW_PHPDIR"mp3_upload.php\">");
	printf ("<b>Upload</b></a> | ");
	printf ("<a class=tb href=\"%s?logout=1\"><b>Logout</b></a>\n",
		script_name);

	printf("</td></tr></table>\n\n");
	//end of top table


	//main table
	printf("<table border=0 cellspacing=5 bgcolor=\"%s\" "
	    "width=\"100%%\">\n\n <tr>\n<TD valign=top>\n",TBL_BGCOLOR);
	
	printf (" <table COLS=1 cellpadding=2 cellspacing=0 WIDTH=\"100%%\">\n");
	printf (" <TR><TD align=middle WIDTH=26 bgcolor=\"%s\"",HDR_BGCOLOR);
	strcpy (tmp, path);
	escape_url (tmp);
        printf(" onDblClick=\"location.href="
	    "'"WWW_PHPDIR"/fnfix/fnfix.php?sess=%s&amp;path="MP3DIR"%s/'\">",sess,tmp);
	printf ("<a href=\"%s?path=/\">"
	  "<img border=0 src=\""WWW_IMAGEDIR"homebtny.gif\" "
	  "TITLE=Home ALT=Home></a></TD> \n", 
	  script_name);
	    
        printf (" <TD width=\"75%%\" BGCOLOR=%s valign=middle  colspan=2>",
	    HDR_BGCOLOR);

	//trans height
	printf("<img align=left height=22 width=1 "
	      "src="WWW_IMAGEDIR"trans.gif>");

	//open current playlist
	printf("&nbsp;&nbsp;&nbsp;"
	    "<a href=\""WWW_PHPDIR"playlist.php?sess=%s&action=refresh\" "
	    "onClick='playlist()' target=\"mp3_playlist\">"
	    "<img border=0 alt=\"Open playlist\" "
	    "title=\"Open current playlist\" "
	    "\n  align=middle src=\""WWW_IMAGEDIR"playlisty.gif\"></a> ",sess);

	//Play all
	if (!strcmp (encoding, "streaming")) {
	  printf ("&nbsp;&nbsp;&nbsp;<a href=\""AVSSWWW"cgi-bin/"AVSSCGI".m3u");
	  printf ("?path=%s&playlist=1\" class=tb>", tmp);
	  printf ("<font color=\"%s\"><b>[Play&nbsp;all]</b></font></a>\n",
	      HDR_FNTCOLOR);
	}


	//INFO
	printf ("</TD>\n <TD bgcolor=%s><span class=big3 style='color:%s'>"
	    "INFO</span>&nbsp;</td></tr>\n",HDR_BGCOLOR,HDR_FNTCOLOR);

	/*Directories */
	//print current directory in link slices
	for (i = 0; i < ddsize; i++) {
	    if (!strcmp (dirs[i], "..")) {
		printf ("\n <TR bgcolor=%s>\n  "
		    "<TD WIDTH=\"20\"><A HREF=\"", DROW1_BGCOLOR);
		/* strcpy(tmp,my_url);*/
		strcpy (tmp3, path);
		unescape_url (tmp3);
		cutlast (tmp3);
		escape_url (tmp3);
		sprintf (tmp, "%s?path=%s", script_name,
			 tmp3);
		printf ("%s", tmp);

		printf ("\"><IMG BORDER=0 ALT=\"[DIR]\" "
		    "SRC=\""WWW_IMAGEDIR"dirup_o.gif\"></A></TD>\n");

		printf("<TD colspan=2>");

		//strip dipla slashes
		for (j=0,s=path;*s;j++,s++) {
		  if ((j==0) && (path[j]!='/')) tmp[j++]='/'; //prepend / to path if missing
		  while (s[0]=='/' && s[1]=='/') s++;
		  tmp[j]=*s;
		}
		tmp[j]=0;

		for ((s=strchr(tmp,'/'));s ;s=strchr(s+1,'/')) {
		  permdepth++;
		  strcpy(tmp2,"/");
		  strncat(tmp2,tmp+1,(s-tmp));
		  tmp2[j=strlen(tmp2)-1]=0;
		  s1=strrchr(tmp2,'/');
		  printf("<b>");
		  if (s1) {
		    strcpy(tmp3,s1+1); 
		    escape_url(tmp2);
		    printf("<a class=tb href=\"%s?path=%s\">%s/</a>",
		      script_name,tmp2,tmp3);

		    if (isperm==2 && dirartist[0]) strcpy(diralbum,tmp3);
		    if (isperm==2 && !dirartist[0]) {
		      strcpy(artistdir,tmp2);
		      strcpy(dirartist,tmp3);
		    }
		    if (isperm==1 && !dirartist[0])  isperm++;
		    if (strstr(tmp3,"artists")) isperm++;
			//printf("s1:%s,tmp3:%s,ISPERM:%d,dirartist:%s\n",s1,tmp3,isperm,dirartist);
		  }
		  else 
		    printf("<a class=tb href=\"%s?path=/\">/</a>",
			script_name);
		  printf("</b>");
		}//for s=strchr

		//if (!isperm) permdepth=0; 
		
		printf("&nbsp;</TD>");
		printf("<TD>&nbsp;</TD>\n </TR>\n");
		continue;
	    } //dir up + dir link slices


	    //find album photos links
	    if (dirartist[0] && !diralbum[0]) {
	      sprintf (tmp3, "%s%s%s/00photo.jpg", MP3DIR,path, dirs[i]);
	      sprintf(tmp,"%s%s",path,dirs[i]); escape_url(tmp);
	      r = stat (tmp3, &stp2);
	      if (r>=0) { //with photo
			sprintf (tmp2, "%s?path=%s/00photo.jpg&playpathfile=1", 
		    script_name,tmp);
	      }
	      else { // no photo available
			strcpy(tmp2,WWW_IMAGEDIR"/nocover.jpg");
	      }

	      sprintf (tmp3,"%s?path=%s",script_name,tmp);

	      albumphotos[ap]=malloc(strlen(tmp2)+2);
	      strcpy(albumphotos[ap],tmp2);

	      albumlinks[ap]=malloc(strlen(tmp3)+2);
	      strcpy(albumlinks[ap],tmp3);

	      albumnames[ap]=malloc(strlen(dirs[i])+2);
	      strcpy(albumnames[ap],dirs[i]);
	      ap++;
	    }
	} // for ddsize

	//print directories
	if ((ddsize>20))
	  for (i = 0,ddsize2=ddsize-2; i < (ddsize2/2+ddsize2%2)+2; i++) {
	    if (dirs[i][0]=='.') continue;
	    if (i%2) strcpy(rowcolor,DROW1_BGCOLOR);
	    else strcpy(rowcolor,DROW2_BGCOLOR);

	    j=ddsize2/2+(ddsize2%2)+i;

	    lcol=i;
	    rcol=j;

	    //print directories
	    printf ("\n <TR bgcolor=%s>\n  <TD  WIDTH=\"20\"><A HREF=\"",
	    rowcolor);
	    strcpy (tmp2, dirs[lcol]); escape_url (tmp2);
	    strcpy (tmp3, path); escape_url (tmp3);
	    sprintf (tmp, "%s?path=%s%s%s%s",  script_name,
		     tmp3, my_url[strlen (my_url) - 1] == '/' ? "" : "/",
		     tmp2, tmp2[strlen (tmp2) - 1] == '/' ? "" : "/");
	    printf ("%s", tmp);
	    printf ("\" class=tbA><IMG   BORDER=0 ALT=\"[DIR]\" "
		 " SRC=\""WWW_IMAGEDIR"dir_o.gif\"></a></TD>\n");
	    printf("<TD width=45%%><A HREF=\"");
	    printf ("%s", tmp);
	    printf ("\" class=tb><b>%s</b></a>\n", dirs[lcol]);

	    if (disc[0] && (dirs2di[i]>=0)){
	      printf("<img src='"WWW_IMAGEDIR"stars%d.gif' "
		"title=\"%s - %d/9\"> %d",
	      di[dirs2di[i]].stars,di[dirs2di[i]].album,
	      di[dirs2di[i]].stars, di[dirs2di[i]].year);
	    }

	    printf("</td>\n");


	    if (rcol>=ddsize) printf("<TD>&nbsp;</TD>\n");
	    else {
	      strcpy (tmp2, dirs[rcol]); escape_url (tmp2);
	      strcpy (tmp3, path); escape_url (tmp3);
	      sprintf (tmp, "%s?path=%s%s%s%s",  script_name,
		       tmp3, my_url[strlen (my_url) - 1] == '/' ? "" : "/",
		       tmp2, tmp2[strlen (tmp2) - 1] == '/' ? "" : "/");
	      printf ("<TD width=45%%><A HREF=\"%s\">", tmp);
	      printf ("<IMG  align=absmiddle BORDER=0 ALT=\"[DIR]\" "
		   " SRC=\""WWW_IMAGEDIR"dir_o.gif\"></a>&nbsp;");
	      printf("<A HREF=\"");
	      printf ("%s", tmp);
	      printf ("\" class=tb><b>%s</b></a>\n", dirs[rcol]);

	      if (disc[0] && (dirs2di[i]>=0)){
		printf("<img src='"WWW_IMAGEDIR"stars%d.gif' "
		  "title=\"%s - %d/9\"> %d",
		di[dirs2di[i]].stars,di[dirs2di[i]].album,
		di[dirs2di[i]].stars, di[dirs2di[i]].year);
	      }
	      printf("</td>\n");

	    }

	    printf ("  <TD>&nbsp;</TD>\n </TR>\n");//info column
	} //for two columns
	else  //one column directories
	  for (i = 0;i < ddsize;i++) {
	    if (dirs[i][0]=='.') continue;
	    lcol=i;
	    if (i%2) strcpy(rowcolor,DROW1_BGCOLOR);
	    else strcpy(rowcolor,DROW2_BGCOLOR);
	    printf ("\n <TR bgcolor=%s>\n <TD WIDTH=\"20\"><A HREF=\"",
	    rowcolor);
	    strcpy (tmp2, dirs[lcol]); escape_url (tmp2);
	    strcpy (tmp3, path); escape_url (tmp3);
	    sprintf (tmp, "%s?path=%s%s%s%s",  script_name,
		     tmp3, my_url[strlen (my_url) - 1] == '/' ? "" : "/",
		     tmp2, tmp2[strlen (tmp2) - 1] == '/' ? "" : "/");
	    printf ("%s", tmp);
	    printf ("\" class=tbA><IMG   BORDER=0 ALT=\"[DIR]\" "
		 " SRC=\""WWW_IMAGEDIR"dir_o.gif\"></a></TD>\n");
	    printf("<TD colspan=2><A HREF=\"");
	    printf ("%s", tmp);
	    printf ("\" class=tb><b>%s</b></a></TD>\n", dirs[lcol]);

	    if (disc[0] && (dirs2di[i]>=0) && !di[dirs2di[i]].stars){
	      printf("<TD>  %d</TD>\n </TR>\n",di[dirs2di[i]].year);
	    }
	    else if (disc[0] && (dirs2di[i]>=0)){
	      printf("<TD><img src='"WWW_IMAGEDIR"stars%d.gif' "
		  "title=\"%s - %d/9\">  %d</TD>\n </TR>\n",
		di[dirs2di[i]].stars,di[dirs2di[i]].album,
		di[dirs2di[i]].stars, di[dirs2di[i]].year);
	    }
	    else
	      printf ("  <TD>&nbsp;</TD>\n </TR>\n");//info column
	  }

	//Directories

	/*Files */
	for (i = 0; i < ffsize; i++) {
	  int flen;
	  flen=strlen(files[i]);

	  if ((i%2)^(ddsize%2))
	    strcpy(rowcolor,FROW2_BGCOLOR);
	  else
	    strcpy(rowcolor,FROW1_BGCOLOR);

	  if (strstr (files[i], "photo.jpg") 
	    || !strcmp(files[i],photo_real)) {
	      ignorefiles++;
	      strcpy (photo, path);
	      strcat (photo, files[i]);
	      sprintf(tmp,MP3DIR"%s",photo);
	      bzero (photo_real, sizeof (photo_real));
	      readlink(tmp, photo_real, 384);
	      //printf("photo:%s,realphoto:%s<br>\n",tmp,photo_real);
	      continue;
	  }

	  if (files[i][0]=='.') {
	    ignorefiles++;
	    continue;
	  }

	  if (S_ISLNK (finfo[i].fmode) && !S_ISDIR (finfo[i].fmode)) {
	      strcpy (tmp2, MP3DIR);
	      strcat (tmp2, path);
	      strcat (tmp2, files[i]);
	      r = readlink (tmp2, tmp3, 384);
	      tmp3[r] = 0;
	      
	      cutlast (tmp3);
	      sprintf (tmp, "%s?path=%s", script_name,
		       &tmp3[14]);
	      printf (" <TR bgcolor=%s>\n <TD> <a href=\"%s\">", 
		  rowcolor,tmp);
	      printf ("<IMG BORDER=0 TITLE=\"%s\" SRC=\""WWW_IMAGEDIR"cdrom.gif\">"
		      "</a></TD>", &tmp3[14]);
	      printf ("   <TD colspan=2>");
	  }
	  else {
	    strcpy (tmp2, files[i]);
	    escape_url (tmp2);
	    strcpy (tmp3, path);
	    escape_url (tmp3);
	    if (!strstr (tmp2, ".mp3")) {
	      printf ("<TR bgcolor=%s><TD>&nbsp;</TD>"
		  "<TD colspan=2>",rowcolor);
	    }
	    else {

	      sprintf (tmp, "%s%s",tmp3,tmp2);

	      printf (" <TR bgcolor=%s><TD style='width:35px'>",
		  rowcolor);
	      printf ("<a id=\"add%d\" ",i);
	      printf("href=\""WWW_PHPDIR"playlist.php?sess=%s&action=addsong&path=%s\" "
		"onClick='playlist()' target=mp3_playlist > "
		"<img border=0 src=\""WWW_IMAGEDIR"add.gif\" alt=\"Add\" "
		"Title=\"Add to current playlist\"></a>" ,sess,tmp);
	      printf("<img style='margin-left:3px' title='Save song' onclick='saveno(%d);' align=top "
		  "border=0 src=\""WWW_IMAGEDIR"save.png\">",i);
		printf("</TD>\n     <TD colspan=2>");
	    }
	  }

	  strcpy (tmp2, files[i]);
	  escape_url (tmp2);
	  strcpy (tmp3, path);
	  escape_url (tmp3);

	  if (!strstr (tmp2, "mp3")) {
	      j = 1;
	      strcpy (tmp4, "<font color=magenta>");
	  }
	  else {
	      tmp4[0] = 0;
	      j = 0;
	  }

	  printf ("<A TITLE=\"%ld&nbsp;Bytes&nbsp;&nbsp;"
	      "%.2f KB&nbsp;&nbsp;%.2f MB\" HREF=\"",
	      fsize[i],fsize[i]/2048.0,fsize[i]/1048576.0);
	  sprintf (tmp, "path=%s%s%s", 
	       tmp3, my_url[strlen (my_url) - 1] == '/' ? "" : "/", tmp2);

	  if (finfo[i].valid) {  /*valid */
	      if (!strcmp (encoding, "streaming")) {
		printf("%s?playpathfile=1&%s", script_name,tmp);
		printf ("\" class=tb> %s </a></TD><TD>", files[i]);
	      }
	      else if (finfo[i].enctype==1 || !strcmp(encoding,"download")){	/*Not streaming||save mode, valid mp3||ogg*/ 
		printf("%s?playpathfile=1&mp3data=1&sess=%s&%s", script_name,sess,tmp);
		  printf ("\" class=tb>%s</a></TD><TD>", files[i]);
	      }
	      else if (strcmp(encoding,"download")  && finfo[i].enctype==2){	/*Not streaming, valid ogg */ 
		printf(""AVSSWWW"cgi-bin/mp3.ogg?playpathfile=1&mp3data=1&sess=%s&%s", sess,tmp);
		  printf ("\" class=tb>%s</a></TD><TD>", files[i]);
	      }
	      else {	
		printf("\"/a>%s: unknown enctype:%d<br>\n",tmp,finfo[i].enctype);
	      }

	      if (!finfo[i].vbr) /*cbr*/
		  printf ("%02d:%02d&nbsp;&nbsp;&nbsp;&nbsp;[%03dKbps]",
			  finfo[i].mins, finfo[i].secs,
			  finfo[i].bitrate);
	      else { /*vbr*/

		if (finfo[i].mins+finfo[i].secs){
		  finfo[i].vbr=fsize[i]*8/(60*finfo[i].mins+finfo[i].secs);
		  finfo[i].vbr/=1000;
		}
		else
		  finfo[i].vbr=0;

		printf ("%02d:%02d&nbsp;&nbsp;&nbsp;&nbsp;"
		    "[%03d&nbsp;&nbsp;<span class=ol>VBR</span>]", 
		      finfo[i].mins,finfo[i].secs,finfo[i].vbr);
	      }
	      printf ("</TD>\n </TR>\n");
	  }
	  else {/*not valid*/
	      if (!strcmp (encoding, "streaming")) {
		/*straming not valid */
		printf("%s?playpathfile=1&%s", script_name,tmp);

		  printf ("\" class=tbA>%s</a>"
		      "</TD><TD>%ld&nbsp;Bytes", files[i], fsize[i]);
		  printf ("</TD>\n  </TR>\n");
	      }
	      else {
		/*Not streaming not valid */
		printf("%s?playpathfile=1&%s", script_name,tmp);

		  printf ("&mp3data=1&sess=%s\" class=tbA>%s</a>"
		      "</TD><TD>%ld&nbsp;Bytes", sess,files[i], fsize[i]);
		  printf ("</TD>\n  </TR>\n");
	      }
	  }
	  totfsize += fsize[i];
	  totlen += finfo[i].mins*60+finfo[i].secs;
	} //Files

	printf("<tr bgcolor=\"%s\">",HDR_BGCOLOR);
	printf("<TD colspan=4><table cellspacing=0 cellpadding=1"
	    " width=100%%><tr><TD align=left>");
	//Add All
	printf("<a href=\"javascript:void(0)\" class=tb "
	  " title=\"Add all to current playlist\" onclick=\"addall()\">"
	  "<font color=%s>[Add&nbsp;all to current playlist]</font></a>",HDR_FNTCOLOR);

	//download album
	if (permdepth>4) {
	  printf(" <a class=tb href=\""WWW_PHPDIR"/mp3_getdir.php?sess=%s&dir=%s\">"
	      "<font color=%s>[Download all]</font></a>",sess,path,HDR_FNTCOLOR);

	}

	printf("</td><TD align=right>");
	printf("<font color=%s><b>",HDR_FNTCOLOR);
	printf("Total:&nbsp;[%02ld:%02ld],&nbsp;",totlen/60,totlen%60);
	if (ffsize-ignorefiles==1)
	  printf(" %d file&nbsp;", ffsize-ignorefiles);
	else
	  printf(" %d files&nbsp;", ffsize-ignorefiles);
	printf("%02.2fMB",(float) (totfsize / 1000000.0));
	printf("</b></td></tr></table></td></tr>");

	printf (" </table>\n");
	
	strcpy (tmp, path);
	escape_url (tmp);


	//Also by this artist
	if (ap) {
	  int i,isgood;

	  printf("<br>\n  <!-- album photos -->\n <table> <tr>");

	  for (i=0;i<ap;i++) {
	    printf("\n<TD width=110 align=center class=tb valign=top>"
		"<a class=tb href=\"%s\"><img width=100 "
		"src=\"%s\"></a><br>", 
		albumlinks[i],albumphotos[i]);
	    sprintf(tmp,"%s&playlist=1",albumlinks[i]);
	    printf("<a class=tb href=\"%s\">"
		"<img border=0 src=\""WWW_IMAGEDIR"/speaker.gif\"></a> ", tmp);

	    if (disc[0] && (dirs2di[i+2]>=0)) //+2 because of .,..
	      isgood=(di[dirs2di[i+2]].stars>6);
	    else isgood=0;

	    if (isgood) printf("<b>");
	    printf("<a class=tb href=\"%s\">%s</a></td>",
		albumlinks[i],albumnames[i]);
	    if (isgood) printf("</b>");

	    if (!((i+1)%5)) printf("</tr>\n\n  <tr>");
	  }
	  printf("</tr></table>\n");
	  printf("<!-- /album photos -->\n");

	}

	//BIOGRAPHY
	if (bio[0]) {
	  printf("  <BR><table><TR><TD bgColor=%s>", HDR_BGCOLOR);
	  printf("<span style='color:%s' class=big3>"
	      "<BIG>B</BIG>IOGRAPHY</span></TD></TR>\n",HDR_FNTCOLOR);

	  printf("  <tr><TD bgColor=%s><font color=%s>\n      ",
	      BIO_BGCOLOR,BIO_FNTCOLOR);
	  printbio(bio,path); 
	  printf("</td></tr></table>\n");

	}
	printf("<br>\n");
	showrecent();


	printf ("</td>\n");

	//Right column start
	printf("<TD valign=top width="RTABLE_WIDTH">\n");

	printf("\n<!-- Start of Info Column -->\n");
	printf("<table border=0 cellpadding=2 cellspacing=0 "
	    "width=\"100%%\">\n");
	
	/*Group image*/  //PHOTO
	if (photosize) {
	    //char *p;
	    strcpy(photo,photos[0]);
	    //printf("PS:%d, photo=%s\n<br>",photosize,photos[0]);
	    printf("  <TR><TD bgcolor=\"%s\"><FONT color=\"%s\">",
		HDR_BGCOLOR,HDR_FNTCOLOR);
	    printf("<img align=left height=22 width=1 "
		  "src=\""WWW_IMAGEDIR"trans.gif\">");
	    printf("<span class=big2>%s</span></TD></TR>",dirartist);
	
	    printf("  <TR><TD height=200 bgcolor=\"%s\" align=top>",
		PHOTO_BGCOLOR);

	    sprintf(tmp,"%s?path=%s&playpathfile=1",script_name,photos[0]);
	    printf ("<a id=photolink href=\"%s\">",tmp);
	    printf ("\n<img id=\"photoimg\" border=0 width="RTABLE_WIDTH" "
		"src=\"%s\">",tmp);
	    printf("</a>\n");
	    printf("</TD></TR>");
	    printf("  <TR><TD bgcolor=\"%s\" align=center>",
		PHOTO_BGCOLOR);
	
	    if (photosize)
	      printf("<a class=tb href='javascript:previmage();'>&lt;</a> ");
	    for (i=0;i<photosize;i++) {
	      printf ("<a class=tb "
		  "href='javascript:showimage(\"%d\");'>",i);
	      printf ("%d</a>",i+1);
	      if (i!=photosize-1) printf("|");
	    }
	    if (photosize)
	      printf(" <a class=tb href='javascript:nextimage();'>&gt;</a>");

	    printf("\n");

	    printf("</TD></TR>\n");
	    printf("<TR><TD><span class=big2>%s</span><br></TD></TR>\n",
		diralbum);
	    printf("  <TR><TD><img height=1 src="WWW_IMAGEDIR"trans.gif>"
		"</TD></TR>\n");
	}

	//SEARCH
	printf("  <TR><TD bgColor=%s>", HDR_BGCOLOR);
	  printf("<span style='color:%s' class=big3>"
	      "<BIG>S</BIG>EARCH</span>\n",HDR_FNTCOLOR);

	//Search Help
	printf("  <A class=tb href=\"/regex.html\"><font color=\"%s\">"
	    "?</font></A>"
	    "<BR><img src=\""WWW_IMAGEDIR"trans.gif\" width="RTABLE_WIDTH" "
	    "height=1><br>\n  " ,HDR_FNTCOLOR); 
	printf( "  </TD></TR>\n"
	    "  <TR><TD align=middle height=50 bgColor=\"%s\" >\n"
	    "    <table border=0 cellpadding=1 cellspacing=0 width=\"90%%\">\n"
	    "      <TR><TD align=center><FORM name=lala>\n"
	    "          <INPUT accesskey=\"s\" name=searchitem title=\"Alt+S\" "
	    " value=\"Search Files\" "
	    " onfocus=\"javascript:if(this.value=='Search Files') "
	    "{this.value='';}\" ",
	    TDSEARCH_BGCOLOR);

	if (strstr(http_user_agent,"Mozilla/4.7"))
	  printf("size=10 >\n");
	else
	  printf("style='width:100%%' >\n");

	strcpy (tmp, path);
	escape_url (tmp);

	printf("        <INPUT type=hidden name=searchtype value=filenames>\n"
	  "	  <INPUT type=hidden name=path value=\"%s\">\n"
	  "          </FORM></TD>\n"

	  "      </TR>\n"
	  "      <TR><TD align=center><FORM name=lala2>\n"
	  "          <INPUT name=searchitem ",tmp);
	  printf(" value=\"Search Styles\" "
	  " onfocus=\"javascript:if(this.value=='Search Styles') "
	  "{this.value='';}\" ");

	if (strstr(http_user_agent,"Mozilla/4.7"))
	  printf("size=10 >\n");
	else
	  printf("style='width:100%%' >\n");


	printf("          <INPUT type=hidden name=searchtype value=styles>\n "
	  "          </FORM></TD>\n"
	  "      </TR>\n"
	  "    </table>\n");

	//WHATSNEW
	printf("   [<A class=tb_sm href=\""AVSSWWW"cgi-bin/"AVSSCGI".m3u?");
	printf("whatsnew=50\">50</A>");
	printf("   <A class=tb_sm href=\""AVSSWWW"cgi-bin/"AVSSCGI".m3u?");
	printf("whatsnew=250\">250</A>");
	printf("   <A class=tb_sm href=\""AVSSWWW"cgi-bin/"AVSSCGI".m3u?");
	printf("whatsnew=500\">500</A>");
	printf("   <A class=tb_sm href=\""AVSSWWW"cgi-bin/"AVSSCGI".m3u?");
	printf("whatsnew=1000\">1000</A>");
	printf(" Newest]");
	printf(" <br>\n");

	//what's new on permanent
	printf("   [<A class=tb_sm href=\""AVSSWWW"cgi-bin/"AVSSCGI".m3u?"); 
	printf("whatsnew=100&newperm=1\">100</A>");
	printf("   <A class=tb_sm href=\""AVSSWWW"cgi-bin/"AVSSCGI".m3u?"); 
	printf("whatsnew=500&newperm=1\">500</A>");
	printf("   <A class=tb_sm href=\""AVSSWWW"cgi-bin/"AVSSCGI".m3u?") ; 
	printf("whatsnew=1000&newperm=1\">1000</A>");
	printf(" Perm. New]");
	printf(" <br>\n");

	//only dirs
	printf("   [<A class=tb_sm href=\""AVSSWWW"cgi-bin/"AVSSCGI".m3u?"); 
	printf("whatsnew=100&newdirs=1\">100</A>");
	printf("   <A class=tb_sm href=\""AVSSWWW"cgi-bin/"AVSSCGI".m3u?"); 
	printf("whatsnew=500&newdirs=1\">500</A>");
	printf("   <A class=tb_sm href=\""AVSSWWW"cgi-bin/"AVSSCGI".m3u?") ; 
	printf("whatsnew=1000&newdirs=1\">1000</A>");
	printf(" New Dirs.]");
	printf(" <br>\n");

	//letters
	printf("<br>");
	printf("   <a class=tb href=\"%s/?path=/music/artists/%%23\">#</a>\n",
	      script_name);
	for (i='A';i<='Z';i++)
	  printf("   <a class=tb href=\"%s/?path=/music/artists/%c\">%c</a>\n",
	      script_name,i,i);
	// /leters

	printf("  </TD></TR>\n");
	printf("  <TR><TD><img height=0 src="WWW_IMAGEDIR"trans.gif></TD>"
	    "</TR>\n");

	//MODE
	printf("  <tr><td bgColor=%s>", HDR_BGCOLOR);
	  printf("<span style='color:%s' class=big3>"
	      "<BIG>M</BIG>ODE</span></td></tr>\n",HDR_FNTCOLOR);

	printf("  <tr><TD bgColor=%s>\n      ",INFO_BGCOLOR);

	strcpy (tmp, path);
	escape_url (tmp);

	if (!strcmp (encoding, "streaming")) { /*Streaming*/
	  strcpy(tmp2,"&#8226;");
	  strcpy(tmp3,"&nbsp;");
	  strcpy(tmp4,"&nbsp;");
	}
	else if (!strcmp (encoding, "nostreaming")) {	/*Not streaming */
	  strcpy(tmp2,"&nbsp;");
	  strcpy(tmp3,"&#8226;");
	  strcpy(tmp4,"&nbsp;");
	}
	else { /*Save mode*/
	  strcpy(tmp2,"&nbsp;");
	  strcpy(tmp3,"&nbsp;");
	  strcpy(tmp4,"&#8226;");
	}
	printf ("%s<a class=tb href=\""AVSSWWW"cgi-bin/"AVSSCGI".m3u?"
	    "path=%s\">Streaming</a><br>\n", tmp2,tmp);
	printf ("%s<a class=tb href=\""AVSSWWW"cgi-bin/"AVSSCGI".mp3?"
	    "path=%s\">Not Streaming</a><br>\n", tmp3,tmp);
	printf ("%s<a class=tb href=\""AVSSWWW"cgi-bin/"AVSSCGI".mpitria?"
	    "path=%s\">Save</a><br>\n",          tmp4,tmp);

	//SIMILAR - related artists
	if (info[0]) {
	  printf("  <TR><TD valign=middle bgColor=%s>", HDR_BGCOLOR);
	  printf("<span style='color:%s' class=big3>"
	      "<BIG>S</BIG>IMILAR</span>\n",HDR_FNTCOLOR);

	  printf (" <a class=tb href=\"%s?path=%s&playsimilar=1\">" 
	      "<font color=\"%s\"><b>[Play]</b></font></a>",
	      script_name,path,HDR_FNTCOLOR);
	  printf (" <a class=tb title=\"Add similar songs to current "
	      "playlist\" target=\"mp3_playlist\" onClick='playlist()' "
	      "href=\""WWW_PHPDIR"playlist.php?path=%s&action=addsimilar\">" 
	      "<font color=\"%s\"><b>[Add to PL]</b></font></a>",
	      path,HDR_FNTCOLOR);
	  printf("  </TD></TR>\n");
	  printf("  <tr><TD bgColor=%s>\n      ",INFO_BGCOLOR);
	  seealso(path); 
	}

	//INFO
	if (info[0]) {
	  printf("  <TR><TD bgColor=%s>", HDR_BGCOLOR);
	  printf("<span class=big3><BIG>I</BIG>NFO</span></td></tr>\n");
	  printf("  <tr><TD bgColor=%s>\n      ",INFO_BGCOLOR);
	  printinfo(&artistinfo); 
	}

	//DISCOGRAPHY
	if (disc[0]) { 
	  printf("  <TR><TD bgColor=%s>", HDR_BGCOLOR);
	  printf("<span class=big3><BIG>D</BIG>ISCOGRAPHY</span></td></tr>\n");
	  printf("  <tr><TD bgColor=%s>\n      ",INFO_BGCOLOR);
	  printdisc(); 
	}

	printf("  </TD></TR>\n  </table>\n");
	printf("\n<!-- End of Info Column -->\n");

	printf("    </TD></TR>\n"
	    "</table>\n");

	if (strlen (http_x_forwarded_for)) {
	    printf ("<font color=red><b>proxy detected</b></font><br>");
	}

	if (strlen(path)<3) { //home page, start
	  DIR * dirp;
	  struct dirent *dp;
	  //select skin
	  printf("\nSkin:<form name=skinform>\n"
	      "<select name=skinname>\n");
	  printf("<option value=\"none\" >none\n");
	  dirp = opendir (SKINZDIR);

	  if (dirp == 0) {
	      printf ("Content-Type: text/plain\n\n");
	      printf ("skins:opendir %s: %s\n", SKINZDIR, strerror (errno));
	      exit (-2);
	  }

	  while ((dp = readdir (dirp)) != NULL) {
	    if (dp->d_name[0]== '.')
		continue;
	    strcpy (tmp, dp->d_name);
	    escape_url(tmp);
	    if (!strcmp(skin,dp->d_name))
	      printf("<option value=\"%s\" selected>%s\n",tmp,dp->d_name);
	    else
	      printf("<option value=\"%s\">%s\n",tmp,dp->d_name);
	  }

	  printf("</select>\n");
	  printf("<input type=submit value=\"Set\">\n");
	  printf("</form>\n\n");

	  printf("<a href='"WWW_PHPDIR"showmyhist.php?username=%s'>"
	      "My History</a><br>\n",username);

	  readfile();
	}

	printf("\n<form method=POST name=\"addallform\" "
	  "target=\"mp3_playlist\" "
	  "action=\""WWW_PHPDIR"playlist.php\">\n"
	  "<input type=\"HIDDEN\" name=\"ppath\" >\n"
	  "<input type=\"HIDDEN\" name=\"sess\" value='%s' >\n"
	  "<input type=\"HIDDEN\" name=\"action\" value=addsong>\n"
	  "</form>\n",sess);

	 /*
	 */
#ifdef MAINTENANCE
	printf("<p>\n");
	printf ("<br>Username:#%s#, Pass:#%s#<br>\n",username,passwd);
	printf ("Path:#%s#<br>\n",path);
	printf ("Skinname:#%s#<br>\n",skinname);
	printf ("Searchitem:#%s#<br>\n",searchitem);
	printf ("request_method:%s<br>\n",request_method);
	printf ("tmp:%s<br>\n",tmp);
	printf ("my_url:%s<br>\n",my_url);
	printf ("req_uri:%s<br>\n",req_uri);
	printf ("script_name:%s<br>\n",script_name);
	printf ("encoding:%s<br>\n",encoding);
	printf ("http_host:%s<br>\n",http_host);
	printf ("remote_addr:%s<br>\n",remote_addr);
	printf ("Ident:%s<br>\n",ident);
	printf ("http_x_forwarded_for:%s<br>\n",http_x_forwarded_for);
	printf ("logout:%s<br>\n",logout);
	printf ("searchtype:#%s#<br>\n",searchtype);
	printf ("newperm:#%s#<br>\n",newperm);
	printf ("http_cookie:#%s#<br>\n",http_cookie);
	printf ("cookie_avss:#%s#<br>\n",cookie_avss);
	printf("<p>\n");
	printf("Bio:%s\n",bio);

	printf("<h2>UNDER MAINTENANCE</h2>\n");
#endif


	gettimeofday (&tv2, NULL);
	timeelapsed=(tv2.tv_sec*1000000.0+tv2.tv_usec)-(tv.tv_sec*1000000.0+tv.tv_usec);

	printf ("<font size=-2>Created in %.1fms</font>\n", timeelapsed / 1000.0);
	printf ("</BODY>\n");
	printf ("</HTML>\n");

	/*Refresh time file */
        sprintf (tmp, "%s%s/timecheck", IPZDIR, cookie_avss);
	touchnow(tmp);
    } //authorized
    return errno;
} //main
