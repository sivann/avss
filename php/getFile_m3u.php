<?php
require("avssinit.php");
if (!$authstatus) { echo "not logged in"; exit; }

getFile_m3u();
?>
