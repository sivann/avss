<?php
include("avssconf.php");
 //sivann@cs.ntua.gr 2000-2002
 $upload_dir="$audioroot/dwse_oti_exeis/";
 $browse_dir="$avsswwwdir/cgi-bin/mp3.m3u?path=/dwse%5Foti%5Fexeis/";
 $logfile="$upload_dir/00-bywho.txt";
?>
<html>
<!-- Version 1.1 &copy;sivann 2000-->
<HEAD>
   <meta HTTP-EQUIV="Content-Type" CONTENT="text/html; charset=iso-8859-7">
   <meta NAME="GENERATOR" CONTENT="vim">
   <title>File Upload</title>
</head>

<body bgcolor=#efefef text=white link=white alink=yellow vlink=white
      background="<?=$avsswwwdir?>/images/seabg.jpg">

<p>

<center>
<img src="<?=$avsswwwdir?>/images/rainbow_trout_underwater.jpg">
</center>
<p>
<table align=center border=0 cellspacing=0 
       cellpadding=3 >
<tr>
 <th colspan=3>
 <big>Feed the trout1!</big>
 </th>
</tr>

  <FORM ENCTYPE="multipart/form-data" METHOD=POST>
  <INPUT TYPE="hidden" name="MAX_FILE_SIZE" value="11000000">

<tr>
 <td align=right>Send this file:</td>
 <td colspan=2><INPUT size=50 NAME="userfile[]" TYPE="file">
 </tr>
 <tr>
 <td align=right>Send this file:</td>
 <td colspan=2><INPUT size=50 NAME="userfile[]" TYPE="file">
 </tr>
 <td colspan=3>

 <INPUT TYPE="submit" VALUE="Send File">
 </td>
</tr>

<tr>
 <td valign=middle align=middle></td>
 <td colspan=2>&nbsp;</td>
</tr>

<tr>
 <td align=right>Status: </td>
 <td> 

<?php
$fexist=0;
$lala= "Count=".count($_FILES['userfile']['name']);
system("echo $lala >>/tmp/out");

if (count($_FILES['userfile']['name'])) {
  foreach ($_FILES['userfile']['name'] as $key=>$name) {
    if ($_FILES['userfile']['size'][$key]) {
      $fname=$_FILES['userfile']['name'][$key];
      $location = $upload_dir.$fname;
      echo "location=$location<br>";
      if (!($e=file_exists("$location")) || isset($overwrite)) {
	$e?$e="existing":$e="";
	if (!copy($_FILES['userfile']['tmp_name'][$key],"$location")) {
	  print("failed to copy ".$_FILES['userfile']['tmp_name'][$key].
		" to $e $location ...<br>\n");
	}
	else  //copy ok, log it.
	  $status="<font color=#11FF11>$fname: glout glout sais the happy ".
	          "trout</font><br>";
	  if (!($fp=fopen("$logfile","a")))
	    print("failed to open $logfile in $upload_dir ...<br>\n");
	  else {
	    $data="$REMOTE_ADDR\t$fname\n"; 
	    fputs($fp,$data); fclose($fp);
	  }
      } 
      else { //exists kai !overwrite
	$status="<font color=orange>$fname already exists</font><br>";
      }

      echo $status;
    } //if $files
  } //foreach
}


?>

 </td>

 <td valign=middle align=right>
 Overwrite Files <INPUT name="overwrite" TYPE="checkbox" VALUE="1">
 </td>
</tr>

</FORM>
<caption align=bottom>
<?php echo "Files go <a href=\"$browse_dir\">here</a> "?>
</table>
<a href=<?=$avsswwwdir?>/cgi-bin/mp3.m3u>back to mp3z</a>
<p>

<table align=center>
<tr><td>
<font size=+1>
ΟΔΗΓΙΕΣ
<ol>
<li>Μη μας παιδεύετε τσάμπα, τα ονόματα των τραγουδιών <b>πρέπει</b> να έχουν 
    την εξής μορφή:
   <br> <h2>artist - song name - other info.mp3</h2>
<li>Ελληνικά filenames δεν υποστηρίζονται ακόμα.
<li>Upload metallica,britney,or "Ελληνικά σκυλάδικα 90's" and get banned.<br> 
    Τα λαϊκά/σκυλάδικα δεκαετιών 50/60/70  επιτρέπονται.
<li>Ξαναβάλε κάτι που υπάρχει και θα φερω τον <a href="/images/julian.jpg">Julian.</a>
</ol>
</td></tr>
</table>

</body>
</html>
