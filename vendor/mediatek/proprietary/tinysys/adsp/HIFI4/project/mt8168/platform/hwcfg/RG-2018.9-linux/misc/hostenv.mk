#  hostenv.mk  --  define host/environment specific variables
#
#  The Makefile that includes this file must set CONFIGDIR
#  to point to the top of the configuration-specific directory
#  (usually as a relative path, assuming the including
#   makefile is in the configuration-specific directory).
#
#  The Xtensa tools (eg. xt-xcc, xt-ld, etc.) are assumed to be
#  on the user's PATH.  Ie. the including makefile typically
#  doesn't add the Xtensa tools to the PATH explicitly, but
#  rather makes the same assumption.
#
#  Customer ID=13943; Build=0x75f5e; Copyright 2001-2005 Tensilica Inc.

include $(CONFIGDIR)/misc/hosttype.mk

ifeq ($(HOSTTYPE),unix)
RM	= rm -f
RM_R	= rm -rf
CP	= cp -fp
MKPATH	= mkdir -p
S       = /
endif

ifeq ($(HOSTTYPE),win)
#  Assume an NT command shell. !!
RM	= del /f/q
RM_R	= rmdir /s/q
CP	= copy
MKPATH	= mkdir
#  $E must be empty, so that $S is a backslash only (neither \ nor \\ by itself works):
E=
S       = \$E
endif

#  For now, assume use of xt-make, which automagically ensures
#  perl is on the PATH, and that PERL5LIB is set correctly for Unix.
#
PERL	= perl -w

