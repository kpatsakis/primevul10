void LanLinkProvider::encrypted()
{
    qCDebug(KDECONNECT_CORE) << "Socket successfully established an SSL connection";

    QSslSocket* socket = qobject_cast<QSslSocket*>(sender());
    if (!socket) return;

    Q_ASSERT(socket->mode() != QSslSocket::UnencryptedMode);
    LanDeviceLink::ConnectionStarted connectionOrigin = (socket->mode() == QSslSocket::SslClientMode)? LanDeviceLink::Locally : LanDeviceLink::Remotely;

    NetworkPacket* receivedPacket = m_receivedIdentityPackets[socket].np;
    const QString& deviceId = receivedPacket->get<QString>(QStringLiteral("deviceId"));

    addLink(deviceId, socket, receivedPacket, connectionOrigin);

    // Copied from tcpSocketConnected slot, now delete received packet
    delete m_receivedIdentityPackets.take(socket).np;
}