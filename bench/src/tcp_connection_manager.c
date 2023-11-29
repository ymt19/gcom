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
#include "message.h"
#include "log_manager.h"

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
    server_config_t *srv_config = worker_info->srv_config;
    txlm_config_t *txlm_config = worker_info->txlm_config;
    size_t target_id = worker_info->target_id;
    int sd = worker_info->sd;

    /****************** Senderプロトコル ******************/
    int ret;
    char buff[MAX_SEND_DATA_SIZE];
    unsigned int msgsize;
    unsigned int nextlsn = TXLOG_MIN_LSN; // 未送信LSNの最小値（送信済みLSNの最大値 + 1）
    message_header recvhdr;
    txlog_t txlog;

    while (1)
    {
        if (txlm_get_current_lsn(txlm_config) >= nextlsn) {
            /* TXLOGメッセージ生成 */
            txlm_read_log(txlm_config, &txlog, nextlsn);
            msgsize = create_txlog_message(buff, srv_config->srv_id, target_id, &txlog);
            
            /* TXLOGメッセージ送信 */
            ret = 0;
            while (ret < msgsize) {
                ret += send(sd, buff + ret, msgsize - ret, 0);
            }
            lm_append_send_message_log(buff, ret);

            /* メッセージ受信 */
            msgsize = 0;
            while (msgsize < sizeof(message_header)) {
                msgsize += recv(sd, buff + msgsize, MAX_SEND_DATA_SIZE - msgsize, 0);
            }
            memcpy(&recvhdr, buff, sizeof(message_header));
            while (msgsize < recvhdr.size) {
                msgsize += recv(sd, buff + msgsize, MAX_SEND_DATA_SIZE - msgsize, 0);
            }
            lm_append_receive_message_log(buff, msgsize);

            /* メッセージタイプごとの処理 */
            if (recvhdr.type == ACK_MESSAGE) {
                nextlsn = recvhdr.lsn + 1;
            } else {
                fprintf(stderr, "RECV ERROR MESSAGE");
            }
        }
    }
    /***************************************************/

    close(sd);
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



    /****************** Recieverプロトコル ******************/
    char buff[MAX_SEND_DATA_SIZE];
    int msgsize;
    message_header recvhdr;
    txlog_t txlog;

    while (1)
    {
        /* メッセージ受信 */
        msgsize = 0;
        while (msgsize < sizeof(message_header)) {
            msgsize += recv(connection_sd, buff + msgsize, MAX_SEND_DATA_SIZE - msgsize, 0);
        }
        memcpy(&recvhdr, buff, sizeof(message_header));
        while (msgsize < recvhdr.size) {
            msgsize += recv(connection_sd, buff + msgsize, MAX_SEND_DATA_SIZE - msgsize, 0);
        }
        lm_append_receive_message_log(buff, msgsize);

        /* メッセージタイプごとの処理 */
        if (recvhdr.type == TXLOG_MESSAGE) {
            memcpy(&txlog, buff + sizeof(message_header), sizeof(txlog_t));
            print_txlog_info(&txlog);
            txlm_wirte_log(txlm_config, &txlog);

            /* ACKメッセージ生成 */
            msgsize = sizeof(message_header);
            create_ack_message(buff, srv_config->srv_id, recvhdr.source_id, txlog.lsn);

            /* ACKメッセージ送信 */
            ret = 0;
            while (ret < msgsize) {
                ret += send(connection_sd, buff + ret, msgsize - ret, 0);
            }
            lm_append_send_message_log(buff, msgsize);
        } else {
            fprintf(stderr, "RECV ERROR MESSAGE");
        }
    }
    /******************************************************/

    close(connection_sd);
    close(listen_sd);
}