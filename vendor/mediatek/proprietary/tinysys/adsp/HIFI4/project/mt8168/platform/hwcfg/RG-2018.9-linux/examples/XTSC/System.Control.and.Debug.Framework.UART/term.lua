-- Customer ID=13943; Build=0x75f5e; Copyright (c) 2016-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
-- These coded instructions, statements, and computer programs are the
-- copyrighted works and confidential proprietary information of
-- Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
-- purchasers for internal use, but neither the original nor any adapted
-- or modified version may be disclosed or distributed to third parties
-- in any manner, medium, or form, in whole or in part, without the prior
-- written consent of Cadence Design Systems, Inc.

-- Operation:
-- This Lua script is used to drive the zombie system (term.inc) modelling the terminal
-- talking to the real system (system.inc).  Its basic operation is:
--   a) Print the "term> " prompt.
--   b) Get a line of input from the console which represents a command to be sent to
--      the real system.
--   c) Send the command plus NEW_LINE_CHAR to the real system by pushing a byte at a
--      time into xtensa_rd_queue.
--   d) If the "command" was "quit" then quit.
--   e) Get a one-line response from the real system by popping a byte at a time from
--      xtensa_wr_queue until NEW_LINE_CHAR is popped.
--   f) Print the response
--   g) goto (a)

-- Wait 1 delta cycle in case user has started with the cmd prompt (xtsc-run -include=term.inc -cmd)
xtsc.cmd("sc wait 0")

NEW_LINE_CHAR = xtsc.cmd("xtsc xtsc_user_state_get NEW_LINE_CHAR")
newline_val = tonumber(NEW_LINE_CHAR)
command  = ""

print("Use 'quit' to quit.")

while true do
  -- Get command from user
  io.write("term> ")
  command = io.read()
  -- Send command to core0 using xtensa_rd_queue
  for i = 1, string.len(command) do
    while xtsc.cmd("xtensa_rd_queue can_push") == "0" do
      xtsc.cmd("xtsc xtsc_host_sleep 20")
    end
    xtsc.cmd("xtensa_rd_queue push " .. string.byte(command, i))
  end
  xtsc.cmd("xtensa_rd_queue push " .. NEW_LINE_CHAR)
  if command == "quit" then break end
  -- Receive response back from core0 using xtensa_wr_queue
  repeat
    while xtsc.cmd("xtensa_wr_queue can_pop") == "0" do
      xtsc.cmd("xtsc xtsc_host_sleep 20")
    end
    byte = xtsc.cmd("xtensa_wr_queue pop")
    val  = tonumber(byte)
    curr = string.char(val)
    if val == newline_val then
      io.write("\n")
      --io.write("<GotOne!>")
    else
      io.write(curr)
      --io.write("<" .. val .. ">")
    end
    io.flush()
  until val == newline_val
end

