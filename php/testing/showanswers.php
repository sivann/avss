<?
$answers=file("/home/dalk/develop/allmusic/stage1/answers");

$cnt=count($answers);
$fno=count(explode("@",$answers[0]));

echo "\n<table border=1>\n";
?>
<tr><th>group</th><th>realgroup</th><th>aka</th>
<th>style</th><th>formed</th><th>disbanded</th>
<th>born</th><th>died</th><th>members</th><th>decs</th>
<th>photolinks</th><th>followers</th><th>influencedby</th>
<th>similar</th><th>seealso</th><th>overviewurl</tr>

<?
for ($i=0;$i<$cnt;$i++) {
  $fields=explode("@",$answers[$i]);
  echo "<tr>";
  for ($f=0;$f<$fno;$f++)  {
    if ($f==10){
      $urls=explode(",",$fields[$f]);
      echo "<td>";
      for ($u=0;$u<count($urls);$u++) 
        echo "<a href=\"$urls[$u]\">$u</a> ";
      echo "</td>";
    }
    else
      echo "<td>$fields[$f]</td>";
  }

  echo "</tr>\n";
}
