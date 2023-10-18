TCP_PRIMARY         = tcp_primary
TCP_SECONDARY		= tcp_secondary

CC                  = gcc
CFLAGS              = -g -Wall -g
LDFLAGS             = -lpthread
INCLUDE             = -I./include

$(TCP_PRIMARY): tcp_primary_server.o tcp_connection_manager.o time_manager.o background.o txlog_manager.o
	$(CC) -o bin/$@ $^ $(CFLAGS) $(LDFLAGS) $(INCLUDE)

tcp_primary_server.o: src/tcp_primary_server.c include/server.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c $^

$(TCP_SECONDARY): tcp_secondary_server.o tcp_connection_manager.o time_manager.o background.o txlog_manager.o
	$(CC) -o bin/$@ $^ $(CFLAGS) $(LDFLAGS) $(INCLUDE)

tcp_secondary_server.o: src/tcp_secondary_server.c include/server.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c $^

tcp_connection_manager.o: src/tcp_connection_manager.c include/tcp_connection_manager.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c $^

time_manager.o: src/time_manager.c include/time_manager.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c $^

background.o: src/background.c include/background.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c $^

txlog_manager.o: src/txlog_manager.c include/txlog_manager.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c $^


clean:
	rm -rf *.o *.out bin/*

.PHONY: clean