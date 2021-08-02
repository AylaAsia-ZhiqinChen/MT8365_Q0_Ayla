-- Customer ID=13943; Build=0x75f5e; Copyright (c) 2014-2017 Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
-- 
-- These coded instructions, statements, and computer programs are the
-- copyrighted works and confidential proprietary information of
-- Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
-- purchasers for internal use, but neither the original nor any adapted
-- or modified version may be disclosed or distributed to third parties
-- in any manner, medium, or form, in whole or in part, without the prior
-- written consent of Cadence Design Systems, Inc.



-- Script to wait for TurboXim to wait on an event and then to check once per clock
-- period for the queue to be become non-empty and then notify m_nonempty_event.
-- This is only needed if the queue producer is in another host OS process.
-- We also keep track of each thing we do and save the counters as user state so
-- they can be printed out once at the end of simulation (see eos.lua).

core            = xtsc.user_state_get("core")
queue           = "Q" .. string.format("%X", core-1) .. string.format("%X", core)
driver_wait     = 0
driver_wait_set = "xtsc xtsc_user_state_set " .. queue .. ".consumer.counter.driver_wait "
cycle_wait      = 0
cycle_wait_set  = "xtsc xtsc_user_state_set " .. queue .. ".consumer.counter.cycle_wait "
notify          = 0
notify_set      = "xtsc xtsc_user_state_set " .. queue .. ".consumer.counter.notify "

xtsc.cmd("xtsc note Beg TIMESTAMP=" .. os.date())

while true do
  xtsc.cmd("sc wait xtsc_driver_wait_on_event_event")
  driver_wait = driver_wait + 1; xtsc.cmd(driver_wait_set .. driver_wait)
  while true do
    if xtsc.cmd(queue .. " can_pop") == "1" then
      xtsc.cmd("xtsc xtsc_event_notify " .. queue .. ".m_nonempty_event")
      notify = notify + 1; xtsc.cmd(notify_set .. notify)
      break
    else
      xtsc.cmd("sc wait 1")
      cycle_wait = cycle_wait + 1; xtsc.cmd(cycle_wait_set .. cycle_wait)
    end
  end
end


