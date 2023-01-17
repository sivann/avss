<html>
<head>
<meta HTTP-EQUIV="Content-Type" CONTENT="text/html; charset=iso-8859-7">

<style type="text/css">
  .tb{font-family: verdana,arial,helvetica; 
   text-decoration:none; font-size: 12px}
  a:hover { color:#336699; text-decoration: none}
  td{font-family: verdana,arial,helvetica; font-size: 10px;	  
     color:#000000}
  .tbA{color:#00B47E;text-decoration:none}
  BODY{ margin: 4px }
  input:focus{background-color:#0FFFE7;color:#000000;} 

  #bandinfo {
    position: fixed;
    font-size:10px;
    width: 150px;
    height: auto;
    top:0;
    bottom: 0;
    right: 0;
    left: auto;
    background-color: #ffffe0;
    font-family: verdana,arial,helvetica; 
    overflow:auto;
  }

  #main {
    position: fixed;
    width: auto;
    height: auto;
    top: 0;
    bottom: 0;
    right: 150px;
    left: 0;
    overflow: auto;
    padding: 2px .5em;
    border: solid 1px black;
    background-color: #ffffff;
    font-size:12px;
  }
	 
</style>

<script>
function bandinfo(info)
{
  x = document.getElementById("bandinfo");
  if (x) x.innerHTML=info;
}

</script>

</head>


<body>

<div id="main">
<b>Click on the Styles to see band names</b>
<table width="100%">
<tr>
<td valign=top>

<?
// sivann 10/2004
include("avssconf.php");
//to work without register_globals
foreach($_REQUEST as $k => $v)
{
  ${$k} = $v;
}

$base="$avssdir/data/";

$bos=file($base."bandofstyle");
$cbos=count($bos);


$cols=3;

$lpc=$cbos/$cols+$cols;

for ($bcnt=0,$i=0;$i<$cbos;$i++) {
 $l=explode(":",$bos[$i]);
 $style=$l[0];
 $bandcnt=$l[1];
 $bcnt+=$bandcnt;
 $bands=explode("@",$l[2]);

 for ($bandlinks="",$j=0;$j<$bandcnt;$j++) {
   $bands[$j] = str_replace("\n", "", $bands[$j]);
   $bn=explode("/",$bands[$j]); 
   $bn=$bn[count($bn)-1];
   $bndlnk=rawurlencode($bands[$j]);
   $bn = str_replace("'", " ", $bn);
   $bn = str_replace("\"", "%22", $bn);
   $bndlnk = str_replace("%2F", "/", $bndlnk);
   $bandlinks.="<a href=\\\"$avsswwwdir"."cgi-bin/avssmain.m3u?path=$bndlnk\\\">$bn</a><br>";
 }


 if ($i&&!($i%$lpc) ) {
   echo "</td>\n<td valign=top>";
 }

 $style = str_replace(" ", "&nbsp;", $style);
 $sname1= "$style&nbsp;($bandcnt)</br>\n";
 $sname= "<div onclick='bandinfo(\"$bandlinks\");'>$sname1</div>";

 if ($bandcnt>=10)  echo "<b>$sname</b>";
 else echo "$sname";

}

?>

</td></tr>
</table>
</div>


<div id=bandinfo>Bands</div>



<?
echo "\n<br><b>$bcnt bands with known styles on server</b>";

?>

</body>
</html>
