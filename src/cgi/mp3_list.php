<?php
/*
    move{top|bottom} must work
    Playlist Editor 
    sivann 2001-2003
    todo: anti megalo url, post data gia na doulevei se IE
*/

$table2_bgcolor="#afafaf";

header("Expires: 0"); 
header("Cache-Control: private"); 

if (isset($HTTP_X_FORWARDED_FOR))
  $remaddr=$HTTP_X_FORWARDED_FOR;
else 
  $remaddr=$REMOTE_ADDR;


$basepath="/var/www/mp3/";
$basepathbin="/var/www/mp3/bin/";
$userfile="$basepath/ipz/$remaddr/username";
$pldir="$basepath/playlists";
$currplfile="$basepath/ipz/$remaddr/playlist";
$linkdir="$basepath/ipz/$remaddr/linkdir";
$findsimilar="/var/www/mp3/bin/findsimilar.pl";
$basehost="http://www.xxx.yyy.com";

$username=file($userfile);
$username=chop($username[0]);


function sec2minsec($totsecs)
{
  $hours=floor($totsecs/3600);
  $mins=floor(($totsecs-$hours*3600)/60);
  $secs=$totsecs%60;
  $ret=sprintf("%d:%02d",$mins,$secs);
  return $ret;
}



function get_currpl()
{
  global $currplfile,$username,$pldir;

  if (!file_exists($currplfile)) {
   $fp=fopen($currplfile,"w");
   flock($fp,2);
   fputs($fp,"default");
   flock($fp,3);
   fclose($fp);
   $currpl="default";
  }
  else {
    $currpl=file($currplfile);
    $currpl=chop($currpl[0]);
    $len=strlen($currpl);
    if ($len==0) $currpl="default";
  }
  return $currpl;
}

function get_plfile()
{
  global $currpl,$pldir,$plfile,$username;

  $currpl=get_currpl();
  $plfile="$pldir/$username-$currpl";
  if (!file_exists($plfile)) {
   touch($plfile);
  }
}

function get_tempplfile()
{
  global $currpl,$pldir,$tempplfile,$username;

  $tempplfile="$pldir/$username-temp";
  if (!file_exists($tempplfile)) {
   touch($tempplfile);
  }
}

function set_currpl($plname)
{
  global $currplfile,$username,$pldir,$currpl;

  $fp=fopen($currplfile,"w");
  flock($fp,2);
  fputs($fp,$plname);
  flock($fp,3);
  fclose($fp);
  $currpl=$plname;
  $plfile="$pldir/$username-$currpl";
  $fp=fopen($plfile,"r");
  flock($fp,3); //remove previous locks
  fclose($fp);
}


function create_playlist($plname)
{
  global $currplfile,$username,$pldir;
  $plname = str_replace (" ","_", $plname);

  $plfile="$pldir/$username-$plname";

  if (file_exists($plfile)) {
    return;
  }
  touch($plfile);
}

function delete_playlist($plname)
{
  global $currplfile,$username,$pldir;

  $plfile="$pldir/$username-$plname";

  if ($plname != "default" && file_exists($plfile))
    unlink($plfile);

  set_currpl("default");
  get_plfile();
}

function printstats()
{
  global $pldata,$plfile;
  $pldata=file($plfile);
  if ($pldata[0][0]=="#") {
    $line=explode("%%%",$pldata[0]);
    $totime=$line[1];
    $totsize=$line[2];
  }
  echo "<b>Total:</b> $totime, $totsize";
}


get_plfile();

