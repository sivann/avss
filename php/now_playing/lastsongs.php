<html>
<body>

<?
include("avssconf.php");
$cmd=$avssdir."bin/lastsongs";

//echo $cmd;exit;
$pfp = popen ($cmd, "r");

$nl=0;
if (!$pfp) { echo 'An error occured'; exit; }
while (!feof ($pfp)) {
    $buffer = fgets($pfp, 4096);
    $line[$nl++]=$buffer;
}
pclose($pfp);

echo "<table border=0 cellspacing=3 cellpadding=2 bgcolor=white width=100%>\n";
echo "<tr><th>No.</th><th>User (host)</th><th>Time</th><th>Song</th></tr>\n";


$dts_now=time();

for ($i=0;$i<$nl && !empty($line[$i]);$i++)  {
  $fields=explode("#",$line[$i]);
  $ip=$fields[0];
  $now_playing[$i][0]=$ip;
  $d=$fields[1].$fields[2];

  $t=$fields[3];
  $t=substr($t,2);

  $dtime="$t[0]$t[1]:$t[2]$t[3]:$t[4]$t[5]";

  $di="$d-2004 $dtime";
  $dts=strtotime($di);
  $dts_diff=($dts_now-$dts);
  //echo "$dts_diff<br>";
  if (!empty($lastsecs) && ($dts_diff> $lastsecs)) {
    break;
  }

//  echo "$dts ".date('l dS of F Y h:i:s A',$dts)." \n <br> ";

  $now_playing[$i][1]=$dtime;
  $song=$fields[4];
  $songdec=chop(rawurldecode($song));
  $now_playing[$i][2]=$songdec;
  $songenc=rawurlencode($songdec);

  //$hostname = gethostbyaddr($ip);
  $hostname=`host -s 1 $ip 147.102.222.210| grep Name | cut -d: -f2`;
  $hostname=trim($hostname);
  $songdec=str_replace(" ","&nbsp;",$songdec);
  if (!strlen($hostname)) 
    $host=$ip;
  else 
    $host=$hostname;

   $fn="$avssdir/data/ipz/$ip/username";
   if (file_exists($fn)) {
     $user=file("/var/www/mp3/ipz/$ip/username");
     $user=chop($user[0]);
   }
   else $user="not-logged-in";

   $songname=basename($songdec);
   $songdir=dirname($songdec);
   $songdir_e=dirname($song);
   $dirurl="<a title=\"$songdir\" href='/avss/cgi-bin/mp3.m3u?path=$songdir_e'>DIR</a>";
   $url="<a title=\"$songdec\" href='/avss/cgi-bin/mp3.m3u?mp3file=1&path=$songenc'>$songname</a>";
   echo "<tr bgcolor=\"#D6DFEC\"><td>".($i+1)."</td><td>$user&nbsp;($host)</td><td>".$now_playing[$i][1].
        "</td><td>[$dirurl]&nbsp;$url</td></tr>\n";
}

echo "</table>\n";

echo "Time now is ".date('H:i:s',time())." <br>\n ";


?>

</body>
</html>

