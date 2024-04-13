void LanLinkProvider::onStart()
{
    const QHostAddress bindAddress = m_testMode? QHostAddress::LocalHost : QHostAddress::Any;

    bool success = m_udpSocket.bind(bindAddress, m_udpListenPort, QUdpSocket::ShareAddress);
    if (!success) {
        QAbstractSocket::SocketError sockErr = m_udpSocket.error();
        // Refer to https://doc.qt.io/qt-5/qabstractsocket.html#SocketError-enum to decode socket error number
        QString errorMessage = QString::fromLatin1(QMetaEnum::fromType<QAbstractSocket::SocketError>().valueToKey(sockErr));
        qCritical(KDECONNECT_CORE)
                << QLatin1String("Failed to bind UDP socket on port")
                << m_udpListenPort
                << QLatin1String("with error")
                << errorMessage;
    }
    Q_ASSERT(success);

    m_tcpPort = MIN_TCP_PORT;
    while (!m_server->listen(bindAddress, m_tcpPort)) {
        m_tcpPort++;
        if (m_tcpPort > MAX_TCP_PORT) { //No ports available?
            qCritical(KDECONNECT_CORE) << "Error opening a port in range" << MIN_TCP_PORT << "-" << MAX_TCP_PORT;
            m_tcpPort = 0;
            return;
        }
    }

    onNetworkChange();
    qCDebug(KDECONNECT_CORE) << "LanLinkProvider started";
}