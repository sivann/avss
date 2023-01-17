<?
header("Expires: 0");      

//header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT"); 
header("Last-Modified: Sat, 20 Nov 1998 20:51:00 GMT"); 
header("Cache-Control: no-cache, must-revalidate"); 


?>

<html>
<head>
<title>Big brother is watching you...</title>
<script lanuage="JavaScript" src="js/dynlayer.js"></script>
<script lanuage="JavaScript" src="js/newsticker.js"></script>
<script lanuage="JavaScript">

<?
$cmd="/var/www/mp3/bin/lastsongs";
$pfp = popen ($cmd, "r");

$nl=0;
if (!$pfp) { echo 'An error occured'; exit; }
while (!feof ($pfp)) {
    $buffer = fgets($pfp, 1024);
    $line[$nl++]=$buffer;
}
pclose($pfp);
?>


<!--
onload = init
function init() {
	ticker.activate()
}

ticker = new NewsTicker(1,1,750,40)

<?

$dts_now=time();
for ($i=0;$i<10 && !empty($line[$i]);$i++)  {
  $fields=explode("#",$line[$i]);
  $ip=$fields[0];
  $d=$fields[1];
  $t=$fields[2];

  $t=substr($t,2);
  $dtime="$t[0]$t[1]:$t[2]$t[3]:$t[4]$t[5]";

  $di="$d-2004 $dtime";
  $dts=strtotime($di);
  $dts_diff=($dts_now-$dts);

  if (!empty($lastsecs) && ($dts_diff> $lastsecs)) {
    break;
  }


  $song=$fields[3];
  $songdec=chop(rawurldecode($song));
  $songenc=rawurlencode($songdec);
  $songname=basename($songdec);
  //$hostname = gethostbyaddr($ip);
  $hostname=`host -s 1 $ip 147.102.222.210| grep Name | cut -d: -f2`;
  $hostname=trim($hostname);
  $songname=str_replace(" ","&nbsp;",$songname);
  if (!strlen($hostname)) 
    $host=$ip;
  else 
    $host=$hostname;

  $fn="/var/www/mp3/ipz/$ip/username";
  if (file_exists($fn)) {
    $user=file("/var/www/mp3/ipz/$ip/username");
    $user=chop($user[0]);
  }
  else $user="not-logged-in";
  $url=($i+1)." <a href='/cgi-bin/mp3.m3u?mp3file=1&path=$songenc'>$songname</a>";

  echo "var i$i  = \"<span class=lala title='$dtime'>".
        "$url&nbsp;:&nbsp;$user&nbsp;($host)</span>\"\n";
  echo "ticker.addItem(i$i);\n";
 }
echo "var i$i = '<span class=lala><b>Afhste ta mp3 kai grapste kanena paper</b></span>';";
echo "ticker.addItem(i$i);\n";

?>



ticker.build();

writeCSS(ticker.css);

//-->
</script>

<style type="text/css">
<!--
.tickerstyle {font-family:"Tahoma"; font-size:12pt; color:#00AA00;}
.lala { color:white; background: black;}
a { color:white; background: black;}
  
-->
</style>

</head>

<body  STYLE="background-color:transparent" bgcolor="white">
<script language="JavaScript">document.write(ticker.div)</script>
</body>
</html>