if ($action == "playsongs" ) { //play selected songs
  $pldata=file($plfile);
  if ($pldata[0][0] !="#") { //an den arxizei me sxolio
    $hasheader=0; 
  }
  else {
    $hasheader=1;
    $header=array_shift($pldata);
  }

  header ("Content-Type: audio/mpeg-url");
  header ("Connection: close");
  $songs=explode(",",$song);
  $songnum=count($songs)-1;
  for ($i=0;$i<$songnum;$i++) {
    $line=$pldata[(int)($songs[$i])-1];
    $line=explode("%%%",$line);$line=$line[0]; //get song path - discard info
    if ($line[0]=="#") continue; //stats
    $songpath=rawurlencode(chop($line));
    $songpath = str_replace ("%2F","/", $songpath); //for winamp song title
    echo "$basehost/cgi-bin/mp3.m3u?mp3file=1&mp3data=1&path=$songpath\n";
  }
  exit;
}
else if ($action == "playlist") {
  $pldata=file($plfile);
  header ("Content-type: audio/mpeg-url");
  while (list ($line_num, $line) = each ($pldata)) {
    $line=explode("%%%",$line);$line=$line[0]; //get song path
    if ($line[0]=="#") continue; //stats
    $songpath=rawurlencode(chop($line));
    $songpath = str_replace ("%2F","/", $songpath); //for winamp song title
    echo "$basehost/cgi-bin/mp3.m3u?mp3file=1&mp3data=1&path=$songpath\n";
  }
  exit;
}

else if ($action == "showplaylist") {
  $pldata=file($plfile);
  header ("Content-type: text/plain");
  while (list ($line_num, $line) = each ($pldata)) {
    $line=explode("%%%",$line);$line=$line[0]; //get song path
    if ($line[0]=="#") continue; //stats
    $songpath=rawurldecode(chop($line));
    $songpath = str_replace ("%2F","/", $songpath); //for winamp song title
    $song=basename($songpath,"/"); //get song path
    //echo "$basehost/cgi-bin/mp3.m3u?mp3file=1&mp3data=1&path=$songpath\n";
    echo "$song\n";
  }
  exit;
}

else if ($action == "plselect") {
  set_currpl($listplaylist);
  get_plfile();
}
else if ($action == "getbigtar") {
  set_time_limit(3600);
  ignore_user_abort(true);
  header("Connection: close\n");


  //katharisma linkdir
  if (file_exists($linkdir)) { //exists
    system("/bin/rm $linkdir/*");
  }
  else
    mkdir($linkdir,0775);

  $pldata=file($plfile);
  $i=0;
  while (list ($line_num, $line) = each ($pldata)) {
    $line=explode("%%%",$line);$line=$line[0]; //get song path - discard info
    if ($line[0]=="#") continue;  //stats
    $songpath=chop($line);
    $i++;$c=sprintf("%02d - ",$i);
    if (isset($nn) && $nn) {
      $lnk=basename($songpath);
      $fname=$currpl."-nn.tar";
    }
    else { //prefix song number
      $lnk=$c.basename($songpath);
      $fname=$currpl.".tar";
    }
    if ($lnk=="") continue; //keno filename

    $lnk1=$linkdir."/".$lnk;
    if (file_exists($lnk1))  {
      if (!unlink($lnk1)) echo "<br>Could not unlink ($lnk1) <br>\n";
      exit;
    }
    symlink("/kill-9/sorted/$songpath",$lnk1);
    $files=$files." \"$lnk\"";
  }

  //find tar size - foverh idea tou vip
  $cmdtot="tar --total -chf /dev/null -C $linkdir $files 2>&1 | grep Total | awk '{print \$4}' ";
  $fp=popen("$cmdtot","r");
  $bytes=fgets($fp,1024);
  pclose($fp);

  header ("Content-length: $bytes\n");
  header ("Content-disposition: attachment; filename=\"$fname\"\n");
  header ("Content-type: application/octet-stream\n\n");

  //kapoia den skotwnontai me broken pipe.
  $cmd=" echo $$ >/tmp/mp3-$username.pid;exec star chf - -C $linkdir $files";
  //$cmd=" echo $$ >/tmp/mp3-$username.pid;exec tar -chf - -C $linkdir $files";
  //$cmd=" echo $$ >/tmp/mp3-$username.pid;exec star cdf - $files";
  //passthru($cmd);
  $fp=popen("$cmd","r");
  while (!($ab=connection_aborted()) && ($s=fread ($fp, 20480))) {
   echo $s;
  }

  //$r=pclose($fp); // buggy, menei to tar kai trexei
  $pid=file("/tmp/mp3-$username.pid");
  system("/bin/kill $pid[0]");
  unlink("/tmp/mp3-$username.pid");

  exit;
}
  
