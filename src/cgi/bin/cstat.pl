#!/usr/bin/perl
# find the newest songs

open mp3db, "</var/www/mp3/mp3.db" or die "cant open /var/www/mp3/mp3.db";
open output, ">/var/www/mp3/mp3.whatsnew" or die "cant open mp3.whatsnew";
while (<mp3db>) {
  chop;
  $line = $_;
  $line2 =$line;
  $line =~ s!^\.!/kill-9/sorted!;
  ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size, $atime,$mtime,$ctime,$blksize,$blocks) = stat($line);
  #$maxtime = $ctime>$rtime?$ctime:$rtime;
  push @files, ("$ctime $line2\n");
}
@sortedfiles = sort {$b <=> $a} @files;

for ($i=0; $i<5000; $i++) {
  $sortedfiles[$i] =~ /(\d+)\ (.*)/;
  ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = gmtime($1);

  printf output ("%02d/%02d %s\n",$mday,$mon+1,$2);
}
