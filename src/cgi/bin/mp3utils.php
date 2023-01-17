<?
//Sivann 2003-

//setlocale(LC_CTYPE, "el_GR");

validateuser();

$pathprefix="/kill-9/sorted/";
if (!isset($path) || empty($path)) $path="$pathprefix";
$path=str_replace("\'" , "'", $path);
$path=realpath($path);

//echo "PATH=$path";
//iconv_set_encoding("internal_encoding", "UTF-8");
//iconv_set_encoding("input_encoding", "UTF-8");
//iconv_set_encoding("output_encoding", "UTF-8");
//var_dump(iconv_get_encoding('all'));
//echo "LALALALLA";


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
  elseif (!strstr($mp3file,".mp3") && !strstr($mp3file,".ogg")) {
    echo "$mp3file Go away hacker<br>";
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
    $cmd=str_replace("\'" , "'", $cmd);
    $cmd=str_replace(" " , "-", $cmd);
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
    if ($w>250) {
     $n=sprintf("%02dphoto.jpg",$np);
     $cmd="mv 00photo.jpg $n;convert $n -resize 240 00photo.jpg 2>&1";
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
    if (file_exists($newfile[$i])) {
      $globmsg.="File \"$newfile[$i]\" already exists\n";
    }
    elseif ($newfile[$i]!=$orgfile[$i]) {
      if (!rename($orgfile[$i],$newfile[$i]))
	print "rename \"$orgfile[$i]\" \"$newfile[$i]\" failed<br>";
    }
  }
}

$cmd="/usr/local/bin/mp3ls";

$fp = popen ("$cmd", "r");
$naf=0;
while ($buffer = fgets($fp, 4096)) {
  $x=explode(":",chop($buffer));
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
echo "<meta HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=iso-8859-7\">\n";
echo "<title>MP3Utils :: $path</title>\n";

?>

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
  document.rechng.rechngto.value='-\\1-\\2';
}


function songinfo(info)
{
  x = document.getElementById("songinfo");
  if (x) x.innerHTML=info;
}

</script>
<?
echo "</head>\n\n<body>\n";

$where=urlencode("$path/$dirs[$i]");
$where=str_replace("%2F" , "/", $where);
echo "<a href=\"$PHP_SELF?path=$where\">";
echo "<img align=middle src=/icons/reload.gif title=Refresh border=0>Refresh</a>&nbsp;&nbsp; \n";
echo "<b>$path</b>\n";
echo "&nbsp;&nbsp;&nbsp;<b>[".count($allfiles["type"])." files]</b>&nbsp;&nbsp;&nbsp;";
$w=ereg_replace("/[A-z0-9]+/[A-z0-9]+(.*)","\\1",$where);
$w=str_replace("+" , "%20", $w);
if (!strstr($w,"Incoming")) { $w="/permanent/$w";}
echo "\n&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
echo "<a href=\"http://kronos/cgi-bin/mp3.m3u?path=$w\">Back to Kronos</a>";
echo "<br>\n";

/*get directories & files*/
for ($nd=0,$i=0;$i<$naf;$i++) {
  if ($allfiles["type"][$i]=="d") 
    $dirs[$nd++]=$allfiles["fname"][$i];
}


echo "\n\n<table border=0 width=100% align=center cellspacing=1 cellpadding=2>\n";

// print dirs
for ($i=0;$i<$nd;$i++) { 
  if ($i%2) $col="#efefef"; else $col="#ceceee";
  if ($dirs[$i]==".") continue;
  echo "<tr><td bgcolor=$col colspan=4>".
       "<a style=\"text-decoration: none\" ".
       "href=\"$PHP_SELF?path=".str_replace("%2F","/",urlencode(realpath("$path/$dirs[$i]")))."\">";
  if ($dirs[$i]!="..") echo "<img border=0 src=\"/icons/small/dir.gif\">&nbsp;";
  else echo "<img border=0 src=\"/icons/small/back.gif\">&nbsp;";
  if ($dirs[$i]=="..") echo "<b><big>$dirs[$i]</big></b>";
  else echo "$dirs[$i]";
  echo "</a>\n</td>";

  if ($dirs[$i]=="..") echo "<td bgcolor=$col>&nbsp;</td>";
  else {
  echo " <td bgcolor=$col>".
       "<a href=\"javascript:".
       "dirdel('$PHP_SELF?path=$path".
       "&delete=rmrf:".basename($dirs[$i])."','$dirs[$i]')\">".
       "<img border=0 title=Delete src=/mp3script/images/delete.gif></a></td>";
       }

  echo "</tr>\n";
}


echo "<form method=post name=filefrm>\n";

