#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef SIGCLD
#   define SIGCLD SIGCHLD
#endif

#define VERSION "0.1.0"

#define LOGFILE "brent.log"

#define BUFSIZE 8096

#define ERROR      42
#define LOG        44
#define FORBIDDEN 403
#define NOTFOUND  404

/* allowed file extensions */
struct {
    char *ext;
    char *filetype;
} extensions [] = {
    {"gif",   "image/gif" },
    {"jpg",   "image/jpg" },
    {"jpeg",  "image/jpeg"},
    {"png",   "image/png" },
    {"ico",   "image/ico" },
    {"zip",   "image/zip" },
    {"gz",    "image/gz"  },
    {"tar",   "image/tar" },
    {"htm",   "text/html" },
    {"html",  "text/html" },
    {"css",   "text/css" },
    {"scss",  "text/css" },
    {"less",  "text/css" },
    {"js",    "text/javascript" },
    {"svg",   "image/svg+xml" },
    {"ttf",   "application/x-font-ttf" },
    {"otf",   "application/x-font-opentype" },
    {"woff",  "application/font-woff" },
    {"woff2", "application/font-woff2" },
    {"eot",   "application/vnd.ms-fontobject" },
    {"sfnt",  "application/font-sfnt" },
    {"tsv",   "text/html" },
    {0,0}
};

/*
    Writes logs to the log file and in case of forbidden or
    not found also writes a corresponding http response
    message to the socket.

    @param type the type of the log messages (LOG, ERROR, FORBIDDEN, NOTFOUND).
    @param s1 the first part of the log message.
    @param s2 the second part of the log message.
    @param socket_fd the socket's file descriptor.
*/
void
logger(int type, char *s1, char *s2, int socket_fd)
{
    int fd;
    char logbuffer[BUFSIZE*2];

    char timestamp[26];
    time_t now = time(0);
    strftime(timestamp, 26, "%Y-%m-%d %H:%M:%S", localtime(&now));

    switch (type) {
    case LOG:
        (void)sprintf(logbuffer, "[%s] - INFO: (%d) %s: %s", timestamp, socket_fd, s1, s2);
        break;

    case ERROR:
        (void)sprintf(logbuffer, "[%s] - ERROR: %s: %s Errno=%d exiting pid=%d", timestamp, s1, s2, errno, getpid());
        break;

    case FORBIDDEN:
        (void)write(socket_fd,
                    "HTTP/1.1 403 Forbidden\n"
                    "Content-Length: 185\n"
                    "Connection: close\n"
                    "Content-Type: text/html\n\n"
                    "<html><head>\n"
                    "<title>403 Forbidden</title>\n"
                    "</head><body>\n"
                    "<h1>Forbidden</h1>\n"
                    "The requested URL, file type or operation is not allowed on this simple static file webserver.\n"
                    "</body></html>\n", 271);
        (void)sprintf(logbuffer, "[%s] - FORBIDDEN: %s: %s", timestamp, s1, s2);
        break;

    case NOTFOUND:
        (void)write(socket_fd,
                    "HTTP/1.1 404 Not Found\n"
                    "Content-Length: 136\n"
                    "Connection: close\n"
                    "Content-Type: text/html\n\n"
                    "<html><head>\n"
                    "<title>404 Not Found</title>\n"
                    "</head><body>\n"
                    "<h1>Not Found</h1>\n"
                    "The requested URL was not found on this server.\n"
                    "</body></html>\n", 224);
        (void)sprintf(logbuffer, "[%s] - NOT FOUND: %s: %s", timestamp, s1, s2);
        break;
    }

    /* No checks here, nothing can be done with a failure anyway */
    if ((fd = open(LOGFILE, O_CREAT | O_WRONLY | O_APPEND, 0644)) >= 0) {
        (void)write(fd, logbuffer, strlen(logbuffer));
        (void)write(fd, "\n", 1);
        (void)close(fd);
    }

    if (type == ERROR || type == NOTFOUND || type == FORBIDDEN) exit(3);
}

