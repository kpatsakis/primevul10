void LanLinkProvider::broadcastToNetwork()
{
    if (!m_server->isListening()) {
        //Not started
        return;
    }

    Q_ASSERT(m_tcpPort != 0);

    qCDebug(KDECONNECT_CORE()) << "Broadcasting identity packet";

    QList<QHostAddress> destinations = getBroadcastAddresses();

    NetworkPacket np;
    NetworkPacket::createIdentityPacket(&np);
    np.set(QStringLiteral("tcpPort"), m_tcpPort);

#ifdef Q_OS_WIN
    //On Windows we need to broadcast from every local IP address to reach all networks
    QUdpSocket sendSocket;
    sendSocket.setProxy(QNetworkProxy::NoProxy);
    for (const QNetworkInterface& iface : QNetworkInterface::allInterfaces()) {
        if ( (iface.flags() & QNetworkInterface::IsUp)
             && (iface.flags() & QNetworkInterface::IsRunning)
             && (iface.flags() & QNetworkInterface::CanBroadcast)) {
            for (const QNetworkAddressEntry& ifaceAddress : iface.addressEntries()) {
                QHostAddress sourceAddress = ifaceAddress.ip();
                if (sourceAddress.protocol() == QAbstractSocket::IPv4Protocol && sourceAddress != QHostAddress::LocalHost) {
                    qCDebug(KDECONNECT_CORE()) << "Broadcasting as" << sourceAddress;
                    sendBroadcasts(sendSocket, np, destinations);
                    sendSocket.close();
                }
            }
        }
    }
#else
    sendBroadcasts(m_udpSocket, np, destinations);
#endif
}