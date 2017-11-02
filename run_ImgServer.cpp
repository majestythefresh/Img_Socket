/***********************************************************************
* FILENAME    :   run_ImgServer.cpp
* AUTHOR      :   Arturo Plauchu (plauchu@gmail.com)
* DATE        :   01 Nov 2017
* DESCRIPTION :   Main function to receive an Image from...
                  ... Client using Server class.
* LICENSE     :   GPL v2.0
*
***********************************************************************/
#include "Server.hpp"

/***********************************************************************/
Errors printResponse( Errors er, Server srv ){
//Helper friend function to handle error messages
//
  switch(er){
    case NO_ERROR:
    cout << "Connection Success binding to " << srv.getIP() << endl;
    break;

    case SOCK_ERROR:
    cout << "Socket creation error to " << srv.getIP() << endl;
    break;

    case CON_ERROR:
    cout << "Connection binding error to " << srv.getIP() << endl;
    break;

    case ACCEPT_ERROR:
    std::cout << "Accept Connection error to " << srv.getIP() << endl;
    break;

    default:
    cout << "Unknow error to [" << srv.getIP()  << "] - Port [" <<
            srv.getPort() << "]" << endl;
    break;
  }
  return er;
}

/***********************************************************************/
int main(int argc, char *argv[]){
//Main function
//
  char * server_ip;
  int server_port;
  char * server_path;

  if(argc < 4){
    cout << "\n\tYou need specify 3 arguments:" << endl;
    cout << "\tImgServer <server_ip> <port> <path to save transferred images>"
         << endl;
    cout << "\n\tExample:" << endl;
    cout << "\tImgServer 192.168.15.228 3491 /Users/plauchu\n" << endl;
    return 1;
  }
  server_ip = argv[1];
  server_port = atoi(argv[2]);
  server_path = argv[3];
  //Server obj instance
  Server srv(server_ip, server_port, server_path);
  cout << "Ip Set: " << srv.getIP() << endl;
  //Open a connection as Server
  if(printResponse(srv.openCon(), srv) == NO_ERROR){
    //If no error Go to Listen!
    srv.goListen();
  }

  return 0;
}
