/***********************************************************************
* FILENAME    :   Server.hpp
* AUTHOR      :   Arturo Plauchu (plauchu@gmail.com)
* DATE        :   01 Nov 2017
* DESCRIPTION :   Class Declaration for Server Class.
                  Class to receive an Image from a Client.
* LICENSE     :   GPL v2.0
*
***********************************************************************/
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <libgen.h>
#include <assert.h>
#include <libexif/exif-data.h>
#include "img_types.h"

using namespace std;

#ifndef SERVER_HPP_INCLUDED
#define SERVER_HPP_INCLUDED

class Server
{
public:
  Server(const char *ip_server, unsigned short port_server,
        const char *path_server);
  ~Server();
  const char * getIP() const;
  const int getPort() const;
  Errors openCon();
  Errors goListen();
  friend Errors printResponse( Errors er, Server *srv );
  friend void quitHandler(int param);

private:
  const char *server_ip;
  unsigned short server_port;
  const char *server_path;
  const char *server_filename;
  static int socket_fd; //Static to close socket trough a signal handler
  int accept_fd;
  struct sockaddr_in server;
  struct sockaddr_in client;
  uint32_t cl_network_addr;
  uint32_t cl_host_addr;
  bool flag_local_request;
  static bool end_flag; //Static to close socket listen loop

  bool rcvImage();
  bool writeExifUserComment(const char * str);
  ExifEntry *create_tag(ExifData *exif, ExifIfd ifd, ExifTag tag, size_t len);
  void setIP(const char *ip_string);
  bool isLocalRequest(unsigned long client_addr);
  uint32_t getIPInRightEndianess(unsigned long client_addr) const;
  bool isReqFromLocal(unsigned long client_addr, uint32_t * network,
                      uint32_t * host);
  uint32_t getServerNetwork() const;
  bool isBigEndian(void) const;

};
#endif
