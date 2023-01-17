//sivann 2003
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"
#include "mp3info.h"

int oggnfo(MPEG_HEAD * mh, off_t size, int fd)
{
  OggVorbis_File vf;
  long bitrate=0;
  double tottime=0;
  FILE *fp;

  fp=fdopen(fd, "r");

  if(ov_open(fp, &vf, NULL, 0) < 0) {
      mh->valid=0;
      return(1);
  }

  bitrate=ov_bitrate(&vf,-1);
  tottime=ov_time_total(&vf,-1);

  mh->bitrate=bitrate/1000;
  mh->vbr=1;
  mh->mins=tottime/60;
  mh->secs=((int)tottime%60);
  ov_clear(&vf);
  return(0);
}
