void LanLinkProvider::onNetworkConfigurationChanged(const QNetworkConfiguration& config)
{
    if (m_lastConfig != config && config.state() == QNetworkConfiguration::Active) {
        m_lastConfig = config;
        onNetworkChange();
    }
}