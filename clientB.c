#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h> 
#include <time.h>
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
    int num_random;
    char* cmd_string;
    int num_cylinders;
    int num_sectors;
    int size;

    //call srand
    srand(time(NULL));

    //check the commandline args
    if (argc != 3) {
        perror("Error Invlaid Number of Command Line Args Provided.");
        return -1;
    }

    //get the number of random commands to perform
    char *end;
    long repLong = strtol(argv[1], &end, 10);

    //check to see if the arg provided is a valid decimal
    if (end == argv[1]) {
        fprintf(stderr, "Arg provided for arg # 1 is not a valid decimal number\n");
        return -1;
    }
    else if(*end != '\0') {
        fprintf(stderr, "Invalid characters in arg # 1.\n");
        return -1;
    }

    //set num_random
    num_random = (int)repLong;

    printf("Num Random: %d\n", num_random);

    //get the ip from the arguments
    ip_address = argv[2];

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

    //set the adress family and port number
	server_address.sin_family = AF_INET; 
	server_address.sin_port = htons(SERVER_PORT);

	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, ip_address, &server_address.sin_addr) <= 0) 
	{ 
		printf("\nInvalid address / Address not supported \n"); 
		return -1; 
	} 

    if (connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 

    for (int i = 0; i <= num_random; i++) {

        //generata random command string
        if (i == 0) {
            cmd_string = "i";
        }
        else {
            char protocol_cmd;
            int cmd_random = (rand() % (2));
            //get the cylinder and sector number
            int random_cylinder = rand() % (num_cylinders - 1) + 1;
            int random_sector = rand() % (num_sectors - 1) + 1;

            if (cmd_random == 0 ) {

                printf("\nSending Write Command\n");

                //set the w command
                protocol_cmd = 'w';

                //we need to generate a random sequence of chars
                char randomData[128];

                for(int j = 0; j < 128; j++) {
                    char random_char = (char)(rand()%(90- 65 + 1)) + 65;
                    randomData[j] = random_char;
                }

                size = asprintf(&cmd_string, "%c %d %d 128 %s\n", protocol_cmd, random_cylinder, random_sector, randomData);
            }
            else {
                printf("\nSending Read Command: (%d, %d)\n", random_cylinder, random_sector);

                protocol_cmd = 'r';

                size = asprintf(&cmd_string, "%c %d %d\n", protocol_cmd, random_cylinder, random_sector);
            }

            printf("%s", cmd_string);

        }

        //send the command to the server
        send(socket_fd, cmd_string, strlen(cmd_string), 0);

        //read the server response
        if ((readResponse(socket_fd)) == -1) {
            break;
        }

        //declare variables to store the results of the info return
        if (cmd_string[0] == 'i' || cmd_string[0] == 'I') {
            //get the data from the response
            sscanf(read_data,"%d %d", &num_cylinders, &num_sectors);
            
            //print the response to the screen
            printf("Number of Cylinders on Disk: %d\n", num_cylinders);
            printf("Number of Sectors per Cylinder on Disk: %d\n", num_sectors);
        }
        else {
            printf("Response: %s\n", read_data);
        }

        //use memset to reset the buffer
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