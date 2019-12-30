CC = gcc
LIBS = -pthread
CFLAGS = -g -Wall

server: message.o
	$(CC) $(CFLAGS) $(LIBS) server.c message.o -o server

client: message.o
	$(CC) $(CFLAGS) $(LIBS) client.c message.o -o client

message.o: message.c message.h
	$(CC) $(CFLAGS)  -c message.c

all: clean client server

clean:
	$(RM) server client *.o


# all: client server
# clean:
# 	$(RM) 






# 	all: httpd client
# # LIBS = -lpthread #-lsocket
# # For GNU Linux， use -pthread instead of -lpthread
# # https://computing.llnl.gov/tutorials/pthreads/#Compiling
# LIBS = -pthread
# httpd: httpd.c
# 	gcc -g -W -Wall $(LIBS) -o $@ $<

# client: simpleclient.c
# 	gcc -W -Wall -o $@ $<
# clean:
# 	rm httpd
