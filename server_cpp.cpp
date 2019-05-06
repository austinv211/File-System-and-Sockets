#include<iostream>
#include<sys/types.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<string.h>
#include<string>
#include<pthread.h>

using namespace std;

//Global(s)
int PORT = 4450;
const int MAX = 3;

void *serverThread(void *arg)
{
    //declarations
    char buffer1[500];
    char buffer2[500];
    int newSocket = *((int*) arg);
    int bytesRcv;// = recv(newSocket, buffer1, 500, 0);

    //While recieving
    while(bytesRcv = recv(newSocket, buffer1, 500, 0) > 0)
    {
        if(bytesRcv == -1)
        {
            cerr << "There was a connection issue\n";
            break;
        }

        if(bytesRcv == 0)
        {
            cout << "The client has disconnected\n";
            break;
        }

        //Display message
        cout << "Received: " << buffer1 << endl;
        int counter = 0;

        //Reverse buffer
        for(int i = strlen(buffer1) - 1; i >= 0; i--)
        {
            buffer2[counter] = buffer1[i];
            counter++;
        }

        //echo message back to client
        write(newSocket, buffer2, strlen(buffer2));

        //clear the buffer
        memset(buffer1, 0, 500);
        memset(buffer2, 0, 500);
    }
}

int main()
{
    //Delaration(s)
    int serverSock,
        newSock,
        ret;
    struct sockaddr_in serverAddr,
                       newAddr;
    socklen_t addrSize;

    //Create Socket
    serverSock = socket(PF_INET, SOCK_STREAM, 0);
    if(serverSock < 0)
    {
        cerr << "Cannot create socket...\n";
        return -1;
    }

    cout << "Socket Created..\n";
    
    //initialize serverAddr struct
    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //bind servAddr to socket
    ret = bind(serverSock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if(ret < 0)
    {
        cerr << "Error in binding....\n";
        return -2;
    }

    //Listen
    if(listen(serverSock, 3) == 0)
    {    cout << "Listening...\n"; }
    else
    {   cerr << "error in binding...\n"; }

    pthread_t tid;
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char cli[NI_MAXSERV];
    
	
    pthread_t pids[2];
    int ids = 0;
    while (true)
    {
        if(newSock = accept(serverSock, (struct sockaddr*)&newAddr, &clientSize))
        {
            if(newSock == -1)
            {
                cerr<< "a client failed to connect...\n";
            }

            cout << "accepted client...\n";
            

            int condi = pthread_create(&pids[ids++], NULL, serverThread, &newSock);
            if(condi)
            {
                cerr << "Thread Failed\n";
                return -3;
            }

            if(ids >= 2)
           {
                ids = 0;
                while(ids > 0)
                {
                    pthread_join(pids[ids++], NULL);
                }
               ids = 0;
            }
        }
    }
    return 0;
}