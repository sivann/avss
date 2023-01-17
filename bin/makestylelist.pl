#!/usr/bin/perl
# make the style list
# sivann 2003
use File::Basename;

$basepath="/home/www/htdocs/avss/";

$infoflist=$basepath."data/infofiles";
$out=$basepath."data/stylelist";

open stylelist, "<$infoflist" or die "cant open $infoflist";
open output, ">$out" or die "cant open $out";

while (<stylelist>) {
  $infofile="$_";
  open infoin , "<$infofile" or die "cant open $infofile";
  chop($infofile);
  $infofile=dirname($infofile);

  while (<infoin>) {
    if (/Style:(.*)/) {
      $s=$1;
      $s =~ s/, /,/g;
      $s =~ s/ ,/,/g;
      #printf ("%s:%s\n",$infofile, $1);
      printf output ("%s:%s\n",$infofile, $s);
    }
  }
  close infoin;
}
