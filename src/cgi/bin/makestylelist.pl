#!/usr/bin/perl
# make the style list
# sivann 2003
use File::Basename;

$infoflist="/var/www/mp3/infofiles";
$out="/var/www/mp3/stylelist";

open stylelist, "<$infoflist" or die "cant open $infoflist";
open output, ">$out" or die "cant open $out";

while (<stylelist>) {
  $infofile="$_";
  open infoin , "<$infofile" or die "cant open $infofile";
  chop($infofile);
  $infofile=dirname($infofile);

  while (<infoin>) {
    if (/Style:(.*)/) {
      printf output ("%s:%s\n",$infofile, $1);
    }
  }
  close infoin;
}
