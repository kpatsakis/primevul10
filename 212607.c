void LanLinkProvider::configureSocket(QSslSocket* socket) {

    socket->setProxy(QNetworkProxy::NoProxy);

    socket->setSocketOption(QAbstractSocket::KeepAliveOption, QVariant(1));

#ifdef TCP_KEEPIDLE
    // time to start sending keepalive packets (seconds)
    int maxIdle = 10;
    setsockopt(socket->socketDescriptor(), IPPROTO_TCP, TCP_KEEPIDLE, &maxIdle, sizeof(maxIdle));
#endif

#ifdef TCP_KEEPINTVL
    // interval between keepalive packets after the initial period (seconds)
    int interval = 5;
    setsockopt(socket->socketDescriptor(), IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));
#endif

#ifdef TCP_KEEPCNT
    // number of missed keepalive packets before disconnecting
    int count = 3;
    setsockopt(socket->socketDescriptor(), IPPROTO_TCP, TCP_KEEPCNT, &count, sizeof(count));
#endif

}