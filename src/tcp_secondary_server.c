#include "server.h"
#include "tcp_connection_manager.h"

static void print_secondary_srv_config(server_config_t *srv_config);
static void usage();
static server_config_t *parse_srv_config(int argc, char *argv[]);

static void print_secondary_srv_config(server_config_t *srv_config)
{
    fprintf(stdout, "---------server_config---------\n");
    fprintf(stdout, "server id: %zu\n", srv_config->srv_id);
    fprintf(stdout, "ip address: %s\n", srv_config->ipaddr);
    fprintf(stdout, "port: %d\n", srv_config->port);
    fprintf(stdout, "run duration: %zu\n", srv_config->duration);
    fprintf(stdout, "-------------------------------\n");
}

static void usage() {
    fprintf(stdout, "Usage: \n");
    fprintf(stdout, "   - Secondary:\n");
    fprintf(stdout, "   ./tcp_secondary <server ID> <address> <port> <duration>\n");
    exit(0);
}

static server_config_t *parse_srv_config(int argc, char *argv[])
{
    // 0      1           2        3      4
    // <exec> <server ID> <ipaddr> <port> <duration>

    if (argc != 5) usage();

    size_t srv_id = atoi(argv[1]);
    if (srv_id < 2) {
        fprintf(stdout, "secondary server ID should be greater than 1.\n");
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

    server_config_t *srv_config = malloc(sizeof(server_config_t));
    srv_config->srv_id = srv_id;
    strncpy(srv_config->ipaddr, ipaddr, strlen(ipaddr));
    srv_config->ipaddr[strlen(ipaddr)] = '\0';
    srv_config->port = port;
    srv_config->duration = duration;

    return srv_config;
}

int main(int argc, char *argv[]) {
    server_config_t *srv_config = parse_srv_config(argc, argv);
    print_secondary_srv_config(srv_config);

    /**** tx log mamanger起動 ****/
    txlm_config_t *txlm_config = txlm_init(srv_config->srv_id);
    /****************************/

    /**** connection manager ****/
    reciever_main(srv_config, txlm_config);
    /****************************/

    return 0;
}