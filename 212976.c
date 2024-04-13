bool CClient::OnJoinMessage(CJoinMessage& Message) {
    CString sChans = Message.GetTarget();
    CString sKeys = Message.GetKey();

    VCString vsChans;
    sChans.Split(",", vsChans, false);
    sChans.clear();

    VCString vsKeys;
    sKeys.Split(",", vsKeys, true);
    sKeys.clear();

    for (unsigned int a = 0; a < vsChans.size(); a++) {
        Message.SetTarget(vsChans[a]);
        Message.SetKey((a < vsKeys.size()) ? vsKeys[a] : "");
        if (m_pNetwork) {
            // May be nullptr.
            Message.SetChan(m_pNetwork->FindChan(vsChans[a]));
        }
        bool bContinue = false;
        NETWORKMODULECALL(OnUserJoinMessage(Message), m_pUser, m_pNetwork, this,
                          &bContinue);
        if (bContinue) continue;

        CString sChannel = Message.GetTarget();
        CString sKey = Message.GetKey();

        if (m_pNetwork) {
            CChan* pChan = m_pNetwork->FindChan(sChannel);
            if (pChan) {
                if (pChan->IsDetached())
                    pChan->AttachUser(this);
                else
                    pChan->JoinUser(sKey);
                continue;
            } else if (!sChannel.empty()) {
                pChan = new CChan(sChannel, m_pNetwork, false);
                if (m_pNetwork->AddChan(pChan)) {
                    pChan->SetKey(sKey);
                }
            }
        }

        if (!sChannel.empty()) {
            sChans += (sChans.empty()) ? sChannel : CString("," + sChannel);

            if (!vsKeys.empty()) {
                sKeys += (sKeys.empty()) ? sKey : CString("," + sKey);
            }
        }
    }

    Message.SetTarget(sChans);
    Message.SetKey(sKeys);

    return sChans.empty();
}