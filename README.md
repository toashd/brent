# Brent

     ____                 _
    |  _ \               | |
    | |_) |_ __ ___ _ __ | |_
    |  _ <| '__/ _ \ '_ \| __|
    | |_) | | |  __/ | | | |_
    |____/|_|  \___|_| |_|\__| NANO WEB SERVER


Brent is a small and simple web server written in C.

It consists of just one file with 3 functions and only supports simple HTTP Get requests which makes it a safe and fast web server for static sites.


## Usage

Install brent with:

    $ make build

Serve brent example app

	$ make serve

Run brent, e.g. on port 8888 serving files from within the examples directory:

	$ brent -p 8888 -d examples/web

Print all options available with:

    $ brent help

Brent will behave like a daemon process, thus disconnects from the user or terminal session, goes into the background, closes input and output I/O & protects itself from you logging off. All errors or warnings go in the log file - see the logs with:

    $ tail -f brent.log

As it is not connected to a users or terminal session, logging out won't have any effects on the server. To stop brent, just find the process and send a kill signal.

Stop brent with

	$ ps | grep brent | awk 'NR==1{print $1}' | xargs kill -9


## Contribute

Brent's main intention was to provide a small and secure web server for static sites written in C without libraries and with a code base being as small as possible with just a few trade-offs for readability.

Please feel free to suggest any kind of improvements, refactorings, bug reports - just [file an issue](https://github.com/toashd/brent/issues) and I'll do my best to accommodate you. If you like the project giving it a star would show your support and would be very much appreciated!

Pull request are always welcome no matter how big or small.

### License
brent is available under the MIT license. See the [LICENSE](LICENSE) for more info.

Copyright (c) 2015 | Tobias Schmid | [toashd.com](http://toashd.com)
