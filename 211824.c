  void moduleInit() override {
    HHVM_RC_INT_SAME(AF_UNIX);
    HHVM_RC_INT_SAME(AF_INET);
    HHVM_RC_INT_SAME(AF_INET6);
    HHVM_RC_INT_SAME(SOCK_STREAM);
    HHVM_RC_INT_SAME(SOCK_DGRAM);
    HHVM_RC_INT_SAME(SOCK_RAW);
    HHVM_RC_INT_SAME(SOCK_SEQPACKET);
    HHVM_RC_INT_SAME(SOCK_RDM);

    HHVM_RC_INT_SAME(MSG_OOB);
    HHVM_RC_INT_SAME(MSG_WAITALL);
    HHVM_RC_INT_SAME(MSG_CTRUNC);
    HHVM_RC_INT_SAME(MSG_TRUNC);
    HHVM_RC_INT_SAME(MSG_PEEK);
    HHVM_RC_INT_SAME(MSG_DONTROUTE);
#ifdef MSG_EOR
    HHVM_RC_INT_SAME(MSG_EOR);
#endif
#ifdef MSG_EOF
    HHVM_RC_INT_SAME(MSG_EOF);
#endif
#ifdef MSG_CONFIRM
    HHVM_RC_INT_SAME(MSG_CONFIRM);
#endif
#ifdef MSG_ERRQUEUE
    HHVM_RC_INT_SAME(MSG_ERRQUEUE);
#endif
#ifdef MSG_NOSIGNAL
    HHVM_RC_INT_SAME(MSG_NOSIGNAL);
#endif
#ifdef MSG_DONTWAIT
    HHVM_RC_INT_SAME(MSG_DONTWAIT);
#endif
#ifdef MSG_MORE
    HHVM_RC_INT_SAME(MSG_MORE);
#endif
#ifdef MSG_WAITFORONE
    HHVM_RC_INT_SAME(MSG_WAITFORONE);
#endif
#ifdef MSG_CMSG_CLOEXEC
    HHVM_RC_INT_SAME(MSG_CMSG_CLOEXEC);
#endif

    HHVM_RC_INT_SAME(SO_DEBUG);
    HHVM_RC_INT_SAME(SO_REUSEADDR);
#ifdef SO_REUSEPORT
    HHVM_RC_INT_SAME(SO_REUSEPORT);
#endif
    HHVM_RC_INT_SAME(SO_KEEPALIVE);
    HHVM_RC_INT_SAME(SO_DONTROUTE);
    HHVM_RC_INT_SAME(SO_LINGER);
    HHVM_RC_INT_SAME(SO_BROADCAST);
    HHVM_RC_INT_SAME(SO_OOBINLINE);
    HHVM_RC_INT_SAME(SO_SNDBUF);
    HHVM_RC_INT_SAME(SO_RCVBUF);
    HHVM_RC_INT_SAME(SO_SNDLOWAT);
    HHVM_RC_INT_SAME(SO_RCVLOWAT);
    HHVM_RC_INT_SAME(SO_SNDTIMEO);
    HHVM_RC_INT_SAME(SO_RCVTIMEO);
    HHVM_RC_INT_SAME(SO_TYPE);
#ifdef SO_FAMILY
    HHVM_RC_INT_SAME(SO_FAMILY);
#endif
    HHVM_RC_INT_SAME(SO_ERROR);
#ifdef SO_BINDTODEVICE
    HHVM_RC_INT_SAME(SO_BINDTODEVICE);
#endif
    HHVM_RC_INT_SAME(SOL_SOCKET);
    HHVM_RC_INT_SAME(SOMAXCONN);
#ifdef TCP_NODELAY
    HHVM_RC_INT_SAME(TCP_NODELAY);
#endif
    HHVM_RC_INT_SAME(PHP_NORMAL_READ);
    HHVM_RC_INT_SAME(PHP_BINARY_READ);

    /* TODO: MCAST_* constants and logic to handle them */

    HHVM_RC_INT_SAME(IP_MULTICAST_IF);
    HHVM_RC_INT_SAME(IP_MULTICAST_TTL);
    HHVM_RC_INT_SAME(IP_MULTICAST_LOOP);
    HHVM_RC_INT_SAME(IPV6_MULTICAST_IF);
    HHVM_RC_INT_SAME(IPV6_MULTICAST_HOPS);
    HHVM_RC_INT_SAME(IPV6_MULTICAST_LOOP);
    HHVM_RC_INT_SAME(IPPROTO_IP);
    HHVM_RC_INT_SAME(IPPROTO_IPV6);

    HHVM_RC_INT(SOL_TCP, IPPROTO_TCP);
    HHVM_RC_INT(SOL_UDP, IPPROTO_UDP);

    HHVM_RC_INT_SAME(IPV6_UNICAST_HOPS);

#define REGISTER_LONG_CONSTANT(name, val, flags) \
    Native::registerConstant<KindOfInt64>(makeStaticString(name), val)
#include "hphp/runtime/ext/sockets/unix_socket_constants.h"
#undef REGISTER_LONG_CONSTANT

    HHVM_FE(socket_create);
    HHVM_FE(socket_create_listen);
    HHVM_FE(socket_create_pair);
    HHVM_FE(socket_get_option);
    HHVM_FE(socket_getpeername);
    HHVM_FE(socket_getsockname);
    HHVM_FE(socket_set_block);
    HHVM_FE(socket_set_nonblock);
    HHVM_FE(socket_set_option);
    HHVM_FE(socket_connect);
    HHVM_FE(socket_bind);
    HHVM_FE(socket_listen);
    HHVM_FE(socket_select);
    HHVM_FE(socket_server);
    HHVM_FE(socket_accept);
    HHVM_FE(socket_read);
    HHVM_FE(socket_write);
    HHVM_FE(socket_send);
    HHVM_FE(socket_sendto);
    HHVM_FE(socket_recv);
    HHVM_FE(socket_recvfrom);
    HHVM_FE(socket_shutdown);
    HHVM_FE(socket_close);
    HHVM_FE(socket_strerror);
    HHVM_FE(socket_last_error);
    HHVM_FE(socket_clear_error);
    HHVM_FE(getaddrinfo);

    loadSystemlib();
  }