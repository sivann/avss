#!/usr/bin/perl
# find the newest songs

$basedir="/home/www/htdocs/avss/";

open dirdb, "<".$basedir."data/dirs.db" or die "cant open ".$basedir."data/dirs.db";
open output, ">".$basedir."data/dirs.whatsnew" or die "cant open ".$basedir."dirs.whatsnew";
while (<dirdb>) {
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
