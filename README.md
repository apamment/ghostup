## Building

compile with:

g++ main.cpp -o ghostup


## Commands

`./ghostup subping /opt/wwiv/ghostnet.dir/` - Send out a ping to all nodes to get a list of subs hosted by nodes. run `./network1` after to actually send out the message.

`./ghostup subcomp /opt/wwiv/ghostnet.dir/` - Compile a subs.lst.new from the subs.inf file (returned data from nodes) check over subs.lst.new to make sure it looks ok.

`./ghostup subsend /opt/wwiv/ghostnet.dir/` - Send out the newly compiled subs.lst to all nodes. this will delete subs.lst.new. run `./network1` after to actually send out the message.

The WWIV network coordinator system wont respond right away, just wait for it to process packets next.
