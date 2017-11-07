/***********************************************************************
* FILENAME    :   Client.cpp
* AUTHOR      :   Arturo Plauchu (plauchu@gmail.com)
* DATE        :   01 Nov 2017
* DESCRIPTION :   Class Definition for Server Class.
                  Class to receive an Image from a Client.
* LICENSE     :   GPL v2.0
*
***********************************************************************/
#include "Server.hpp"

/***********************************************************************/
Server::Server(const char *ip_server, unsigned short port_server,
               const char *path_server){
//Constructor
//
  setIP(ip_server);
  server_port = port_server;
  server_path = path_server;
}

Server::~Server(){
//Destructor
//
  close(accept_fd);
  cout << "Goodbye!..." << endl;
}

/***********************************************************************/
const char * Server::getIP() const{
//Get server ip to listen
//
  return server_ip;
}

/***********************************************************************/
const int Server::getPort() const{
//Get server port to listen
//
  return server_port;
}

/***********************************************************************/
Errors Server::openCon(){
//Open a connection in Server to bind connections
//
  //Open Socket DARPA TCP
  socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (socket_fd  < 0) {
    return SOCK_ERROR;
  }
  //Set socket structure with custom data
  memset(&server, 0, sizeof(sockaddr_in));
  server.sin_family = AF_INET;
  server.sin_port = htons(server_port);
  server.sin_addr.s_addr = inet_addr(server_ip);
  //Bind connection attempts
  if (bind(socket_fd, (struct sockaddr *) &server, sizeof(sockaddr_in)) < 0){
    return CON_ERROR;
  }
  return NO_ERROR;
}

/***********************************************************************/
Errors Server::goListen(){
// Put Server to listen and accept or denied Image transfers
//
  bool is_from_local = false;
  char msgStr[FILE_COMMENT_LEN];
  //Start listen connection attempts
  listen(socket_fd, 0);
  cout << "Listening in [" << server_ip << "] - Port [" << server_port <<
           "]" << endl;
  socklen_t client_len = sizeof (client);
  //Keep listening tranfers
  while (!end_flag){
    //accept any try to request to Server
    accept_fd = accept(socket_fd, (struct sockaddr *) &client, &client_len);
    if (accept_fd < 0){
      return ACCEPT_ERROR;
    }
    else{
      cout << "* Connection attempt from " << inet_ntoa(client.sin_addr)
           << endl;
      printf("[ Request Network Byte Order IP: %x ]\n", client.sin_addr.s_addr);
      //To proceed a transfer the client request need to come from local network
      if (isLocalRequest(client.sin_addr.s_addr)){
        //Once the connection request came from local/same network, send an ACK
        send(accept_fd, ACK_MSG ,ACK_BUF,0);
        cout << "--- ACK Message to client sent ---" << endl;
        is_from_local = true;
      }
      else{
        //If the client connection request came from outside, send NO ACK msg
        send(accept_fd, NO_ACK_MSG ,ACK_BUF,0);
        cout << "--- NO ACK Message to client sent ---" << endl;
        is_from_local = false;
      }
      //Start receiving Image transfer
      if(rcvImage()){
        //Inject Exif UserComment
        sprintf(msgStr, FILE_COMMENT,
               inet_ntoa(client.sin_addr),
               (is_from_local?"true":"false"));
        cout << "--- " << msgStr << " ---" << endl;
        writeExifUserComment(msgStr);
      }
    }
    close(accept_fd);
  }
  return NO_ERROR;
}

/***********************************************************************/
bool Server::rcvImage(){
//Receive an Image transfer and write to a file in Server path
//
  char buffer[BUF_4KB];
  char * buf_args;
  char file_server_path[BUF_256B];
  bool flag_rcv = false;
  char * file_name;
  char * file_size;
  FILE* cp_file;
  int count = 0;

  //Start to read header request of Image type
  recv(accept_fd, buffer, sizeof(buffer), 0);
  cout << " Receiving File transfer request: " << endl;
  cout << "   [" << buffer << "]" << endl;
  //If the header is Image type start to write stream to a file.
  if(! strncmp(buffer,"Image",5) ){
    cout << "\t-- File request type: Image [ OK ]" << endl;
    //Splitting header to ger file name
    buf_args = strtok( buffer, "|" );
    buf_args = strtok( NULL, "|" );
    file_name = basename(buf_args);
    buf_args = strtok( NULL, "|" );
    buf_args = strtok( NULL, "|" );
    file_size = buf_args;
    cout << "\t    Message Header Image size : "<< file_size << endl;
    cout << "\t    Message Header Image filename : "<< file_name << endl;
    //Concatenate path where to save image with image file name.
    sprintf(file_server_path, "%s/%s", server_path, file_name);
    server_filename = file_server_path;
    cout << "\t-- Saving Image to "<< file_server_path << endl;
    cp_file = fopen(file_server_path,"wb" );
    buffer[0] = 0;
    //Start saving to a file the data stream.
    while(!flag_rcv){
      if( recv(accept_fd, buffer, 1, 0) != 0 ){
        fwrite (&buffer , sizeof(buffer[0]) , 1 , cp_file );
        buffer[0] = 0;
        count++;
      }
      else{
        flag_rcv = true;
        //flushing the descriptor stream to get image without corruption
        fflush(cp_file);
      }
    }
    if(atoi(file_size) != count){
      cout << "\t-- Image Received : "<< count << " Bytes [INCOMPLETE]" << endl;
      //Removing incomplete file
      remove(file_server_path);
      return false;
    }
    cout << "\t-- Image Received : "<< count << " Bytes [COMPLETE]" << endl;
    fclose(cp_file);
  }
  else{
    cout << "\t-- Wrong File request type: No Image [ X ]" << endl;
    return false;
  }
  return true;
}

