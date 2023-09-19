#include "server.h"

static int server_main(srv_config *srv_config)
{

}

static void print_srv_config(srv_config *srv_config)
{
    fprintf(stdout, "---------\n");
    fprintf(stdout, "server id: %zu\n", srv_config->srv_id);
    fprintf(stdout, "run duration: %zu\n", srv_config->duration);
    if (srv_config->srv_id == 1) {
        fprintf(stdout, "traffic: %zu ops/secf\n", srv_config->iops);
        fprintf(stdout, "client threads: %zu\n", srv_config->num_threads);
        fprintf(stdout, "send log size: %zu bytes\n", srv_config->log_size);
    }
    fprintf(stdout, "---------\n");
}

static usage() {
    fprintf(stdout,
            "Usage: \n
                - Primary:\n
                server 1 <address> <port>\n
                    <duration> <IOPS> <threads> <log size>\n
                    <server2 addr> <server2 port> <server3 addr> <server3 port> ...\n\n
                    
                - Secondary:\n
                server <server ID> <address> <port>\n");
    exit(0);
}

static srv_config *parse_srv_config(int argc, char *argv[])
{
    // 0      1           2        3      4
    // <exec> <server ID> <ipaddr> <port> <duration>
    // 5      6                7
    // <IOPS> <num of threads> <send log size>
    // 8           9         10          11
    // <S2 ipaddr> <S2 port> <S3 ipaddr> <S3 port> ...

    if (argc < 5) usage();

    size_t srv_id = atoi(argv[1]);
     if (srv_id < 1) {
        fprintf(stdout, "server ID should be greater than zero.");
        exit(0);
    }

    char ipaddr[16];

    unsigned short port = atoi(argv[3]);

    size_t duration = atoi(argv[4]);
    if (duration < 1) {
        fprintf(stdout, "duration should be greater than zero.");
        exit(0);
    }

    if (srv_id > 1) {
        // Secondary, not Primary
        srv_config *srv_config = malloc()
    }
}

int main(int argc, char *argv[]) {
    srv_config *srv_config = parse_srv_config(argc, argv);

    return 0;
}