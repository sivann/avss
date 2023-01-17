#!/usr/bin/perl

print "Password:";
$pass = <>;
chop $pass;
print "Encrypted password is: ", crypt($pass, "pw"), "\n";
