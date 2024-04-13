LanLinkProvider::LanLinkProvider(
        bool testMode,
        quint16 udpBroadcastPort,
        quint16 udpListenPort
        )
    : m_server(new Server(this))
    , m_udpSocket(this)
    , m_tcpPort(0)
    , m_udpBroadcastPort(udpBroadcastPort)
    , m_udpListenPort(udpListenPort)
    , m_testMode(testMode)
    , m_combineBroadcastsTimer(this)
{

    m_combineBroadcastsTimer.setInterval(0); // increase this if waiting a single event-loop iteration is not enough
    m_combineBroadcastsTimer.setSingleShot(true);
    connect(&m_combineBroadcastsTimer, &QTimer::timeout, this, &LanLinkProvider::broadcastToNetwork);

    connect(&m_udpSocket, &QIODevice::readyRead, this, &LanLinkProvider::udpBroadcastReceived);

    m_server->setProxy(QNetworkProxy::NoProxy);
    connect(m_server, &QTcpServer::newConnection, this, &LanLinkProvider::newConnection);

    m_udpSocket.setProxy(QNetworkProxy::NoProxy);

    //Detect when a network interface changes status, so we announce ourselves in the new network
    QNetworkConfigurationManager* networkManager = new QNetworkConfigurationManager(this);
    connect(networkManager, &QNetworkConfigurationManager::configurationChanged, this, &LanLinkProvider::onNetworkConfigurationChanged);

}