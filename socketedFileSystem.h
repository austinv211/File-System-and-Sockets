#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <arpa/inet.h>
#include <string.h>
#include <sys/mman.h>
#include <netdb.h>
#include <ctype.h>
#include "disk.h"
#define SERVER_PORT 55000
#define BUFFER_SIZE 1024

//define a struct for the block
struct block {
    char data[BLOCK_SIZE];
};