else if ($action == "cdcover") {
  //header ("Content-length: $bytes\n");
  $fname=get_currpl();
  get_plfile();
  $cmd1="grep -v '#' \"$plfile\" | cut -d'%' -f1  |sed -e 's/.*permanent.//' -e 's/^.\///' -e 's/.*Incoming\/.*\///' >/tmp/cdlabel.txt";
  $fp=popen("$cmd1","r");
  pclose($fp);
  header ("Content-disposition: attachment; filename=\"$fname.ps\"\n");
  header ("Content-type: application/octet-stream\n\n");

  $cmd="/usr/local/bin/cdlabelgen -f /tmp/cdlabel.txt";
  //$cmd="grep -v '#' \"$plfile\" | cut -d'%' -f1 | /usr/local/bin/cdlabelgen";
  $fp=popen("$cmd","r");
  while (!($ab=connection_aborted()) && ($s=fread ($fp, 20480))) {
   echo $s;
  }

exit;
}

?>

<html>
<!-- 
  (c) sivann 2001-
  ρεμάλι ασε κάτω τον κώδικα 

-->
<head>
<title>Playlist Editor v0.117 - &copy; sivann 2001-2004</title>
<script>

function songinfo(info)
{
  x = document.getElementById("songinfo");
  if (x) x.innerHTML=info;
}

function recalculate() {
   window.open('/mp3script/php/mp3_list.m3u?calcsongstats=1', 'mp3_playlist');
}

function expand() 
{
  dh=document.body.offsetHeight;
  wh=window.innerHeight;
  fs=document.form.elements['list[]'].size;
  //window.resizeTo(document.body.clientWidth,document.body.clientHeight+250);
  if (wh != undefined) {
    if((fs+(wh-dh)/14)<0) return;
    document.form.elements['list[]'].size=fs+(wh-dh)/14;
  }
  else {
    if (dh<250) s=30;
    else s=dh-250;
    document.form.elements['list[]'].size=s/14;
  }
}


function clear_list()
{
  var msg = "Are you sure you want to erase the whole playlist?";
  songlen=document.form.elements['list[]'].length;

  if(songlen){
    if(confirm(msg)) {
      window.open("/mp3script/php/mp3_list.m3u?action=clear", "mp3_playlist");
    }
  }
  else { alert("The playlist is already empty."); }
}

function showplaylist()
{
  window.open("/mp3script/php/mp3_list.m3u?action=showplaylist", "mp3_playlist_txt");
}


function move_song(amount)
{
  selsonglen=0;
  selsongs='';
  songlen=document.form.elements['list[]'].length;

  if (document.form.elements['list[]'].selectedIndex==-1){
    alert('Select something to move first');
    return;
  }

  if (songlen) {
    for(i=0;i<songlen;i++) 
      if (document.form.elements['list[]'][i].selected) {
	selsongs=document.form.elements['list[]'][i].value;
	selsonglen++;
      }
  }
  else {
    alert("The playlist is empty nothing to move."); 
    return;
  }

  if (selsonglen>1) {
    alert("Please select only one song to move."); 
    return;
  }

  window.open("/mp3script/php/mp3_list.m3u?action=movesong&song="
  //+document.form.list.options[document.form.list.selectedIndex].value
  +selsongs
  +"&amount="
  +amount, "mp3_playlist") ;
    
}

function plaction()
{
  if(document.form.listname.value) {
    window.open("/mp3script/php/mp3_list.m3u?action=plaction"
    +"&listname="+document.form.listname.value
    +"&listaction="+document.form.listaction.options[document.form.listaction.selectedIndex].value, 
    "mp3_playlist");
  }
  else { 
    alert("Please choose a playlist name."); 
  }
}


function plselect()
{
  window.open("/mp3script/php/mp3_list.m3u?action=plselect"
  +"&listplaylist="+document.form.listplaylist.options[document.form.listplaylist.selectedIndex].value, 
  "mp3_playlist");
}



function delete_songs()
{
  //txtf=document.form.elements['list[]'][1].value;
  var songs;
  songlen=document.form.elements['list[]'].length;
  selsonglen=0;
  selsongs='';
  if (songlen) {
    for(i=0;i<songlen;i++) 
      if (document.form.elements['list[]'][i].selected) {
	selsongs=selsongs+document.form.elements['list[]'][i].value+',';
	selsonglen++;
      }
  }
  else { 
    alert("Playlist Empty."); 
    return;
  }

  if(selsonglen){
    window.open("/mp3script/php/mp3_list.m3u?action=delsong&amount=-1&song="+selsongs,"mp3_playlist");
  }
  else { alert("You have not selected any song."); }
}


