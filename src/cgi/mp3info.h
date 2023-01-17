
typedef struct {
  int valid;
  int enctype;
  mode_t fmode;
  int skiped;
  int id3v2skiped;
  int vbr;
  int version;
  int layer;
  int bitrate;
  char mode[16];
  int mins;
  int secs;
  int length;
} MPEG_HEAD;


int get_mp3header(MPEG_HEAD *,off_t,int);

