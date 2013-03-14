#!/usr/bin/env perl
# $Id: TextReplacer.pl 42 2012-08-04 18:23:44Z wdlee91 $

use strict;
use warnings;

if($#ARGV < 1)
{
	print 'Usage: TextReplacer.pl infile mapfile' . "\n";
	exit;
}

my $infile = $ARGV[0];
my $mapfile = $ARGV[1];

my $content = '';

if(-e $infile)
{
	open(INFILE, $infile);

	my ($read, $data);
	while(($read = read(INFILE, $data, 1024)) > 0)
	{
		$content .= $data;
	}

	close(INFILE);
}
else
{
	print 'File ' . $infile . ' does not exist.' . "\n";
	exit;
}

my @map;

if(-e $mapfile)
{
	open(MAPFILE, $mapfile);
	@map = <MAPFILE>;
	close(MAPFILE);
}
else
{
	print 'File ' . $mapfile . ' does not exist.' . "\n";
	exit;
}

my ($from, $to);
foreach my $line (@map)
{
	chomp($line);
	if($line =~ /^(.+)=(.*)$/)
	{
		$from = quotemeta($1);
		$to = quotemeta($2);
		$content =~ s{$from}{$to};
	}
}

print $content;