/***********************************************************************/
bool Server::writeExifUserComment(const char * str){
//Function that injects EXIF USER COMMENT data to received image
//
  FILE *imgFile, *newImgFile;
  char *imgFileBuf;
  long fileLen;
	unsigned char *exif_data;
	unsigned int exif_data_len;
	ExifEntry *entry;
  ExifData *exif;

  const unsigned int image_data_offset = IMAGE_DATA_OFFSET;
  const unsigned char exif_header[] = EXIF_HEADER_OFFSET;
  // length of data in exif_header
  const unsigned int exif_header_len = sizeof(exif_header);

  //Open image received to get in raw data
  imgFile = fopen(server_filename, "rb");
  fseek(imgFile, 0, SEEK_END);
  fileLen = ftell(imgFile);
  rewind(imgFile);
  //Getting memory for raw data buffer
  imgFileBuf = (char *)malloc((fileLen+1)*sizeof(char));
  //Set char raw data in buffer
  fread(imgFileBuf, fileLen, 1, imgFile);
  fclose(imgFile);
  exif = exif_data_new();
  if (!exif) {
		fprintf(stderr, "Out of memory\n");
		return false;
	}

  // Create a EXIF_TAG_USER_COMMENT tag.
	entry = create_tag(exif, EXIF_IFD_EXIF, EXIF_TAG_USER_COMMENT,
			sizeof(ASCII_COMMENT) + FILE_COMMENT_LEN - 2);
	// Write the special header needed for a comment tag
	memcpy(entry->data, ASCII_COMMENT, sizeof(ASCII_COMMENT)-1);
	// Write the actual comment text, without the trailing NUL character
	memcpy(entry->data+8, str, FILE_COMMENT_LEN-1);
	// Get a pointer to the EXIF data block we just created
	exif_data_save_data(exif, &exif_data, &exif_data_len);
	assert(exif_data != NULL);

  newImgFile = fopen(server_filename, "wb+");
	if (!newImgFile) {
		fprintf(stderr, "Error creating file %s\n",  server_filename);
		exif_data_unref(exif);
		return false;
	}
	// Write EXIF header
	if (fwrite(exif_header, exif_header_len, 1, newImgFile) != 1) {
		fprintf(stderr, "Error writing to file %s\n",  server_filename);
		return false;
	}
	// Write EXIF block length in big-endian order
	if (fputc((exif_data_len+2) >> 8, newImgFile) < 0) {
		fprintf(stderr, "Error writing to file %s\n",  server_filename);
		return false;
	}
	if (fputc((exif_data_len+2) & 0xff, newImgFile) < 0) {
		fprintf(stderr, "Error writing to file %s\n",  server_filename);
		return false;
	}
	// Write EXIF data block
	if (fwrite(exif_data, exif_data_len, 1, newImgFile) != 1) {
		fprintf(stderr, "Error writing to file %s\n",  server_filename);
		return false;
	}
	// Write JPEG image data, skipping the non-EXIF header
	if (fwrite(imgFileBuf + image_data_offset, fileLen, 1, newImgFile) != 1) {
		fprintf(stderr, "Error writing to file %s\n",  server_filename);
		return false;
	}
	printf("Exif UserComment modified in file %s\n", server_filename);

	if (fclose(newImgFile)) {
		fprintf(stderr, "Error writing to file %s\n", server_filename);
		return false;
	}
	// The allocator we're using for ExifData is the standard one, so use
	// it directly to free this pointer.
	free(exif_data);
	exif_data_unref(exif);

  return true;
}