for ($j=0;$j<$naf;$j++) { /*print files*/
  if (strstr($allfiles["fname"][$j],".php") 
  //|| strstr($allfiles["fname"][$j],".txt")
  ) continue;
  if (($allfiles["fname"][$j]==""))  continue;
  if (($allfiles["fname"][$j]=="."))  continue;
  if (($allfiles["fname"][$j]==".."))  continue;
  if (($allfiles["type"][$j]=="d"))  continue;
  //echo " EMPTY $j".$allfiles["type"][$j]; echo "<br>";

  if ($j%2) $col="#C6E7FF"; else $col="#9CCFFF";


  $mtime=filemtime($allfiles["fname"][$j]);
  $size=$allfiles["size"][$j];
  $bps=$allfiles["bitrate"][$j];
  $bpsinfo=$allfiles["bitrateinfo"][$j];


  if ($bpsinfo=="VBR") 
    $bps="<span style='background-color:#efefef'>[$bps] VBR</span>";
  elseif ($bps<128) 
    $bps="<span style='background-color:#0c0c0c;color:white'>[$bps]</span>";
  elseif ($bps==128) 
    $bps="<span style='background-color:#5BBDFF'>[$bps]</span>";
  elseif ($bps==160) 
    $bps="<span style='background-color:lightgreen'>[$bps]</span>";
  elseif ($bps==192) 
    $bps="<span style='background-color:orange'>[$bps]</span>";
  elseif ($bps>192) 
    $bps="<span style='background-color:pink'>[$bps]</span>";

  //ID3
  $cmd="/usr/bin/id3v2 -l \"$path/".$allfiles["fname"][$j]."\" | ".
       " egrep -v \"id3v|unimplem|Content type|No ID3|Position\" ".
       " |sed -e 's/^/<b>/' -e 's/:/:<\/b>/'| tr '\n' '@' | sed 's/@/<br>/g'";
  $fp = popen ("$cmd", "r");
  $buffer = fgets($fp, 4096);
  $buffer=str_replace("'" , "", $buffer);
  $buffer=str_replace("\n" , "", $buffer);
  $ID3s[$j]=chop($buffer) ;
  pclose($fp);


  if (stristr($allfiles["fname"][$j],".mp3") || stristr($allfiles["fname"][$j],".ogg"))  {
    $isaudio=1; 
    $isphoto=0;
    $img="sound1.gif";
  }
  elseif (stristr($allfiles["fname"][$j],".jpg") )  {
    $isaudio=0;
    $isphoto=1;
    $img="generic.gif";
  }
  else {
    $isaudio=0;
    $isphoto=0;
    $img="generic.gif";
  }

  echo "<tr bgcolor=\"$col\" >\n <td valign=middle width=10>".
       "<a href=\"?path=".
        urlencode($path)."&mp3file=".urlencode($allfiles["fname"][$j])."\">".
      "<img border=0 src=/icons/$img></a></td>\n";
  echo "  <td><input size=60 type=text name=\"orgfile[]\" ".
       " onmouseover='songinfo(\"$ID3s[$j]\");' ".
       " value=\"".$allfiles["fname"][$j]."\"></td>\n";
  echo "  <td><input size=60 type=text id=newf".($j-$i)." name=\"newfile[]\" ".
       " onmouseover='songinfo(\"$ID3s[$j]\");' ".
       " value=\"";
  if ($isaudio) {
    $newfile=clean1($allfiles["fname"][$j]);
    $newfile=actions($newfile,$j-$i);
  }
  else $newfile=$allfiles["fname"][$j];
  echo "$newfile\"></td>\n";

	 $duration=$allfiles["mins"][$j].":".$allfiles["secs"][$j];
  echo "  <td valign=middle align=right>";

  if ($isaudio) {
    echo "<table width=100% cellspacing=0 cellpadding=0><tr><td>";
    echo "<font size=-1>"."($duration) ". (int)($size/1000)." </td>".
	 "<td><font size=-1>&nbsp;$bps</font></td></tr></table>\n";
  }
  elseif ($isphoto) {
   $x=($j-$i);
   for ($xx=0;$xx<10;$xx++){
     echo "<a href=\"javascript:void(0);\" onclick=\"".
     "x=document.getElementById('newf$x');x.value='0".$xx."photo.jpg';\">".
     "$xx</a>|";
   }
  }


  echo "  <td><a href=\"$PHP_SELF?path=".
	urlencode("$path/$dirs[$i]")."&trans=$trans".
       "&delete=".urlencode($allfiles["fname"][$j])."\">".
       "<img border=0 title=Delete src=/mp3script/images/delete.gif></a></td>";

  echo "</tr>\n";
}
echo "<tr><td bgcolor=#ceceff colspan=6>".
     "<input type=submit value=\"Rename All\"></td></tr>\n";
echo "<input type=hidden name=formaction value=\"renameall\"></td></tr>\n";
echo "<input type=hidden name=path value=\"$path\"></td></tr>\n";
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

  $fn=ereg_replace("[ ]+$" , "", $fn);
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
  $newfn=ereg_replace("-" , " - ", $newfn);
  $newfn=ereg_replace("-[ ]+-" , " -  ", $newfn);
  $newfn=ereg_replace("[ ]+" , " ", $newfn);
  $newfn=ereg_replace("^[ ]+" , "", $newfn);
  $newfn=str_replace("\\\\" , "", $newfn);
  $newfn=strtolower($newfn);

  return $newfn.$ext;
}

