#define AVSSCGI   "avssmain"
#define AVSSDIR   "/home/www/htdocs/avss/"
#define AVSSWWW   "/avss/"
#define PASSFILE  AVSSDIR"etc/passwd"
#define IPZDIR 	  AVSSDIR"data/ipz/"
#define DBFILE 	  AVSSDIR"data/mp3.db"
#define LOGDB 	  AVSSDIR"data/mp3log.db"
#define WHATSNEW  AVSSDIR"data/mp3.whatsnew"
#define NEWDIRS   AVSSDIR"data/dirs.whatsnew"
#define STATISTIK AVSSDIR"data/statistik.out"
#define STYLELIST AVSSDIR"data/stylelist"

#define MSG "msg.html"
#define RTABLE_WIDTH "200"
#define MP3DIR "/MMROOT/audio/"
#define ABSDATADIR AVSSDIR"etc/"
#define WWW_IMAGEDIR AVSSWWW"images/"
#define FAVICON "mp3_favicon.ico"
#define WWW_PHPDIR AVSSWWW"php/"
#define SKINZDIR AVSSDIR"etc/skinz/"
#define USERPREFSDIR AVSSDIR"data/userprefs/"
#define FINDREL AVSSDIR"bin/findrel"
#define FINDSIMILAR AVSSDIR"bin/findsimilar.pl"
#define GREP  AVSSDIR"bin/agrep2 -V -i -y -B -2"
//#define GREP  AVSSDIR"bin/agrep2 -i -y -1"
#define IDLE 3600
#define SRCH_BODY_TAG "<BODY bgcolor=\"white\" LINK=#0065CE VLINK=#00309C ALINK=RED>\n"



#define JAVASCRIPTCODE "\n<script>\n var pmode=0; var pwind;\n\
var currimg=0;\n\
 function playlist() {\n\
   if (pmode==0 || pwind.closed==true) {\n\
     pwind=window.open(\"\",\"mp3_playlist\",\
\"resizable,width=460,height=400,scrollbars=no,menubar=no,hotkeys=no\");\n\
     pmode++;\n\
   }\n }\n\n\
function rightString(str, sstr) {\n\
   if (str.indexOf(sstr) == -1) {\n\
      return \"\";\n\
   } else {\n\
      return (str.substring(str.indexOf(sstr)+sstr.length, str.length));\n\
   }\n\
}\n\
\n\
function addall() {\n\
  var bigurl='',url;\n\
  playlist();	\n\
  for (i=0;i<100;i++) {\n\
      idname=\"add\" + i;\n\
      k = document.getElementById(idname);\n\
      if (!k) { continue; }\n\
      url=rightString(k.getAttribute(\"href\"),\"path=\");\n\
      bigurl=bigurl+url+'@@';\n\
  }\n\
  document.addallform.ppath.value=bigurl;\n\
  document.addallform.submit();\n\
}\n\
function saveno(no) {\n\
  var bigurl='"AVSSWWW"cgi-bin/"AVSSCGI".mpitria?playpathfile=1&mp3data=1&path=';\n\
  idname=\"add\" + no;\n\
  k = document.getElementById(idname);\n\
  if (!k) { return; }\n\
  url=rightString(k.getAttribute(\"href\"),\"path=\");\n\
  bigurl=bigurl+url;\n\
  location.href=bigurl;\n\
}\n\
\n\
\n\
function previmage() {\n\
    if (currimg>0)\n\
        showimage(currimg-1);\n\
}\n\
\n\
function nextimage() {\n\
    if (currimg<(Pictures.length-1))\n\
        showimage(++currimg);\n\
}\n\
\n\
function showimage(imgno) {\n\
  x1 = document.getElementById('photolink');\n\
  x1.href=Pictures[imgno];\n\
  x2 = document.getElementById('photoimg');\n\
  x2.src=Pictures[imgno];\n\
  currimg=imgno\n\
}\n\
\n\
</script>\n\n"

#define STYLE    "<style type=\"text/css\">\n\
	  .tb{font-family: arial,verdana,helvetica; \n\
	   text-decoration:none; font-size: 12px}\n\
	  .tb_sm{font-family: arial,verdana,helvetica; \n\
	   text-decoration:none; font-size: 10px}\n\
	  .ol{text-align: left; text-decoration:overline}\n\
	  a:hover { color:%s; text-decoration: none}\n\
	  td{font-family: arial,verdana,helvetica;font-size: 12px;color:%s}\n\
	  .tbA{color:#00B47E;text-decoration:none}\n\
	  BODY{ margin: 4px }\n\
	  input:focus{background-color:#FFFFE7;color:#000000;} \n\
	  INPUT, SELECT, OPTION, TEXTAREA { \n\
	    font-family: Arial, Helvetica, sans-serif; font-size: 11px; \n\
	      font-weight: bold; color: #003366;}\n\
	  .big1{font-family: Trebuchet MS,arial,helvetica; \n\
	   text-decoration:none; font-size: 20px}\n\
	  .big2{font-family: Trebuchet MS,arial,helvetica; \n\
	   text-decoration:none; font-size: 17px}\n\
	  .big3{font-family: Trebuchet MS,arial,helvetica; \n\
	   text-decoration:none; font-size: 14px;font-weight:bold }\n\
	  \n</style>\n"


#define LOGINSTYLE    "<style type=\"text/css\">\n\
	  a:hover { color : #FF00FF; text-decoration: none}\n\
	  .tb{font-family: verdana,arial,helvetica; \n\
	   text-decoration:none; font-size: 12px}\n\
	  .tb_sm{font-family: verdana,arial,helvetica; \n\
	   text-decoration:none; font-size: 10px}\n\
	  .ol{text-align: left; text-decoration:overline}\n\
	  td{font-family: verdana,arial,helvetica; \
	  font-size: 12px}\n\
	  .tbA{color:#00B47E;text-decoration:none}\n\
	  BODY{ margin: 4px }\n\
	  input:focus{background-color:#FFFFCE;color:#000000;} \n\
	  input.subm {\n\
		  font-family:	Verdana,Tahoma,Arial;\n\
		  font-weight:	bold;\n\
		  font-size:	14px;\n\
		  color:	#FFFFFF;\n\
		  background-color:	#CC0000;\n\
		  background-image: url(/avss/images/red_btndots.gif);\n\
		  background-repeat: no-repeat;\n\
		  background-position : 5px 3px;\n\
		  border-width: 1px 1px 1px 1px;\n\
		  padding-left : 17px;\n\
	  }\n\
	  \n</style>\n"
