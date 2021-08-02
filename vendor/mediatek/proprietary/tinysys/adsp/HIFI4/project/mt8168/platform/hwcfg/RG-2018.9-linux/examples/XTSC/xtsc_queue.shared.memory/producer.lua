-- Customer ID=13943; Build=0x75f5e; Copyright (c) 2014-2016 Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
-- 
-- These coded instructions, statements, and computer programs are the
-- copyrighted works and confidential proprietary information of
-- Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
-- purchasers for internal use, but neither the original nor any adapted
-- or modified version may be disclosed or distributed to third parties
-- in any manner, medium, or form, in whole or in part, without the prior
-- written consent of Cadence Design Systems, Inc.


YIELD = tonumber(xtsc.cmd("xtsc xtsc_user_state_get YIELD"))
if (not YIELD or (YIELD > 1)) then
  xtsc.throw("YIELD must be 0 or 1")
end

LOG = tonumber(xtsc.cmd("xtsc xtsc_user_state_get LOG"))
if (not LOG or (LOG > 1)) then
  xtsc.throw("LOG must be 0 or 1")
end

TOTAL = tonumber(xtsc.cmd("xtsc xtsc_user_state_get TOTAL"))
if (not TOTAL) then
  xtsc.throw("Cannot convert TOTAL to a number")
end

QUEUE = xtsc.cmd("xtsc xtsc_user_state_get QUEUE")
if (QUEUE == "") then
  xtsc.throw("You must define user state QUEUE")
end


print("Producer starting to produce!")
babe = 0xbabe0000
str  = "000000000000000000000000"
unlimited = (TOTAL == 0)
while unlimited or (TOTAL > 0) do
  if xtsc.cmd(QUEUE .. " can_push") == "1" then
    str = string.sub(str, 9, 24) .. string.format("%08x", babe)
    xtsc.cmd(QUEUE .. " push 0x" .. str)
    if (LOG == 1) then
      print("produced: 0x" .. str)
    end
    babe = babe + 1
    TOTAL = TOTAL - 1
  elseif (YIELD == 1) then
    xtsc.cmd("sc wait 0")
  end
end

