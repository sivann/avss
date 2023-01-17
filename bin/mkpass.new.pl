#!/usr/bin/perl

if (@ARGV > 0) {
	$password=shift(@ARGV);
}
if (@ARGV > 0) {
	$salt=shift(@ARGV);
}
if (! defined $password) {
	chomp($password=<STDIN>);
}
if (! defined $salt) {
	$salt="pw";
}

print "Encrypted password is: ", crypt($password, $salt), "\n";

