<?php
include("avssconf.php");
 //sivann@cs.ntua.gr 2000-2002
 $upload_dir="$audioroot/dwse_oti_exeis/";
 $browse_dir="$avsswwwdir/cgi-bin/mp3.m3u?path=/dwse%5Foti%5Fexeis/";
 $logfile="$upload_dir/00-bywho.txt";
 $ipdir="$avssdir/data/ipz/";
 set_time_limit(1200);

//to work without register_globals
foreach($_REQUEST as $k => $v)
{
  ${$k} = $v;
}


if (isset($_SERVER["HTTP_X_FORWARDED_FOR"]))
   $remaddr=$_SERVER["HTTP_X_FORWARDED_FOR"];
else  
   $remaddr=$_SERVER["REMOTE_ADDR"];

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
 <big>Feed the trout!</big>
 </th>
</tr>

  <FORM ENCTYPE="multipart/form-data" 
   <?php echo " ACTION=\"http://$HTTP_HOST$SCRIPT_NAME\""; ?>
      METHOD=POST>
  <INPUT TYPE="hidden" name="MAX_FILE_SIZE" value="48000000">

 <tr>
 <td align=right>Send this file:</td>
 <td colspan=2><INPUT size=50 NAME="userfile[]" TYPE="file">
 </tr>

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

uplerror();
if (count($_FILES['userfile']['name'])) {
  foreach ($_FILES['userfile']['name'] as $key=>$name) {
    if ($_FILES['userfile']['size'][$key]) {
      $fname=$_FILES['userfile']['name'][$key];
      //or disable magic_quotes_gpc in php.ini
      $fname=str_replace("\\'","'",$fname); 
      $location = $upload_dir.$fname;
      //echo "location=$location<br>";
      if (!($e=file_exists("$location")) || isset($overwrite)) {
	$e?$e="existing":$e="";
	if (!copy($_FILES['userfile']['tmp_name'][$key],"$location")) {
	  print("failed to copy ".$_FILES['userfile']['tmp_name'][$key].
		" to $e $location ...<br>\n");
	}
	else {  //copy ok, log it.
	  $status="<font color=#11FF11>$fname: glout glout sais the happy ".
	          "trout</font><br>";
	  chmod ("$location", 0644);
	  if (!($fp=fopen("$logfile","a")))
	    print("failed to open $logfile in $upload_dir ...<br>\n");
	  else {
	    $username=file("$ipdir/$remaddr/username");
	    $username=$username[0];
	    $username=chop($username);
	    $today = date("D M j G:i:s T Y");
	    //$data="($today)\t($REMOTE_ADDR)\t($username)\t$fname\n"; 
	    $data=sprintf("%-32s %-16s %-10s (%s)\n",$today,$remaddr,$username,$fname); 
	    fputs($fp,$data); 
	    fclose($fp);
	  }
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
<?php echo "Files go <a href=\"$browse_dir\">here</a> ";
echo "<center>MAX file size: ".
     ini_get("upload_max_filesize"). 
     " bytes.</center>";
?>
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
<li>Upload metallica,britney,or "Ελληνικά σκυλάδικα 90's,00's" and get banned.<br> 
    Τα λαϊκά/σκυλάδικα δεκαετιών 50/60/70/80  επιτρέπονται.
<li>Ξαναβάλε κάτι που υπάρχει και θα φερω τον <a href="/images/julian.jpg">Julian.</a>
</ol>
</td></tr>
</table>

</body>
</html>

<?
function uplerror()
{
global $_FILES;

 foreach($_FILES["userfile"]["error"] as $key => $value){
    echo "<br>File ".($key+1).":";
    switch ($_FILES['userfile']['error'][$key]) {
     case UPLOAD_ERR_INI_SIZE:
	 echo "The uploaded file exceeds the upload_max_filesize directive (".
	       ini_get("upload_max_filesize").") bytes in php.ini.";
     break;
     case UPLOAD_ERR_FORM_SIZE:
	 echo "The uploaded file exceeds the MAX_FILE_SIZE ".
               "directive that was specified in the HTML form.";
     break;
     case UPLOAD_ERR_PARTIAL:
	 echo "The uploaded file was only partially uploaded.";
     break;
     case UPLOAD_ERR_NO_FILE:
	 echo "No file was uploaded.";
     break;
     case UPLOAD_ERR_NO_TMP_DIR:
	 echo "Missing a temporary folder.";
     break;
     case UPLOAD_ERR_OK:
	 echo "No upload error";
     break;
     default:
	 echo  "An unknown file upload error occured" ;
    }
 }//foreach
echo "<br>";

}



