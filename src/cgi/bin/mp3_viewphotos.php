<html>
<head>
<title>MP3 Photos</title>

<style>
A:link { COLOR: #ffffff; font-family: Verdana; 
        font-size: 12px; text-decoration: none;}
A:active { COLOR: #ffffff; FONT-FAMILY: Verdana; 
        font-size: 12px; TEXT-DECORATION: none;}
A:visited { COLOR: #ffffff; FONT-FAMILY: Verdana; 
        font-size: 12px; TEXT-DECORATION: none;}
A:hover { COLOR: yellow; FONT-FAMILY: Verdana; 
        font-size: 12px; TEXT-DECORATION: none;}
.txtsm { COLOR: #cecece; font-family: Verdana; 
        font-size: 10px; text-decoration: none;}
</style>
</head>

<body bgcolor="black" 
      vlink=white alink=white link=white marginwidth=0 
      marginheight=0  topmargin=0 leftmargin=0
      text=white>


<?
$photodb="/var/www/mp3/photos.txt";
$httpprefix="http://xxx.yyy.zzz..com/cgi-bin/mp3.m3u?mp3file=1&path=";

$photos=file($photodb);
$pn=count($photos);
if (empty($page) || !isset($page)) $page=0;


echo "<table border=0 width=100% cellspacing=3 cellpadding=3>";
$rpp=7; //rows per page
$cpr=4; //cols per row

$tdbgc="#222250";

$lastpage=$pn/($rpp*$cpr)+1;


for ($row=(0+$page*$rpp);($row<($pn/$cpr))&& $row<(($page+1)*$rpp);$row++) {
  echo "\n<tr>";
  for ($col=0;$col<$cpr;$col++) {
    $currphoto=$photos[$col+$cpr*$row];
    $currphotodir=dirname($currphoto);
    $fsdir="/kill-9/sorted/".$currphotodir;
    $photo=$httpprefix.rawurlencode(trim($currphoto));
    $photodir=$httpprefix.rawurlencode(trim(dirname($currphoto)));
    $photodir=str_replace("mp3file=1&","",$photodir);

    echo "\n  <td width=\"5%\" bgcolor=\"$tdbgc\">".
      "<a href=\"$photodir\">".
      "<img  height=100 width=100 src=\"$photo\" border=0>".
      "</td>";
      echo "\n  <td bgcolor=\"$tdbgc\"><a href=\"$photodir\">";



    if ($inc=strstr($currphotodir,"/Incoming")) {
      $incexpl=explode("/",$inc);$Iname=$incexpl[2];
      $Iloc=""; for ($i=3;$i<count($incexpl);$i++) $Iloc.="$incexpl[$i]/ ";
      echo "<b><font color=yellow>$Iname's</font><br>$Iloc</b>";
    }
    elseif ($p=strstr($currphotodir,"/greek")) {
      $expl=explode("/",$p);$Gname=$expl[3];
      $Gloc=""; for ($i=4;$i<count($expl);$i++) $Gloc.="$expl[$i]/ ";
      echo "<b>$Gname<br><font color=yellow>$Gloc</font></b>";
    }
    elseif ($p=strstr($currphotodir,"/permanent")) {
      $expl=explode("/",$p);$Pname=$expl[3];
      $Palbum=$expl[4];
      $photolnk=$fsdir."/00photo.jpg";
      $style="";$artist="";
      //get artist.info
      if (file_exists($photolnk) && is_link($photolnk)) {
	$artistphoto=readlink($photolnk);
	$artist=substr($artistphoto,0,strlen($artistphoto)-4);
	$infofile=($fsdir."/.".$artist.".info");
	if (file_exists($infofile)) {
	  $info=file($infofile);
	  for ($i=0;$i<count($info);$i++) {
	    $infexpl=explode(":",$info[$i]);
	    if ($infexpl[0]=="Style") {$style=$infexpl[1];break;};
	  }
	}
      }

      $Ploc=""; for ($i=5;$i<count($expl);$i++) $Ploc.="$expl[$i]/ ";
      echo "<b>$Pname<font color=yellow><br>$Palbum</font>".
           "<font size=-1 class=txtsm>$style</font><br>$Ploc</b>";
    }
    else { 
      echo "<b>$currphotodir<br> ($Iloc)</b>";
    }


    echo "</a></td>\n";
  }
  echo "</tr>\n";
}

echo "</table>\n\n";
pagenav();
echo "\n</body>\n</html>\n";


function pagenav()
{
global $page,$lastpage;

  echo "Page ";
  for ($i=1;$i<$lastpage;$i++) {
    $lnk="$PHP_SELF?page=".($i-1);
    if (($page+1)==$i) echo "<a  href=\"$lnk\">".
    "<font color=yellow><b>$i</b></font></a>\n";
    else echo "<a href=\"$lnk\">$i</a>\n";
  }
  if ($page<($lastpage-2)) {
    $lnk="$PHP_SELF?page=".($page+1);
    echo "<a href=\"$lnk\">&gt;&gt;</a>\n";
  }

}