function actions($fn,$j)
{ global $prependstr,$chngfrom,$chngto,$rechngfrom,$rechngto,$txtnam;

  if (isset($prependstr) && !empty($prependstr)) {
    $fn=$prependstr.$fn;
  }
  elseif (isset($chngfrom) && !empty($chngfrom)) {
    $fn=str_replace("$chngfrom" , "$chngto", $fn);
    $fn=str_replace("\\" , "", $fn);
  }
  elseif (isset($rechngfrom) && !empty($rechngfrom)) {
    $fn=ereg_replace("$rechngfrom" , "$rechngto", $fn);
    $fn=str_replace("\\" , "", $fn);
  }
  elseif (isset($txtnam) && !empty($txtnam)) {
    $x = ereg_replace("(\r\n|\n|\r)", "@", $txtnam); 
    $x=explode("@",$x);
    $fn=$x[$j]." -".(sprintf("%02d",$j+1)).".mp3";
    $fn = ereg_replace("(\t|[ ]+)", " ", $fn); 
  }

  return $fn;

}

echo "<table width=100% cellspacing=0  border=1>\n<tr><td width=40%>";

echo "<b>Transformations:</b><br>";
$where=urlencode("$path/$dirs[$i]");
echo "<a href=\"$PHP_SELF?path=$where&trans=21\">21</a>\n";
echo "<a href=\"$PHP_SELF?path=$where&trans=132\">132</a>&nbsp;\n";
echo "<a href=\"$PHP_SELF?path=$where&trans=213\">213</a>&nbsp;\n";
echo "<a href=\"$PHP_SELF?path=$where&trans=231\">231</a>&nbsp;\n";
echo "<a href=\"$PHP_SELF?path=$where&trans=3412\">3412</a>&nbsp;\n";
echo "<a href=\"$PHP_SELF?path=$where&trans=3421\">3421</a>\n";
echo "<a href=\"$PHP_SELF?path=$where&trans=1432\">1432</a>\n";
echo "<a href=\"$PHP_SELF?path=$where&trans=1toend\">1toend</a>\n";
echo "<a href=\"$PHP_SELF?path=$where&trans=2toend\">2toend</a><br>\n";

echo "</td><td>";
echo "<font size=-1>";
echo "<div style=\"height:70px;overflow:auto;\" id=songinfo>&nbsp;</div>";
echo "</font></td></tr>\n</table>\n<br>";


echo "<form method=post name=prep>Prepend:<input name=prependstr type=text>".
     "<input type=submit value=Doit></form>\n";

echo "<form method=post name=rechng>Change:<input name=rechngfrom type=text>\n";
echo "To: <input name=rechngto type=text>".
     "<input type=submit value=Doit> (Regular Expression match) ";
echo "(<a href='javascript:repl1();'>first space to '-'</a>) ";
echo "(<a href='javascript:repl2();'>first 2digit number to '-xx-'</a>)";
echo "\n</form>\n";

echo "<form method=post name=chng>Change:<input name=chngfrom type=text>\n";
echo "To: <input name=chngto type=text>".
     "<input type=submit value=Doit> (Normal match)</form>\n";


echo "<p><a href=\"$PHP_SELF?path=$where\">";
echo "<img align=middle src=/icons/reload.gif title=Refresh border=0>Refresh</a>\n";

echo "<hr>";
echo "Paste below a .jpg url (p.x. an album cover from allmusic.com) to ".
     "download in this directory as 00photo.jpg<br>\n";

echo "<form method=post name=photofrm>".
     "URL:<input size=70 name=photourl type=text>\n".
     "<input type=submit value=Getit></form>\n";

echo "<a href=\"$PHP_SELF?path=".urlencode($path)."&fixphotos=1\"><big>FixBigPhotos</big></a>";
?>
(if 00photo.jpg is too big, it gets resized to 240px and the original is renamed)
<hr>

<table>
<tr>
<th>Messages</th><th>Get filenames from this text area</th>
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
</textarea>
<input type=submit value="Doit">
</form>

</td>

</tr>
</table>

<p>

&copy; sivann 2003 (version 1.187)


</html>

<?
function validateuser()
{
  global $HTTP_ENV_VARS,$HTTP_X_FORWARDED_FOR,$REMOTE_ADDR,$path;

  if (isset($HTTP_X_FORWARDED_FOR))
    $remaddr=$HTTP_X_FORWARDED_FOR;
  else 
    $remaddr=$REMOTE_ADDR;

  $timefile="/var/www/mp3/ipz/$remaddr/timecheck";
  $username=file("/var/www/mp3/ipz/$remaddr/username");
  $username=$username[0];
  $mtime=filemtime($timefile);
  $tim=time();
  if (($tim - $mtime) > 3600) { //expired
    header("Content-type: text/html");
    echo "<h3>Time expired</h3>";
    exit;
  }
  elseif ((!strstr($path,chop($username))) && 
    ($remaddr!="147.102.11.130") && 
    ($remaddr!="147.102.11.134") && 
    ($remaddr!="147.102.11.104") && 
    ($remaddr!="147.102.11.1") && 
    ($remaddr!="147.102.11.103")) {
    header("Content-type: text/html");
    echo "<h3>You can access only your Incoming directory $username from $remaddr ($path)</h3>";
    exit;
  }

}

?>
