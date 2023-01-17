<?php
 //sivann@cs.ntua.gr 2000
 $upload_dir="/tmp";

 $output_dir="/home/dalk/www/tmp";
 $browse_dir='/~dalk/tmp';
 $logfile="$upload_dir/bywho";

 //$cdl_file="$upload_dir/cdcover.cdl";
 //$cdl="/home/dalk/bin/cdl/cdl.pl";


 //CDL command:
 //$cmd="$cdl -c 1 -s <$cdl_file>cdcover.tex;latex cdcover;dvips -o $output_dir/cdcover.ps cdcover.dvi";

 $trackfile="/tmp/tracks";
 $cdcover="/home/dalk/bin/cdcover";

?>

<html>
<!-- Version 0.1 &copy;sivann 2002-->
<HEAD>
   <meta HTTP-EQUIV="Content-Type" CONTENT="text/html; charset=iso-8859-7">
   <meta NAME="GENERATOR" CONTENT="vim">
   <title>File Upload</title>
</head>

<body bgcolor=#efefef>

<p>

<center>
</center>
<p>
<table align=center border=1 cellspacing=0 
       cellpadding=3 >
<tr>
 <th colspan=3>
 <big>CD Cover Generator</big>
 </th>
</tr>

<tr>
  <FORM ENCTYPE="multipart/form-data" <?php echo "ACTION=\"$PHP_SELF\""; ?>
      METHOD=POST>
  <INPUT TYPE="hidden" name="MAX_FILE_SIZE" value="11000000">
<tr><td align=right>CD Title:<td colspan=2><INPUT size=50 NAME="cdtitle" value="<?=$cdtitle?>"></td></tr>
<tr><td align=right>CD Artist:<td colspan=2><INPUT size=50 NAME="cdartist" value="<?=$cdartist?>"></td></tr>
<tr><td align=right>CD Info:<td colspan=2><INPUT size=50 NAME="cdinfo" value="<?=$cdinfo?>"></td></tr>
<tr><td align=right>Flap Text:<td colspan=2><INPUT size=50 NAME="cdflap" value="<?=$cdflap?>"></td></tr>
<tr><td align=right>Cover Image (only with jewel case):<td colspan=2><INPUT size=50 TYPE=file value="<?=$cdimg?>" NAME="cdimg"></td></tr>
<tr>
 <td align=right><b>Enter Playlist file (.m3u)</b></td>
 <td colspan=2><INPUT size=50 NAME="userfile" TYPE="file" value="<?=$userfile?>">
	       <INPUT TYPE="submit" VALUE="Send File">
 </td>
</tr>

<tr>
 <td align=right>
   Slim case (default=jewel case)
   <INPUT name="wantslim" TYPE="checkbox" VALUE="1">
 </td>
 <td align=right>
   Separate covers on printout
   <INPUT name="vspace" TYPE="checkbox" VALUE="1">
 </td>
 <td align=right>
   Capitalize 1st letter
   <INPUT name="capitalize" TYPE="checkbox" checked VALUE="1">
 </td>
</tr>

<tr>
 <td align=right>Status: </td>
 <td> 
<?php

$fexist=0;
if (isset($userfile) && $userfile!="none" && $userfile_size < $MAX_FILE_SIZE) {
  if (!isset($overwrite) && file_exists("$upload_dir/$userfile_name")) {
    $fexist=1;
  }
  else if (!copy($userfile,"$upload_dir/$userfile_name")) 
    print("failed to copy $userfile_name to $upload_dir ...<br>\n");
  else  if (!unlink($userfile)) 
    print("failed to delete $userfile\n");
  
  if (!$fexist) {
    if (!($fp=fopen("$logfile","a")))
      print("failed to open $logfile in $upload_dir ...<br>\n");
    else {
      $data="$REMOTE_ADDR\t$userfile_name\n";
      fputs($fp,$data);
      fclose($fp);
    }
  }
}

$generate=0;

if ($fexist)
  echo "<font color=red>file already exists!</font>";
else if ($userfile=="none")
  echo "<font color=red>no file sent</font>";
