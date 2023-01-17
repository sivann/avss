#!/usr/bin/perl
##sivann 2003
#todo: epilogh kai apo subdirs.

$maxsongs=50;
$artist=@ARGV[0];

@rel=`/var/www/mp3/findrel "$artist"`;
if ((scalar(@rel) <1) || (@rel[0] =~/^-/)) {exit;}; #no matches

$j=0;
foreach $art (@rel) {
  if (length($art)<6) { next;} #blank line

  $dir=$art; 
  $dir=~s/^.://;chomp($dir); 
  $artdir=$dir;
  $dirprefix="/kill-9/sorted/";
  @mp3=();

  @mp3=find_songs_inside("$dirprefix","$dir");

  $i=0;$songs=scalar(@mp3);
  if ($songs<3) {$songsperartist=$songs} else {$songsperartist=3;};
   
  #randomize artist selections
  @songnums=unique_rand ($songsperartist,$songs); #maximum
  #push @mp3s,"@mp3[@songnums]" ;

  for $value (@mp3[@songnums]) {
      push @mp3s,"$value";
  }

}

#foreach $f (@mp3s) { print $f,"\n";};print "\n\n#############\n\n";
#randomize final list
@mp3list=unique_rand (scalar(@mp3s),scalar(@mp3s)); #maximum
@mp3s2=@mp3s[@mp3list];

if (scalar(@mp3s2)<$maxsongs) {$maxsongs=(scalar(@mp3s2)-1);};
foreach $f (@mp3s2[0..$maxsongs]) { print $f,"\n";}

#-----------------------

sub unique_rand {
    my ($n, $lim) = @_;
    my (@res, %buf);
    while (@res < $n) {
	my $rn = int rand $lim--;
	push @res, $buf{$rn} || $rn;
	$buf{$rn} = $buf{$lim} || $lim;
    }
    return @res;
}

#------------------

sub find_songs_inside(){
  my $dirprefix=shift;
  my $dir=shift;
  my @flist,@songs;

  $level++;
  #print "level $level on dir ${dirprefix}/${dir}\n";

  if ($level == 1) {@songs=();} #clear static var @songs

  if (opendir(DIRH,"${dirprefix}/${dir}")){
    @flist=readdir(DIRH);
    closedir DIRH;
    foreach (@flist){
      # ignore . and .. :
      next if ($_ eq "." || $_ eq "..");
      if ($_ =~ /(\.mp3$|\.ogg$)/){
	  push @songs, "$dir/$_";
      }
      if (-d "$dirprefix/$dir/$_" && ! -l "$dirprefix/$dir/$_") {
	find_songs_inside("$dirprefix","$dir/$_") 
      }
    }
  }#opendir
  else{
    print "ERROR: can not read directory $dir : $!\n";
  }
  $level--;
  return @songs;
}
#-----------------------


