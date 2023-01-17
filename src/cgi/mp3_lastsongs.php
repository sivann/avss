<?
$cmd="/var/www/mp3/bin/lastsongsof";
$pfp = popen ($cmd, "r");

$nl=0;
if (!$pfp) { echo 'An error occured'; exit; }
while (!feof ($pfp)) {
    $buffer = fgets($pfp, 512);
    $line[$nl++]=$buffer;
}
pclose($pfp);


echo "<table border=1 width=100%>\n";
echo "<tr><th>Time</th><th>Username</th><th>Song</th></tr>\n";

for ($i=0;$i<$nl && !empty($line[$i]);$i++)  {
  //echo "$line[$i]<br>";
  $fields=explode("#",$line[$i]);
  $time=$fields[0];
  $user=$fields[1];
  $song=$fields[2];
  $songenc=urlencode($song);
  echo "<tr><td>($time)</td><td>$user</td><td><a href=\"/cgi-bin/mp3.m3u?path=$songenc&mp3file=1\">$song</a></td></tr>\n";
}

echo "</table>\n";



?>
