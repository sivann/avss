<?
include("avssconf.php");
header("Expires: 0"); 
header("Cache-Control: private"); 

//to work without register_globals
foreach($_REQUEST as $k => $v)
{
  ${$k} = $v;
}

if (isset($_SERVER["HTTP_X_FORWARDED_FOR"]))
   $remaddr=$_SERVER["HTTP_X_FORWARDED_FOR"];
else  
   $remaddr=$_SERVER["REMOTE_ADDR"];

$basepath="$avssdir/data/";
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
