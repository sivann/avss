<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
	"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<?
include("avssconf.php");
$rpp=7; //rows per page
$cpr=4; //cols per row
$imwidth=100;
$imheight=$imwidth;
$php_self=$_SERVER['PHP_SELF'];
?>
<html>
<head>
<meta HTTP-EQUIV="Content-Type" CONTENT="text/html; charset=UTF-8">
<title>MP3 Photos</title>

<style>
A.A1:link { COLOR: #ffffff; font-family: Verdana; 
        font-size: 12px; text-decoration: none;}
A.A1:active { COLOR: #ffffff; FONT-FAMILY: Verdana; 
        font-size: 12px; TEXT-DECORATION: none;}
A.A1:visited { COLOR: #ffffff; FONT-FAMILY: Verdana; 
        font-size: 12px; TEXT-DECORATION: none;}
A.A1:hover { COLOR: yellow; FONT-FAMILY: Verdana; 
        font-size: 12px; TEXT-DECORATION: none;}
.txtsm { COLOR: #cecece; font-family: Verdana; 
        font-size: 10px; text-decoration: none;}
A.nums       { COLOR: #ffe0e0; text-decoration: none;}
A.nums:hover { COLOR: #ff0000; text-decoration: none;}
body {height:100%; max-height:100%; overflow:hidden; padding:0; margin:0; border:0; background:#000000;}
#content {overflow:auto; position:absolute; z-index:3; 
          top:80px; bottom:0px; left:0; right:0; 
          background:#000;}
#numbers {position:absolute; margin:0; top:0; left:0; display:block; 
          width:100%; height:80px; 
          font-family:verdana;font-size:10px; 
          z-index:5; overflow:hidden; color:#fff;}

</style>
</head>

<body>
      


<?
$page=$_GET["page"];
$photodb="$avssdir/data/photos.txt";
$httpprefix="http://".$avsshost.$avsswwwdir."cgi-bin/avssmain.m3u?playpathfile=1&path=";

$photos=file($photodb);
$pn=count($photos);
if (empty($page) || !isset($page)) $page=0;



$tdbgc="#222250";

$lastpage=$pn/($rpp*$cpr)+1;


pagenav();


echo "\n<div id=\"content\">";

echo "\n<table border=0 width=100% cellspacing=3 cellpadding=3>";

for ($row=(0+$page*$rpp);($row<($pn/$cpr))&& $row<(($page+1)*$rpp);$row++) {
  echo "\n<tr>";
  for ($col=0;$col<$cpr;$col++) {
    $currphoto=$photos[$col+$cpr*$row];
    $currphotodir=dirname($currphoto);
    $fsdir=$audioroot.$currphotodir;
    $photo=$httpprefix.rawurlencode(trim($currphoto));
    $photodir=$httpprefix.rawurlencode(trim(dirname($currphoto)));
    $photodir=str_replace("playpathfile=1&","",$photodir);

    //image
    echo "<td><table cellpadding=1 cellspacing=1 width=100% ".
	 "bgcolor=\"$tdbgc\" border=0><tr>";
    echo "\n <td rowspan=2 width=\"5%\" bgcolor=\"$tdbgc\">".
      "<a class=A1 href=\"$photodir\">".
      "<img  height=$imheight width=$imwidth src=\"$photo\" border=0>".
      "</td>";

    //text
    echo "\n <td valign=top bgcolor=\"$tdbgc\">".
    "<a class=A1 href=\"$photodir\">";

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
    elseif ($p=strstr($currphotodir,"/music/artists")) {
      $expl=explode("/",$p);$Pname=$expl[3];

      if (count($expl)<=4) $Palbum="";
      else $Palbum=$expl[4];
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
    echo "</a>";
    //end of text

    echo "</td></tr> <tr><td valign=bottom>";

    echo "<a href=\"$photodir/&playlist=1\">";
    echo "<img src='$avsswwwdir/images/play-button4.gif' border=0></a>";
    echo "</td></tr></table>";

    echo "</td>\n";
  }
  echo "</tr>\n";
}

echo "</table>\n\n";
//pagenav();
echo "</div>\n";
echo "\n</body>\n</html>\n";


function pagenav()
{
global $page,$lastpage,$imwidth,$cpr,$php_self;

//$w=$imwidth*$cpr*2+100;
$w=round($lastpage*0.5);

//echo "\n<div style=\"position:fixed; top:0; left:0; width:".$w."em; height:2em; ".
//" font-family:Verdana; font-size:10px; background:#000000\">\n";

echo "<div id=\"numbers\">";
//echo "\n<div style=\"position:fixed; top:0; left:0; width:100%; height:2em; ".
//" font-family:Verdana; font-size:10px; background:#000000\">\n";

  echo "Page ";
  for ($i=1;$i<$lastpage;$i++) {
    $lnk="$php_self?page=".($i-1);
    if (($page+1)==$i) echo "<a class=nums href=\"$lnk\">".
    "<font color=yellow><b>$i</b></font></a>\n";
    else echo "<a class=nums href=\"$lnk\">$i</a>\n";
  }
  if ($page<($lastpage-2)) {
    $lnk="$php_self?page=".($page+1);
    echo "<a class=nums href=\"$lnk\">&gt;&gt;</a>\n";
  }

echo "</div>\n\n";
}


