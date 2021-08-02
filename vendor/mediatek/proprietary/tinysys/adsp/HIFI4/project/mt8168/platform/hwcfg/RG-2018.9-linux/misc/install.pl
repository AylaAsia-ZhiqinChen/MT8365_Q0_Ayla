# Install a Core Package to Xtensa Tools
# $Id: //depot/main/Xtensa/SWConfig/misc/install.pl

# Customer ID=13943; Build=0x75f5e; Copyright (c) 2004-2008 Tensilica Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of Tensilica Inc.
# They may not be modified, copied, reproduced, distributed, or disclosed to
# third parties in any manner, medium, or form, in whole or in part, without
# the prior written consent of Tensilica Inc.

# Workhorse perl script intended to be invoked by a wrapper that
# passes this script the following arguments:
#
#  1.  the full path of Xtensa Tools
#  2.  the full path of a Core Package
#  3.  interactive mode? "yes" or "no"
#  4.  Xtensa registry path (ignored in interactive mode)
#  5.  default config? "yes" or "no" (ignored in interactive mode)
#  6.  replace existing config? "yes" or "no" (ignored in interactive mode)
#  7.  "virtual" path of Xtensa Tools to be used as install-prefix
#  7.  "virtual" path of a Core Package to be used as config-prefix

use File::Basename;
use File::Copy;

sub hasWindowsLineEndings {
  my ($filename) = @_;
  open FL, $filename or die "cannot open $filename";
  binmode FL;
  my $hasCR = (<FL> =~ /\cM/); # only Windows files have carriage return (^M)
  close FL;
  return $hasCR;
}


if ( $^O =~ /MSWin32/i or $^O =~ /cygwin/i ) {
  $mswin32 = 1;
  $slash = "\\";  # for cosmetics
} else {
  $mswin32 = 0;
  $slash = "/";  # for cosmetics
}

# No HUP in Windows world.
if ($mswin32) {
  $SIG{INT} = $SIG{QUIT} = $SIG{TERM} = sub { exit 1 };
} else {
  $SIG{HUP} = $SIG{INT} = $SIG{QUIT} = $SIG{TERM} = sub { exit 1 };
}

if ( ! defined $ARGV[0] ) {
  die "INTERNAL ERROR: Argument missing; expecting path to Xtensa Tools."
}
if ( ! defined $ARGV[1] ) {
  die "INTERNAL ERROR: Argument missing; expecting path to config."
}
if ( ! defined $ARGV[2] ) {
  die "INTERNAL ERROR: Argument missing; expecting interactive-mode flag."
}
if ( ! defined $ARGV[3] ) {
  die "INTERNAL ERROR: Argument missing; expecting registry path."
}
if ( ! defined $ARGV[4] ) {
  die "INTERNAL ERROR: Argument missing; expecting no-default flag."
}
if ( ! defined $ARGV[5] ) {
  die "INTERNAL ERROR: Argument missing; expecting no-replace flag."
}
if ( ! defined $ARGV[6] ) {
  die "INTERNAL ERROR: Argument missing; expecting vpath to Xtensa Tools."
}
if ( ! defined $ARGV[7] ) {
  die "INTERNAL ERROR: Argument missing; expecting vpath to config."
}
$xttools = $ARGV[0];
$config = $ARGV[1];
$interactive = $ARGV[2];
$registry = $ARGV[3];
$default_config = $ARGV[4];
$replace_config = $ARGV[5];
$xttools_vpath = $ARGV[6];
$config_vpath = $ARGV[7];

# make sure this configuration has not already been installed
if ( -r "$config/misc/install-status" ) {
  print <<EOF;

Error: This configuration has already been installed.  If you want to
reinstall it in a different location, or if you want to relocate the
Xtensa Tools files, you will need to start over with the original files
that you downloaded from the Xtensa Processor Generator.

EOF
    exit 1;
}

# make sure this configuration was untarred properly
$longpath="long_name_used_to_test_for_problems_with_tar/long1/long2/long3";
$ignoredir="$config/misc/.ignore";
if ( ! -r "$ignoredir/$longpath/check_tar"
     || ($mswin32 == 0 && ! -l "$ignoredir/check_link")
     || ($mswin32 != hasWindowsLineEndings("$config/misc/OSversion"))) {
  print <<EOF;

Error: The files for this Xtensa configuration were extracted using
an incompatible version of tar.  Please re-extract the files using
Xtensa Xplorer or GNU tar.

EOF
    exit 1;
}

