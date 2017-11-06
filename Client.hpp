/***********************************************************************
* FILENAME    :   Client.hpp
* AUTHOR      :   Arturo Plauchu (plauchu@gmail.com)
* DATE        :   01 Nov 2017
* DESCRIPTION :   Class Declaration for Client Class.
                  Class to send an Image to a Server.
* LICENSE     :   GPL v2.0
*
***********************************************************************/
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include "img_types.h"

using namespace std;

#ifndef CLIENT_HPP_INCLUDED
#define CLIENT_HPP_INCLUDED

class Client
{
public:
  Client(const char *ip_server, unsigned short port_server);
  const char * getIP();
  unsigned short getPort();
  Errors openCon();
  Errors sendImage(const char * img_path);
  friend Errors printResponse( Errors er, Client cl );

private:
  const char *server_ip;
  unsigned short server_port;
  int socket_fd;
  struct sockaddr_in server;
  char ack_buffer[6];

  void setIP(const char *ip_string);

};
#endif
