INSTDIR="../../../avss/cgi-bin/"
###########################################################
#CFLAGS = -g
CFLAGS = -O3 -g -Wall -Wmissing-prototypes -finline-functions   #-D__KERNEL__
#DFLAGS = -DDEBUG
#DFLAGS = -DMP3INFOLIB -DESCGRK=1
DFLAGS = -DMP3INFOLIB 
#DFLAGS = -I/usr/gnu/include
#LFLAGS = -L/usr/gnu/lib
OBJECT = util.o mp3info.o mp3.o my_strcmp.o dxhead.o id3.o oggnfo.o strnstr.o md5c.o mddriver.o



mp3.m3u: $(OBJECT)
	gcc $(CFLAGS) $(LFLAGS) -o mp3.m3u $(OBJECT) -lcrypt -lvorbis -lvorbisfile -lsqlite3
		
util.o : util.c
	gcc $(CFLAGS) $(DFLAGS) -c util.c

strnstr.o : strnstr.c
	gcc $(CFLAGS) $(DFLAGS) -c strnstr.c

id3.o : id3.c id3.h
	gcc $(CFLAGS) $(DFLAGS) -c id3.c

mp3info.o : mp3info.c mp3info.h dxhead.o
	gcc $(CFLAGS) $(DFLAGS) -c mp3info.c

oggnfo.o : oggnfo.c mp3info.h dxhead.o
	gcc $(CFLAGS) $(DFLAGS) -c oggnfo.c

my_strcmp.o : my_strcmp.c 
	gcc $(CFLAGS) $(DFLAGS) -c my_strcmp.c

mp3.o : mp3.c mp3info.h id3.h mp3.h mp3defs.h
	gcc $(CFLAGS) $(DFLAGS) -c mp3.c

clean:
	rm -f $(OBJECT) mp3.m3u core 

install: mp3.m3u
	rm  $(INSTDIR)/avssmain.m3u
	rm  $(INSTDIR)/avssmain.mp3
	rm  $(INSTDIR)/avssmain.ogg
	rm  $(INSTDIR)/avssmain.mpitria
	cp mp3.m3u $(INSTDIR)/avssmain.m3u
	cp mp3.m3u $(INSTDIR)/avssmain.mp3
	cp mp3.m3u $(INSTDIR)/avssmain.ogg
	cp mp3.m3u $(INSTDIR)/avssmain.mpitria
