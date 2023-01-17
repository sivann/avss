#!/usr/bin/perl
#akav
#sivann
use IO::Handle;

stdout->autoflush(1);
# ---- CONFIGURATION -----
#$dest_dir="/n/theseas/usr2/kill-9/sorted";
$dest_dir="/kill-9/sorted/greek";
#$mv_prg="/usr/gnu/bin/mv";
$mv_prg="/bin/mv";
$mp3info="/usr/local/bin/mp3info";
$kill9_gid=121;
# ---- END OF CONFIGURATION -----

foreach $fn (@ARGV) {
	$netfn=$fn;
	$netfn=~s:^.*/::;
	if (($artist,$name)= $netfn=~/^([A-Za-z0-9!#&\' ]+) \- ([A-Za-z0-9!#&,\-\' ]+)( \- [A-Za-z0-9!#&,\-\' ]+)*\.mp3$/) {
		($start_letter)=$artist=~/^ *(.)/;
		$start_letter=~y/a-z/A-Z/;
		$start_letter=~s/^[0-9]$/#/;
		$artist=~y/A-Z/a-z/;
		$directory="$dest_dir/$start_letter/$artist";
		#print STDERR "DEBUG ",__LINE__,": artist:$artist name:$name trackno:$trackno directory:$directory\n";
		unless (-d $directory) {
			print "[1;31mCreate[0m directory $directory? (Y/n)";
			chomp($ans=<STDIN>);
			if ($ans ne "n") {
				mkdir($directory,02770);
				chown(-1,kill9_gid,$directory);
			} else {
				next;
			}
		}

     		if (! -e "$directory/$fn") {
		  print "[1;32mMove[0m \'$fn\' to \'$directory/$netfn\' ? (Y/n)";
		  chomp($ans=<STDIN>);
		  if ($ans ne "n") {
			  system($mv_prg,$fn,"$directory/");
		  }
		}
		else {
		  print "[1;35m $directory/$fn Already exists[0m\n";
		  print "Old (bitrate:seconds):";
		  system("$mp3info -s \"$directory/$fn\" | cut -d: -f2,8");
		  print "New (bitrate:seconds):";
		  system("$mp3info -s \"$fn\" | cut -d: -f2,8");
		  print "[1;32mReplace[0m ? (y/N)";
		  chomp($ans=<STDIN>);
		  if ($ans eq "y") {
			  system($mv_prg,$fn,"$directory/");
		  }
		}
	  } 
	  else {
	    printf "File: \`$fn\' is not in correct form\n";
	  }
}
