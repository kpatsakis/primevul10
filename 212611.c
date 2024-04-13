void LanLinkProvider::onStop()
{
    m_udpSocket.close();
    m_server->close();
    qCDebug(KDECONNECT_CORE) << "LanLinkProvider stopped";
}