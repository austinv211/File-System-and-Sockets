#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h> 
#define SERVER_PORT 55000
#define BUFFER_SIZE 1024

char read_data[BUFFER_SIZE] = {0};

//prototype the function for reading socket responses
int readResponse(int socket_fd);

int main(int argc, char* argv[]) {
    //variables used in client
    char* ip_address; //the ip address of the server
    struct sockaddr_in server_address;
    struct hostent *host;
    int socket_fd;
    char cmd_string[BUFFER_SIZE];

    //check the commandline args
    if (argc != 2) {
        perror("Error Invlaid Number of Command Line Args Provided.");
        return -1;
    }

    //get the ip from the arguments
    ip_address = argv[1];

    //check that the ip address is not null
    if (ip_address == NULL) {
        perror("Error Getting Ip Address");
        return -1;
    }

    /* create endpoint */
	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

    //use memset to set the server address to '0's
	memset(&server_address, '\0', sizeof(server_address)); 

	server_address.sin_family = AF_INET; 
	server_address.sin_port = htons(SERVER_PORT);

	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

    if (connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 

    //create a infinite loop to gather user input
    printf("\nClientA:Basic-File-System$ ");

    while (fgets(cmd_string, BUFFER_SIZE, stdin) != NULL) {

        //compare the string to exit to see if we should exit the client (case-insensitive)
        if (strncasecmp(cmd_string, "exit", 4) == 0) {
            printf("Exiting the Client\n");
            break;
        }

        //send the command to the server
        send(socket_fd, cmd_string, sizeof(cmd_string), 0);

        //read the server response
        if ((readResponse(socket_fd)) == -1) {
            break;
        }

        printf("Response: %s\n", read_data);

        //declare variables to store the results of the info return
        if (cmd_string[0] == 'i' || cmd_string[0] == 'I') {
            int num_cylinders;
            int num_sectors;

            //get the data from the response
            sscanf(read_data,"%d %d", &num_cylinders, &num_sectors);
            
            //print the response to the screen
            printf("Number of Cylinders on Disk: %d\n", num_cylinders);
            printf("Number of Sectors per Cylinder on Disk: %d\n", num_sectors);
        }

        //print enter a command again
        printf("\nClientA:Basic-File-System$ ");

        //use memset to reset the buffer and cmd_str
        memset(cmd_string, '\0', strlen(cmd_string));
        memset(read_data, '\0', strlen(read_data));
    }

    //close the socket file descriptor
    close(socket_fd);

    //return 0 for all is well
    return 0;
}

//function to read the server response into the buffer
int readResponse(int socket_fd) {
    //set an int for the num_read int result of reading from the socket
    int num_read = 0;

    //the read and count the number read
    num_read = read(socket_fd, read_data, BUFFER_SIZE);

    if (num_read == -1) {
        perror("Error Reading from client");
    }

    //return num_read
    return num_read;

}