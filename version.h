#ifndef VERSION_H
#define VERSION_H

#define VERSION "0.1.0"

#define VERSION_STRING "brent nano web server - v %s\n" \

#define USAGE_STRING "usage:   brent [OPTIONS]\n"  \
            "\t  -b | --bind ADDR: address (default: 0.0.0.0)\n" \
            "\t  -p | --port PORT: port number (default: 8000)\n" \
            "\t  -d | --dir  DIR:  root directory (default: public)\n" \
            "\t  -h | --help \n" \
            "\t  -v | --version \n\n"

#define HELP_STRING " ____                 _\n"  \
             "|  _ \\               | |\n"  \
             "| |_) |_ __ ___ _ __ | |_\n"  \
             "|  _ <| '__/ _ \\ '_ \\| __|\n"  \
             "| |_) | | |  __/ | | | |_\n"  \
             "|____/|_|  \\___|_| |_|\\__| NANO WEB SERVER - v %s\n\n"  \
             USAGE_STRING  \
             "example: brent -p 8080 -d /var/www/app\n\n" \
             "\t brent is a small and safe nano web server.\n"  \
             "\t There are no fancy features = safe and secure.\n" \
             "\t brent only serves files/web pages with a limitd set of extensions and mime types\n"  \
             "\t  and only from the specified application and its sub-directories.\n\n"  \
             "\t Not supported: URLs including \"..\", java, php, cgi, etc.\n" \
             "\t Not supported: directories / /etc /bin /lib /tmp /usr /dev /sbin \n\n" \
             "\t No warranty given or implied.\n" \
             "\t Copyright © 2015 - Tobias Schmid <toashd@gmail.com>\n"
#endif
