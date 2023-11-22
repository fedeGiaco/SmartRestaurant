# Federico Giacomelli, matricola 578016, progetto di Reti Informatiche 2022/23
# FILE: makefile

all: cli td kd server

# File principali
server: server.o function/global.c flow.o server_liste.o server_td.o server_cli.o server_kd.o server_debug.o server_utility.o
	gcc -Wall function/global.c flow.o server_liste.o server_td.o server_cli.o server_kd.o server_debug.o server_utility.o server.o -o server

cli: cli.o flow.o
	gcc -Wall cli.o flow.o -o cli

td: td.o flow.o
	gcc -Wall td.o flow.o -o td

kd: kd.o flow.o
	gcc -Wall kd.o flow.o -o kd

	touch storage/book.txt 
	touch storage/book_attive.txt
	touch storage/logServer.txt

# Funzioni
flow.o: function/flow.c header/flow.h
	gcc -Wall -g -c function/flow.c -o flow.o

server_liste.o: function/server_liste.c header/server_liste.h
	gcc -Wall -g -c function/server_liste.c -o server_liste.o

server_td.o: function/server_td.c header/server_td.h
	gcc -Wall -g -c function/server_td.c -o server_td.o

server_cli.o: function/server_cli.c header/server_cli.h
	gcc -Wall -g -c function/server_cli.c -o server_cli.o

server_kd.o: function/server_kd.c header/server_kd.h
	gcc -Wall -g -c function/server_kd.c -o server_kd.o

server_debug.o: function/server_debug.c header/server_debug.h
	gcc -Wall -g -c function/server_debug.c -o server_debug.o

server_utility.o: function/server_utility.c header/server_utility.h
	gcc -Wall -g -c function/server_utility.c -o server_utility.o

clean:
	rm *.o cli td kd server;  rm storage/book*.txt;  rm storage/logServer.txt
