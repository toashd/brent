Brent
--

     ____                 _
    |  _ \               | |
    | |_) |_ __ ___ _ __ | |_
    |  _ <| '__/ _ \ '_ \| __|
    | |_) | | |  __/ | | | |_
    |____/|_|  \___|_| |_|\__| NANO WEB SERVER


Brent is a small and simple web server written in C.

It consists of one file with 3 functions and only supports simple HTTP Get requests which makes it a safe and fast web server for static sites.


### Usage

To compile brent you need a basic C compiler, e.g. use:

    $ gcc brent.c -o brent

If you want it to run faster you could include optimization:

    $ gcc -O2 brent.c -o brent

Run brent, e.g. on port 8888 serving files from within the examples directory:

	  $ ./brent 8888 examples/web


Brent will behave like a daemon process, thus disconnects from the user or terminal session, goes into the background, closes input and output I/O & protects itself from you logging off. All errors or warnings go in the log file - see the logs with:

    $ tail -f brent.log


As it is not connected to a users or terminal session, logging out won't have any effects on the server. To stop brent, just find the process and send a kill signal.

Stop brent with

	  $ ps | grep brent | awk 'NR==1{print $1}' | xargs kill -9


### License
The MIT License (MIT). Please see [LICENSE](LICENSE) for more information.
