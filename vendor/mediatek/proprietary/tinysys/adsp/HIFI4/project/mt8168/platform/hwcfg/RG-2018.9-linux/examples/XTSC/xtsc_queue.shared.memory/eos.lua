-- Customer ID=13943; Build=0x75f5e; Copyright (c) 2014-2016 Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
-- 
-- These coded instructions, statements, and computer programs are the
-- copyrighted works and confidential proprietary information of
-- Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
-- purchasers for internal use, but neither the original nor any adapted
-- or modified version may be disclosed or distributed to third parties
-- in any manner, medium, or form, in whole or in part, without the prior
-- written consent of Cadence Design Systems, Inc.


-- Get statistics saved in user state counters and log them
-- See turbo.producer.lua and turbo.consumer.lua

function dump_stats(agent)
  stats = xtsc.cmd("xtsc xtsc_user_state_dump *." .. agent .. ".counter.*")
  for line in string.gmatch(stats .. "\n", "[^\n]*\n") do 
    line = string.gsub(line, "\n", "")
    if (line ~= "") then
      xtsc.cmd("xtsc note stats: " .. line)
    end
  end
end

xtsc.cmd("xtsc note End TIMESTAMP=" .. os.date())
dump_stats("consumer")
dump_stats("producer")

