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
  $dir="/kill-9/sorted/$dir";

   opendir(DIR, $dir) || die "can't opendir $some_dir: $dir";
   @mp3 = grep {/\.mp3$|\.ogg$/} readdir(DIR); closedir DIR; 
   $i=0;$songs=scalar(@mp3);
   if ($songs<3) {$songsperartist=$songs} else {$songsperartist=3;};
   
  #randomize directory
   @songnums=unique_rand ($songsperartist,$songs); #maximum
   #push @mp3s,"@mp3[@songnums]" ;

   for $value (@mp3[@songnums]) {
       push @mp3s,"$artdir/$value";
   }

}

#foreach $f (@mp3s) { print $f,"\n";}
#randomize final list
@mp3list=unique_rand (scalar(@mp3s),scalar(@mp3s)); #maximum
@mp3s2=@mp3s[@mp3list];

if (scalar(@mp3s2)<$maxsongs) {$maxsongs=(scalar(@mp3s2)-1);};
foreach $f (@mp3s2[0..$maxsongs]) { print $f,"\n";}

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