# grab the version string
open(VER, "$config/misc/version") or
  die "Error: Incomplete core package; $config/misc/version not found.\n\n";
$cfg_version = <VER>;
chomp $cfg_version;
close VER;

# make sure the Xtensa Tools were untarred properly
$ignoredir="$xttools/misc/.ignore";
if ( ! -r "$ignoredir/$longpath/check_tar"
     || ($mswin32 == 0 && ! -l "$ignoredir/check_link")
     || ($mswin32 != hasWindowsLineEndings("$xttools/misc/OSversion"))) {
  print <<EOF;

Error: The Xtensa Tools directory you have specified does not contain a
recognized version of the Xtensa Tools package.  Please check the directory
path and rerun this script.  If the directory is correct, the Xtensa Tools
files may have been corrupted by extracting them with an incompatible
version of tar.  In that case, re-extract the files using Xtensa Xplorer
or GNU tar.

EOF
  exit 1;
}

# check for the correct Xtensa Tools version
open(VER, "$xttools/misc/version") or
  die "Error: Incomplete Xtensa Tools; $xttools/misc/version not found.\n\n";
$xt_version = <VER>;
chomp $xt_version;
close VER;
if ( "$xt_version" ne "$cfg_version" ) {
  print <<EOF;

Error: The version of the Xtensa Tools package does not match the version
of this Xtensa processor configuration.  You need to get the $cfg_version
version of the Xtensa Tools package.

EOF
  exit 1;
}

$tools = "$xttools/Tools";
$tools_vpath = ("$xttools_vpath" eq "__default") ? "__default" : "$xttools_vpath/Tools";

# substitute the correct installation directory names
if ("$interactive" eq "yes") {
  print <<EOF;

The files for this Xtensa processor configuration will now be set up
to work with the installation directories that you have chosen.  This
process will take a few minutes, and once it has begun the
installation directories cannot be changed.  If you abort this script
after this point, or if you need to change the installation
directories for some reason, you will need to start over with the
original files that you downloaded for this configuration.  (The
Xtensa Tools files are not modified in this process so you do not need
to reinstall the Xtensa Tools package.)  The directories to be used are:

    Xtensa Tools:         $xttools
    Configured Processor: $config

EOF

  print "Do you want to continue? [y] ";
  $ans = <STDIN>;
  chomp $ans;
  if ( $ans =~ /n/i ) {
    print "Exiting without making any changes...\n";
    exit 0;
  }
}

open(STATUS, ">$config/misc/install-status") or
  die "Error: Cannot write $config/misc/install-status; verify file-system permissions.\n\n";
print STATUS "Installed in $config using Xtensa Tools from $xttools\n";
close STATUS;

# implement set_install_paths here
# "$config/misc/set_install_paths" "$tools" "$xttools" "$config"

# directory sanity checks, paths should be absolute by now
die "\nError: \"$tools\" does not exist or is not a directory\n\n"   if ( ! -d $tools );
die "\nError: \"$xttools\" does not exist or is not a directory\n\n" if ( ! -d $xttools );
die "\nError: \"$config\" does not exist or is not a directory\n\n"  if ( ! -d $config );

# run the script to set the paths for the software tools
$tools_perl = $mswin32 ? "$tools/perl/bin/MSWin32-x86/perl" :
			 "$tools/bin/perl";
system_or_die ("$tools_perl", "-w",
	       "$config/misc/fix_install_paths",
	       "$tools", "$xttools", "$config",
	       "$tools_vpath", "$xttools_vpath", "$config_vpath");

# run the script to set the paths for the hardware files
# (ONLY on non-windows systems)
if ($mswin32 == 0) {
  system_or_die ("$config/misc/fix_hw_install_paths",
		 "$tools", "$xttools", "$config",
		 "$tools_vpath", "$xttools_vpath", "$config_vpath");
}

if ("$interactive" eq "yes") {
  print <<EOF;

The files for this processor configuration have now been set to use
the directory names you have chosen.

The next installation step is to add this processor configuration to
the list of available configurations in a registry of Xtensa cores.
The configuration will be registered with the default name, which is
the Core ID from the Xtensa Processor Generator. You must ensure that
each core in the registry has a unique name. Please refer to the
"Xtensa Software Development Toolkit User's Guide" to learn how to
register this configuration with a different name.

This script will update only one registry of Xtensa cores, and in most
cases, you should use the default Xtensa registry.  If you are sharing
the Xtensa Tools installation with others, and you do not want this
processor configuration to be shared, you can specify an alternate
registry.  Please refer to the "Xtensa Software Development Toolkit
User's Guide" for instructions on adding this configuration to
additional Xtensa core registries.

EOF
}

