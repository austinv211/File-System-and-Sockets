all: clientA.c clientB.c basicStorageServer.c server_cpp.cpp SC.cpp lss.cpp lsc.cpp
	gcc clientA.c -o Problem3ClientA
	gcc clientB.c -o Problem3ClientB
	gcc basicStorageServer.c -o Problem3Server
	g++ server_cpp.cpp -o Problem1Server -pthread
	g++ SC.cpp -o Problem1Client
	g++ lss.cpp -o LsServer -pthread
	g++ lsc.cpp -o LsClient