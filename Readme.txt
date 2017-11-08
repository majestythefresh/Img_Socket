* Compile:
$ g++ Server.cpp run_ImgServer.cpp -o ImgServer -lexif
$ g++ Client.cpp run_ImgClient.cpp -o ImgClient

* How to run Server:
 ImgServer <server_ip> <port> <images path>
   server_ip   : ip to start listening.
   port        : port to start listening.
   images path : Server folder to save tranferred images.
 To stop server: hit Ctrl+c

* How to run Client:
 ImgClient <server_ip> <port> <image path> [-t]
   server_ip : server ip to connect.
   port      : Server port to connect.
   image path: path to Image to transfer.
   -t        : Send Image even NO ACK (Request from not a local network) received.

* Run Example:
$ ./ImgServer 172.17.0.2 3491 /home/img_server
$ ./ImgClient 172.17.0.2 3491 Img_Socket/test-images/test1.jpg -t



* Run Docker container:
$ cd Img_Socket/
$ docker build --no-cache -f docker-container/ImgApp.txt -t dev_linux:latest .
$ docker run --name server_dev -itd dev_linux:latest
$ docker run --name client_dev -itd dev_linux:latest
$ docker attach server_dev
$ docker attach client_dev


* Check exif User Comment tag data:
$ exif /home/img_server/test2.jpg
EXIF tags in '/home/img_server/test2.jpg' ('Motorola' byte order):
--------------------+----------------------------------------------------------
Tag                 |Value
--------------------+----------------------------------------------------------
...                 |...
User Comment        |source_ip:<172.17.0.3>,  is_from_local_network:<true>
Exif Version        |Exif Version 2.1
...                 |...
--------------------+----------------------------------------------------------
