<?php
require("avssinit.php");
if (!$authstatus) { echo "not logged in"; exit; }


savefile($path,$file);

?>