# If interactive, prompt for the registry directory; else we already have it.
if ("$interactive" eq "yes") {
  $registry = "$xttools" . "$slash" . "config";
  print "The default registry is:\n";
  print "$registry\n\n";
  print "What registry would you like to use? [default] ";
  $new_registry = <STDIN>;
  chomp $new_registry;
  $registry = $new_registry if ( "$new_registry" ne "" );

  # check that the registry exists and is writable
  while ( ! -w $registry or ! -d $registry ) {
    if ( ! -d $registry ) {
      print "This Xtensa registry directory does not exist.\n";
      print "Do you want to create it? [y] ";
      $ans = <STDIN>;
      chomp $ans;
      if ( $ans eq "" or $ans =~ /y/i ) {
	if ( mkdir "$registry" ) {
	  next;
	}
	else {
	  print <<EOF;

Error: This Xtensa registry directory cannot be created.

You may try again, either retrying the same registry directory after
creating the directory or specifying a different Xtensa
registry, or you may exit this script and register this Xtensa
configuration manually as described in the "Xtensa Software
Development Toolkit User's Guide".

EOF
	}
      }
      else {
	print <<EOF;

You may try again with a different Xtensa registry directory,
or you may exit this script and register this Xtensa
configuration manually as described in the "Xtensa Software
Development Toolkit User's Guide".

EOF
      }
    }
    else {
      print <<EOF;

Error: This Xtensa registry directory is not writable.

You may try again, either retrying the same registry directory after
arranging to have write permission or specifying a different Xtensa
registry, or you may exit this script and register this Xtensa
configuration manually as described in the "Xtensa Software
Development Toolkit User's Guide".

EOF
    }

    print "Do you want to try again? [y] ";
    $ans = <STDIN>;
    chomp $ans;
    if ( $ans =~ /n/i ) {
      print "\nExiting without registering this configuration....\n\n";
      exit 1;
    }

    print "\n";
    $registry = "";
    while ( $registry eq "" ) {
      print "What registry would you like to use? ";
      $registry = <STDIN>;
      chomp $registry;
    }
  }
}

# find the core name that was used at build-time
$name = "";
while (glob "$config/config/*-params") {
  s|\\|/|g;	# use forward slashes
  s|.*/||;      # strip all but the filename
  if ( ! /^default-params/ ) {
    s/-params//;
    $name = $_;
  }
}

# check if <name>-params already exists
if ("$interactive" eq "yes") {
 while ( -f "$registry/$name-params" ) {
  print "\nThe Xtensa registry already contains a core named \"$name\"\n";
  print "Do you want to replace it? [n] ";
  $ans = <STDIN>;
  chomp $ans;
  if ( $ans =~ /y/i ) {
    print <<EOF;

Removing the old "$name" core from the Xtensa core registry...
(Note: The installed files for the old processor configuration have
not been removed.  You can manually re-register this core or delete
the files if the configuration is no longer needed.)

EOF

    @del = "$registry/$name-params";
    # Check if the old core was the default
    if ( $mswin32 ) {
      # no symlinks on windows
      if (filediff("$registry/$name-params", "$registry/default-params") == 0) {
	push @del, "$registry/default-params";
      }
    }
    elsif ( -l "$registry/default-params") {
      $old_default = readlink "$registry/default-params";
      $old_default =~ s|.*/||;
      $old_default =~ s|-params||;
      push @del, "$registry/default-params" if ("$old_default" eq "$name");
    }

    unless (unlink @del) {
      print <<EOF;

Error: Unable to remove the old "$name" core.  You will have to
manually add this processor configuration to the Xtensa core registry.
Please refer to the "Xtensa Software Development Toolkit User's Guide"
for instructions on managing Xtensa core registries.

EOF
      exit 1;
    }
  }

  else {
    print "\nDo you want to try a different name? [y] ";
    $ans = <STDIN>;
    chomp $ans;
    if ( $ans =~ /n/i ) {
      print "\n\nExiting without registering this configuration....\n\n";
      exit 1;
    }
    else {
      print "\n";
      $name = "";
      while ( $name eq "" ) {
	print "Enter another name for this Xtensa core? ";
	$name = <STDIN>;
	chomp $name;
      }
    }
  }
 }
}
else {
# non-interactive case: checking if <name>-params already exists
  if ( -f "$registry/$name-params" ) {
    if ("$replace_config" eq "yes") {
      print "\nReplacing existing config of the same name: $name\n";
      if ( ! unlink "$registry/$name-params" ) {
	print "\nWarning: Preserving original $name-params, ";
	print "cannot delete $registry/$name-params\n";
      }
      else {
	# Check if the old core was the default
	if ( $mswin32 ) {
	  if (filediff("$registry/$name-params", "$registry/default-params") == 0) {
	    unlink "$registry/default-params";
	  }
	}
	else {
	  if ( -l "$registry/default-params" ) {
	    $old_default = readlink "$registry/default-params";
	    $old_default =~ s|.*/||;
	    $old_default =~ s|-params||;
	    if ( "$old_default" eq "$name" ) {
	      unlink "$registry/default-params";
	    }
	  }
	}
      }
    }
    else {
      print "\nError:  A core named $name already exists in the registry.\n";
      print "Preserving the original core.  The rest of the install will\n";
      print "continue.\n";
    }
  }
}

