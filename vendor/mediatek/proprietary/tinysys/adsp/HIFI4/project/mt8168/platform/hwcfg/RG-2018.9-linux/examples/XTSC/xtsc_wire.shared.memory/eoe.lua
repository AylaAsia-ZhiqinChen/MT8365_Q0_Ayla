-- Customer ID=13943; Build=0x75f5e; Copyright (c) 2014-2017 Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
-- 
-- These coded instructions, statements, and computer programs are the
-- copyrighted works and confidential proprietary information of
-- Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
-- purchasers for internal use, but neither the original nor any adapted
-- or modified version may be disclosed or distributed to third parties
-- in any manner, medium, or form, in whole or in part, without the prior
-- written consent of Cadence Design Systems, Inc.


-- If this is the sink then prompt user to create the go file
if (xtsc.user_state_get("last") == "yes") then
  print("Create file go to start (Linux: touch go   Windows: copy nul go)")
end

-- Wait for the go file to exist
repeat until io.open("go", "r")

print("Starting simulation on " ..  xtsc.user_state_get("entity"))
-- When this script exits the elaboration phase ends and the simulation phase starts
