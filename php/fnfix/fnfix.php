<?
include ("../avssconf.php");
include ("../avssfunc.php");

//Sivann 2003-2009
/*TODO: handle filenames with ':'
 *
 */

setlocale(LC_ALL,'en_US.UTF-8');
//LC_CTYPE="en_US.UTF-8"

//to work without register_globals
foreach($_REQUEST as $k => $v) { ${$k} = $v; }
foreach($_SERVER as $k => $v) { ${$k} = $v; }


$userdir=$avssdir."/data/ipz/".$sess;
$isexpired=isexpired($userdir);

if ($isexpired) {
  echo "Session expired ".($isexpired/60)." minutes ago, please login again<br>";
  exit;
}
$table2_bgcolor="#afafaf";
$globmsg="";



//fix string to include in javascript variables
function jsreplace($str){

$str = str_replace(
  array(">","<","\"", "'", "&", "\(", "\)" ),
  array("&gt;","&lt;","&quot;","&rsquo;","&amp;","&#40;","&#41;"), $str);
return $str;
}

//setlocale(LC_CTYPE, "el_GR");

$wlimit=300; //photo width limit 


$pathprefix="$audioroot";
if (!isset($path) || empty($path)) $path="$pathprefix";
$path=str_replace("\'" , "'", $path);
$path=realpath($path);

if (!isset($trans)) $trans="";

//play audio
if (isset($mp3file) && !empty($mp3file)) {
  $mp3file=str_replace("\'" , "'", $mp3file);

  if (strstr($mp3file,".jpg")) {
    $fn=$path."/".$mp3file;
    $sz=filesize($fn);
    header ("Content-type: image/jpeg");
    header ("Content-Length: $sz");
    readfile($fn);
    exit;
  }
  elseif (strstr($mp3file,".txt")) {
    $fn=$path."/".$mp3file;
    header ("Content-type: text/plain");
    readfile ($fn);
    exit;
  }
  elseif (!strstr($mp3file,".mp3") && !strstr($mp3file,".ogg")) {
    echo "$mp3file:Go away hacker<br>";
    exit;
  }

  $fn=$path."/".$mp3file;
  $sz=filesize($fn);
  if (stristr($mp3file,".ogg"))
    header ("Content-type: audio/ogg");
  else
    header ("Content-type: audio/mpeg");
  header ("Content-Length: $sz");
  readfile($fn);
  exit;
}




$goto=$path;
if (!chdir($goto)) {
  echo "<b>cd \"$goto\" failed</b><br>";
  chdir($pathprefix);
  $path=$pathprefix;
};

if (!empty($photourl)) {
    //$cmd=str_replace("\'" , "'", $cmd); $cmd=str_replace(" " , "-", $cmd);
    $cmd="wget \"$photourl\" -O 00photo.jpg";
    $pfp = popen ("$cmd", "r");
    $buffer = fgets($pfp, 4096);
    pclose($pfp);
    $globmsg.="wget: $cmd $buffer\n";
}

if (isset($fixphotos) && !empty($fixphotos)) {
    $dir=$x[1];
    $cmd="/bin/ls *photo.jpg 2>&1";
    $fp = popen ("$cmd", "r");
    $np=0;
    while ($buffer = fgets($fp, 4096)) {
      $ph[$np]=chop($buffer); 
      $ph[$np]="$ph[$np]";
      $np++;
    }
    pclose($fp);
    if (!$np) return; // no images

    $x=getimagesize($ph[0]); //00photo.jpg
    $w=$x[0]; //width
    if ($w>$wlimit) {
     $n=sprintf("%02dphoto.jpg",$np);
     $cmd="mv 00photo.jpg $n;convert $n -resize 300 00photo.jpg 2>&1";
     //echo "<br>$cmd<br>";
    }
    $fp = popen ("$cmd", "r");
    while ($buffer = fgets($fp, 4096)) {
      echo "<pre>$buffer</pre>";
    }
    pclose($fp);
}


