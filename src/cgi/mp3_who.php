<?
header("Expires: 0"); 
header("Cache-Control: private"); 

if (isset($HTTP_X_FORWARDED_FOR))
  $remaddr=$HTTP_X_FORWARDED_FOR;
else 
  $remaddr=$REMOTE_ADDR;


$basepath="/var/www/mp3/data/";
$whonames="$basepath/whonames";

$fp = popen ($whonames, "r");

if (!$fp) {
 echo 'An error occured';
 exit;
}

$nl=0;
while (!feof ($fp)) {
    $buffer = fgets($fp, 4096);
    $line[$nl++]=$buffer;
}
pclose($fp);

echo "<table border=1 width=100%>\n";
echo "<tr><th>Username</th><th>Full Name</th></tr>\n";

for ($i=0;$i<$nl && !empty($line[$i]);$i++)  {
  //echo "$line[$i]<br>";
  $fields=explode(":",$line[$i]);
  echo "<tr><td>$fields[0]</td><td>$fields[1]</td></tr>\n";
}

echo "</table>\n";


?>
