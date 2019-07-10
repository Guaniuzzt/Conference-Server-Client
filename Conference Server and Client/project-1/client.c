/*--------------------------------------------------------------------*/
/* conference client */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <netdb.h>
#include <time.h> 
#include <errno.h>
#include <stdlib.h>

#define MAXMSGLEN  1024

extern char *  recvdata(int sd);
extern int     senddata(int sd, char *msg);

extern int     connecttoserver(char *servhost, ushort servport);
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
  int  sock;
  fd_set client_fds;

  /* check usage */
  if (argc != 3) {
    fprintf(stderr, "usage : %s <servhost> <servport>\n", argv[0]);
    exit(1);
  }

  /* connect to the server */
  sock = connecttoserver(argv[1], atoi(argv[2]));
  printf("sock is : %d\n",sock);
  if (sock == -1)
    exit(1);


    FD_ZERO(&client_fds);
    



  while (1) {
    
    /*
      TODO: 
      use select() to watch for user inputs and messages from the server
    */
    int fd = sock;
    FD_SET(fd, &client_fds);
    FD_SET(0, &client_fds);

    select(sock+1, &client_fds, NULL, NULL, NULL);
    //printf("start first select\n");
    //if (FD_ISSET(sock, &client_fds))
    if (FD_ISSET(sock, &client_fds)){
      //printf("got data\n");
      char *msg;
      //printf("start rev data\n");
      msg = recvdata(sock);
      //printf("finish rev data\n");
      if (!msg) {
	/* server died, exit */
	fprintf(stderr, "error: server died\n");
	exit(1);
      }

      /* print the message */
      printf(">>> %s", msg);
      free(msg);
    }

    if (FD_ISSET(0, &client_fds)) {
      char msg[MAXMSGLEN];

      if (!fgets(msg, MAXMSGLEN, stdin))
	exit(0);
      senddata(sock, msg);
    }
  }
}
/*--------------------------------------------------------------------*/