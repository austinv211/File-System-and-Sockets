#include "socketedFileSystem.h"

//get a block pointer
struct block* block_data;
int file_descriptor;
int file_size;
char buffer[BUFFER_SIZE];
char reader[BLOCK_SIZE + 2];
int isConnected = 1;

int createDiskFile(char* fileName);

int main(int argc, char* argv[]) {

    //define our variables used in main
    char* fileName;
    int server_fd;
    int client_fd;
    struct sockaddr_in address;
    int socket_length;
    char protocol_cmd;
    int cylinder_num, sector_num = 0;
    int num_read;
    char *errorLine = "Invalid Syntax";
    char* sendData;
    char* writeData;

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
                num_cylinders = (int)repLong;
                break;
            case 2:
                num_sectors = (int)repLong;
                break;             
            default:
                break;
        }
    }

    //get the filename from the commandline args
    fileName = argv[3];

    //create the diskfile
    create_disk(fileName);

    printf("\nNum Cylinders: %d\n", num_cylinders);
    printf("Num Blocks: %d\n", disk_size());
    printf("File Size: %d\n", file_size);

    //get the file descriptor representation of the file
    //file_descriptor = get_disk_file();


    //create an endpoint and assign to our socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		return -1;
	} 

    //set our server address properties
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons(SERVER_PORT); 

	// bind our socket
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
	{ 
		perror("bind failed"); 
		return -1;
	} 

    //place the socket in a state where it is listening for a connection
	if (listen(server_fd, 3) < 0) 
	{ 
		perror("listen"); 
		return -1;
	}

    //create an infinite loop to wait for a connection
    while(isConnected) {
        //print that we are waiting for a connectin
        printf("\nWaiting for a connection ...\n");

        //assign the socket_length
        socket_length = sizeof(address);

        if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&socket_length))<0) 
        { 
            perror("accept"); 
            return -1;
        } 

        //print the sucess result
        printf("Accepting Connection Successful..\n");
    
        //create an infinite loop to gather client commands
        while(isConnected) {
            //read from the client
            num_read = read(client_fd, buffer, BUFFER_SIZE);

            //check to see if the number read from the client into the buffer is equal to 0
            if (num_read == 0) {
                printf("Closing Client...\n");
                break;
            }

            //scan in the request
            int buffer_count_received = strlen(buffer);
            char delim[] =  " ";
            char* ptr = strtok(buffer, delim);

            int counter = 1;
            int isDone = 0;

            while (ptr != NULL) {
                switch (counter) {
                    case 1:
                        protocol_cmd = ptr[0];
                        break;
                    case 2:
                        cylinder_num = (int) strtol(ptr, (char **)NULL, 10);
                        break;
                    case 3:
                        sector_num = (int) strtol(ptr, (char **)NULL, 10);
                        break;
                    case 4:
                        writeData = ptr;
                        break;
                    default:
                        isDone = 1;
                        break;
                }

                counter++;

                ptr = strtok(NULL, delim);

                if (isDone) {
                    break;
                }        
            }

            //print out the request received
            printf("\nReceived Protocol: %c\n", protocol_cmd);

            //convert to the upper variant of the command character recieved
            protocol_cmd = toupper(protocol_cmd);

            switch (protocol_cmd)
            {
                /*
                I is an information request. The disk returns two integers representing the disk geometry:
                the number of cylinders, and the number of sectors per cylinder
                */
                case 'I':
                    asprintf(&sendData, "%d %d", num_cylinders, num_sectors);
                    send(client_fd, sendData, strlen(sendData), 0);
                    printf("Sent Disk Info\n");
                    break;
                /*
                R is a read request for the contents of the cylinder c sector s. The disk returns 'I' followed by those
                128 bytes of information, or '0' if no such block exists. (This will return whatever data happens
                to be on the disk in a given sector, even if nothing has ever been explicitly written there before)
                */
                case 'R':
                    //get the data from the block
                    disk_read(cylinder_num, sector_num, reader);
                    if(strlen(reader) == 0) {
                        send(client_fd, "0", 1, 0);
                    }
                    else {
                        send(client_fd, reader, sizeof(reader), 0);
                    }
                    printf("Sent Read Data\n");
                    break;
                case 'W':
                    disk_write(cylinder_num, sector_num, writeData);
                    asprintf(&sendData, "Wrote to: (%d, %d)", num_cylinders, num_sectors);
                    send(client_fd, sendData, strlen(sendData), 0);
                    printf("Sent Write Data\n");
                    break;
                default:
                    send(client_fd, errorLine, strlen(errorLine), 0);
                    printf("Sent Error Line\n");
                    break;
            }
        }

        //close the client file descriptor
        close(client_fd);
    }

    // free(info);

}
