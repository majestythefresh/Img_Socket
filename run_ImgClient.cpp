/***********************************************************************
* FILENAME    :   run_ImgClient.cpp
* AUTHOR      :   Arturo Plauchu (plauchu@gmail.com)
* DATE        :   01 Nov 2017
* DESCRIPTION :   Main function to send an Image to...
                  ... Server using Client class.
* LICENSE     :   GPL v2.0
*
***********************************************************************/
#include "Client.hpp"

/***********************************************************************/
Errors printResponse( Errors er, Client *cl ){
//Helper friend function to handle error messages
//
  switch(er){
    case NO_ERROR:
    cout << "Connection Success to [" << cl->getIP()  << "] - Port [" <<
            cl->getPort() << "]" << endl;
    break;

    case SOCK_ERROR:
    cout << "Socket creation error to [" << cl->getIP()  << "] - Port [" <<
            cl->getPort() << "]" << endl;
    break;

    case CON_ERROR:
    cout << "Connection error to [" << cl->getIP()  << "] - Port [" <<
            cl->getPort() << "]" << endl;
    cout << "The server maybe this off or IP is out of reach" << endl;
    break;

    case ACCESS_ERROR:
    cout << "Connection access error to [" << cl->getIP()  << "] - Port [" <<
            cl->getPort() << "]" << endl;
    break;

    default:
    cout << "Unknow error to [" << cl->getIP()  << "] - Port [" <<
            cl->getPort() << "]" << endl;
    break;
  }
  return er;
}

/***********************************************************************/
int main(int argc, char *argv[]){
//Main function
//
  char * img_path;
  char * server_ip;
  int server_port;
  struct stat stat_file;
  bool ext_flag = false;
  char * ext_file;

  if(argc < 4){
    cout << "\n\tYou need specify 3 arguments at least" << endl;
    cout << "\tImgClient <server_ip> <port> <image path> [-t]" << endl;
    cout << "\t   server_ip : server ip to connect." << endl;
    cout << "\t   port      : Server port to connect." << endl;
    cout << "\t   image path: path to Image to transfer." << endl;
    cout << "\t   -t        : Send Image even NO ACK (Request from not a local network) received." << endl;
    cout << "\n\tExample:" << endl;
    cout << "\tImgClient 192.168.15.228 3491 /Users/plauchu/Pictures/test3.jpeg" << endl;
    cout << "\tImgClient 192.168.15.228 3491 /Users/plauchu/Pictures/test3.jpeg -t\n" << endl;
    return 1;
  }
  server_ip = argv[1];
  server_port = atoi(argv[2]);

  if (stat(argv[3], &stat_file) == 0){
    if(S_ISDIR(stat_file.st_mode)){
      cout << argv[3] <<" : is not a File" << endl;
      return 1;
    }
    else{
      ext_file = strrchr(argv[3], '.');
      if (!ext_file) {
        cout << "No extension in file, it needs to being .jpg or .jpeg: " << argv[3] << endl;
        return 1;
      }
      else {
        if(strcasecmp(ext_file,".jpg") && strcasecmp(ext_file,".jpeg")){
          cout << "File needs to being .jpg or .jpeg: " << ext_file << endl;
          return 1;
        }
      }
      img_path = argv[3];
    }
  }
  else {
    cout << "File doesn't exist: " << argv[3] << endl;
    return 1;
  }
  //Set flag to accept outside from network transfers
  if( argv[4] != NULL){
    if(!strcmp(argv[4], "-t")){
      ext_flag = true;
    }
  }

  //Client obj instance
  Client cl(server_ip, server_port);
  cl.setTranferFlag(ext_flag);
  cout << "Ip Set: " << cl.getIP() << endl;
  //Open a connection to Server
  if(printResponse(cl.openCon(), &cl) == NO_ERROR){
    //If no error Send an Image!
    printResponse(cl.sendImage(img_path),&cl);
  }
  return 0;
}