if (isset($delete) && !empty($delete)) {
  $delete=str_replace("\'" , "'", $delete);
  if (strstr($delete,"rmrf")) {
    $x=explode(":",$delete);
    $dir=$x[1];
    //echo "PATH=".($path); echo "<br>DIR=".($dir);
    
    $cmd="/bin/rm -fr \"$path/$dir\" 2>&1";
    $fp = popen ("$cmd", "r");
    $naf=0;
    while ($buffer = fgets($fp, 4096)) {
      echo "<pre>$cmd: $buffer</pre>";
    }
    pclose($fp);
  }
  elseif (!unlink("$delete")) 
    echo "<b>unlink \"$delete\" failed</b><br>";
}

elseif (isset($formaction) && ($formaction=="renameall")) {
  $trans="";
  for ($i=0;$i<count($newfile);$i++) {
    $orgfile[$i]=str_replace("\'" , "'", $orgfile[$i]);
    $orgfile[$i]=str_replace("\\\\" , "\\", $orgfile[$i]);
    $newfile[$i]=str_replace("\'" , "'", $newfile[$i]);
    //$orgfile[$i]=str_replace("’" , "&rsquo;", $orgfile[$i]);
    $newfile[$i]=str_replace("’" , "'", $newfile[$i]);
    $newfile[$i]=html_entity_decode($newfile[$i]);

    if (file_exists($newfile[$i])) {
      $globmsg.="File \"$newfile[$i]\" already exists\n";
    }
    elseif ($newfile[$i]!=$orgfile[$i]) {
      if (!rename($orgfile[$i],$newfile[$i]))
	print "<br>rename \"$orgfile[$i]\" \"$newfile[$i]\" failed<br>";
    }
  }
}

$cmd="$avssdir/bin/lsaudio";

$fp = popen ("$cmd", "r");
$naf=0;
while ($buffer = fgets($fp, 4096)) {
  $x=explode("/",chop($buffer));
  $allfiles['type'][$naf]=$x[0];
  $allfiles["size"][$naf]=$x[1];
  $allfiles["mins"][$naf]=$x[2];
  $allfiles["secs"][$naf]=$x[3];
  $allfiles["bitrate"][$naf]=$x[4];
  $allfiles["bitrateinfo"][$naf]=$x[5];
  $allfiles["fname"][$naf]=$x[6];
  $naf++;
}
pclose($fp);


echo "<!--\n   (c) sivann 2003-\n-->\n";
echo "<html>\n";
echo "<head>\n";
echo "<meta HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=UTF-8\">\n";
echo "<title>MP3Utils :: $path</title>\n";

?>

<script type="text/javascript" src="overlib.js"><!-- overLIB (c) Erik Bosrup --></script>

<style type="text/css">
 body  {font-family: lucida,verdana,helvetica;font-size: 12px;}
 td    {font-family: lucida,verdana,helvetica;font-size: 12px;}


