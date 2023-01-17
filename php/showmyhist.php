<?php
include("avssconf.php");
//sivann 11/2005

//to work without register_globals
foreach($_REQUEST as $k => $v)
{
  ${$k} = $v;
}

$dbfile="$avssdir/data/mp3log.db";
//$v= sqlite_libversion(); // only sqlite 2 supported by php

if ($username==""||!isset($username)) {
echo "username not set";
exit;
}
$username = str_replace("'", "A", "$username");
$username = str_replace(";", "B", "$username");

$cmd="sqlite3 $dbfile \"select * from songhits where user = '$username' ORDER BY time DESC LIMIT 500\"";

$fp=popen("$cmd","r");

echo "<html><body>";
echo "<table border=0 cellspacing=1 cellpadding=1 bgcolor=white>\n";
$i=0;
while (!($ab=connection_aborted()) && ($s=fgets ($fp, 20480))) {
  if (($i++)%2) $c="#ffffff"; else $c="#D6DFEC";
  list($id,$time,$ip,$ip2,$user,$path,$song,$agent,$xx)=explode("|",$s);
  $path=rawurlencode($path);
  $when=date("d/m, H:i",$time); 
  $when = str_replace(" ", "&nbsp;", "$when");
  echo "<tr bgcolor='$c'><td>$when</td><td> $ip-$ip2 </td><td>$user </td><td><a href='$avsswwwdir/cgi-bin/mp3.m3u?mp3file=1&path=$path'>$song</a></td></tr>\n";

}

echo "</table>";
echo "</body></html>";
pclose($fp);
