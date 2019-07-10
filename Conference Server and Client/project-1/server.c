/*--------------------------------------------------------------------*/
/* conference server */

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
#include <unistd.h>

extern char *  recvdata(int sd);
extern int     senddata(int sd, char *msg);

extern int     startserver();

#define _MAX_SIZE_ 10
int fd_arr[_MAX_SIZE_];


static int add_fd_arr(int fd)
{
	//fd add to fd_arr
	int i=0;
	for(;i<_MAX_SIZE_;++i)
	{
		if(fd_arr[i]==-1)
		{
			fd_arr[i]=fd;
			return 0;
		}
	}
	return 1;
}
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/* main function*/
int main(int argc, char *argv[])
{
  int    serversock;    /* server socket*/

  fd_set liveskset;   /* set of live client sockets */
  int    liveskmax =3;   /* maximum socket */
  struct sockaddr_in cli;
  int max_fd = 0;

  /* check usage */
  if (argc != 1) {
    fprintf(stderr, "usage : %s\n", argv[0]);
    exit(1);
  }

  /* get ready to receive requests */
  serversock = startserver();
  if (serversock == -1) {
    exit(1);
  }
  
  /*
    TODO:
    init the live client set 
  */
  int i=0;
	for(;i<_MAX_SIZE_;++i)
	{
		fd_arr[i]=-1;
	}

  add_fd_arr(serversock);
  FD_ZERO(&liveskset);

  /* receive and process requests */
  while (1) {
    int   itsock;      /* loop variable */

    //reset fd_arr           
	for(i=0;i<_MAX_SIZE_;++i)     
		{
		if(fd_arr[i]!=-1)      
		{
			FD_SET(fd_arr[i],&liveskset);    
			if(fd_arr[i]>max_fd)        
			{
				max_fd=fd_arr[i];
			}
		}
	}


    /*
      TODO:
      using select() to serve both live and new clients
    */
    select(max_fd+1,&liveskset,NULL,NULL,NULL);
    
   // printf("first select finish\n");
    /* process messages from clients */
    for (itsock=3; itsock <= liveskmax; itsock++) {
      /* skip the listen socket */
      if (itsock == serversock) continue;

      if ( FD_ISSET(itsock, &liveskset) ) {
	char *  clienthost;  /* host name of the client */
	ushort  clientport;  /* port number of the client */
	
	/*
	  TODO:
	  obtain client's host name and port
	  using getpeername() and gethostbyaddr()
	*/
     /*struct sockaddr_in peerAddr;
     int peerLen;
     char ipAddr[INET_ADDRSTRLEN];
     getpeername(itsock, (struct sockaddr *)&peerAddr, &peerLen); 
     printf("connected peer address = %s:%d\n", inet_ntop(AF_INET, &peerAddr.sin_addr, ipAddr, sizeof(ipAddr)), ntohs(peerAddr.sin_port));

     //clienthost = inet_ntop(AF_INET, &peerAddr.sin_addr, ipAddr, sizeof(ipAddr));
     clientport = ntohs(peerAddr.sin_port);*/

     struct sockaddr_in c;
     socklen_t cLen = sizeof(c);
     getpeername(itsock, (struct sockaddr*) &c, &cLen);
     //printf("Client: %s:%d\n", inet_ntoa(c.sin_addr), ntohs(c.sin_port));

     clientport=ntohs(c.sin_port);
     clienthost = "localhost";



     /*struct hostent *host;
     const char *add = inet_ntoa(c.sin_addr);
     char p[30];
     inet_pton(AF_INET, add, p);
     host = gethostbyaddr(p, strlen(p), AF_INET);
     printf("hostname:%s\n",host->h_name);*/
	
	/* read the message */
    char *msg;
	msg = recvdata(itsock);
	
	if (!msg) {
	  /* disconnect from client */
	  printf("admin: disconnect from '%s(%hu)'\n",
		 clienthost, clientport);

	  //printf("give up message\n");

	  /*
	    TODO:
	    remove this client from 'liveskset'  
	  */
	  int j=0;
	  for(;j<_MAX_SIZE_;++j)
		{
			
		if(fd_arr[j]==fd_arr[i])
		{
			fd_arr[j]=-1;
			printf("remove this client from 'liveskset' \n");
			break;
		}
	}

	
		
	FD_CLR(itsock, &liveskset);
	close(itsock);
	} else {
	  /*
	    TODO:
	    send the message to other clients
	  */
		//printf("start send message to others\n");
		int k=1;
		for(k=1; k<_MAX_SIZE_;++k){
			if (fd_arr[k] == itsock ) 
				//printf("this is self,skip\n");
				continue;

			if(fd_arr[k] != -1){
				//printf("start send message to others2\n");
				senddata(fd_arr[k], msg);
				//printf("finish send message to others\n");
			}
				
		}
		
	  
	  /* print the message */
	  printf("%s(%hu): %s", clienthost, clientport, msg);
	  free(msg);
	}
      }
    }

    if (FD_ISSET(fd_arr[0], &liveskset) ) {
    	//printf("select fucntion realize new request\n");

      /*
	TODO:
	accept a new connection request
      */
    socklen_t len=sizeof(cli);
	int new_fd=accept(serversock,(struct sockaddr*)&cli,&len);
	//printf("finish accept the reuqest, the new fd is %d\n", new_fd);


      if (-1!=new_fd) {
      	printf("get a new link\n");
	char *  clienthost;  /* host name of the client */
	ushort  clientport;  /* port number of the client */
    	 
	/*
	  TODO:
	  get client's host name and port using gethostbyaddr() 
	*/
     struct sockaddr_in c;
     socklen_t cLen = sizeof(c);
     getpeername(itsock, (struct sockaddr*) &c, &cLen);
     //printf("Client: %s:%d\n", inet_ntoa(c.sin_addr), ntohs(c.sin_port));

     clientport= ntohs(c.sin_port);
     clienthost = "localhost";



	printf("admin: connect from '%s' at '%hu'\n",
	       clienthost, clientport);
	

	//change the liveskmax 
    //printf("change the liveskmax\n");
	if(liveskmax < new_fd)
		liveskmax = new_fd;
	/*
	  TODO:
	  add this client to 'liveskset'
	*/
	if(1==add_fd_arr(new_fd))
	{
		perror("fd_arr is full,close new_fd\n"); //if the number of clien above 10 kill it
		close(new_fd);
	}
      } else {
	perror("accept");
	exit(0);
      }
    }
  }
}
/*--------------------------------------------------------------------*/