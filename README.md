## Ghostup

Ghostup is a utility for helping administer a wwivnet style messaging network. It was written for use with [Ghostnet](https://github.com/robbiew/ghostnet).

## Building

compile with:

g++ main.cpp -o ghostup -std=c++17


## Commands

`./ghostup subping /opt/wwiv/ghostnet.dir/` - Send out a ping to all nodes to get a list of subs hosted by nodes. run `./network1` after to actually send out the message.

`./ghostup subcomp /opt/wwiv/ghostnet.dir/` - Compile a subs.lst.new from the subs.inf file (returned data from nodes) check over subs.lst.new to make sure it looks ok.

`./ghostup subsend /opt/wwiv/ghostnet.dir/` - Send out the newly compiled subs.lst to all nodes. this will delete subs.lst.new. run `./network1` after to actually send out the message.

`./ghostup sendfile [file] /opt/wwiv/ghostnet.dir/` - Send a file from the network directory, where file is one of bbslist, connect, fbackhdr, wwivnews, categ, networks, binkp. Do not specify the extension on the command line.

`./ghostup sendother [/path/to/file.net] /opt/wwiv/ghostnet.dir/` - Send an arbitary .net or .zip file, must be full path and include extension. Files will arrive in systems network directory.

The WWIV network coordinator system wont respond right away, just wait for it to process packets next.
