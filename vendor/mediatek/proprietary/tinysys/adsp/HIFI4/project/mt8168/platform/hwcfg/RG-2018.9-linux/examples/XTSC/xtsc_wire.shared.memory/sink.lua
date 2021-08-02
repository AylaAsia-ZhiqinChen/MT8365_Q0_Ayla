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

LOG = tonumber(xtsc.cmd("xtsc xtsc_user_state_get LOG"))
if (not LOG) then
  xtsc.throw("Cannot convert LOG to a number")
end

TOTAL = tonumber(xtsc.cmd("xtsc xtsc_user_state_get TOTAL"))
if (not TOTAL) then
  xtsc.throw("Cannot convert TOTAL to a number")
end

WIRE = "core0_to_core1"


print("sink.lua starting to read:  LOG=" .. LOG .. " TOTAL=" .. TOTAL .. " YIELD=" .. (YIELD and YIELD or "nil"))
count = 0
unlimited = (TOTAL == 0)
prev = ""
while unlimited or (TOTAL > 0) do
  str = xtsc.cmd(WIRE .. " read")
  count = count + 1
  if (str ~= prev) then
    if (LOG > 0) then
      print("read:  " .. str .. " " .. count)
    end
    TOTAL = TOTAL - 1
    prev = str
  end
  if (YIELD) then
    xtsc.cmd("sc wait " .. YIELD)
  end
end


