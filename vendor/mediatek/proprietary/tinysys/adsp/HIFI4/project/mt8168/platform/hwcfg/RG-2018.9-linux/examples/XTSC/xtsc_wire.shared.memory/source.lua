-- Customer ID=13943; Build=0x75f5e; Copyright (c) 2014-2017 Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
-- 
-- These coded instructions, statements, and computer programs are the
-- copyrighted works and confidential proprietary information of
-- Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
-- purchasers for internal use, but neither the original nor any adapted
-- or modified version may be disclosed or distributed to third parties
-- in any manner, medium, or form, in whole or in part, without the prior
-- written consent of Cadence Design Systems, Inc.

YIELD = tonumber(xtsc.cmd("xtsc xtsc_user_state_get YIELD"))

SLEEP = tonumber(xtsc.cmd("xtsc xtsc_user_state_get SLEEP"))

LOG = tonumber(xtsc.cmd("xtsc xtsc_user_state_get LOG"))
if (not LOG) then
  xtsc.throw("Cannot convert LOG to a number")
end

TOTAL = tonumber(xtsc.cmd("xtsc xtsc_user_state_get TOTAL"))
if (not TOTAL) then
  xtsc.throw("Cannot convert TOTAL to a number")
end

WIRE = "core0_to_core1"

print("source.lua starting to write:  LOG=" .. LOG .. " TOTAL=" .. TOTAL .. " YIELD=" .. (YIELD and YIELD or "nil")
                                                                         .. " SLEEP=" .. (SLEEP and SLEEP or "nil"))
count = 0
unlimited = (TOTAL == 0)
while unlimited or (TOTAL > 0) do
  str = string.format("0x3babe%08x", count)
  xtsc.cmd(WIRE .. " write " .. str)
  if ((LOG > 0) and ((count % LOG) == 0)) then
    print("wrote: " .. str)
  end
  count = count + 1
  TOTAL = TOTAL - 1
  if (YIELD) then
    xtsc.cmd("sc wait " .. YIELD)
  end
  if (SLEEP) then
    os.execute("sleep " .. SLEEP)
  end
end

-- Stay alive just a bit longer to help ensure we don't exit and delete shared memory before sink even starts
os.execute("sleep 1")

