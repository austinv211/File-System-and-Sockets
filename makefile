all: clientA.c clientB.c basicStorageServer.c
	gcc clientA.c -o ClientA
	gcc clientB.c -o ClientB
	gcc basicStorageServer.c -o BasicServer