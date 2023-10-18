#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "tcp_connection_manager.h"
#include "server.h"

struct sender_worker_thread_info_t
{
    server_config_t *srv_config;
    txlm_config_t *txlm_config;
    size_t target_id;
    int sd;
};
typedef struct sender_worker_thread_info_t sender_worker_thread_info_t;

static void sender_worker(sender_worker_thread_info_t *worker_info)
{
    int ret;
    server_config_t *srv_config = worker_info->srv_config;
    txlm_config_t *txlm_config = worker_info->txlm_config;
    size_t target_id = worker_info->target_id;

    int sd = worker_info->sd;
    char buff[MAX_SEND_DATA_SIZE];
    size_t msg_len;
    unsigned int next_lsn = TXLOG_MIN_LSN; // 未送信LSNの最小値（送信済みLSNの最大値 + 1）

    fprintf(stdout, "sender thread id:%zu, ipaddr:%s, port:%d\n",
            target_id,
            srv_config->srvs_ipaddr[target_id],
            srv_config->srvs_port[target_id]);

    while (1)
    {
        if (txlm_get_current_lsn(txlm_config) >= next_lsn) {
            msg_len = txlm_read_header(txlm_config, buff, next_lsn);

            ret = send(sd, buff, msg_len, 0);
            if (ret != msg_len) {
                fprintf(stderr, "tcp_connection_manager.c: (line:%d) %s", __LINE__, strerror(errno));
            }
            fprintf(stdout, "[send] log data\n");

            msg_len = recv(sd, buff, MAX_SEND_DATA_SIZE, 0);
            if (msg_len < 0) {
                fprintf(stderr, "tcp_connection_manager.c: (line:%d) %s", __LINE__, strerror(errno));
            }
            buff[msg_len] = '\0';
            fprintf(stdout, "[reciev] %s\n", buff);

            next_lsn++;
        }
    }
}

void sender_main(server_config_t *srv_config, txlm_config_t *txlm_config)
{
    // 各secondaryサーバごとにsender_workerを生成
    for (size_t target_id = 1; target_id <= srv_config->num_servers; ++target_id)
    {
        if (srv_config->srv_id == target_id) continue;

        int ret;
        pthread_t worker;
        sender_worker_thread_info_t *worker_info;
        int sd;
        struct sockaddr_in sv_addr;

        worker_info = malloc(sizeof(sender_worker_thread_info_t));
        worker_info->srv_config = srv_config;
        worker_info->txlm_config = txlm_config;
        worker_info->target_id = target_id;

        sd = socket(AF_INET, SOCK_STREAM, 0);
        if(sd < 0) {
            fprintf(stderr, "tcp_connection_manager.c: (line:%d) %s", __LINE__, strerror(errno));
            exit(1);
        }

        fprintf(stdout, "%s:%d\n", srv_config->srvs_ipaddr[target_id], srv_config->srvs_port[target_id]);
        sv_addr.sin_family = AF_INET;
        sv_addr.sin_addr.s_addr = inet_addr(srv_config->srvs_ipaddr[target_id]);
        sv_addr.sin_port = htons(srv_config->srvs_port[target_id]);

        ret = connect(sd, (struct sockaddr *)&sv_addr, sizeof(sv_addr));
        if(ret != 0) {
            fprintf(stderr, "tcp_connection_manager.c: (line:%d) %s\n", __LINE__, strerror(errno));
            exit(1);
        } else {
            fprintf(stdout, "connect with %s:%d(server id:%zu)\n",
                    srv_config->srvs_ipaddr[target_id],
                    srv_config->srvs_port[target_id],
                    target_id);
        }

        worker_info->sd = sd;

        pthread_create(&worker,
                        NULL,
                        (void *)sender_worker,
                        (void *)worker_info);
        pthread_detach(worker);
    }
}

void reciever_main(server_config_t *srv_config, txlm_config_t *txlm_config)
{
    int ret;
    int listen_sd, connection_sd;
    struct sockaddr_in sv_addr;
    struct sockaddr_in cl_addr;
    int cl_addr_len;
    char buff[MAX_SEND_DATA_SIZE];
    int msg_len;

    listen_sd = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_sd < 0)
    {
        fprintf(stderr, "tcp_connection_manager.c: (line:%d) %s", __LINE__, strerror(errno));
        exit(1);
    }

    sv_addr.sin_family = AF_INET;
    sv_addr.sin_addr.s_addr = INADDR_ANY;
    sv_addr.sin_port = htons(srv_config->port);

    ret = bind(listen_sd, (struct sockaddr *)&sv_addr, sizeof(sv_addr));
    if(ret != 0)
    {
        fprintf(stderr, "tcp_connection_manager.c: (line:%d) %s", __LINE__, strerror(errno));
        exit(1);
    }

    ret = listen(listen_sd, SOMAXCONN);
    if(ret != 0)
    {
        fprintf(stderr, "tcp_connection_manager.c: (line:%d) %s", __LINE__, strerror(errno));
        exit(1);
    }
    fprintf(stdout, "ok\n");

    cl_addr_len = sizeof(cl_addr);
    connection_sd = accept(listen_sd, (struct sockaddr *)&cl_addr, (socklen_t *)&cl_addr_len);
    if (connection_sd < 0)
    {
        fprintf(stderr, "tcp_connection_manager.c: (line:%d) %s", __LINE__, strerror(errno));
        exit(1);
    }
    fprintf(stdout, "connect\n");


    txlog_t txlog;

    while (1)
    {
        // DATA受信
        msg_len = recv(connection_sd, buff, MAX_SEND_DATA_SIZE, 0);
        if (msg_len < 0)
        {
            fprintf(stderr, "tcp_connection_manager.c: (line:%d) %s", __LINE__, strerror(errno));
            exit(1);
        }
        buff[msg_len] = '\0';
        txlm_get_info_from_header(&txlog, buff);
        printf("[recieve] lsn:%d clientid:%d appendtime:%lf writetime:%lf\n", txlog.lsn, txlog.client_id, txlog.append_time, txlog.write_time);
        txlm_wirte_log(txlm_config, &txlog, 0);

        // ACK返信
        msg_len = sprintf(buff, "ACK %d", txlog.lsn);
        ret = send(connection_sd, buff, msg_len, 0);
        if(ret != msg_len)
        {
            fprintf(stderr, "tcp_connection_manager.c: (line:%d) %s", __LINE__, strerror(errno));
            exit(1);
        }
    }
    
}