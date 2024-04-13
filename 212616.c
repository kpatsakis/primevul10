void LanLinkProvider::connectError(QAbstractSocket::SocketError socketError)
{
    QSslSocket* socket = qobject_cast<QSslSocket*>(sender());
    if (!socket) return;

    qCDebug(KDECONNECT_CORE) << "Socket error" << socketError;
    qCDebug(KDECONNECT_CORE) << "Fallback (1), try reverse connection (send udp packet)" << socket->errorString();
    NetworkPacket np(QLatin1String(""));
    NetworkPacket::createIdentityPacket(&np);
    np.set(QStringLiteral("tcpPort"), m_tcpPort);
    m_udpSocket.writeDatagram(np.serialize(), m_receivedIdentityPackets[socket].sender, m_udpBroadcastPort);

    //The socket we created didn't work, and we didn't manage
    //to create a LanDeviceLink from it, deleting everything.
    delete m_receivedIdentityPackets.take(socket).np;
    socket->deleteLater();
}