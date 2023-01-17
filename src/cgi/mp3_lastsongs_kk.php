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
echo "<tr><th>No.</th><th>Time</th><th>Username</th><th>Song</th></tr>\n";

for ($i=0;$i<$nl && !empty($line[$i]);$i++)  {
//  echo "$line[$i]<br>";
  $fields=explode("#",$line[$i]);
  $time=$fields[0];
  $now_playing[$i][0]=$time;
  $user=$fields[1];
  $now_playing[$i][1]=$user;
  $song=$fields[2];
  $now_playing[$i][2]=$song;
  $songenc=urlencode($song);
  $rank=$i+1;
  echo "<tr><td>".$rank."</td><td>".$now_playing[$i][0]."</td><td>".$now_playing[$i][1]."</td><td>".$now_playing[$i][2]."</td></tr>\n";
}

echo "</table>\n";



?>
