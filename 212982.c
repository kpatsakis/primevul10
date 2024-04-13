bool CClient::OnTopicMessage(CTopicMessage& Message) {
    bool bReturn = false;
    CString sChan = Message.GetTarget();
    CString sTopic = Message.GetTopic();
    if (m_pNetwork) {
        // May be nullptr.
        Message.SetChan(m_pNetwork->FindChan(sChan));
    }

    if (!sTopic.empty()) {
        NETWORKMODULECALL(OnUserTopicMessage(Message), m_pUser, m_pNetwork,
                          this, &bReturn);
    } else {
        NETWORKMODULECALL(OnUserTopicRequest(sChan), m_pUser, m_pNetwork, this,
                          &bReturn);
        Message.SetTarget(sChan);
    }

    return bReturn;
}