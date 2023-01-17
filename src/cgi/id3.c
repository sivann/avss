/* MPEG-Audio ID3 TAG reader/guesser*/
/* Credits to Woo-jae Jung for the original TAG code fragments*/
/* Spiros Ioannou 1998*/
/* New Features: 
 *   capability of guessing artist/song from filename
 *   id3v1.1 support (track number)
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include "id3.h"

#define trimspace(str)  {int i=sizeof(str)-1;while(str[i]<=32 && (i)>=0) str[(i)--]=0;}


/*
void
printinfo (char *filename, ID3 * data, int flag)
{
    printf ("\n"
	    "Filename: %s\n"
	    "\tTitle   :%s\n"
	    "\tArtist  :%s\n"
	    "\tAlbum   :%s\n"
	    "\tYear    :%s\n"
	    "\tComment :%s\n"
	    "\tTrack   :%d\n"
	    "\tType    :%d\n",
	    filename,
	    data->title,
	    data->artist,
	    data->album, 
	    data->year, 
	    data->comment, 
	    data->track, 
	    data->genre);

}
*/

void
guessID3 (char *filename, ID3 * data)
{
    char *p, *q, *r, *sl, *t;
    char fn[256];
    char buff[256];
    int i, j, k, m;

    bzero (fn, 256);
    memcpy (fn, filename, strlen (filename));


    p = q = r = 0;
    m = 0;

    sl = strrchr (fn, '/');
    if (sl)
	sl++;
    else
	sl = fn;
    strcpy (buff, sl);

    p = strtok (sl, "-");	/*Artist */
    while (p && *p == ' ') {
	p++;
	m++;
    }
    if ((t = strstr (p, ".mp3"))) {
	*t = 0;
	m += 4;
    };

    if (p) {
	q = strtok (0, "-");	/*Songname */
	while (q && *q == ' ') {
	    q++;
	    m++;
	}
	if (q) {
	    if ((t = strstr (q, ".mp3"))) {
		*t = 0;
		m += 4;
	    }
	    r = buff + strlen (q) + strlen (p) + m + 1;	/*the rest */
	    while (r && *r && !isalnum (*r))
		r++;
	    if ((t = strstr (r, ".mp3")))
		*t = 0;
	}
    }
    /*Cut off the trailing spaces */
    i = j = k = 0;
    if (p)
	for (i = strlen (p) - 1; i >= 0 && p[i] == ' '; i--)
	    p[i] = 0;
    if (q)
	for (j = strlen (q) - 1; j >= 0 && q[j] == ' '; j--)
	    q[j] = 0;
    if (r)
	for (k = strlen (r) - 1; k >= 0 && r[k] == ' '; i--)
	    r[k] = 0;


    //printf("\tArtist:%s\n\tSong:%s\n\tOther:%s\n\n",p,q,r);

    if (p)
      strncpy (data->artist, p, 30);
    else
      strncpy (data->artist, "unknown", 30);

    if (q)
      strncpy (data->title, q, 30);
    else
      strncpy (data->title, "unknown", 30);

    if (r && strlen (r) < 3 && atoi (r))
      data->track = atoi (r);
    else if (r)
	strncpy (data->comment, r, 28);
    else 
      data->track=0;

    strcpy(data->album,"");
    data->zero=0;
    data->genre=-1;
}

//output an ID3 tag (normally at the end of a file)
void printID3 (ID3 * id3)
{
  printf("TAG");
  write(1,id3->title,30);
  write(1,id3->artist,30);
  write(1,id3->album,30);
  write(1,id3->year,4);
  strncpy(id3->comment,"auto-guessed from filename",28);
  write(1,id3->comment,28);
  write(1,&id3->zero,1);
  write(1,&id3->track,1);
  write(1,&id3->genre,1);

}

  /*
  void
readID3 (int fd, ID3 * data)
{
  read (fd,data->title,30);
  read (fd,data->artist, 30);
  read (fd,data->album, 30);
  read (fd,data->year, 4);
  read (fd,data->comment, 28);
  read (fd,&data->zero, 1);
  read (fd,&data->track, 1);
  read (fd,&data->genre, 1);
}
*/

/*
int
main (int argc, char **argv)
{
    int fd;
    int flag = 0;
    ID3 *data;
    char filename[128];
    char tag[4];


    if (argc != 2) {
	printf ("Usage: id3 <file.mp3>\n");
	exit (1);
    }

    data = (ID3 *) malloc (sizeof (ID3));
    memset(data,0,sizeof(ID3));
    strncpy (filename, argv[1], 128);

    if (!(fd = open (filename, O_RDONLY))) {
	char str[128];
	sprintf (str, "id3:open:%s", filename);
	perror (str);
	exit (-1);
    }
    lseek (fd, -128, SEEK_END);
    read(fd,tag,3);

    if (tag[0] == 'T' && tag[1] == 'A' && tag[2] == 'G') {
	readID3 (fd, data);
	flag = 1;
    }
    else {
	printf ("guessing from filename...\n");
	guessID3 (filename, data);
    }

    close (fd);

    printinfo (filename, data, flag);

    if (data)
	free (data);
    exit (flag);
}
*/