# install the parameter file
copy("$config/config/default-params", "$registry/$name-params");

# optionally make this core the default
if ("$interactive" eq "no") {
  # if not interactive, just for it if called for
  if ("$default_config" eq "yes") {
    create_new_default($registry, $name);
  }
}
else {
  print "\nDo you want to make \"$name\" the default Xtensa core? [y] ";
  $ans = <STDIN>;
  chomp $ans;
  if ( $ans =~ /n/i ) {
    print "\nLeaving the previous default unchanged....\n";
  }
  else {
    if ( -f "$registry/default-params" ) {

      if ( $mswin32 ) {
	# There are no symlinks on windows.  Just replace the file.
	print "\nThere is already a default core.";
      }
      else {

	if ( -l "$registry/default-params" ) {
	  $old_default = readlink "$registry/default-params";
	  print "There is already a default core ($old_default).\n";
	}
	else {
	  print <<EOF;

Warning! There is already a default core but it is not installed as a
symbolic link to a named core.  Removing the old default will remove
that core from the Xtensa registry.

EOF
	}
      }
      print "\nDo you want to replace it? [y] ";
      $ans = <STDIN>;
      chomp $ans;
      if ( $ans =~ /n/i ) {
	print "\nLeaving the previous default unchanged....\n";
      }
      else {
	create_new_default($registry, $name);
      }
    }
    else {
      create_new_default($registry, $name);
    }
  }
}

print "\nThe installation process is now complete.\n";
exit 0;


######################################################################
#
# utilities
#


sub create_new_default {
  my ($dir, $config_name) = @_;
  unlink "$dir/default-params";
  if ( $mswin32 ) {
    copy ("$dir/$config_name-params", "$dir/default-params");
  } else {
    chdir "$dir";
    symlink "$config_name-params", "default-params";
  }
}

# Diff two files,
#   return 1 if different
#   return 0 if same

sub filediff {
  my ($a, $b) = @_;
  my $line;
  open(A,$a) or return 1;
  unless (open(B,$b)) { close A; return 1; }
  my $different = 0;
  while (<A>) {
    if ($line = <B>) {
      if ( $_ ne $line ) {
	$different = 1;
	last;
      }
    }
    else {
      $different = 1;
      last;
    }
  }
  $different = 1 if (<B>);
  close A;
  close B;
  return $different;
}


# Extract the string in single quotes.  Turn something like
#    factory_tools='c:\usr\xtensa\tools-N.0';
# into the real parameter
#    c:\usr\xtensa\tools-N.0

sub getparm {
  my ($str) = @_;
  chomp $str;
  $str =~ s/.*=\'//;
  $str =~ s/\';$//;
  return $str;
}

sub system_or_die {
    my $error = system (@_);
    if ( $error != 0 ) {
	my ($package,$filename,$line) = caller;
	die "\@\@ (command failed with exit code ".($error >> 8).", error $error at $filename line $line)\n"
	   ."*** Error executing: @_\n"
	   ."*** Stopped";
    }
}
