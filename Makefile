TCP_PRIMARY = tcp_primary
TCP_SECONDARY = tcp_secondary

CC                  = gcc
CFLAGS              = -g -Wall -g
LDFLAGS             = -lpthread
INCLUDE       		= -I./bench/include

$(TCP_PRIMARY): tcp_primary_server.o tcp_connection_manager.o time_manager.o background.o txlog_manager.o message.o log_manager.o
	$(CC) -o bin/$@ $^ $(CFLAGS) $(LDFLAGS) $(INCLUDE)

tcp_primary_server.o: bench/src/tcp_primary_server.c bench/include/server.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c $<

$(TCP_SECONDARY): tcp_secondary_server.o tcp_connection_manager.o time_manager.o background.o txlog_manager.o message.o log_manager.o
	$(CC) -o bin/$@ $^ $(CFLAGS) $(LDFLAGS) $(INCLUDE)

tcp_secondary_server.o: bench/src/tcp_secondary_server.c bench/include/server.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c $<

tcp_connection_manager.o: bench/src/tcp_connection_manager.c bench/include/tcp_connection_manager.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c $<

time_manager.o: bench/src/time_manager.c bench/include/time_manager.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c $<

background.o: bench/src/background.c bench/include/background.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c $<

txlog_manager.o: bench/src/txlog_manager.c bench/include/txlog_manager.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c $<

test_txlog_manager: bench/test/test_txlog_manager.c bench/txlog_manager.o time_manager.o
	$(CC) -o bin/$@ $^ $(CFLAGS) $(LDFLAGS) $(INCLUDE)
	./bin/$@

message.o: bench/src/message.c bench/include/message.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c $<

log_manager.o: bench/src/log_manager.c bench/include/log_manager.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c $<


clean:
	rm -rf *.o *.out bin/* *.log *.bin

.PHONY: clean