function temp2sel()
{
 window.open("/mp3script/php/mp3_list.m3u?action=temp2sel&amount=-1","mp3_playlist");

}

function sel2temp()
{
  var songs;
  songlen=document.form.elements['list[]'].length;
  selsonglen=0;
  selsongs='';
  if (songlen) {
    for(i=0;i<songlen;i++) 
      if (document.form.elements['list[]'][i].selected) {
	selsongs=selsongs+document.form.elements['list[]'][i].value+',';
	selsonglen++;
      }
  }
  else { 
    alert("Playlist Empty."); 
    return;
  }

  if(selsonglen){
    window.open("/mp3script/php/mp3_list.m3u?action=sel2temp&amount=-1&song="+selsongs,"mp3_playlist");
  }
  else { alert("You have not selected any song."); }
}





function refresh_list()
{
  var songs;
  songlen=document.form.elements['list[]'].length;
  selsonglen=0;
  selsongs='';
  if (songlen) {
    for(i=0;i<songlen;i++) 
      if (document.form.elements['list[]'][i].selected) {
	selsongs=selsongs+document.form.elements['list[]'][i].value+',';
	selsonglen++;
      }
  }
  if(selsonglen==0){
    selsongs='01,';
  }

  if(songlen){
    window.open("/mp3script/php/mp3_list.m3u?action=refresh&selsongs="+selsongs, "mp3_playlist");
  }
}

function play_list()
{
  if (document.form.elements['list[]'].length){
    window.open("/mp3script/php/mp3_list.m3u?action=playlist", "mp3_playlist");
  }
  else { alert("The playlist is empty."); }
}

function play_songs()
{
  //txtf=document.form.elements['list[]'][1].value;
  var songs;
  songlen=document.form.elements['list[]'].length;
  selsonglen=0;
  selsongs='';
  if (songlen) {
    for(i=0;i<songlen;i++) 
      if (document.form.elements['list[]'][i].selected) {
	selsongs=selsongs+document.form.elements['list[]'][i].value+',';
	selsonglen++;
      }
  }
  else { 
    alert("Playlist empty."); 
    return;
  }

  if(selsonglen){
    window.open("/mp3script/php/mp3_list.m3u?action=playsongs&song="+selsongs, "mp3_playlist");
  }
  else { alert("You have not selected any song."); }
}

function tralala()
{
   window.location.replace('/mp3script/php/mp3_list.m3u?action=dummy');
}

</script>

<style>
  input {font-family:verdana,sans-serif;text-decoration:none;font-size:12px}
  .topbtn {width: 100px; height: 22px; font-size:10px}
  .recalcbtn {height: 20px; font-size:9px}
  .btnimg {height: 14px; vertical-align: middle;}
  select {
   font-family:sans-serif;
   text-decoration:none;
   font-size:11px;
  }

  .songlist { 
    width: 403px ;
  }

  a {
   font-family: verdana,sans-serif,arial;
   font-size:12px;
   text-decoration:none;
  }
  a:hover { color : pink; text-decoration: none} 

  .txtsm {font-family: verdana,sans-serif,arial;font-size:12px}
        
</style>

</head>

<body bgcolor="#009AFF" text="white" vlink=#f0f5CE alink=red link=#f0f5CE 
      marginwidth=0 marginheight=0  topmargin=0 
      background="/images/dot.gif"
      leftmargin=0 >
      <!-- onBlur='self.focus();' -->

<FORM NAME="form" METHOD="POST" ACTION="" ENCTYPE="x-www-form-encoded">
<table border=0 cellpadding=0 cellspacing=1>
<tr align="center">
<td><!--INPUT class=topbtn width=20 TYPE="button" VALUE="PLAY  LIST" 
  onClick='play_list()'-->
