void LanLinkProvider::onNetworkChange()
{
    if (m_combineBroadcastsTimer.isActive()) {
        qCDebug(KDECONNECT_CORE) << "Preventing duplicate broadcasts";
        return;
    }
    m_combineBroadcastsTimer.start();
}