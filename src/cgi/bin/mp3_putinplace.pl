#!/usr/bin/perl
#akav
#sivann
use IO::Handle;

stdout->autoflush(1);
# ---- CONFIGURATION -----
$dest_dir="/kill-9/sorted/permanent";
$mp3base="/kill-9/sorted";
$mv_prg="/bin/mv";
$mp3info="/usr/local/bin/mp3info";
$kill9_gid=122;
$mp3db="/var/www/mp3/mp3.db";
# ---- END OF CONFIGURATION -----

foreach $fn (@ARGV) {
	$netfn=$fn;
	$netfn=~s:^.*/::;
	if (($artist,$name)= $netfn=~/^([A-Z\.a-z0-9!#&\' ]+) \- ([A-Z\.a-z0-9!#&,\-\' ]+)( \- [A-Za-z0-9!#&,\-\' ]+)*\.mp3$/) {
		($start_letter)=$artist=~/^ *(.)/;
		$start_letter=~y/a-z/A-Z/;
		$start_letter=~s/^[0-9]$/#/;
		$artist=~y/A-Z/a-z/;
		$directory="$dest_dir/$start_letter/$artist";
		$escdirectory="$dest_dir/$start_letter/[1m$artist[0m";
		#print STDERR "DEBUG ",__LINE__,": artist:$artist name:$name trackno:$trackno directory:$directory\n";
		unless (-d $directory) {
			print "\n[1;31mCreate[0m directory $escdirectory? (y/N)";
			chomp($ans=<STDIN>);
			if ($ans eq "y") {
				mkdir("$directory",02770);
				chown(0,$kill9_gid,"$directory");
				chmod 02770, "$directory"; 
			} else {
				next;
			}
		}

		#directory exists, file doesn't
		#first check for a match in db
     		if (! -e "$directory/$fn") {
		  chomp($fn);
		  print "\n";
		  $nsimilar=findsimilar($fn);

                  open( mpinfocmd, "/usr/local/bin/mp3info -o \"$fn\"|" ); 
		  $mpinfo = <mpinfocmd>; chomp($mpinfo); close  mpinfocmd ;

		  if ($nsimilar==0) {
		    print "\n[1;32mMove[0m $mpinfo \'$fn\'\n\t\t  to \'$escdirectory/\' ? (Y/n)";
		    $defans="y";
		  }
		  else {
		    print "\n[1;35mMove[0m $mpinfo \'$fn\'\n\t\t  to \'$escdirectory/\' ? (y/N)";
		    $defans="n";
		  }
		  $ans=$defans;
		  if ($defans ne "y") {
		    chomp($ans=<STDIN>);
		    if (length($ans)<1) {$ans=$defans;}
		  }
		  if ($ans eq "y") {
			  system($mv_prg,$fn,"$directory/");
			  chown(0,$kill9_gid,"$directory/$fn");
			  chmod 0640, "$directory/$fn"; 
		  }
		}
		else {
		  print "\n[1;35m $directory/$fn Already exists[0m\n";


                  open( mpinfocmd, "/usr/local/bin/mp3info -o \"$directory/$fn\"|" ); 
                  $mpinfo = <mpinfocmd>; chomp($mpinfo); close  mpinfocmd ;
                  ($oldbr, $tmp) = split(/ /, $mpinfo);
                  print "Old (bitrate:seconds):",$mpinfo;

                  open( mpinfocmd, "/usr/local/bin/mp3info -o \"$fn\"|" );  
                  $mpinfo = <mpinfocmd>; chomp($mpinfo); close  mpinfocmd ;
                  ($newbr, $tmp) = split(/ /, $mpinfo);
                  print "\nNew (bitrate:seconds):",$mpinfo;

		  print "\n[1;32mReplace[0m ? (y/N/d)";
		  chomp($ans=<STDIN>);
		  if ($ans eq "y") {
			  system($mv_prg,$fn,"$directory/");
			  chown(0,$kill9_gid,"$directory/$fn");
			  chmod 0640, "$directory/$fn"; 
		  }
		  elsif ($ans eq "d") {unlink $fn ;}
		}
	  }
	  else {
	    printf "\nFile: \`$fn\' is not in correct form\n";
	  }
  print "\n";
}




sub findsimilar {
  $searchsong=shift;
  #print "SS=",$searchsong,"\n";
  $s=0;

  ($s1, $s2) = split(/-/, $searchsong, 3);
  $s1=~s/^ //; $s1=~s/ $//; #get rid of leading space in songname
  $s2=~s/^ //; $s2=~s/ $//; #get rid of leading space in songname
  $s2=~s/.mp3//;

  #get only second word from artist if more than one
  if ($s1 =~ / /){ ($tmp, $arthis) = split(/ /, $s1, 2); }
  else{ $arthis=$s1; }
   
  #get second word from songnam if more than one
  if ($s2 =~ / /){ 
   ($tmp, $songthis) = split(/ /, $s2); 
   if (length($songthis)<=3){ $songthis=$s2; }
  }
  else{ $songthis=$s2; }

  #remove leading+trailing spaces
  $arthis=~s/^ //; $arthis=~s/ $//; 
  $songthis=~s/^ //; $songthis=~s/ $//; 

  #print "DBG: ", $arthis,"+", $songthis,"\n";
  #
  #search
  open (F, $mp3db) || die "Can't open - $!\n"; @db = <F>; close (F);

  #@same=grep {/${arthis}.*${songthis}/i}  @db;
  #@same=grep {/$arthis[^\/]*$songthis[^\/]*/i}  @db;
  @same=grep {/permanent\/.*$arthis[^\/]*$songthis[^\/]*/i}  @db;
  $s=scalar @same;

  if ($s>20) { 
    print "findsimilar: too many matches\n"; 
  } 
  else { 
    for $song (@same) {
	chomp($song);
	open( mpinfocmd, "/usr/local/bin/mp3info -o \"$mp3base/$song\"|" ); $mpinfo = <mpinfocmd>;
	chomp($mpinfo);
	print $mpinfo,' ',$song,",\n";
    }
    #print "\n"; 
  }

  return $s;
}#sub findsimilar



