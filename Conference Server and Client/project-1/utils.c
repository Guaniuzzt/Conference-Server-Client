/*--------------------------------------------------------------------*/
/* functions to connect clients and server */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <netdb.h>
#include <time.h> 
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXNAMELEN 256



/*--------------------------------------------------------------------*/


/*----------------------------------------------------------------*/


int startserver()
{
  int     sd, err;        /* socket */
  char *  serverhost;  /* hostname */
  ushort  serverport;  /* server port */
  struct sockaddr_in server;

  /*
    TODO:
    create a TCP socket        OK
  */ 

  sd = socket(AF_INET, SOCK_STREAM, 0);
  if (sd < 0) perror("Could not create socket\n");

  server.sin_family = AF_INET;
  server.sin_port = INADDR_ANY;
  server.sin_addr.s_addr = 0;

  /*
    TODO:
    bind the socket to some random port, chosen by the system  OK
  */

  err = bind(sd, (struct sockaddr *) &server, sizeof(server));
   if (err < 0) perror("Could not bind socket\n");



  /* ready to receive connections */
  listen(sd, 5);
  /*
    TODO:
    obtain the full local host name (serverhost)
    use gethostname() and gethostbyname()       OK
  */
  char host[100] = {0};
  if (gethostname(host, sizeof(host)) < 0) {
      perror("gethostname error");
  }
  struct hostent *hp;
  if ((hp = gethostbyname(host)) == NULL) {
        perror("gethostbyname error");
  }  
  int i = 0;
  while (hp->h_addr_list[i] != NULL) {
      printf("hostname: %s\n", hp->h_name);
      printf("ip:%s\n", inet_ntoa(*(struct in_addr*)hp->h_addr_list[i]));
      serverhost = hp->h_name;
      i++;
  }
  /*
    TODO:
    get the port assigned to this server (serverport)
    use getsockname()
  */
  struct sockaddr_in myaddr;
  int myaddr_len=sizeof(myaddr);
  char bind_ip[20]={0};
  getsockname(sd,(struct sockaddr *)&myaddr,&myaddr_len);
  inet_ntop(AF_INET, &myaddr.sin_addr, bind_ip, sizeof(bind_ip));
  serverport = ntohs(myaddr.sin_port);


  /* ready to accept requests */
  printf("admin: started server on '%s' at '%hu'\n",
   serverhost, serverport);
  return(sd);
}
/*----------------------------------------------------------------*/

/*----------------------------------------------------------------*/
/*
  establishes connection with the server
*/
int connecttoserver(char *serverhost, ushort serverport)
{
  int     sd;          /* socket */
  ushort  clientport;  /* port assigned to this client */
  struct hostent *server;
  struct sockaddr_in serv_addr;

  /*
    TODO:
    create a TCP socket 
  */
  sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) perror("Could not create socket\n");
  /*
    TODO:
    connect to the server on 'serverhost' at 'serverport'
    use gethostbyname() and connect()
  */
  server = gethostbyname(serverhost); 
  if(server == NULL){
    fprintf(stderr, "Error , no such host\n" );
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server -> h_addr , (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(serverport);   //?

  if(connect(sd,(struct  sockaddr *) &serv_addr, sizeof(serv_addr))<0)
      perror("Connection Failed");

  /*
    TODO:
    get the port assigned to this client
    use getsockname()
  */

  struct sockaddr_in myaddr;
  int myaddr_len=sizeof(myaddr);
  char bind_ip[20]={0};
  getsockname(sd,(struct sockaddr *)&myaddr,&myaddr_len);
  inet_ntop(AF_INET, &myaddr.sin_addr, bind_ip, sizeof(bind_ip));
  clientport = ntohs(myaddr.sin_port);

  /* succesful. return socket */
  printf("admin: connected to server on '%s' at '%hu' thru '%hu'\n",
	 serverhost, serverport, clientport);
  return(sd);
}
/*----------------------------------------------------------------*/


/*----------------------------------------------------------------*/
int readn(int sd, char *buf, int n)
{
  int     toberead;
  char *  ptr;

  toberead = n;
  ptr = buf;
  while (toberead > 0) {
    int byteread;

    byteread = read(sd, ptr, toberead);
    if (byteread <= 0) {
      if (byteread == -1)
	perror("read");
      return(0);
    }
    
    toberead -= byteread;
    ptr += byteread;
  }
  return(1);
}

char *recvdata(int sd)
{
  char *msg;
  long  len;
  
  /* get the message length */
  if (!readn(sd, (char *) &len, sizeof(len))) {
    return(NULL);
  }
  len = ntohl(len);

  /* allocate memory for message */
  msg = NULL;
  if (len > 0) {
    msg = (char *) malloc(len);
    if (!msg) {
      fprintf(stderr, "error : unable to malloc\n");
      return(NULL);
    }

    /* read the message */
    if (!readn(sd, msg, len)) {
      free(msg);
      return(NULL);
    }
  }
  
  return(msg);
}

int senddata(int sd, char *msg)
{
  long len;

  /* write lent */
  len = (msg ? strlen(msg) + 1 : 0);
  len = htonl(len);
  write(sd, (char *) &len, sizeof(len));

  /* write message data */
  len = ntohl(len);
  if (len > 0)
    write(sd, msg, len);
  return(1);
}
/*----------------------------------------------------------------*/