<button class=topbtn onclick='play_list()' title="lalala" type=button>
<img class=btnimg align=left src="/mp3script/images/playlistsm1.gif">Play List</button>
</td>
<td><!--INPUT class=topbtn width=20 TYPE="button" VALUE="PLAY  SONGS" onClick='play_songs()'-->
<button class=topbtn onclick='play_songs()' type=button><img class=btnimg align=left src="/mp3script/images/audio_play.gif">Play Songs</button></td>
</td>
<td><!--INPUT class=topbtn width=20 TYPE="button" VALUE="REMOVE SONGS" onClick='delete_songs()'-->
<button class=topbtn onclick='delete_songs()' type=button>
<img class=btnimg align=left src="/mp3script/images/trashsm.gif">Del Songs</button>
</td>
<td><!--INPUT class=topbtn width=20 TYPE="button" VALUE="CLEAR  LIST" onClick='clear_list()'-->
<button class=topbtn onclick='clear_list()' type=button>
<img class=btnimg  align=left src="/mp3script/images/deletepl.gif">Clear List</button>
</td>
</tr>

<tr align="center">
<td><INPUT class=topbtn width=20 TYPE="button" 
  TITLE="Move Selected songs to temp playlist" 
  VALUE="Sel/ed-&gt;Temp" onClick='sel2temp()'></td>
<td><INPUT class=topbtn width=20 TYPE="button" 
    TITLE="Copy temp playlist to current" 
    VALUE="Temp-&gt;Current" onClick='temp2sel()'></td>
<td><INPUT class=topbtn width=20 TYPE="button" VALUE="" onClick='void(0);'></td>
<td><INPUT class=topbtn width=20 TYPE="button" VALUE="Resize" onClick='expand();'></td>
</tr>


</table>
<table name=table1 align=center width=100% cellspacing=0 cellpadding=0 
       border=0>
<tr>
<td width="1%" valign=top>
<span STYLE="font: normal 11px Arial, Helvetica">

