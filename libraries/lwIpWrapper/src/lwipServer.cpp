extern "C" {
#include "string.h"
}

#include "CNetIf.h"
#include "lwipClient.h"
#include "lwipServer.h"

extern err_t tcp_recv_callback(void* arg, struct tcp_pcb* tpcb, struct pbuf* p, err_t err);
extern err_t tcp_sent_callback(void* arg, struct tcp_pcb* tpcb, u16_t len);
extern void tcp_err_callback(void* arg, err_t err);

std::shared_ptr<struct tcp_struct> lwipServer::_tcp_client[MAX_CLIENT];

err_t lwipServer::tcp_accept_cb(void* arg, struct tcp_pcb* newpcb, err_t err)
{
    (void *)arg;
    err_t ret_err;
    int accepted = -1;
    
    /* set priority for the newly accepted tcp connection newpcb */
    tcp_setprio(newpcb, TCP_PRIO_MIN);

    if (ERR_OK == err) {
        /* Looking for an empty socket */
        for (uint16_t i = 0; i < MAX_CLIENT; i++) {
            if (_tcp_client[i] == nullptr) {
                 
                _tcp_client[i] = std::make_shared<struct tcp_struct>();
                if (_tcp_client[i] != nullptr) {
                    _tcp_client[i]->state = TCP_ACCEPTED;
                    _tcp_client[i]->pcb = newpcb;
                    _tcp_client[i]->data.p = nullptr;
                    _tcp_client[i]->data.available = 0;
                    accepted = i;
                    break;
                }
                else {
                    /*  close tcp connection */
                    tcp_close(newpcb);
                    /* return memory error */
                    ret_err = ERR_MEM;
                    break;
                }
            }
        }
        if (accepted >= 0) {
            /* pass newly allocated client structure as argument to newpcb */
            tcp_arg(newpcb, static_cast<void*>(_tcp_client[accepted].get()));

            /* initialize lwip tcp_recv callback function for newpcb  */
            tcp_recv(newpcb, tcp_recv_callback);

            /* initialize lwip tcp_err callback function for newpcb  */
            tcp_err(newpcb, tcp_err_callback);

            /* initialize LwIP tcp_sent callback function */
            tcp_sent(newpcb, tcp_sent_callback);

            ret_err = ERR_OK;
        }
        
    } else {
        tcp_close(newpcb);
        ret_err = ERR_ARG;
    }
    return ret_err;
}


lwipServer::lwipServer(uint16_t port)
{
    _port = port;
    for (int i = 0; i < MAX_CLIENT; i++) {
        _tcp_client[i] = nullptr;
    }
    _tcp_server = {};
}

lwipServer::~lwipServer() {
    for (int i = 0; i < MAX_CLIENT; i++) {
        _tcp_client[i].reset();
        _tcp_client[i] = nullptr;
    }
}

void lwipServer::begin()
{
    if (_tcp_server.pcb != NULL) {
        return;
    }

    _tcp_server.pcb = tcp_new();

    if (_tcp_server.pcb == NULL) {
        return;
    }

    tcp_arg(_tcp_server.pcb, nullptr);
    _tcp_server.state = TCP_NONE;

    if (ERR_OK != tcp_bind(_tcp_server.pcb, IP_ADDR_ANY, _port)) {
        memp_free(MEMP_TCP_PCB, _tcp_server.pcb);
        _tcp_server.pcb = NULL;
        return;
    }

    _tcp_server.pcb = tcp_listen(_tcp_server.pcb);
    tcp_accept(_tcp_server.pcb, lwipServer::tcp_accept_cb);
}

void lwipServer::begin(uint16_t port)
{
    _port = port;
    begin();
}

void lwipServer::accept()
{
    /* Free client if disconnected */
    for (int n = 0; n < MAX_CLIENT; n++) {
        if (_tcp_client[n] != NULL) {
            lwipClient client(_tcp_client[n]);
            if (client.status() == TCP_CLOSING) {
                _tcp_client[n].reset();
                _tcp_client[n] = nullptr;
            }
        }
    }
}

lwipClient lwipServer::available()
{
    accept();

    for (int n = 0; n < MAX_CLIENT; n++) {
        if (_tcp_client[n] != NULL) {
            if (_tcp_client[n]->pcb != NULL) {
                lwipClient client(_tcp_client[n]);
                uint8_t s = client.status();
                if (s == TCP_ACCEPTED) {
                    if (client.available()) {
                        return client;
                    }
                }
            }
        }
    }
    std::shared_ptr<struct tcp_struct> default_client = nullptr;
    return lwipClient(default_client);
}

size_t lwipServer::write(uint8_t b)
{
    return write(&b, 1);
}

size_t lwipServer::write(const uint8_t* buffer, size_t size)
{
    size_t n = 0;

    accept();

    for (int n = 0; n < MAX_CLIENT; n++) {
        if (_tcp_client[n] != NULL) {
            if (_tcp_client[n]->pcb != NULL) {
                lwipClient client(_tcp_client[n]);
                uint8_t s = client.status();
                if (s == TCP_ACCEPTED) {
                    n += client.write(buffer, size);
                }
            }
        }
    }

    return n;
}
