#!/usr/bin/env perl

use strict;
use warnings;
use utf8;

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
    $line =~ s/\r//;
    if($line =~ /^(.+)=(.*)$/)
    {
        $from = quotemeta($1);
        $to = quotemeta($2);
        $content =~ s{$from}{$to};
    }
}

print $content;
