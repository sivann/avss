<?php
/*
    sivann 2001-2004
    
*/
//to work without register_globals
foreach($_REQUEST as $k => $v)
{
  ${$k} = $v;
}

include("avssconf.php");

header("Expires: 0"); 
header("Cache-Control: private"); 


if (isset($_SERVER["HTTP_X_FORWARDED_FOR"]))
   $remaddr=$_SERVER["HTTP_X_FORWARDED_FOR"];
else 
   $remaddr=$_SERVER["REMOTE_ADDR"];

$userfile="$avssdir/data/ipz/$remaddr/username";
$basedir=$audioroot;

$username=file($userfile);
$username=chop($username[0]);


validateuser();

function validateuser()
{
  global $avssdir,$remaddr,$path,$sess;
  $sess=$_GET['sess'];


  $timefile="$avssdir/data/ipz/$sess/timecheck";
  $username=file("$avssdir/data/ipz/$sess/username");
  $username=$username[0];
  $mtime=filemtime($timefile);
  $tim=time();
  if (($tim - $mtime) > 3600) { //expired
    header("Content-type: text/html");
    echo "<h3>Time expired for $sess, please login again</h3>";
    exit;
  }

}

if ("$dir") {
  set_time_limit(3600);
  ignore_user_abort(true);
  header("Connection: close\n");


  $dir = str_replace ("./","/", $dir); 
  $dir = ereg_replace ("/+","/", $dir); 

  if ($dir[strlen($dir)-1]=="/") 
   $dir[strlen($dir)-1]=" "; 
   $dir=trim($dir);

  if (!strstr($dir,"artists")) {
    echo "invalid dir, not in artists";
    exit;
  }


  if (count(explode("/",$dir))<4){
    echo "invalid dir, not enough depth";
    exit;
  }


  $songsdir="$basedir$dir";
  $dir="./$dir";
  chdir($basedir);

  if (!is_dir($dir)) {
    echo "dir not found";
    exit;
  }

  //find tar size - foverh idea tou vip
  $cmdtot="tar --total -chf /dev/null -C \"$basedir\" \"$dir\" 2>&1 | grep Total | awk '{print \$4}' ";
  $fp=popen("$cmdtot","r");
  $bytes=fgets($fp,1024);
  pclose($fp);

  $x=basename($dir);
  $fname=$x.".tar";


  header ("Content-length: $bytes\n");
  header ("Content-disposition: attachment; filename=\"$fname\"\n");
  header ("Content-type: application/octet-stream\n\n");

  //kapoia den skotwnontai me broken pipe.
  $cmd=" echo $$ >/tmp/mp3-lala.pid;exec tar chf - -C \"$basedir\" \"$dir\"";
  $fp=popen("$cmd","r");
  while (!($ab=connection_aborted()) && ($s=fread ($fp, 20480))) {
   echo $s;
  }

  //$r=pclose($fp); // buggy, menei to tar kai trexei
  $pid=file("/tmp/mp3-lala.pid");
  system("/bin/kill $pid[0]");
  unlink("/tmp/mp3-lala.pid");

  exit;
}
  

?>

<html>
<!-- 
  (c) sivann 2001-
  ρεμάλι ασε κάτω τον κώδικα 

-->
lele
</html>
