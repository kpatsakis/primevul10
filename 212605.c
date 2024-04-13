void LanLinkProvider::sendBroadcasts(
        QUdpSocket& socket, const NetworkPacket& np, const QList<QHostAddress>& addresses)
{
    const QByteArray payload = np.serialize();

    for (auto& address : addresses) {
        socket.writeDatagram(payload, address, m_udpBroadcastPort);
    }
}