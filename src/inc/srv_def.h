/** @file  server_define.h
 *  @brief define various necessary things for server
 *
 *  @author Chen Chen (chenche1)
 *  @bug no bug known
 */


#ifndef __SRV_DEF_H_
#define __SRV_DEF_H_

#include <netinet/in.h>
#include <netinet/ip.h>


#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>


#include "list.h"
#include "http.h"

/* define various macro */
#define TCP_PORT 9999
#define SSL_PORT 9998
#define BUF_IN_SIZE 4096
#define BUF_PROC_SIZE 2*BUF_IN_SIZE
#define BUF_OUT_SIZE 4096

#define BUF_HDR_SIZE 2048
#define TIMEOUT_TIME 1    /* in sec */
#define HASH_SIZE    0xff     /* size of hash size of client list */

#define DEFAULT_FD "../static_site/"
#define FILENAME_MAX_LEN 256


struct cli_cb;
typedef struct cli_cb cli_cb_t;

struct cli_cb_mthd{
    int (*new_connection)(cli_cb_t *cb);
    int (*close)(cli_cb_t *cb);
    int (*recv)(cli_cb_t *cb);
    int (*parse)(cli_cb_t *cb);
    int (*handle_req_msg)(cli_cb_t *cb);
    int (*send)(cli_cb_t *cb);
    
};

typedef struct cli_cb_mthd cli_cb_mthd_t;

enum cli_cb_type{
    LISTEN,
    CLI,
};


typedef enum cli_cb_type cli_cb_type_t;

struct cli_cb{
    struct sockaddr_in cli_addr;
    int cli_fd;
    cli_cb_type_t type;
    int is_ssl;

    cli_cb_mthd_t mthd;

    SSL *ssl;

    char buf_in[BUF_IN_SIZE + 1];        /* recv'd str goes here */
    int buf_in_ctr;
    char buf_proc[BUF_PROC_SIZE + 1]; /* buf for processing pipelined reqs */   
    int buf_proc_ctr;
    char *buf_out;
    int buf_out_ctr;

    /* variables for parser */
    char *par_pos;
    char *par_next;
    char *par_msg_end;

    /* req msg list */
    struct list_head req_msg_list;                /* curr req msg to process */

    int handle_req_pending;
    struct list_head cli_link;              /* link for global hash table */
    
};






int parse_cli_cb(cli_cb_t *cb);
void insert_req_msg(req_msg_t *msg, cli_cb_t *cb);




/* for srv function */
int establish_socket(void);
int select_wrapper(struct timeval *t);
int is_new_connection(int fd);
int create_new_connection(int fd);
int kill_connections(void);
int recv_wrapper(int fd);
int send_wrapper(int fd);
int process_request(void);

/* shutdown liso server */
void liso_shutdown(void);

/* daemonize the server */
int daemonize(char* lock_file);

#endif /* end of __SRV_DEF_H_ */
