#!/usr/bin/perl -w
use strict;

#
# Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2010 Tensilica Inc.  ALL RIGHTS RESERVED.
# 
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Tensilica Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Tensilica Inc.


sub usage {
  print "Parse the files whose names are provided on the command line (or STDIN) and " . "\n";
  print "check that they conform to their own self-description." . "\n";
  print "\n";
  print "If any violation of a file's self-description is encountered, processing" . "\n";
  print "of all files ceases and this script dies with an appropriate message." . "\n";
  print "\n";
  print "Files describe their own content using lines in the following format: " . "\n";
  print "\n";
  print "   [<AnyDesiredText> ]&grep[!] <PatternRegEx>[ << <HereRegEx>]" . "\n";
  print "\n";
  print "<PatternRegEx> and <HereRegEx> are any perl regular expressions." . "\n";
  print "\n";
  print "A line in the above format causes a search to start in the file for occurances" . "\n";
  print "of <PatternRegEx>.  The search starts on the line following the &grep line " . "\n";
  print "and ends on the line preceeding the first line containing a match with " . "\n";
  print "<HereRegEx> (or the end of file if <HereRegEx> was not specified)." . "\n";
  print "Multiple self-descriptions can be active at the same time." . "\n";
  print "\n";
  print "Self-description violations:" . "\n";
  print " - grep (without !) is specified and <PatternRegEx> is not found prior to <HereRegEx>" . "\n";
  print "   or end-of-file." . "\n";
  print " - grep! is specified and <PatternRegEx> is found prior to <HereRegEx> or end-of-file." . "\n";
  print " - file contains no self-description lines." . "\n";
  print "\n";
  print "Examples:" . "\n";
  print "- To ensure string \"01 02 03 04\" appears between this line and the next line contain the" . "\n";
  print "  string \"ENDSEARCH\"" . "\n";
  print "    yadda yadda yadda &grep 01 02 03 04 << ENDSEARCH" . "\n";
  print "     ..." . "\n";
  print "    yadda ENDSEARCH yadda" . "\n";
  print "- To ensure string \"RSP_NACC\" does not appear between this line and the end of file" . "\n";
  print "    yadda yadda yadda &grep! RSP_NACC" . "\n";
  print "- To ensure a line starting with \"foo\" and ending with \"bar\" appears between here and" . "\n";
  print "  the next grep description line:" . "\n";
  print "    yadda yadda yadda &grep ^foo.*bar\$ << &grep" . "\n";
  print "     ..." . "\n";
  print "    yadda yadda yadda &grep . . ." . "\n";
  print "Known Issues:" . "\n";
  print "  - Using \$ as a metacharacter to denote end-of-line may not work on DOS files.  A" . "\n";
  print "    workaround is to run dos2unix (from cygwin) on the file before using this script." . "\n";
}


if ((@ARGV == 1) && (($ARGV[0] eq "-help") || ($ARGV[0] eq "--help"))) {
  usage();
  exit(0);
}


my $HANDLE;

if (@ARGV == 0) {
  open $HANDLE, "<&STDIN";
  process($HANDLE, "<STDIN>");
}
else {
  foreach my $file (@ARGV) {
    open $HANDLE, $file or die "Cannot open file $file.";
    process($HANDLE, $file);
    close $HANDLE;
  }
}



sub process {

  my ($FH, $filename)   = @_;
  my @greptable;

  my $grepcount         = 0;    # Total greps performed
  my $linenum           = 0;    # Total lines of input file processed so far

  # The following define the index into the an @grep array
  my $greplinenum       = 0;    # Line number containing latest grep command
  my $grepline          = 1;    # The line containing the grep command
  my $grepcmd           = 2;    # The grep command from the input file
  my $regex             = 3;    # The regex portion of the grep command
  my $greptohere        = 4;    # 0=grep to end-of-file, 1=grep to $hereregex
  my $hereregex         = 5;    # String indicating where grepping should end.
  my $erroriffound      = 6;    # 1=its an error if pattern is found, 0=its an error if pattern is not found
  my $patternfound      = 7;    # 0=pattern has not been found, 1=pattern has been found


  while (<$FH>) {
    $linenum += 1;
    my $line = $_;
#   print "Line #$linenum: " . $line;
    chomp($line);
    foreach my $index (reverse 0 .. $#greptable) {
      my $grep = $greptable[$index];
#     print "Checking grepcmd: " . $grep->[$grepcmd] . " patternfound=" . $grep->[$patternfound] . "\n";
      if ($grep->[$greptohere] && ($line =~ m|$grep->[$hereregex]|)) {
        if (!$grep->[$patternfound] && !$grep->[$erroriffound]) {
          die "Expected regex was not found before line $linenum of file \"$filename\": $line" . "\n" .
              " Description line $grep->[$greplinenum]: \"$grep->[$grepline]\"" . "\n" .
              " ";
        }
        else {
          # We're done with this one, so delete it.
          splice(@greptable, $index, 1);
        }
      }
      elsif ($line =~ m|$grep->[$regex]|) {
        $grep->[$patternfound] = 1;
#       print "pattern found on line: $line \n";
        if ($grep->[$erroriffound]) {
          die "Forbidden regex found on line $linenum of file \"$filename\": $line" . "\n" .
              " Description line $grep->[$greplinenum]: \"$grep->[$grepline]\"" . "\n" .
              " ";
        }
      }
    }
    if ($line =~ /(^|\s)&grep(!?) +([^ ].*)$/) {
      my @grep;
      $grepcount               += 1;
      $grep[$greplinenum]       = $linenum;
      $grep[$grepline]          = $line;
      $grep[$grepcmd]           = $&;
      $grep[$erroriffound]      = (($2 eq "!") ? 1 : 0);
      $grep[$greptohere]        = 0;
      $grep[$hereregex]         = "";
      $grep[$patternfound]      = 0;
      my $rem                   = $3;
      $rem                      =~ s|\s+$||;  # Trim trailing spaces
      if ($rem =~ /(.*[^ ]) +<< *(.*)$/) {
        $rem                    = $1;
        $grep[$greptohere]      = 1;
        $grep[$hereregex]       = $2;
      }
      $grep[$regex]             = $rem;
      push(@greptable, [@grep]);
#     print "grepcmd: >$grep[$grepcmd]<"."\n". "erroriffound: $grep[$erroriffound]"."\n". "hereregex: >$grep[$hereregex]<"."\n". "regex: >$grep[$regex]<"."\n";
    }
  }

  foreach (@greptable) {
    my $grep = $_;
    if (!$grep->[$erroriffound] && !$grep->[$patternfound]) {
      die "At end of file \"$filename\" and expected regex has not been found." . "\n" .
          " Description line $grep->[$greplinenum]: \"$grep->[$grepline]\"" . "\n" .
          " ";
    }
  }

  if (!$grepcount) {
    die "No self-description entries found in file \"$filename\"." . "\n" .
        " ";
  }

}