<?php 
// make action here
switch ($action) {
  case "addsimilar": 
		$artist=basename($path);
		$cmd="$findsimilar \"$artist\"";
                echo "cmd=$cmd";
		$pfp=popen("$cmd","r");
		if (!$pfp) {
		  echo "Error calling $basepathbin/showplsecs $plfile";
		  break;
		}
		$fp = fopen ($plfile, "a");
	        flock($fp,2);
		while (!feof ($pfp)) {
		  $buff = fgets($pfp, 256);
		  $newsong=str_replace("\\","",$buff);
		  fputs ($fp, $newsong);
		}
		pclose($pfp);
	        flock($fp,3);
		fclose($fp);
 
                break;
  case "addsong": 
		//ppath=path from post data
		if (isset($ppath) && strlen($ppath)) {
		  $path=urldecode($ppath);
		}
		//echo "PPATH=$ppath<p>";
		//echo "PATH=$path<p>";
		//echo "PPP[0]=$PPP[0]<br>";
		if (isset($ppp) && strlen($ppp)) {
		  $urls=$ppp;
		  $path='@@';
		}
		else if (isset($path) && strlen(strstr($path,"@@"))) 
		  $urls=explode("@@",$path);//multiple path 

		$fp = fopen ($plfile, "a");
	        flock($fp,2);

		if (!strlen(strstr($path,"@@"))) { //single path
		  $newsong=str_replace("\\","",$path)."\n";
		  fputs ($fp, $newsong);
		}
		else { //many urls (add all)
		  for ($i=0;$i<count($urls)-1;$i++) {
		    $url=urldecode($urls[$i]);
		    if (!strlen($url)) continue;
		    $newsong=str_replace("\\","",$url)."\n";
		    fputs ($fp, $newsong);
		  }
		}

	        flock($fp,3);
		fclose($fp);
  		break;
  case "clear": 
  		unlink ($plfile);
  		touch ($plfile);
  		break;
  case "refresh": 
  		break;
  case "plaction": 
  		if ($listaction==1)
		  create_playlist($listname);
		else if ($listaction==2)
		  delete_playlist($listname);
  		break;
  case "temp2sel":
		$pldata=file($plfile);
		get_tempplfile();
		if (!strcmp($plfile,$tempplfile)) {break;}
		//get rid of prepending zeros
		$fp = fopen ($plfile, "a"); flock($fp,2);
		$fpt = fopen ($tempplfile, "r");
		$tmp=fread($fpt,filesize($tempplfile));
		fwrite($fp,$tmp);
	        flock($fp,3);
		fclose($fp);
		fclose($fpt);
  		break;
   case "sel2temp":
		$pldata=file($plfile);
		get_tempplfile();
		if (!strcmp($plfile,$tempplfile)) {break;}
		$songs=explode(",",$song);
		$songnum=count($songs)-1;
		//get rid of prepending zeros
		for ($i=0 ; $i < $songnum ; $i++)
		  $songs[$i]=(int)$songs[$i];
		$fp = fopen ($plfile, "w"); flock($fp,2);
		$fpt = fopen ($tempplfile, "w");flock($fpt,2);
		$c=count($pldata);
		  for ($i=0 ; $i < $c ; $i++) {
		    if (!in_array($i+1,$songs))  {
		      fputs($fp,$pldata[$i]);
		    }
		    else {
		      fputs($fpt,$pldata[$i]);
		    }
		  }
	        flock($fp,3); fclose($fp);
	        flock($fpt,3); fclose($fpt);
  		break;
  case "delsong":
		$pldata=file($plfile);
		if ($pldata[0][0] !="#") { //an den arxizei me sxolio
		  $hasheader=0; 
		}
		else {
		  $hasheader=1;
		  $header=array_shift($pldata);
		}
	
		$songs=explode(",",$song);
		$songnum=count($songs)-1;
		//get rid of prepending zeros
		for ($i=0 ; $i < $songnum ; $i++)
		  $songs[$i]=(int)($songs[$i]);
		$fp = fopen ($plfile, "w");
		flock($fp,2);
		$c=count($pldata);
		    
		if ($hasheader) fputs($fp,$header);

		for ($i=0 ; $i < $c ; $i++) {
		  if (!in_array(($i+1),$songs))  {
		    fputs($fp,$pldata[$i]);
		  }
		}
	        flock($fp,3);
		fclose($fp);
  		break;

  case "movesong": 
		//echo "Under construction";exit;
		$pldata=file($plfile);
		$song=(int)$song;
		$c=count($pldata);
		if ($pldata[0][0] !="#") { //an den arxizei me sxolio
		  $hasheader=0;
		}
		else { //arxizei me sxolio
		  $hasheader=1;
		  $header=array_shift($pldata);
		}

		if ($amount==5000) { //bottom
		  $amount=abs($c-$song);
		}
		else if ($amount==-5000) { //top
		  $amount=-$song+1;
		}

		//metakinhsh entos oriwn
		if ((($song+$amount)<=0) || ($song+$amount)>$c) {
		  //re-select the same
		  $amount=0;
		  break;
		}

		if ($amount>0) {
		  //1-afto->temp
		  //2-metafora aftou+1 mexri kai afto+amount mia thesh panw
		  //3-temp-> ayto+amount

		  $temp=$pldata[$song-1];
		  for ($i=($song+1) ; $i <= $song+$amount ; $i++) {
		    $pldata[($i-2)]=$pldata[$i-1];
		  }
		  $pldata[$song+$amount-1]=$temp;
		}
		elseif ($amount<0) {
		  //1-afto->temp
		  //2-metafora aftou-1 mexri kai afto-amount mia thesh katw
		  //3-temp-> ayto-amount
		  $amnt=abs($amount);

	//echo "song=$song, c=$c, am=$amount  $temp<br>";

		  $temp=$pldata[$song-1];
		  for ($i=($song-1) ; $i >= ($song-$amnt) ; $i--) {
		    $pldata[$i]=$pldata[$i-1];
		  }
		  $pldata[$song-$amnt-1]=$temp;

		}

		$fp = fopen ($plfile, "w");
	        flock($fp,2);
		if ($hasheader) fputs($fp,$header);
		for ($i=0 ; $i < $c ; $i++) {
		    fputs($fp,$pldata[$i]);
		}
	        flock($fp,3);
		fclose($fp);
  		break;
  default:
  		break;
}

// print new playlist

echo "<SELECT multiple class=songlist NAME=\"list[]\" SIZE=12 >\n";

