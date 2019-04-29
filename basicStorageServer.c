#include <stdio.h>;
#include <stdlib.h>;
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <string.h>
#define BLOCK_SIZE 128
#define SERVER_PORT 8000
#define BUFFER_SIZE 1024

//define a struct for the block
struct block {
    char data[BLOCK_SIZE];
};

//define our struct for sockets to use
struct sockaddr {
    //Address Family (I.E AF_INET)
    unsigned short sa_family;
    //Family-Specific Address Info
    char sa_data[14]; 
};

struct in_addr {
    //Internet Address (32-bit)
    unsigned long s_addr; 
};

struct sockaddr_in {
    //Internet Protocol (AF_INET);
    unsigned short sin_family;
    //Address Port (16-bit); 
    unsigned short sin_port;
    //Internet Address (32-bit); 
    struct in_addr sin_addr; 
    //Not used
    char sin_zero[8]; 
};

//get a block pointer
struct block* block_data;
int file_descriptor;
int file_size;
char buffer[BUFFER_SIZE];
char reader[BLOCK_SIZE + 1];


int main(int argc, char* argv[]) {

    //define our variables used in main
    int num_tracks;
    int num_sectors;
    char* fileName;
    char* info = (char * )malloc(sizeof(char) * BUFFER_SIZE);
    int num_blocks;
    int socket_fd;
    int client_fd;
    int num_read = 0;
    struct sockaddr_in server_address, client_address;
    socklen_t socket_length;
    int protocol_cmd;
    int track_num, sector_num, block_write_length, buffer_start = 0;

    //check command line args length
    if (argc != 4) {
        perror("Incorrect number of args provided. Syntax is .\\basicStorageServer <#Tracks> <#Sectors> <Disk File Name>");
        return 1;
    }

    //get the number of tracks and sectors commandline args and strtol, with error handling
    for (int i = 1; i < 3; i++) {
        char *end;
        long repLong = strtol(argv[i], &end, 10);

        if (end == argv[i]) {
            fprintf(stderr, "Arg provided for arg # %d is not a valid decimal number\n", i);
            return 1;
        }
        else if(*end != '\0') {
            fprintf(stderr, "Invalid characters in arg # %d for numPassengers.\n", i);
            return 1;
        }
            
        switch (i)
        {
            case 1:
                num_tracks = (int)repLong;
                break;
            case 2:
                num_sectors = (int)repLong;
                break;             
            default:
                break;
        }
    }

    //get the filename from the commandline args
    fileName = argv[4];

    //calculate the file size
    file_size = num_tracks * num_sectors * BLOCK_SIZE;

    //calculate the number of blocks
    num_blocks = num_tracks * num_sectors;

    //create an endpoint and assign to our socket file descriptor
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) > 0) {
        perror("Error Creating Endpoint.");
        return -1;
    }

    //set our server address properties
    server_address.sin_family = AF_INET; //Set the Internet protocol
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);  //set the address, INADDR_ANY binds the socket to all available interfaces
    server_address.sin_port = htons(SERVER_PORT);

    //bind our socket
    if (bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Error Binding Socket.");
        return -1;
    }

    //place the socket in a state where it is listening for a connection
    listen(socket_fd, 5);


    //create an infinite loop to wait for a connection
    while(1) {
        //print that we are waiting for a connectin
        printf("Waiting for a connection ...");
        //assign the socket_length
        socket_length = sizeof(client_address);
        //accept the connection and assign to the client file descriptor
        if ((client_fd = accept(socket_fd, (struct sockaddr*)&client_address, &socket_length)) > 0) {
            perror("Client FD Error");
            continue;
        }
    

        //create an infinite loop to gather client commands
        while(1) {
            num_read = (client_fd, buffer, BUFFER_SIZE);

            //check to see if the number read from the client into the buffer is equal to 0
            if (num_read == 0) {
                printf("Closing Client...");
                break;
            }

            //set the null byte at the end of the buffer
            buffer[num_read] = '\0';

            //scan in the request
            sscanf(buffer,"%c %d %d %d %n", &protocol_cmd, &track_num, &sector_num, &block_write_length, &buffer_start);

            //print out the request received
            printf("\nRecieved Request: %s\n", buffer);

            //convert to the upper variant of the command character recieved
            protocol_cmd = toupper(protocol_cmd);
        }

        //close the client file descriptor
        close(client_fd);
    }

    free(info);

}


int createDiskFile(char* fileName) {
    //define our result variable
    int result;

    //create our file descriptor and use O_RDRW so that it can be read and written to
    // use O_CREAT to create the file if it does not exist
    // mode_t 0600 allows the owner to read and write
    file_descriptor = open(fileName, O_RDWR | O_CREAT, (mode_t) 0600);

    //catch the error description for the file descriptor
    if (file_descriptor == -1) {
        perror("Issue Creating Disk File.");
        return -1;
    }

    //set the result the the lseek result
    //we are spanning the file to the size of the disk
    if (lseek(file_descriptor, file_size, SEEK_SET) == -1) { 
        close(file_descriptor);
        perror("Error spanning the file to the correct size.");
        return -1;
    }

    //fill out a last byte in the file
    lseek(file_descriptor, 0, SEEK_SET);
}