/**
    Child process handling the request

    @param fd the requests file descriptor.
    @param req the number of the request.
*/
void
handle_request(int fd, int req)
{
    int j, file_fd, buflen;
    long i, ret, len;
    char *fstr;
    static char buffer[BUFSIZE+1];  /* static so zero filled */

    ret = read(fd, buffer, BUFSIZE);  /* read request in one go */
    if (ret == 0 || ret == -1) {  /* read failure stop now */
        logger(FORBIDDEN, "failed to read request", "", fd);
    }

    if (ret > 0 && ret < BUFSIZE)  /* return code is valid chars */
        buffer[ret] = 0;  /* terminate the buffer */
    else
        buffer[0] = 0;

    if (strncmp(buffer,"GET ", 4) && strncmp(buffer, "get ", 4)) {
        logger(FORBIDDEN, "Only simple GET requests supported", buffer, fd);
    }

    /* strip everything after the uri */
    for (i = 4; i < BUFSIZE; i++) { /* null terminate after the second space to ignore extra stuff */
        if (buffer[i] == ' ') { /* string is "GET URL " + lots of other stuff */
            buffer[i] = 0;
            break;
        }
    }

    logger(LOG, "Request", buffer, req);

    for (j = 0; j < i-1; j++)  /* check for illegal parent directory use .. */
        if (buffer[j] == '.' && buffer[j+1] == '.') {
            logger(FORBIDDEN, "Parent directory (..) path not supported", buffer, fd);
        }

    if (!strncmp(&buffer[0], "GET /\0", 6) || !strncmp(&buffer[0], "get /\0", 6))  /* convert no filename to index file */
        (void)strcpy(buffer, "GET /index.html");

    /* check if file type is supported */
    buflen = strlen(buffer);
    fstr = (char *)0;
    for (i = 0; extensions[i].ext != 0; i++) {
        len = strlen(extensions[i].ext);
        if (!strncmp(&buffer[buflen-len], extensions[i].ext, len)) {
            fstr = extensions[i].filetype;
            break;
        }
    }

    if (fstr == 0) logger(FORBIDDEN, "file extension type not supported", buffer, fd);

    if ((file_fd = open(&buffer[5], O_RDONLY)) == -1) {  /* open the file for reading */
        logger(NOTFOUND, "failed to open file", &buffer[5], fd);
    }

    logger(LOG, "Response (200 OK)", &buffer[5], req);

    len = (long)lseek(file_fd, (off_t)0, SEEK_END);  /* lseek to the file end to find the length */
          (void)lseek(file_fd, (off_t)0, SEEK_SET);  /* lseek back to the file start ready for reading */

    (void)sprintf(buffer,
                  "HTTP/1.1 200 OK\n"
                  "Server: brent/%s\n"
                  "Content-Length: %ld\n"
                  "Connection: close\n"
                  "Content-Type: %s\n\n", VERSION, len, fstr);  /* Header + a blank line */

    (void)write(fd, buffer, strlen(buffer));

    /* send file in 8KB blocks - last block may be smaller */
    while ((ret = read(file_fd, buffer, BUFSIZE)) > 0 )
        (void)write(fd, buffer, ret);

    sleep(1);  /* allow socket to drain before signalling the socket is closed */

    close(fd);

    exit(1);
}

/**
    Main - brent server

    Initializes and sets up the network socket and starts the
    server in a deamon mode. For every request the server forks
    a new child process, which handles the request and responds
    accordingly.
*/
int
main(int argc, char **argv)
{
    int i, port, pid, listen_fd, socket_fd, req;
    socklen_t length;

    static struct sockaddr_in client_addr;  /* static = initialized to zeros */
    static struct sockaddr_in server_addr;  /* static = initialized to zeros */

    if (argc < 3  || argc > 3 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
        (void)printf(" ____                 _\n"
                     "|  _ \\               | |\n"
                     "| |_) |_ __ ___ _ __ | |_\n"
                     "|  _ <| '__/ _ \\ '_ \\| __|\n"
                     "| |_) | | |  __/ | | | |_\n"
                     "|____/|_|  \\___|_| |_|\\__| NANO WEB SERVER - v %s\n\n"
                     "usage:  brent <port> <appdir>\n"
                     "\tbrent is a small and safe nano web server\n"
                     "\tbrent only serves files/web pages with extensions named below\n"
                     "\t and only from the specified application and its sub-directories.\n"
                     "\tThere are no fancy features = safe and secure.\n\n"
                     "\tExample: brent 8383 /var/www/app\n\n"
                     "\tOnly supports:", VERSION);

        for (i = 0; extensions[i].ext != 0; i++)
            (void)printf(" %s", extensions[i].ext);

        (void)printf("\n\tNot supported: URLs including \"..\", java, php, cgi, etc.\n"
                     "\tNot supported: directories / /etc /bin /lib /tmp /usr /dev /sbin \n"
                     "\tNo warranty given or implied\n\tTobias Schmid <toashd@gmail.com>\n");

        exit(0);
    }

    if (!strncmp(argv[2], "/"   , 2) || !strncmp(argv[2], "/etc", 5) ||
        !strncmp(argv[2], "/bin", 5) || !strncmp(argv[2], "/lib", 5) ||
        !strncmp(argv[2], "/tmp", 5) || !strncmp(argv[2], "/usr", 5) ||
        !strncmp(argv[2], "/dev", 5) || !strncmp(argv[2], "/sbin",6)) {

        (void)printf("ERROR: Bad top directory %s, see brent -h\n", argv[2]);

        exit(3);
    }

    if (chdir(argv[2]) == -1) {
        (void)printf("ERROR: Can't change to directory %s\n", argv[2]);

        exit(4);
    }

    /* become deamon and unstopable and no zombies children (= no wait()) */
    if (fork() != 0)
        return 0;  /* parent returns OK to shell */

    (void)signal(SIGCLD, SIG_IGN);  /* ignore child death */
    (void)signal(SIGHUP, SIG_IGN);  /* ignore terminal hangups */

    for (i = 0; i < 32; i++)
        (void)close(i);  /* close open files */

    (void)setpgrp();  /* break away from process group */

    logger(LOG, "brent starting on port", argv[1], getpid());

    /* setup the network socket */

    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        logger(ERROR, "system call", "socket", 0);

    port = atoi(argv[1]);

    if (port < 0 || port > 60000)
        logger(ERROR, "Invalid port number (use 1 up to 60000)", argv[1], 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        logger(ERROR, "system call", "bind", 0);

    if (listen(listen_fd, 64) < 0)
        logger(ERROR, "system call", "listen", 0);

    for (req = 1; ; req++) {
        length = sizeof(client_addr);

        if ((socket_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &length)) < 0)
            logger(ERROR, "system call", "accept", 0);

        if ((pid = fork()) < 0) {
            logger(ERROR, "system call", "fork", 0);
        } else {
            if (pid == 0) {  /* child */
                (void)close(listen_fd);
                handle_request(socket_fd, req);  /* never returns */
            } else {  /* parent */
                (void)close(socket_fd);
            }
        }
    }
}