$pldata=file($plfile);
$cmod=0;
while (list ($line_num, $line) = each ($pldata)) {
     $line=explode("%%%",$line);
     if ($line[0]=="#") {$cmod=1;continue;}  //stats
     if ($line[1]) {
       $info=explode(":",$line[1]);
       $songrate="rate: ".$info[0];
       $songtime=sec2minsec($info[1]);
       $kb=floor($info[2]/1024);
       $songsize=" size: $kb KBytes";
     }
     else {
       $info="";
       $songtime="------";
       $songrate="";
     }

     $dir=dirname($line[0]);
     $ln=sprintf("%02d",$line_num+1-$cmod);
     echo "\n<OPTION title=\"$dir\" VALUE=\"".$ln."\" ";
     if ($action!="refresh" && ($song+$amount-1==($line_num-$cmod)))
       echo "SELECTED";
     else if ($action=="refresh") { //keep selected as selected
       $songs=explode(",",$selsongs); $songnum=count($songs)-1;
       for ($i=0 ; $i < $songnum ; $i++) {
	 $sn=(int)$songs[$i]-1;
 	 if (($sn)==($line_num-$cmod))
	   echo "SELECTED";
       }
	
     }
     $tmp=chop(str_replace(".mp3","",basename($line[0])));
     if ($kb == 0) echo " STYLE=\"background-color:#fefece; color:gray; \" ";
     echo " onmouseover=\"songinfo('$songrate $songsize');\" ";
     echo ">"."$songtime | ".$ln.". ".$tmp;
}
?>
</SELECT>
</span>
</td>
<td valign=middle>

<a href="javascript:move_song(-5000)" ><img 
   border=0 src="/mp3script/images/top.gif" TITLE="Move song to top"></a><br>
<!--button onclick="javascript:move_song(-5000)"><img src="/images/top.gif"></button-->

<a href="javascript:move_song(-7)" ><img 
   border=0 src="/mp3script/images/upfast.gif" TITLE="Move song upwards 7 places"></a><br>

<a href="javascript:move_song(-1)" ><img 
   border=0 src="/mp3script/images/up.gif" TITLE="Move song upwards"></a><br>

<a href="javascript:move_song(1)" ><img 
   border=0 src="/mp3script/images/down.gif" TITLE="Move song downwards"></a><br>

<a href="javascript:move_song(7)" ><img 
  border=0 src="/mp3script/images/downfast.gif" TITLE="Move song downwards 7 places"></a><br>

<a href="javascript:move_song(5000)" ><img 
   border=0 src="/mp3script/images/bottom.gif" TITLE="Move song to bottom"></a><br>

<a href="javascript:refresh_list()" ><img 
   border=0 src="/images/reload.gif" TITLE="Refresh" ALT="Refresh"></a>
</td>
</tr>
</table>

<table width=400 name=table2 cellspacing=0 cellpadding=1 border=0>

<tr gcolor=gray>
<td colspan=2 class=txtsm >

<?
if ($calcsongstats) {
  $lines=0;
  $pfp=popen("$basepathbin/showplsecs $plfile","r");
  if (!$pfp) {
    echo "Error calling $basepathbin/showplsecs $plfile";
  }
  $totsecs=0;$totsize=0;

  while (!feof ($pfp)) {
    $buff = fgets($pfp, 256);
    $buff=explode(":",$buff);
    if (count($buff)!=3) {
      $songdata[$lines++]="000:000:000";
      continue;
    }
    $bitrate=$buff[0]; $secs=$buff[1]; $fsize=$buff[2];
    $totsize+=$fsize;$totsecs+=$secs;
    $songdata[$lines++]="$bitrate:$secs:$fsize";
  }//while
  pclose($pfp);


  $hours=floor($totsecs/3600);
  $mins=floor(($totsecs-$hours*3600)/60);
  $totmins=floor($totsecs/60);
  $secs=$totsecs%60;
  $kb=floor($totsize/1024);
  $gb=floor($totsize/(1024*1024));
  $stats="[".$hours."h ".$mins."m ".$secs."s] ($totmins mins)%%%".
	$gb."MB ($totsize Bytes)";

  $pldata=file($plfile);

  while (list ($line_num, $line) = each ($pldata)) {
    $line=explode("%%%",$line);
    $line[0]=$songpath=chop($line[0]);
    $pldata_new[$line_num]=$line[0]."%%%".$songdata[$line_num];
  }

  $lines=count($pldata_new);
  //save new playlist
  $fp = fopen ($plfile, "w");
  flock($fp,2);
  //put stats line
  fputs($fp,"#%%%$stats\n");
  for ($i=0 ; $i < $lines ; $i++) {
    if ($pldata_new[$i][0]=="#") continue; //prev stats
    $pldata_new[$i]=chop($pldata_new[$i]);
    fputs($fp,$pldata_new[$i]."\n");
  }
  flock($fp,3);
  fclose($fp);
  	
  echo "Done, press refresh";
  echo "<script>refresh_list();</script>";
}
else 
  printstats();

