-- Customer ID=13943; Build=0x75f5e; Copyright (c) 2016-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
-- These coded instructions, statements, and computer programs are the
-- copyrighted works and confidential proprietary information of
-- Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
-- purchasers for internal use, but neither the original nor any adapted
-- or modified version may be disclosed or distributed to third parties
-- in any manner, medium, or form, in whole or in part, without the prior
-- written consent of Cadence Design Systems, Inc.

-- Operation:
-- This Lua script creates a watchfilter for writes (response size=0) to ADDR_UART_WR
-- and then waits on the associated uart_wr_event.  Each time a write occurs the script
-- does a peek to ADDR_UART_WR to get the value written (wr_byte) and then checks to see
-- if there is room for it in xtensa_wr_queue:
-- a) If there is no room and BLOCK was specified, the Lua script spins until there is
--    room using xtsc_host_sleep (which freezes the SystemC simulation).
-- b) If there is no room and BLOCK was not specified the script does a poke of the
--    failure indicator value 0xFFFFFFFF to ADDR_UART_WR.
-- c) If there is room in the queue, then the script adds wr_byte to it and does a poke
--    of the success indicator value (0x00000000) to ADDR_UART_WR.

BLOCK = (xtsc.cmd("xtsc xtsc_user_state_get BLOCK") == "1")
ADDR_UART_WR = xtsc.cmd("xtsc xtsc_user_state_get ADDR_UART_WR")
--xtsc.cmd("xtsc note uart_wr.lua: BLOCK=" .. (BLOCK and "1" or "0") .. " ADDR_UART_WR=" .. ADDR_UART_WR)
xtsc.cmd("xtsc xtsc_filter_create xtsc_response uart_wr_filter " ..  "address=" .. ADDR_UART_WR .. " status=RSP_OK size=0")
uart_wr_event = xtsc.cmd("xtsc xtsc_event_create uart_wr_event")
xtsc.cmd("uartregs watchfilter_add uart_wr_filter " .. uart_wr_event)
msg = ""

while true do
  xtsc.cmd("sc wait " .. uart_wr_event)
  wr_byte = xtsc.cmd("uartregs peek " .. ADDR_UART_WR .. " 1")
  if BLOCK then
    while xtsc.cmd("xtensa_wr_queue can_push") == "0" do
      xtsc.cmd("xtsc xtsc_host_sleep 10")
    end
  end
  if xtsc.cmd("xtensa_wr_queue can_push") == "0" then
    xtsc.cmd("uartregs poke " .. ADDR_UART_WR .. " 4 0xFF 0xFF 0xFF 0xFF")
  else
    xtsc.cmd("xtensa_wr_queue push " .. wr_byte)
    xtsc.cmd("uartregs poke " .. ADDR_UART_WR .. " 4 0 0 0 0")
  end
end


