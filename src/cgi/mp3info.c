/* (c) Spiros Ioannou 1999 */
/* 12/2002: support for ID3v2 (skip it!)
 */


#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "mp3info.h"
#include "dxhead.h"

#define BUFSIZE 256


#ifndef MP3INFOLIB
main(int argc,char **argv) {
  int i;
  unsigned char buf[BUFSIZE];
  MPEG_HEAD mh;
  struct stat st;
  int fd;
  
  int r;

  
  if (argc<2) {
    fprintf(stderr,"mp3info V0.9 - Spiros Ioannou 1999\n\n");
    fprintf(stderr,"Usage mp3info [-s] <file.mp3>\n");
    fprintf(stderr,"\t-s\tshort format good for scripts\n\n");
    exit(1);
  }

  if ((fd=open(argv[argc-1],O_RDONLY))<0) {
    perror(argv[argc-1]);
    exit(2);
  }

  fstat(fd,&st);

  get_mp3header(&mh,st.st_size,fd);
  
  if (!strcmp(argv[1],"-s") && !mh.vbr) {
    printf ("%d:%d:%d:%d:%d:%d:%d:%d:%d\n",mh.skiped,mh.id3v2skiped,
	mh.bitrate,mh.vbr,mh.layer,mh.version,
	mh.mins,mh.secs,mh.length);
  }
  else if (!strcmp(argv[1],"-s") && mh.vbr) {
    printf ("%d:VBR:%d:%d:%d:%d:%d:%d\n",mh.skiped,
	mh.vbr,mh.layer,mh.version,
	mh.mins,mh.secs,mh.length);
  }
  else {
    printf("Skipped: %d bytes\n",mh.skiped);
    printf("ID3v2Skipped: %d bytes\n",mh.id3v2skiped);
    if (mh.vbr) 
      printf("BitRate: VBR\n");
    else 
      printf("BitRate: %d\n",mh.bitrate);
    printf("Layer:%d\n",mh.layer);
    printf("Version:%d\n",mh.version);
    printf("Length: [%02d:%02d]  (%0d sec)\n",mh.mins,mh.secs,mh.length);
  }
  close(fd);
}
#endif


int get_mp3header(MPEG_HEAD * mh,off_t size,int fd)
{
  unsigned char bytes[8];
  unsigned char bits[33];
  unsigned char buf[BUFSIZE];
  register int i;
  int times=0;
  int version=0,layer=0,crc_protected=0,bitrate=0;
  int length=0,mins,secs;

  XHEADDATA xing_header;
  

  int v1l1[16] ={-1,32,64,96,128,160,192,224,256,288,320,352,384,416,448,-1};
  int v1l2[16] ={-1,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,-1};
  int v1l3[16] ={-1,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,-1};
  int v2l1[16] ={-1,32,48,56, 64, 80, 96,112,128,144,160,176,192,224,256,-1};
  int v2l23[16]={-1,8 ,16,24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,-1};
    
  mh->vbr=0;
  mh->skiped=0;
  mh->id3v2skiped=0;
  xing_header.frames=0;
  xing_header.samprate=1;
  xing_header.toc=0;
    

  do {

    if (read(fd,buf,BUFSIZE)>0)
      times++;
    else break;

    
    mh->valid=0;
    mh->enctype=1;
    /* Detect VBR (header on first frame's data) */
    if ( times==1 ) {
      if(buf[0] == 'I' && buf[1]=='D' && buf[2]=='3' ) { // id3v2, must skip it
	int num=0;
	//skip id3v2
	//synchronize synchsafe integers
	num=buf[6];
	num=num*128+buf[7];
	num=num*128+buf[8];
	num=num*128+buf[9];

	num+=10; //we ignore the footer (would be +20)
	//printf("H:%d\n",num);
	lseek(fd,num,SEEK_SET);
	mh->skiped+=num;
	mh->id3v2skiped=num;

	if (read(fd,buf,BUFSIZE)>0); //times++;
	else break;

      }

      if (i=GetXingHeader(&xing_header,buf)) { //vbr
	mh->vbr=1;
	if (i<0) mh->valid=0;else mh->valid=1;
      }

    }

    i=0;
    while ((i < BUFSIZE) && !((buf[i]==0xFF) && ((buf[i+1] & 0xF0) == 0xF0)))
      i++;

    memcpy(&bytes[1],&buf[i],4);

    mh->skiped+=i;

    for (i=1;i<33;i++) {
      bits[i] = (bytes[ ((i - 1)/ 8) +1] >> ((7 - (i -1) % 8))) & 0x01;
    }

    if (bits[12] && bits[13]) version = 1;
    else if (bits[12] && !bits[13]) version =2 ;
    else if (!bits[12] && !bits[13]) version = 25;
    else version =0;
      
    mh->version=version;
    
    if (!bits[14] && bits[15]) layer = 3;
    else if(bits[14] && !bits[15]) layer = 2;
    else if(bits[14] && bits[15]) layer = 1;
    else layer=0;
    
    mh->layer=layer;

    if(bits[16]) crc_protected=1;
    else crc_protected=0;
    
    i = (bits[17] << 3) + (bits[18] << 2) + (bits[19] << 1) + bits[20];

    if (version == 1) {
      if (layer == 3)
	bitrate = v1l3[i];
      else if (layer == 2)
	bitrate = v1l2[i];
      else if (layer == 1)
	bitrate = v1l1[i];
      else bitrate=-1 ;
    }
    else if ((version == 2) || (version == 25)) {
      if (layer == 1)
	bitrate = v2l1[i];
      else if ((layer == 2) || (layer == 3))
	bitrate = v2l23[i];
      else bitrate=-1 ;
    }
    else bitrate=-1  ;
    
    mh->bitrate=bitrate;
    
    if(!bits[25] && !bits[26]) strcpy(mh->mode,"stereo");  
    else if(!bits[25] && bits[26]) strcpy(mh->mode,"joint-stereo");  
    else if(bits[25] && !bits[26]) strcpy(mh->mode,"dual channel"); 
    else if(bits[25] && bits[26])  strcpy(mh->mode,"Mono");        
    else mh->mode[0]=0;

    if (!mh->vbr)
      length=(size*8)/(bitrate*1000);
    else if (mh->valid)
      length=xing_header.frames*1152/xing_header.samprate;
    else
      mh->length=mh->mins=mh->secs=length=0;


    if (bitrate>0 && length>0) { 
      mins=length/60;
      secs=(length%60);
      mh->length=length;
      mh->mins=mins;
      mh->secs=secs;
      mh->valid=1;
    }
    
  } while(!mh->valid && times<50 &&!mh->vbr);

  return (mh->valid);
}
