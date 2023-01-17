<?
include_once ("avssconf.php");

function isexpired($userdir)
{
  
  if (!file_exists($userdir."/timecheck")) {
    echo "$userdir/timecheck  does not exist<br>";
    return 1;
  }
  else {
    $stat = stat($userdir."/timecheck");
    $mtime=$stat['mtime'];
    $age=time()-$mtime;
    if ($age<(120*60)) return 0;
  }
  return $age;
}

?>
