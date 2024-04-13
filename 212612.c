void LanLinkProvider::tcpSocketConnected()
{
    QSslSocket* socket = qobject_cast<QSslSocket*>(sender());

    if (!socket) return;
    // TODO Delete me?
#if QT_VERSION < QT_VERSION_CHECK(5,15,0)
    disconnect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &LanLinkProvider::connectError);
#else
    disconnect(socket, &QAbstractSocket::errorOccurred, this, &LanLinkProvider::connectError);
#endif

    configureSocket(socket);

    // If socket disconnects due to any reason after connection, link on ssl failure
    connect(socket, &QAbstractSocket::disconnected, socket, &QObject::deleteLater);

    NetworkPacket* receivedPacket = m_receivedIdentityPackets[socket].np;
    const QString& deviceId = receivedPacket->get<QString>(QStringLiteral("deviceId"));
    //qCDebug(KDECONNECT_CORE) << "tcpSocketConnected" << socket->isWritable();

    // If network is on ssl, do not believe when they are connected, believe when handshake is completed
    NetworkPacket np2(QLatin1String(""));
    NetworkPacket::createIdentityPacket(&np2);
    socket->write(np2.serialize());
    bool success = socket->waitForBytesWritten();

    if (success) {

        qCDebug(KDECONNECT_CORE) << "TCP connection done (i'm the existing device)";

        // if ssl supported
        if (receivedPacket->get<int>(QStringLiteral("protocolVersion")) >= MIN_VERSION_WITH_SSL_SUPPORT) {

            bool isDeviceTrusted = KdeConnectConfig::instance().trustedDevices().contains(deviceId);
            configureSslSocket(socket, deviceId, isDeviceTrusted);

            qCDebug(KDECONNECT_CORE) << "Starting server ssl (I'm the client TCP socket)";

            connect(socket, &QSslSocket::encrypted, this, &LanLinkProvider::encrypted);

            connect(socket, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors), this, &LanLinkProvider::sslErrors);

            socket->startServerEncryption();

            return; // Return statement prevents from deleting received packet, needed in slot "encrypted"
        } else {
            qWarning() << receivedPacket->get<QString>(QStringLiteral("deviceName")) << "uses an old protocol version, this won't work";
            //addLink(deviceId, socket, receivedPacket, LanDeviceLink::Remotely);
        }

    } else {
        //I think this will never happen, but if it happens the deviceLink
        //(or the socket that is now inside it) might not be valid. Delete them.
        qCDebug(KDECONNECT_CORE) << "Fallback (2), try reverse connection (send udp packet)";
        m_udpSocket.writeDatagram(np2.serialize(), m_receivedIdentityPackets[socket].sender, m_udpBroadcastPort);
    }

    delete m_receivedIdentityPackets.take(socket).np;
    //We don't delete the socket because now it's owned by the LanDeviceLink
}