/***********************************************************************/
ExifEntry *Server::create_tag(ExifData *exif, ExifIfd ifd, ExifTag tag, size_t len)
{
//Helper function to allocate Image Exif data entry
//
	void *buf;
	ExifEntry *entry;

	/* Create a memory allocator to manage this ExifEntry */
	ExifMem *mem = exif_mem_new_default();
	assert(mem != NULL); /* catch an out of memory condition */

	/* Create a new ExifEntry using our allocator */
	entry = exif_entry_new_mem (mem);
	assert(entry != NULL);

	/* Allocate memory to use for holding the tag data */
	buf = exif_mem_alloc(mem, len);
	assert(buf != NULL);

	/* Fill in the entry */
	entry->data = (unsigned char *) buf;
	entry->size = len;
	entry->tag = tag;
	entry->components = len;
	entry->format = EXIF_FORMAT_UNDEFINED;

	/* Attach the ExifEntry to an IFD */
	exif_content_add_entry (exif->ifd[ifd], entry);

	/* The ExifMem and ExifEntry are now owned elsewhere */
	exif_mem_unref(mem);
	exif_entry_unref(entry);

	return entry;
}

/***********************************************************************/
void Server::setIP(const char *ip_string){
//Set Ip server to bind
//
  server_ip = ip_string;
}

/***********************************************************************/
bool Server::isLocalRequest(unsigned long client_addr){
//Get true or false if an IP is part of same network
//
  uint32_t ip;
  //Show host byte order (just display purposes)
  ip = getIPInRightEndianess(client_addr);
  printf("[ Request Host Byte Order IP -> %x ]\n", ip);
  //Client Request from local?
  flag_local_request = isReqFromLocal(client_addr, &cl_network_addr,
                                      &cl_host_addr);
  if(flag_local_request){
    return true;
  }
  return false;
}

/***********************************************************************/
uint32_t Server::getIPInRightEndianess(unsigned long client_addr) const{
//return IP in host byte order
//
  uint32_t ip_addr = ZERO_4BYTES;
  if(isBigEndian()){
    ip_addr = uint32_t(client_addr);
  }
  else {
    ip_addr |= uint32_t(client_addr & FIRST_BYTE_LE) << 24;
    ip_addr |= uint32_t(client_addr & SECOND_BYTE_LE) << 8;
    ip_addr |= uint32_t(client_addr & THIRD_BYTE_LE) >> 8;
    ip_addr |= uint32_t(client_addr & FOURTH_BYTE_LE) >> 24;
  }
  return ip_addr;
}

/***********************************************************************/
bool Server::isReqFromLocal(unsigned long client_addr, uint32_t * network,
                            uint32_t * host){
//Function to split client request IP in the Host byte order,...
//... check Network Class and apply Mask according to.
//The function return true if the ip came from local/same network after analisys
//
  uint32_t server;
  uint8_t cl_first_octet;
  uint32_t cl_ip_addr = ZERO_4BYTES;
  uint32_t sub_a_mask = ZERO_4BYTES;
  uint32_t sub_b_mask = ZERO_4BYTES;
  uint32_t sub_c_mask = ZERO_4BYTES;
  bool flag_return = false;

  if(isBigEndian()){
    // As Big Endian the order remains the same as network byte order
    cl_first_octet = uint8_t(client_addr);
    cl_ip_addr = uint32_t(client_addr);
    //Setting subnet mask according to endiannes
    sub_a_mask = CLASS_A_SUB_MASK_BE;
    sub_b_mask = CLASS_B_SUB_MASK_BE;
    sub_c_mask = CLASS_C_SUB_MASK_BE;
  }
  else {
    // As Little Endian the order is different to network byte order.
    // So, we shift the right order
    cl_first_octet = uint8_t(client_addr & FIRST_BYTE_LE);
    cl_ip_addr |= uint32_t(client_addr & FIRST_BYTE_LE) << 24;
    cl_ip_addr |= uint32_t(client_addr & SECOND_BYTE_LE) << 8;
    cl_ip_addr |= uint32_t(client_addr & THIRD_BYTE_LE) >> 8;
    cl_ip_addr |= uint32_t(client_addr & FOURTH_BYTE_LE) >> 24;
    //Setting subnet mask according to endiannes
    sub_a_mask = CLASS_A_SUB_MASK_LE;
    sub_b_mask = CLASS_B_SUB_MASK_LE;
    sub_c_mask = CLASS_C_SUB_MASK_LE;
  }
  //With first octet from IP we match the Network Class
  if(cl_first_octet >= 0 && cl_first_octet <= 127 ){    // Class A Network
     //Getting network applying subnet mask A
    *network = cl_ip_addr & sub_a_mask;
    //Getting host applying reverse subnet mask A
    *host = cl_ip_addr & ~sub_a_mask;
    cout << "[ Client Network Class : A ]" << endl;
  }
  if(cl_first_octet >= 128 && cl_first_octet <= 191 ){  // Class B Network
     //Getting network applying subnet mask B
    *network = cl_ip_addr & sub_b_mask;
    //Getting host applying reverse subnet mask B
    *host = cl_ip_addr & ~sub_b_mask;
    cout << "[ Client Network Class : B ]" << endl;
  }
  if(cl_first_octet >= 192 && cl_first_octet <= 223 ){  // Class C Network
    //Getting network applying subnet mask C
    *network = cl_ip_addr & sub_c_mask;
    //Getting host applying reverse subnet mask C
    *host = cl_ip_addr & ~sub_c_mask;
    cout << "[ Client Network Class : C ]" << endl;
  }

  printf("[ Client Network Address -> %x ]\n", *network);
  printf("[ Client Host Address -> %x ]\n", *host);
  //Get Server network in 4bytes
  server = getServerNetwork();
  if(isBigEndian()){
    //Check if final analisys brings a local/same network match. Big Endian
    if( *network == LOCAL_NET_ADDR_BE || *network == server){
      flag_return = true;
    }
    else{
      flag_return = false;
    }
  }
  else {
    //Check if final analisys brings a local/same network match. Little Endian
    if( *network == LOCAL_NET_ADDR_LE || *network == server){
      flag_return = true;
    }
    else{
      flag_return = false;
    }
  }
  return flag_return;
}

