all: httpd client
# LIBS = -lpthread #-lsocket
# For GNU Linux， use -pthread instead of -lpthread
# https://computing.llnl.gov/tutorials/pthreads/#Compiling
LIBS = -pthread
httpd: httpd.c
	gcc -g -W -Wall $(LIBS) -o $@ $<

client: simpleclient.c
	gcc -W -Wall -o $@ $<
clean:
	rm httpd
