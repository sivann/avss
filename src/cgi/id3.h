
typedef struct
{
  char title[31];
  char artist  [31];
  char album   [31];
  char year    [5];

  char comment [29];
  unsigned char zero ;
  unsigned char track;
  unsigned char genre;
}ID3; //id3v1.1


void guessID3 (char *, ID3 *);
void printID3 (ID3 *);
