void CClient::SetNetwork(CIRCNetwork* pNetwork, bool bDisconnect,
                         bool bReconnect) {
    if (m_pNetwork) {
        m_pNetwork->ClientDisconnected(this);

        if (bDisconnect) {
            ClearServerDependentCaps();
            // Tell the client they are no longer in these channels.
            const vector<CChan*>& vChans = m_pNetwork->GetChans();
            for (const CChan* pChan : vChans) {
                if (!(pChan->IsDetached())) {
                    PutClient(":" + m_pNetwork->GetIRCNick().GetNickMask() +
                              " PART " + pChan->GetName());
                }
            }
        }
    } else if (m_pUser) {
        m_pUser->UserDisconnected(this);
    }

    m_pNetwork = pNetwork;

    if (bReconnect) {
        if (m_pNetwork) {
            m_pNetwork->ClientConnected(this);
        } else if (m_pUser) {
            m_pUser->UserConnected(this);
        }
    }
}