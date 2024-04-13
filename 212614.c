void LanLinkProvider::deviceLinkDestroyed(QObject* destroyedDeviceLink)
{
    const QString id = destroyedDeviceLink->property("deviceId").toString();
    //qCDebug(KDECONNECT_CORE) << "deviceLinkDestroyed" << id;
    QMap< QString, LanDeviceLink* >::iterator linkIterator = m_links.find(id);
    Q_ASSERT(linkIterator != m_links.end());
    if (linkIterator != m_links.end()) {
        Q_ASSERT(linkIterator.value() == destroyedDeviceLink);
        m_links.erase(linkIterator);
        auto pairingHandler = m_pairingHandlers.take(id);
        if (pairingHandler) {
            pairingHandler->deleteLater();
        }
    }

}