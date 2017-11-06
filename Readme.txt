Compile:
g++ Server.cpp run_ImgServer.cpp -o ImgServer
g++ Client.cpp run_ImgClient.cpp -o ImgClient

How to run Server:
 ImgServer <server_ip> <port> <path to save transferred images>
 To stop server: hit Ctrl+c

How to run Client:
 ImgClient <server_ip> <port> <image path>

Run Example:
./ImgServer 192.168.15.228 3491 /home/server_user
./ImgClient 192.168.15.228 3491 /home/client_user/Pictures/test.jpeg



Run Docker container:

docker build --no-cache -f docker-container/ImgApp.txt -t dev_linux:latest .
docker run --name server_dev -itd dev_linux:latest
docker run --name client_dev -itd dev_linux:latest
docker attach server_dev
docker attach client_dev
