#include "server.h"
#include "background.h"
#include "tcp_connection_manager.h"
#include "time_manager.h"

static void print_primary_srv_config(server_config_t *srv_config);
static void usage();
static server_config_t *parse_srv_config(int argc, char *argv[]);

static void print_primary_srv_config(server_config_t *srv_config)
{
    fprintf(stdout, "---------server_config---------\n");
    fprintf(stdout, "server id: %zu\n", srv_config->srv_id);
    fprintf(stdout, "ip address: %s\n", srv_config->ipaddr);
    fprintf(stdout, "port: %d\n", srv_config->port);
    fprintf(stdout, "run duration: %zu\n", srv_config->duration);
    fprintf(stdout, "traffic: %zu [ops/sec]\n", srv_config->iops);
    fprintf(stdout, "number of client threads: %zu\n", srv_config->num_threads);
    fprintf(stdout, "send log size: %zu [bytes]\n", srv_config->send_log_size);
    fprintf(stdout, "number of secondary servers: %zu\n", srv_config->num_secondary_servers);
    fprintf(stdout, "number of servers: %zu\n", srv_config->num_servers);
    for (int id = 2; id <= srv_config->num_servers; ++id) {
        fprintf(stdout, "server id: %d, ip address: %s, port: %d\n", id, srv_config->srvs_ipaddr[id], srv_config->srvs_port[id]);
    }
    fprintf(stdout, "-------------------------------\n");
}

static void usage() {
    fprintf(stdout, "Usage: \n");
    fprintf(stdout, "   - Primary:\n");
    fprintf(stdout, "   ./tcp_primary 1 <address> <port> <duration>\n");
    fprintf(stdout, "       <IOPS> <threads> <send log size> <secondary servers>\n");
    fprintf(stdout, "       <server2 addr> <server2 port> <server3 addr> <server3 port> ...\n");
    exit(0);
}

static server_config_t *parse_srv_config(int argc, char *argv[])
{
    // 0      1           2        3      4
    // <exec> <server ID> <ipaddr> <port> <duration>
    // 5      6                7               8
    // <IOPS> <num of threads> <send log size> <num of server>
    // 9           10        11          12
    // <S2 ipaddr> <S2 port> <S3 ipaddr> <S3 port> ...

    if (argc < 9) {
        fprintf(stderr, "Error: fewer than 8 arguments.\n");
        usage();
    }

    size_t srv_id = atoi(argv[1]);
    if (srv_id != 1) {
        fprintf(stdout, "primary server ID should be 1.\n");
        exit(0);
    }

    char ipaddr[32];
    strncpy(ipaddr, argv[2], strlen(argv[2]));
    ipaddr[strlen(argv[2])] = '\0';

    unsigned short port = atoi(argv[3]);

    size_t duration = atoi(argv[4]);
    if (duration < 1) {
        fprintf(stdout, "duration should be greater than zero.\n");
        exit(0);
    }

    size_t iops = atoi(argv[5]);
    if (iops < 1) {
        fprintf(stdout, "valid IOPS range: 1-[].\n");
        exit(0);
    }

    size_t num_threads = atoi(argv[6]);
    if (num_threads < 1 || num_threads > MAX_NUM_CLIENT_THREADS) {
        fprintf(stdout, "valid threads number range: 1-%d.\n", MAX_NUM_CLIENT_THREADS);
        exit(0);
    }

    size_t send_log_size = atoi(argv[7]);
    if (send_log_size < 1) {
        fprintf(stdout, "valid send log size range: 1-[].\n");
        exit(0);
    }

    size_t num_secondary_servers = atoi(argv[8]);
    if (num_secondary_servers < 1 || num_secondary_servers > MAX_NUM_SECONDARY_SERVERS) {
        fprintf(stdout, "valid send log size range: 1-%d.\n", MAX_NUM_SECONDARY_SERVERS);
        exit(0);
    }

    if (argc - 9 != num_secondary_servers * 2) {
        fprintf(stdout, "Error: Fewer IP address and port arguments for other servers.\n");
        fprintf(stdout, "   Since number of secondary servers is %zd, expected %zd, but actually %d.\n", num_secondary_servers, num_secondary_servers*2, argc-9);
        exit(0);
    }

    server_config_t *srv_config = malloc(sizeof(server_config_t));
    srv_config->srv_id = srv_id;
    strncpy(srv_config->ipaddr, ipaddr, strlen(ipaddr));
    srv_config->ipaddr[strlen(ipaddr)] = '\0';
    srv_config->port = port;
    srv_config->duration = duration;
    srv_config->iops = iops;
    srv_config->num_threads = num_threads;
    srv_config->send_log_size = send_log_size;
    srv_config->num_secondary_servers = num_secondary_servers;
    srv_config->num_servers = num_secondary_servers+1;

    for (int id = 2; id <= srv_config->num_servers; ++id) {
        int srvs_ipaddr_index = 9 + (id-2)*2;
        int srvs_port_index = srvs_ipaddr_index+1;
        strncpy(srv_config->srvs_ipaddr[id], argv[srvs_ipaddr_index], strlen(argv[srvs_ipaddr_index]));
        srv_config->srvs_ipaddr[id][strlen(argv[srvs_ipaddr_index])] = '\0';
        srv_config->srvs_port[id] = atoi(argv[srvs_port_index]);
    }

    return srv_config;
}

int main(int argc, char *argv[]) {
    server_config_t *srv_config = parse_srv_config(argc, argv);
    print_primary_srv_config(srv_config);

    /**** log mamanger起動 ****/

    /*************************/

    /**** tx log mamanger起動 ****/

    /****************************/

    /**** connection manager ****/
    sender_main(srv_config);
    /****************************/

    /***** background *****/
    background(srv_config);
    /**********************/

    free(srv_config);
    return 0;
}