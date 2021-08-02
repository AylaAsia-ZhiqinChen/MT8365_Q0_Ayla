/*
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2003-2006 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * 
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of
 * Tensilica Inc.  They may be adapted and modified by bona fide
 * purchasers for internal use, but neither the original nor any adapted
 * or modified version may be disclosed or distributed to third parties
 * in any manner, medium, or form, in whole or in part, without the prior
 * written consent of Tensilica Inc.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "iss/mp.h"

static void
tie_check(XTMP_core c)
{
  unsigned i, j;
  unsigned num_groups = XTMP_getTieGroupCount(c);

  fprintf(stdout, "Core %s has %u TIE port groups\n",
          XTMP_getComponentName(c), num_groups);

  for (i = 0; i < num_groups; ++i) {

    XTMP_tieGroup g = XTMP_getTieGroupByIndex(c, i);
    const char *group_name = XTMP_getTieGroupName(g);
    unsigned num_ports = XTMP_getTieGroupPortCount(g);

    fprintf(stdout, "TIE port group: %s\n", group_name);

    for (j = 0; j < num_ports; ++j) {

      XTMP_tiePort p1;
      XTMP_tiePort p = XTMP_getTieGroupPortByIndex(g, j);
      const char *port_name = XTMP_getTiePortName(p);
      unsigned width = XTMP_getTiePortBitWidth(p);
      XTMP_tiePortDirection dir = XTMP_getTiePortDirection(p);

      fprintf(stdout, "  %-20s width: %-3u type: %s\n",
	      port_name, width, dir == XTMP_TIE_PORT_IN ? "In" : "Out");

      p1 = XTMP_getTiePortByName(c, port_name);
      if (!p1) {
	fprintf(stdout, "  Error: Could not look up TIE port %s\n", port_name);
      } 
      else  if (p1 != p) {
	const char *port_name1 = XTMP_getTiePortName(p1);
	fprintf(stdout, "  Error: port %s != port %s\n", port_name1, port_name);
      }

    }
  }
}

int 
XTMP_main(int argc, char **argv)
{
  char *xtensaSystemDirs[] = { XTENSA_SYSTEM, 0 };
  XTMP_params p;
  XTMP_core core;

  p = XTMP_paramsNewFromPath(xtensaSystemDirs, XTENSA_CORE, 0);
  if (!p) {
    fprintf(stderr, "Cannot create XTMP_params "XTENSA_CORE"\n");
    exit(1);
  }

  core = XTMP_coreNew("cpu", p, 0);
  if (!core) {
    fprintf(stderr, "Cannot create XTMP_core cpu\n");
    exit(1);
  }

  tie_check(core);

  return 0;
}
