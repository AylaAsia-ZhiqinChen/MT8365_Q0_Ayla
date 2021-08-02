-- Customer ID=13943; Build=0x75f5e; Copyright (c) 2014 by Tensilica Inc.  ALL RIGHTS RESERVED.
-- These coded instructions, statements, and computer programs are the
-- copyrighted works and confidential proprietary information of
-- Tensilica Inc.  They may be adapted and modified by bona fide
-- purchasers for internal use, but neither the original nor any adapted
-- or modified version may be disclosed or distributed to third parties
-- in any manner, medium, or form, in whole or in part, without the prior
-- written consent of Tensilica Inc.


-- See file pc_changed.aliases.cmd for instructions

-- event_name is the full hierarchical name in SystemC 2.3
event_name = xtsc.cmd("xtsc xtsc_event_create pc_changed_event")
cnt = 0
while true do
  xtsc.cmd("xtsc xtsc_user_state_set core0.pc_changed_count " .. cnt)
  pc = xtsc.cmd("core0 get_pc")
  while pc == xtsc.cmd("core0 get_pc") do
    xtsc.cmd("sc wait 1")
  end
  cnt = cnt + 1
  xtsc.cmd("xtsc xtsc_event_notify " .. event_name)
end

