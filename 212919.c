void CClient::Disconnected() {
    DEBUG(GetSockName() << " == Disconnected()");
    CIRCNetwork* pNetwork = m_pNetwork;
    SetNetwork(nullptr, false, false);

    if (m_pUser) {
        NETWORKMODULECALL(OnClientDisconnect(), m_pUser, pNetwork, this,
                          NOTHING);
    }
}