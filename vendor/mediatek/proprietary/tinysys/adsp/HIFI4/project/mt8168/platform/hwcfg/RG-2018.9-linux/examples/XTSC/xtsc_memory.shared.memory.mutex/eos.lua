-- Customer ID=13943; Build=0x75f5e; Copyright (c) 2014-2017 Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
-- 
-- These coded instructions, statements, and computer programs are the
-- copyrighted works and confidential proprietary information of
-- Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
-- purchasers for internal use, but neither the original nor any adapted
-- or modified version may be disclosed or distributed to third parties
-- in any manner, medium, or form, in whole or in part, without the prior
-- written consent of Cadence Design Systems, Inc.



-- On MS Windows, wait before ending simulation so that shared memory can be inspected.
if (os.getenv("OS") == "Windows_NT") then
  print("If desired inspect shared memory then create file stop to cause the last XTSC simulation to end (for example: copy nul stop)")
  while (not  io.open("stop", "r")) do
    xtsc.cmd("xtsc xtsc_host_sleep 100")
  end
end

