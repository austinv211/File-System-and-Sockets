#include<iostream>
#include<sys/types.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<string.h>
#include<string>

using namespace std;

int main()
{
    //Create Socket
    int mySocket = socket(AF_INET, SOCK_STREAM, 0);
    
    if(mySocket == -1)
        return 1;

    //Info for connecting to server?
    int port = 4450;

    //accessKey scruct
    sockaddr_in accessKey;
    accessKey.sin_family = AF_INET;
    accessKey.sin_port = htons(port); 
    inet_pton(AF_INET, "127.0.0.1", &accessKey.sin_addr);

    //connect to the server
    int connectResponse = connect(mySocket, (sockaddr*)&accessKey, sizeof(accessKey));
    if(connectResponse == -1)
    {
        cout << "failed to connect to server\n";
        return 1;
    }
    

    char buffer[500];
    string userInput;
    do{
        //Enter lines of text
        cout << "Input: ";
        getline(cin, userInput);

        //Check for send message
        int sendResposnse = send(mySocket, userInput.c_str(), userInput.size() + 1, 0);
        if(sendResposnse == -1){
            cout << "Could not connect to server...";
            continue;
        }

        //wait for response
        memset(buffer, 0, 500);
        int bytesRecieved = recv(mySocket, buffer, 500, 0);

        if(bytesRecieved == -1){
            cout <<"There wsa an error getting response from server\n";
        }
        else{
            cout << "Server: " << string(buffer, bytesRecieved) << "\r\n";}

    }while(true);

    close(mySocket);
    return 0;
}