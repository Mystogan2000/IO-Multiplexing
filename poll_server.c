#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <poll.h>

#define MAXEVENTS 64


  struct pollfd poll_fds[MAXEVENTS];
  int nfds;
  
static int make_socket_non_blocking (int sfd)
{
  int flags, s;

  flags = fcntl (sfd, F_GETFL, 0);
  if (flags == -1)
    {
      perror ("fcntl");
      return -1;
    }

  flags |= O_NONBLOCK;
  s = fcntl (sfd, F_SETFL, flags);
  if (s == -1)
    {
      perror ("fcntl");
      return -1;
    }
 
  return 0;
}



static int create_and_bind (char *port)
{
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  int s, sfd;

  memset (&hints, 0, sizeof (struct addrinfo));
  hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
  hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
  hints.ai_flags = AI_PASSIVE;     /* All interfaces */

  s = getaddrinfo (NULL, port, &hints, &result);
  if (s != 0)
    {
      fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
      return -1;
    }

  for (rp = result; rp != NULL; rp = rp->ai_next)
    {
      sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
      if (sfd == -1)
        continue;

      s = bind (sfd, rp->ai_addr, rp->ai_addrlen);
      if (s == 0)
        {
          /* We managed to bind successfully! */
          break;
        }

      close (sfd);
    }

  if (rp == NULL)
    {
      fprintf (stderr, "Could not bind\n");
      return -1;
    }

  freeaddrinfo (result);

  return sfd;
}



int main (int argc, char *argv[])
{
  int sfd, s;

  //a message 
  char *message = "Connection Established to Server \r\n";  
  
  
  //checking whether port number is used in argument or not!
  if (argc != 2)
    {
      fprintf (stderr, "Usage: %s [port]\n", argv[0]);
      exit (EXIT_FAILURE);
    }
  
  //creating a socket and binding with ip & port and returning the fd listing fd returned
  sfd = create_and_bind (argv[1]);

  
  //making socket non blocking
  s = make_socket_non_blocking (sfd);

  //Enters into listner process
  s = listen (sfd, SOMAXCONN);
 

  //creating epoll instance


  
  //adding the fd into epoll list with kind of event to watch for
poll_fds[0].fd= sfd;
poll_fds[0].events = POLLIN ;
nfds = 1;
  
  while (1)
 {
    int n, i;
    poll(poll_fds, nfds, -1);
   
    
    for (i = 0; i < (nfds +1); i++)
	 {
	 
	 		if(poll_fds[i].fd <=0)
	 						continue;
	   if (poll_fds[i].events == POLLIN)
	   {
	   		if(poll_fds[i].fd == sfd){
                
                  struct sockaddr in_addr;
                  socklen_t in_len;
                  int infd;
                  char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

                  in_len = sizeof in_addr;
                  infd = accept (sfd, &in_addr, &in_len);
                  

                  s = getnameinfo (&in_addr, in_len,  hbuf, sizeof hbuf, sbuf, sizeof sbuf, NI_NUMERICHOST | NI_NUMERICSERV);
                  if (s == 0)
                    {
                      printf("Accepted connection on descriptor %d  (host=%s, port=%s)\n", infd, hbuf, sbuf);
                    }

                  /* Make the incoming socket non-blocking and add it to the list of fds to monitor. */
                  s = make_socket_non_blocking (infd);
           
                  send(infd, message, strlen(message), 0) ;
                  
                  poll_fds[i].fd = infd;
                  poll_fds[i].events = POLLIN;
                  nfds++;
           

            }
          else
            {

                  int done = 0;

              
                  ssize_t count,c1;
                  char buf[512];
                 
                  
                  count = read (poll_fds[i].fd, buf, sizeof buf);
                
                  s = write (1, buf, count);
                
            
            
            }//end for epool_IN
        
        }//end of for loop
    }
    }//end of while(1)

 

  close (sfd);//final sfd

  return EXIT_SUCCESS;
}