/***********************************************************************/
uint32_t Server::getServerNetwork() const{
////Function to split Server IP in the Host byte order,...
//... then return in 4bytes the Network address (without host)
//... according to Network Class (A, B, C)
//
  uint32_t server_network;
  unsigned long server_addr;
  uint8_t server_first_octet;
  uint32_t server_ip_addr = ZERO_4BYTES;
  uint32_t sub_a_mask = ZERO_4BYTES;
  uint32_t sub_b_mask = ZERO_4BYTES;
  uint32_t sub_c_mask = ZERO_4BYTES;

  //Getting addr 4 byte from string IP
  server_addr = inet_addr(server_ip);
  if(isBigEndian()){
    // As Big Endian the order remains the same as network byte order
    server_first_octet = uint8_t(server_addr);
    //Setting subnet mask according to endiannes
    sub_a_mask = CLASS_A_SUB_MASK_BE;
    sub_b_mask = CLASS_B_SUB_MASK_BE;
    sub_c_mask = CLASS_C_SUB_MASK_BE;
  }
  else {
    // As Little Endian the order is different to network byte order.
    // So, we shift the right order
    server_first_octet = uint8_t(server_addr & FIRST_BYTE_LE);
    server_ip_addr |= uint32_t(server_addr & FIRST_BYTE_LE) << 24;
    server_ip_addr |= uint32_t(server_addr & SECOND_BYTE_LE) << 8;
    server_ip_addr |= uint32_t(server_addr & THIRD_BYTE_LE) >> 8;
    server_ip_addr |= uint32_t(server_addr & FOURTH_BYTE_LE) >> 24;
    //Setting subnet mask according to endiannes
    sub_a_mask = CLASS_A_SUB_MASK_LE;
    sub_b_mask = CLASS_B_SUB_MASK_LE;
    sub_c_mask = CLASS_C_SUB_MASK_LE;
  }

  printf("[ Server IP Address -> %x ]\n", server_ip_addr);
  //With first octet from IP we match the Network Class
  if(server_first_octet >= 0 && server_first_octet <= 127 ){   //Class A Network
    //Getting network applying subnet mask A
    server_network = server_ip_addr & sub_a_mask;
    cout << "[ Server Network Class : A ]" << endl;
  }
  if(server_first_octet >= 128 && server_first_octet <= 191 ){ //Class B Network
    //Getting network applying subnet mask B
    server_network = server_ip_addr & sub_b_mask;
    cout << "[ Server Network Class : B ]" << endl;
  }
  if(server_first_octet >= 192 && server_first_octet <= 223 ){ //Class C Network
    //Getting network applying subnet mask C
    server_network = server_ip_addr & sub_c_mask;
    cout << "[ Server Network Class : C ]" << endl;
  }

  printf("[ Server Network Address -> %x ]\n", server_network);

  return server_network;
}

/***********************************************************************/
bool Server::isBigEndian(void) const{
//Return the current Arch endianess
//
  int32_t n = 1;
  if(*(int8_t *)&n == 1) {
    //Little endian
    return false;
  }
  //Big endian
  return true;
}
