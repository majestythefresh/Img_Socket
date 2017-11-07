/***********************************************************************
* FILENAME    :   img_types.h
* AUTHOR      :   Arturo Plauchu (plauchu@gmail.com)
* DATE        :   01 Nov 2017
* DESCRIPTION :   Library to set common types and macros.
* LICENSE     :   GPL v2.0
*
***********************************************************************/
#ifndef IMG_TYPES_H
#define IMG_TYPES_H

#define ACK_MSG    "ACK"
#define NO_ACK_MSG "No ACK"
#define ACK_BUF    6

#define CLASS_A_SUB_MASK_LE  0xFF000000
#define CLASS_B_SUB_MASK_LE  0xFFFF0000
#define CLASS_C_SUB_MASK_LE  0xFFFFFF00
#define CLASS_A_SUB_MASK_BE  0x000000FF
#define CLASS_B_SUB_MASK_BE  0x0000FFFF
#define CLASS_C_SUB_MASK_BE  0x00FFFFFF

#define LOCAL_NET_IP_LE      0x7F000001
#define LOCAL_NET_IP_BE      0x0100007F
#define LOCAL_NET_ADDR_LE    0x7F000000
#define LOCAL_NET_ADDR_BE    0x0000007F

#define ZERO_4BYTES          0x00000000
#define FIRST_BYTE_LE        0xFF
#define SECOND_BYTE_LE       0xFF00
#define THIRD_BYTE_LE        0xFF0000
#define FOURTH_BYTE_LE       0xFF000000

//start of JPEG image data section
#define IMAGE_DATA_OFFSET 20
//Raw Exif Header offset
#define EXIF_HEADER_OFFSET { 0xff, 0xd8, 0xff, 0xe1 }
// special header required for EXIF_TAG_USER_COMMENT
#define ASCII_COMMENT "ASCII\0\0\0"
// comment to write into the EXIF block
#define FILE_COMMENT "source_ip:<%s>,  is_from_local_network:<%s>"
#define FILE_COMMENT_LEN 128

//Error types
enum Errors {
  NO_ERROR = 0,
  SOCK_ERROR,
  CON_ERROR,
  ACCEPT_ERROR,
  ACCESS_ERROR,
  SEND_ERROR
};

#endif
