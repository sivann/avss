/* external prototypes */
int my_strcmp (const char *s1, const char *s2);
char *crypt (const char *key, const char *salt);
int oggnfo(MPEG_HEAD * mh, off_t size, int fd);
char *
strnstr(const char *s, const char *find, size_t slen);

/* prototypes */
void qsort1 (register int, register int, char *[]);
void qsort2 (register int, register int, char *[], off_t *, MPEG_HEAD *);
void qsort3 (register int l, register int r, char *a[], int  *dirs2di);
void readfile(void);
void readdisc(discinfo_s *di, char *disc,char * path);
void printdisc(void);
void printbio(char *bio,char * path);
void readinfo(artistinfo_s * artistinfo,char *info,char * path);
void printinfo(artistinfo_s * artistinfo);
void seealso(char * path);
void printStyleAsLinks(char * style);
void highlight(char *text);
int readskin(char *fname);
void showrecent(void);
int ldisti(char *s,char*t);
int getident(char *host,char *port, char *remport) ;
void authuser(void);
int isexpired(void);
void dologout(void);
void disploginscr(void);
void dosearchstyles(void);
void dosearchitem(void);
void dispwhatsnew(void);
void doplaysimilar(void);
void returnstreamurl(void);
void db_logplay(void);
int touchnow(char *s);
void readcookie_avss(void);