?>

<tr><td>
<!--form name=recfrm>
<INPUT class=recalcbtn width=12 TYPE="button" 
       VALUE="Recalculate" onClick='recalculate();'>
</form-->
<b><a 
   href="javascript:recalculate();" 
   title="song &amp; playlist durations">[Recalculate]</a></b>

</td>

<td align=right>
&nbsp;&nbsp;&nbsp;&nbsp;<span id=songinfo>&nbsp;</span>
</td>
</tr>


<tr bgcolor=<?=$table2_bgcolor?>>
<td>
<small>
<?php 
  echo "<SELECT NAME=\"listplaylist\" SIZE=3 onChange='plselect();'>\n";
  $pfp=popen("cd $pldir;/bin/ls $username-*","r");
  $c=0;
  while ($buffer = fgets($pfp, 4096)) {
    $buffer=chop($buffer);
    $c++;
    $plnam=str_replace("$username-","",$buffer);
    if ($plnam == "default") 
      $extra=" STYLE=\"background-color:#fefece; ".
             "color:black; \" ";
    else $extra="";
    echo "\n<OPTION $extra VALUE=\"".$plnam."\" ";
    if ("$username-$currpl"==$buffer)
      echo "SELECTED";
    echo ">".$c.". $plnam";
  }
?>
</SELECT>
</small>
</td>

<td align=left>
  <table border=0 cellspacing=0 cellpadding=3 width=99%>
  <tr>
  <td align=left class=txtsm>
    <SELECT name=listaction>
    <option value=1 selected>Create a new playlist
    <option value=2>Delete the playlist
    </select> <b>named:</b>
    <input style='background-color:#FFF7D5' size=9
         maxlength=12 type=text name=listname>
    <INPUT style='height:20px;width:30px' TYPE="button" VALUE="GO" onClick='plaction()'>
  </td>
  </tr>
  <tr>
  <td align=left class=txtsm>
    <a href="javascript:showplaylist();">[Display as Text]</a>,
    <a href="/mp3script/php/mp3_list.m3u?action=cdcover">[Generate CdCover]</a>,
    
  </td>
  </tr>
  </table>
</td>
</tr>

</FORM>
</table>

<table width=400 border=0 gcolor=gray>
<tr>
<td class=txtsm align=left>Last command:<b> <?php echo "$action"?></b></td>
<td class=txtsm align=right>
<a href="mailto:sivann@kill-9.gr?Subject=I love it!&Body=My credit card number is ">Send</a> your suggestions</td>
</tr>
<tr>
<td class=txtsm colspan=1>
<a href="<? echo "$SCRIPT_NAME?action=showplaylist"?>">Playlist</a> of user:&nbsp;<b><?=$username?></b></td>
<td class=txtsm align=right> 


<table><tr><td class=txtsm valign=middle>
download&nbsp;<img align=absmiddle 
   src="/mp3script/images/download_list.gif">
</td>
<td class=txtsm>
<a title="Containing all the songs in this playlist"  
href="<? echo "$SCRIPT_NAME?action=getbigtar&nn=0"?>"> <?=$currpl?>.tar</a>
<br>
<a title="Containing all the songs in this playlist without number prefix"  
href="<? echo "$SCRIPT_NAME?action=getbigtar&nn=1"?>"> <?=$currpl?>-nn.tar</a>
</td></tr></table>

</td>
  
</tr>
</table>

<br>

</body>
<html>

