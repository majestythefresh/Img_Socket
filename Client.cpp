/***********************************************************************
* FILENAME    :   Client.cpp
* AUTHOR      :   Arturo Plauchu (plauchu@gmail.com)
* DATE        :   01 Nov 2017
* DESCRIPTION :   Class Definition for Client Class.
                  Class to send an Image to a Server.
* LICENSE     :   GPL v2.0
*
***********************************************************************/
#include "Client.hpp"

/***********************************************************************/
Client::Client(const char *ip_server, unsigned short port_server){
//Constructor
//
  setIP(ip_server);
  server_port = port_server;
}

/***********************************************************************/
const char * Client::getIP() const{
//Get server ip to connect
//
  return server_ip;
}

/***********************************************************************/
unsigned short Client::getPort() const{
//Get server port to connect
//
  return server_port;
}

/***********************************************************************/
Errors Client::openCon(){
//Open a connection with Server and check ACK message to
// proceed a transfer request.
//
  //Open Socket DARPA TCP
  socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (socket_fd  < 0) {
    return SOCK_ERROR;
  }
  //Set socket structure with custom data
  memset(&server, 0, sizeof(sockaddr_in));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(server_ip);
  server.sin_port = htons(server_port);
  //Send connection attemp to Server
  if(connect(socket_fd, (struct sockaddr *) &server, sizeof(sockaddr_in))< 0){
    return CON_ERROR;
  }
  //In case of the server accept the transfer will send an ACK
  recv(socket_fd, ack_buffer, ACK_BUF, 0);
  if ( ! strcmp(ack_buffer, ACK_MSG) ){
    cout << "--- Recive ACK ---" << endl;
  }
  else {// If NO ACK, the transfer can't be
    return ACCESS_ERROR;
  }
  return NO_ERROR;
}

/***********************************************************************/
Errors Client::sendImage(const char * img_path){
// Send Image from local path to connected Server.
//
  struct stat file_st;
  int img_fd;
  char message[1024];
  char img_buf[65000];
  int next_char_st;
  char chunk[1];
  int count = 0;
  //Open a descriptor to get stat info about the file
  img_fd = open(img_path,O_RDONLY);
  fstat(img_fd,&file_st);

  printf("Image Size: %lld \n", file_st.st_size);
  sprintf(message,"Image|%s|Size: %lld", img_path, file_st.st_size);
  //Send a message to tell Server that an Image will be sent.
  send(socket_fd, message, sizeof(message), 0);
  //Get a file stream from the file descriptor opened
  FILE* file_stream = fdopen(img_fd, "rb");
  cout << "--> Sending Image" << endl;
  while( (next_char_st=getc(file_stream)) != EOF ){
    chunk[0] = next_char_st;
    //Sending byte per byte binary data in stream
    if ( ( send(socket_fd, chunk, 1, 0)) < 0){
      //Closing descriptors
      fclose(file_stream);
      close(img_fd);
      close(socket_fd);
      return SEND_ERROR;
    }
    count++;
  }
  cout << "--- Image Sent : "<< count << " Bytes ---" << endl;
  //Closing descriptors
  fclose(file_stream);
  close(img_fd);
  close(socket_fd);

  return NO_ERROR;
}

/***********************************************************************/
void Client::setIP(const char *ip_string){
// Set Server Ip to connect.
//
  server_ip = ip_string;
}
