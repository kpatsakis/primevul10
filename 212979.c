bool CClient::OnPartMessage(CPartMessage& Message) {
    CString sChans = Message.GetTarget();

    VCString vsChans;
    sChans.Split(",", vsChans, false);
    sChans.clear();

    for (CString& sChan : vsChans) {
        bool bContinue = false;
        Message.SetTarget(sChan);
        if (m_pNetwork) {
            // May be nullptr.
            Message.SetChan(m_pNetwork->FindChan(sChan));
        }
        NETWORKMODULECALL(OnUserPartMessage(Message), m_pUser, m_pNetwork, this,
                          &bContinue);
        if (bContinue) continue;

        sChan = Message.GetTarget();

        CChan* pChan = m_pNetwork ? m_pNetwork->FindChan(sChan) : nullptr;

        if (pChan && !pChan->IsOn()) {
            PutStatusNotice(t_f("Removing channel {1}")(sChan));
            m_pNetwork->DelChan(sChan);
        } else {
            sChans += (sChans.empty()) ? sChan : CString("," + sChan);
        }
    }

    if (sChans.empty()) {
        return true;
    }

    Message.SetTarget(sChans);

    return false;
}