input.itxt {height: '100%'; width:30em;margin-top:0;
	border: 1px solid #cecece;
	font-family: lucida,verdana,helvetica;font-size: 12px;}
 a:hover { color:#FF00FF; text-decoration: none}
 a { text-decoration: none}
xxxtable {-moz-border-radius: 10px; border-style: solid; border-color:#dedede }
</style>

<script>
function dirdel(loc,dname) 
{
  $i=window.prompt("Write YES to DELETE \""+dname+"\"",'NO');
  if ($i=='YES') {
    window.location=loc;
  }
  else
    alert('ABORTED...');
}

function repl1()
{
  document.rechng.rechngfrom.value='([ ])(.*)';
  document.rechng.rechngto.value='\\1- \\2';
}

function repl2()
{
  document.rechng.rechngfrom.value='([0-9][0-9])(.*)';
  document.rechng.rechngto.value='\\1-\\2';
}


function songinfo(info)
{
  x = document.getElementById("songinfo");
  if (x) x.innerHTML=info;
}

</script>

</head>
<body>

<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>

<?

/*get directories & files*/
for ($nd=0,$i=0;$i<$naf;$i++) {
  if ($allfiles["type"][$i]=="d") 
    $dirs[$nd++]=$allfiles["fname"][$i];
}


//$where=urlencode("$path/$dirs[$i]");
$where=urlencode("$path");
$where=str_replace("%2F" , "/", $where);
echo "<a href=\"".$_SERVER['PHP_SELF']."?path=$where&amp;sess=$sess\">";
echo "<img align=absmiddle src=$avsswwwdir/images/reload.gif title=Refresh border=0>[Refresh]</a>&nbsp;&nbsp; \n";

$hdr= "<b>$path</b>\n";
$hdr.= "&nbsp;&nbsp;&nbsp;<b>[".count($allfiles["type"])." files]</b>&nbsp;&nbsp;&nbsp;";

//$w=ereg_replace("/[A-z0-9]+/[A-z0-9]+(.*)","\\1",$where);
$w=preg_replace("@/[A-z0-9]+/[A-z0-9]+(.*)@","\\1",$where);
$w=str_replace("+" , "%20", $w);
$wwwpath=$w;
if (!strstr($w,"Incoming")) { $w="/permanent/$w";}

echo "[<a href=\"http://".$avsshost.$avsswwwdir."cgi-bin/avssmain.m3u?path=$wwwpath&amp;sess=$sess\">Back to Kronos</a>]  ";

$x=explode("/",$path);
$cx=count($x);
if ($cx>2) {
  $isrch=$x[$cx-1]." ".$x[$cx-2];
  $isrch=urlencode($isrch);
  echo "[<a target=isrch href=\"http://images.google.com/images?q=$isrch\">".
       "Find album cover</a>]";
}


echo "\n\n<table border=0 width='100%' align=center cellspacing=1 cellpadding=0>\n";

// print dirs
for ($i=0;$i<$nd;$i++) { 
  if ($i%2) $col="#fefefe"; else $col="#edf3fe";
  if ($dirs[$i]==".") continue;
  echo "<tr><td bgcolor=$col colspan=4>".
       "<a style=\"text-decoration: none\" ".
       "href=\"$PHP_SELF?sess=$sess&amp;path=".str_replace("%2F","/",urlencode(realpath("$path/$dirs[$i]")))."\">";
  if ($dirs[$i]!="..") echo "<img align=absmiddle border=0 src=\"$avsswwwdir/images/folder_small.png\">&nbsp;";
  else echo "<img border=0 src=\"/icons/small/back.gif\">&nbsp;";
  if ($dirs[$i]=="..")  echo "<b><big>$dirs[$i]</big></b></a>&nbsp;&nbsp;&nbsp;$hdr";
  else echo "$dirs[$i]</a>";
  echo "</td>\n" ;

  if ($dirs[$i]=="..") echo "<td bgcolor=$col>&nbsp;</td>";
  else {
  echo " <td bgcolor=$col>".
       "<a href=\"javascript:".
       "dirdel('$PHP_SELF?sess=$sess&amp;path=$path".
       "&delete=rmrf:".basename($dirs[$i])."','$dirs[$i]')\">".
       "<img border=0 title=Delete src='$avsswwwdir/images/trash-icon.png'></a></td>";
       }

  echo "</tr>\n";
}


//ID3
for ($af=0,$j=0;$j<$naf;$j++) { /*print files*/
  $cmd="$avssdir/bin/id3v2-avss -l \"$path/".$allfiles["fname"][$j]."\" ";
  $fp = popen ("$cmd", "r");
  while ($buffer = fgets($fp, 1024)){
    //$buffer=str_replace("'" , "", $buffer);
    //$buffer=str_replace("\n" , "", $buffer);
    $buffer=chop($buffer) ;
    $x=explode(":",$buffer,2); $f=explode(" ",$x[0],2); //first word
    if (isset($x[1])) $x[1] = jsreplace($x[1]);
    else $x[1]="";
    $TAGS[$j][$f[0]]=$x[1];

    if (isset($ID3s[$j])) $ID3s[$j].="<b>".$f[0].":</b>".$x[1]." ";
    else $ID3s[$j]="<b>".$f[0].":</b>".$x[1]." ";

  }
  pclose($fp);
}




echo "<form method=post name=filefrm action='$PHP_SELF?path=".urlencode($path)."&amp;sess=$sess'>\n";

for ($af=0,$j=0;$j<$naf;$j++) { /*print files*/
  if (strstr($allfiles["fname"][$j],".php") 
  //|| strstr($allfiles["fname"][$j],".txt")
  ) continue;
  if (($allfiles["fname"][$j]==""))  continue;
  if (($allfiles["fname"][$j]=="."))  continue;
  if (($allfiles["fname"][$j]==".."))  continue;
  if (($allfiles["type"][$j]=="d"))  continue;
  //echo " EMPTY $j".$allfiles["type"][$j]; echo "<br>";

  //if ($j%2) $col="#C6E7FF"; else $col="#9CCFFF";
  $rcolor1="#ffffff";
  $rcolor2="#edf3fe";
  if ($j%2) $col=$rcolor1; else $col=$rcolor2;


  $mtime=filemtime($allfiles["fname"][$j]);
  $size=$allfiles["size"][$j];
  $bps=$allfiles["bitrate"][$j];
  $bpsinfo=$allfiles["bitrateinfo"][$j];


  if ($bpsinfo=="VBR") 
    $bps="<span style='font-size:-1;background-color:#A0E998;text-decoration:overline'>[$bps]</span>";
  elseif ($bps<128) 
    $bps="<span style='font-size:-1;background-color:#0c0c0c;color:white'>[$bps]</span>";
  elseif ($bps==128) 
    $bps="<span style='font-size:-1;background-color:#77AFCA'>[$bps]</span>";
  elseif ($bps==160) 
    $bps="<span style='font-size:-1;background-color:lightgreen'>[$bps]</span>";
  elseif ($bps==192) 
    $bps="<span style='font-size:-1;background-color:#FDA782'>[$bps]</span>";
  elseif ($bps>192) 
    $bps="<span style='font-size:-1;background-color:pink'>[$bps]</span>";
  



  if (stristr($allfiles["fname"][$j],".mp3") || stristr($allfiles["fname"][$j],".ogg"))  {
    $isaudio=1; 
    $isphoto=0;
    $img="sound18.gif";
  }
  elseif (stristr($allfiles["fname"][$j],".jpg") )  {
    $isaudio=0;
    $isphoto=1;
    $img="generic18.gif";
  }
  else {
    $isaudio=0;
    $isphoto=0;
    $img="generic18.gif";
  }

  echo "<tr bgcolor=\"$col\" >\n <td valign=middle width=10>";

  if ($isaudio) {
       echo "<a href=\"$avsswwwdir/cgi-bin/avssmain.m3u?playpathfile=1".
	    "&amp;sess=$sess&amp;path=".htmlentities($wwwpath."/".$allfiles["fname"][$j])."\">";
  }
  else {
       echo "<a href=\"?sess=$sess&amp;path=".
	    urlencode($path)."&mp3file=".urlencode($allfiles["fname"][$j])."\">";
  }

  echo "<img border=0 src='$avsswwwdir/images/$img'></a></td>\n";
  echo "  <td width='20%'><input  class='itxt' readonly style='background-color:$col' type=text name=\"orgfile[]\" ".
       " onmouseover=\"return overlib('$ID3s[$j]',RELX,-4,RELY,4,WIDTH,500)\" ".
       " onmouseout=\"return nd();\"".
       " value=\"".$allfiles["fname"][$j]."\"></td>\n";
  echo "  <td width='20%'><input  class='itxt' style='background-color:$col' type=text id=newf".($j-$nd)." name=\"newfile[]\" ".
       " onmouseover=\"return overlib('$ID3s[$j]',RELX,-4,RELY,4,WIDTH,500)\" ".
       " onmouseout=\"return nd();\"".
       " value=\"";
  if ($isaudio) {
    $af++; //audiofiles
    $newfile=clean1($allfiles["fname"][$j]);
    $newfile=actions($newfile,$j-$nd,$af);
  }
  else $newfile=$allfiles["fname"][$j];
  echo "$newfile\"></td>\n";

	 $duration=$allfiles["mins"][$j].":".$allfiles["secs"][$j];
  echo "  <td valign=middle align=right>";

  if ($isaudio) {
    $x=($j-$nd);
    if (!isset($TAGS[$j]['TPE1'])) $TAGS[$j]['TPE1']=""; //do not complain for undefined
    if (!isset($TAGS[$j]['Artist'])) $TAGS[$j]['Artist']=""; //do not complain for undefined
    if (!isset($TAGS[$j]['Title'])) $TAGS[$j]['Title']=""; //do not complain for undefined
    if (!isset($TAGS[$j]['Track'])) $TAGS[$j]['Track']=""; //do not complain for undefined
    if (!isset($TAGS[$j]['TIT2'])) $TAGS[$j]['TIT2']=""; //do not complain for undefined
    if (!isset($TAGS[$j]['TRCK'])) $TAGS[$j]['TRCK']=""; //do not complain for undefined
    $id3v1fn=$TAGS[$j]['Artist']." - ".$TAGS[$j]['Title']." - ".sprintf("%02d",$TAGS[$j]['Track']);
    $id3v1fn=str_replace("'" , "\\'", $id3v1fn);
    $id3v1fn=str_replace("\"" , "\\'", $id3v1fn);
    $id3v1fn.=".mp3";
    $id3v2fn=$TAGS[$j]['TPE1']." - ".$TAGS[$j]['TIT2']." - ".sprintf("%02d",$TAGS[$j]['TRCK']);
    $id3v2fn=str_replace("'" , "\\'", $id3v2fn);
    $id3v2fn=str_replace("\"" , "\\'", $id3v2fn);
    $id3v2fn.=".mp3";
    echo "<table width=100% cellspacing=0 cellpadding=0><tr><td width='40'>";


    //get fn from id3v1 tag 
    echo "<a href='javascript:void(0);' ".
         " onclick=\"x=document.getElementById('newf$x');x.value='".$id3v1fn."';\" ".
         " onmouseover=\"return overlib('ID3v1 rename to: <b>$id3v1fn</b>',ABOVE,LEFT,SNAPY,10,WIDTH,450)\"".
         " onmouseout=\"return nd();\">".
         "<img src='$avsswwwdir/images/tag.png' border=0></a>";

    echo "<a href='javascript:void(0);' ".
         " onclick=\"x=document.getElementById('newf$x');x.value='".$id3v2fn."';\" ".
         //" onmouseover=\"return overlib('Rename from ID3V2: $id3v2fn',RELX,-4,RELY,4,WIDTH,500)\"".
         " onmouseover=\"return overlib('ID3v2 rename to:<b> $id3v2fn</b>',ABOVE,LEFT,SNAPY,10,WIDTH,450)\"".
         " onmouseout=\"return nd();\">".
         "<img src='$avsswwwdir/images/tag.png' border=0></a>";


    echo "<td align=left><font size=-1>"."($duration)&nbsp;".sprintf("%2.1f",($size/(1024*1024)))."MB</td>".
	 "<td align=right>$bps </td></tr></table>\n";
  }
  elseif ($isphoto) {
   $x=($j-$nd);
   for ($xx=0;$xx<10;$xx++){
     echo "<a href=\"javascript:void(0);\" onclick=\"".
     "x=document.getElementById('newf$x');x.value='0".$xx."photo.jpg';\">".
     "$xx</a>|";
   }
  }

  echo "  <td><a href=\"$PHP_SELF?sess=$sess&amp;path=".  urlencode("$path")."&trans=$trans";
  echo "&delete=".urlencode($allfiles["fname"][$j])."\" ".
       "onmouseover=\"return overlib('Delete this file',WIDTH,100,LEFT);\" onmouseout=\"return nd();\" ".
       ">".
       "<img border=0 title='Delete' src='$avsswwwdir/images/trash-icon.png'></a></td>";

  echo "</tr>\n";
}
//echo "<pre>"; print_r($TAGS); echo "</pre>";
echo "<tr><td bgcolor=#efefef colspan=6>".
     "<input type=submit value=\"Rename All\">".
     " <b>After all rename operations (field, manual, regex, etc), the 'Rename All' button must be pressed to actually ".
     "rename the files on the left with the filenames of the right column. </b><p>";
     "</td></tr>\n";
echo "<input type=hidden name=formaction value=\"renameall\"></td></tr>\n";
echo "<input type=hidden name=path value=\"$path\"></td></tr>\n";
echo "<input type=hidden name=sess value=\"$sess\">\n";
echo "</form>";

echo "</table>";

function dotrans($fn)
{
  global $trans;

  if (stristr($fn,".mp3")){
    $parts=explode(".mp3",$fn); 
    $ext=".mp3";
  }
  else {
    $parts=explode(".ogg",$fn); 
    $ext=".ogg";
  }
  

  $fn=$parts[0];
  $rest="";

  $fnarr=explode("-",$fn);
  if (count($fnarr) > strlen($trans)) { // exei extra data
    $rest="";
    for ($i=strlen($trans);$i<count($fnarr);$i++) {
      $rest=$rest." - ".$fnarr[$i];
    }
  }

  switch ($trans) {
    case "21":$fn="$fnarr[1] - $fnarr[0] $rest";break;
    case "132":$fn="$fnarr[0] - $fnarr[2] - $fnarr[1] $rest";break;
    case "213":$fn="$fnarr[1] - $fnarr[0] - $fnarr[2] $rest";break;
    case "231":$fn="$fnarr[1] - $fnarr[2] - $fnarr[0] $rest";break;
    case "3412":$fn="$fnarr[2] - $fnarr[3] - $fnarr[0] - $fnarr[1] $rest";break;
    case "3421":$fn="$fnarr[2] - $fnarr[3] - $fnarr[1] - $fnarr[0] $rest";break;
    case "1432":$fn="$fnarr[0] - $fnarr[3] - $fnarr[2] - $fnarr[1] $rest";break;
    case "2toend":$rest="";
               for ($i=2;$i<count($fnarr);$i++) $rest=$rest." - ".$fnarr[$i];  
	       $fn="$fnarr[0] - $rest - $fnarr[1]"; break;
    case "1toend":$rest="";
               for ($i=1;$i<count($fnarr);$i++) $rest=$rest.$fnarr[$i]." - ";  
	       $fn="$rest - $fnarr[0]"; break;
  }

  //$fn=ereg_replace("[ ]+$" , "", $fn);
  $fn=preg_replace("/[ ]+$/" , "", $fn);
  $fn=$fn.$ext;

  return $fn;
}

function clean1($fn)
{
global $trans;

  $newfn=$fn;

  $newfn=strtolower($newfn);

  if (isset($trans) && !empty($trans)) $newfn=dotrans($newfn);
  if (stristr($newfn,".mp3")){
    $parts=explode(".mp3",$newfn); 
    $ext=".mp3";
  }
  else {
    $parts=explode(".ogg",$newfn); 
    $ext=".ogg";
  }
  
  
  $newfn=$parts[0];


  //$newfn=str_replace("." , "-", $newfn);
  $newfn=str_replace("]" , " ", $newfn);
  $newfn=str_replace("[" , " ", $newfn);
  $newfn=str_replace("(" , "-", $newfn);
  $newfn=str_replace(")" , "", $newfn);
  $newfn=str_replace("_" , " ", $newfn);

/*
  $newfn=ereg_replace("-" , " - ", $newfn);
  $newfn=ereg_replace("-[ ]+-" , " -  ", $newfn);
  $newfn=ereg_replace("[ ]+" , " ", $newfn);
  $newfn=ereg_replace("^[ ]+" , "", $newfn);
*/
  $newfn=preg_replace("/-/" , " - ", $newfn);
  $newfn=preg_replace("/-[ ]+-/" , " -  ", $newfn);
  $newfn=preg_replace("/[ ]+/" , " ", $newfn);
  $newfn=preg_replace("/^[ ]+/" , "", $newfn);

  $newfn=str_replace("\\\\" , "", $newfn);
  $newfn=strtolower($newfn);

  return $newfn.$ext;
}

function actions($fn,$j,$af)
{ global $prependstr,$chngfrom,$chngto,$rechngfrom,$rechngto,$txtnam;

  if (isset($prependstr) && !empty($prependstr)) {
    $fn=$prependstr.$fn;
  }
  elseif (isset($chngfrom) && !empty($chngfrom)) {
    $fn=str_replace("$chngfrom" , "$chngto", $fn);
    $fn=str_replace("\\" , "", $fn);
  }
  elseif (isset($rechngfrom) && !empty($rechngfrom)) {
    //$fn=ereg_replace("$rechngfrom" , "$rechngto", $fn);
    $fn=preg_replace("@$rechngfrom@" , "$rechngto", $fn);
    $fn=str_replace("\\" , "", $fn);
  }
  elseif (isset($txtnam) && !empty($txtnam)) {
    //$x = ereg_replace("(\r\n|\n|\r)", "@", $txtnam); 
    $x = preg_replace("/(\r\n|\n|\r)/", "@", $txtnam); 
    $x=explode("@",$x);
    $fn=$x[$j]." -".(sprintf("%02d",$af)).".mp3";
    //$fn = ereg_replace("(\t|[ ]+)", " ", $fn); 
    $fn = preg_replace("/(\t|[ ]+)/", " ", $fn); 
  }

  return $fn;

}

echo "<table width='100%' cellspacing=0  cellpadding=2 >\n<tr><td width=60%>";

echo "<b>Filename field Transformations (1-2-3.mp3):</b><br>";
$where=urlencode("$path/$dirs[$i]");
echo "<a href=\"$PHP_SELF?sess=$sess&amp;path=$where&trans=21\">[2-1]</a>\n";
echo "<a href=\"$PHP_SELF?sess=$sess&amp;path=$where&trans=132\">[1-3-2]</a>&nbsp;\n";
echo "<a href=\"$PHP_SELF?sess=$sess&amp;path=$where&trans=213\">[2-1-3]</a>&nbsp;\n";
echo "<a href=\"$PHP_SELF?sess=$sess&amp;path=$where&trans=231\">[2-3-1]</a>&nbsp;\n";
echo "<a href=\"$PHP_SELF?sess=$sess&amp;path=$where&trans=3412\">[3-4-1-2]</a>&nbsp;\n";
echo "<a href=\"$PHP_SELF?sess=$sess&amp;path=$where&trans=3421\">[3-4-2-1]</a>\n";
echo "<a href=\"$PHP_SELF?sess=$sess&amp;path=$where&trans=1432\">[1-4-3-2]</a>\n";
echo "<a href=\"$PHP_SELF?sess=$sess&amp;path=$where&trans=1toend\">[1&nbsp;to&nbsp;end]</a>\n";
echo "<a href=\"$PHP_SELF?sess=$sess&amp;path=$where&trans=2toend\">[2&nbsp;to&nbsp;end]</a><br>\n";

echo "</td><td>";
echo "<font size=-1>";
echo "<div style=\"height:70px;overflow:auto;\" id=songinfo>&nbsp;</div>";
echo "</font></td></tr>\n";


echo "<tr><td colspan=2>";
echo "<form method=post name=prep>Prepend:<input name=prependstr type=text>";
echo "<input type=hidden name=sess value=\"$sess\">\n";
echo "<input type=submit value=Doit></form>\n";

echo "<form method=post name=rechng>Change:<input name=rechngfrom type=text>\n";
echo "To: <input name=rechngto type=text>".
     "<input type=submit value=Doit> (Regular Expression match) ";
echo "(<a href='javascript:repl1();'>first space to '-'</a>) ";
echo "(<a href='javascript:repl2();'>first 2digit number to 'xx-'</a>)";
echo "<input type=hidden name=sess value=\"$sess\">\n";
echo "\n</form>\n";

echo "<form method=post name=chng>Change:<input name=chngfrom type=text>\n";
echo "<input type=hidden name=sess value=\"$sess\">\n";
echo "To: <input name=chngto type=text>".
     "<input type=submit value=Doit> (Normal match)</form>\n";
echo "</tr></td>\n</table>\n\n";


echo "<p><a href=\"$PHP_SELF?sess=$sess&amp;path=$where\">";
echo "<img align=absmiddle src=$avsswwwdir/images/reload.gif title=Refresh border=0>Refresh</a>\n";

echo "<hr>";
echo "<b><big>Photos</big></b><br>";
echo "Paste below a .jpg url (p.x. an album cover from allmusic.com) to ".
     "download in this directory as 00photo.jpg<br>\n";

echo "<form method=post name=photofrm>".
     "URL:<input size=70 name=photourl type=text>\n".
     "<input type=submit value=Getit></form>\n";

echo "<a href=\"$PHP_SELF?sess=$sess&amp;path=".urlencode($path)."&fixphotos=1\"><big>FixBigPhotos</big></a> ";
echo "(if 00photo.jpg is wider than $wlimit pixels, it gets resized to 300px and the original is renamed)";
?>
<hr>

<table>
<tr>
<th>Messages</th><th>Rename files from filenames in this text area</th>
</tr>

<tr>

<td>
<textarea rows=15 cols=60>
<?=$globmsg?>
</textarea>
</td>

<td valign=top>
<form method=post name=txtnamfrm>
<textarea name=txtnam  rows=13 cols=50>
<?=$txtnam?>
</textarea>
<input type=submit value="Doit">
</form>

</td>

</tr>
</table>

<p>

&copy; sivann 2003 (version 1.187)


</html>