else if ($userfile!="none" && isset($userfile)) {
  echo "<font color=green>File accepted, wokring..</font>";
  $generate=1;
}
else if ($userfile!="none" && !isset($userfile))
  echo "Waiting for file";
else if ( $userfile_size < $MAX_FILE_SIZE) 
  echo "<font color=red>no file sent: size limit is $MAX_FILE_SIZE bytes</font>";


if (isset($cdimg) && ($cdimg !="none")) {
  move_uploaded_file($cdimg, "/tmp/cdcover-img");
  chdir ("/tmp");
  $pfp = popen ("convert cdcover-img cdcover-img.eps", "r");
  pclose($pfp);
  unlink ("/tmp/cdcover-img");
  $wantcover=1;
}
else
  $wantcover=0;


?>

 </td>

 <td valign=middle align=right>
 Overwrite Files <INPUT name="overwrite" TYPE="checkbox" checked VALUE="1">
 </td>
</tr>
</FORM>
</table>

<?php
if ($generate) {
  //parse m3u
  //echo "$userfile_name, $upload_dir";
  $thefile="$upload_dir/$userfile_name";
  clearstatcache();
  $st_ar=stat($thefile);
  if (empty($st_ar)) {
    print("failed to stat $thefile<br>\n");
    exit;
  }
  $playlist=file($thefile);

  if (strncmp($playlist[0],"#EXTM3U",5)) {
    echo '#EXTM3U header not found. Invalid m3u file, maby old winamp version?';
    exit;
  }

  if (!($fp=fopen("$trackfile","w"))) {
    print("failed to open $trackfile <br>\n");
    exit;
  }

  if (!isset($cdtitle)) $cdtitle="";
  if (!isset($cdartist)) $cdartist="";
  if (!isset($cdinfo)) $cdinfo="";

  //fputs($fp,"Title: $cdtitle\n");
  //fputs($fp,"Artist: $cdartist\n");
  //fputs($fp,"Note: $cdinfo\n\n");

  $songnum=0;
  while (list ($lnum, $line) = each ($playlist)) {
      if (strncmp($line,"#EXTINF",6))
        continue;
      $duration=substr($line,8,3);
      $songname=rtrim(substr($line,12));
      $songnum++;
      $min=(int)($duration/60);
      $sec=$duration-$min*60;
      $min=sprintf("%02d",$min); //put a leading zero when needed
      $sec=sprintf("%02d",$sec); 
      if (isset($capitalize) && $capitalize) {
        $songname=strtolower($songname);
        $songname=ucwords($songname);
      }
      if (isset($vspace) && $vspace) {
        $vspace="-vspace";
      }
      if (isset($wantslim) && $wantslim) {
        $wantslim="-wantslim";
      }
      if ($wantcover) {
        $cdimg="-img \"cdcover-img.eps\"";
      }
      else
        $cdimg="";
      //echo "[<b>Line $lnum:</b> $line] $songnum-$songname [$min:$sec]<br>\n";
      fputs($fp,"$min:$sec@$songname\n");
  }
  fclose($fp);

 $cmd="$cdcover -f \"$trackfile\" -title \"$cdtitle\" -artist \"$cdartist\" -info \"$cdinfo\" -flap \"$cdflap\" $wantslim $vspace $cdimg";
  echo "CMD: $cmd<p><pre>";
  chdir($upload_dir);


  $pfp = popen ($cmd, "r");
  while (!feof ($pfp)) {
    $buffer = fgets($pfp, 4096);
    echo $buffer;
  }
  pclose($pfp);
  echo "</pre>";


  //unlink("$upload_dir/cdcover.tex");
if (!copy("/tmp/cdcover.ps","$output_dir/cdcover.ps")) 
  print("failed to copy /tmp/cdcover.ps to $output_dir...<br>\n");
unlink("/tmp/cdcover.ps");
unlink($infile);
if ($wantcover) unlink("/tmp/cdcover-img.ps");

echo '<p>';
echo "<b>Get it: <a href=\"$browse_dir/cdcover.ps\">cdcover.ps</a></b>";

}
?>

<p>
&copy; Spiros Ioannou 2002

</body>
</html>
