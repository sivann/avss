#!/usr/bin/perl
# find the newest songs

$basedir="/home/www/htdocs/avss/";

open mp3db, "<" . $basedir . "data/mp3.db" or die "cant open " . $basedir . "/data/mp3.db";
open output, ">".$basedir."data/mp3.whatsnew" or die "cant open ".$basedir."mp3.whatsnew";
while (<mp3db>) {
  chop;
  $line = $_;
  $line2 =$line;
  $line =~ s!^\.!/MMROOT/audio!;
  ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size, $atime,$mtime,$ctime,$blksize,$blocks) = stat($line);
  #$maxtime = $ctime>$rtime?$ctime:$rtime;
  push @files, ("$ctime $line2\n");
}
@sortedfiles = sort {$b <=> $a} @files;

for ($i=0; $i<9000; $i++) {
  $sortedfiles[$i] =~ /(\d+)\ (.*)/;
  ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = gmtime($1);

  printf output ("%02d/%02d %s\n",$mday,$mon+1,$2);
}
