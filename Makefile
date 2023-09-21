TCP_PRIMARY         = tcp_primary
TCP_SECONDARY		= tcp_secondary

CC                  = gcc
CFLAGS              = -g -Wall
LDFLAGS             = -lpthread
INCLUDE             = -I./include

$(TCP_PRIMARY): tcp_primary_server.o tcp_connection_manager.o time_manager.o background.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(INCLUDE)

tcp_primary_server.o: src/tcp_primary_server.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c $^

$(TCP_SECONDARY): tcp_secondary_server.o tcp_connection_manager.o time_manager.o background.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(INCLUDE)

tcp_secondary_server.o: src/tcp_secondary_server.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c $^

tcp_connection_manager.o: src/tcp_connection_manager.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c $^

time_manager.o: src/time_manager.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c $^

background.o: src/background.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c $^


clean:
	rm -rf *.o *.out

.PHONY: clean