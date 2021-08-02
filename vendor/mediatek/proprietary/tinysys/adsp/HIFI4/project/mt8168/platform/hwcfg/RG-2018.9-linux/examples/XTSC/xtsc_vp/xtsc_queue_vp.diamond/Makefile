$(error Makefile disabled because config might not support this example)
# Customer ID=13943; Build=0x75f5e; Copyright (c) 2007-2011 by Tensilica Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Tensilica Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Tensilica Inc.

COMP_NAME = xtsc_queue_vp.diamond
TARGET_OBJECTS = target/consumer.out target/producer.out

EXAMPLE_NAME = $(subst _vp,,$(COMP_NAME))
TARGET_SOURCE_PATH = ../../$(EXAMPLE_NAME)

ADDITIONAL_CORE_INTERFACES = -add=tie=OPQ -add=tie=IPQ

include ../Makefile.include
include ../Makefile.common
