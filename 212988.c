bool CClient::OnActionMessage(CActionMessage& Message) {
    CString sTargets = Message.GetTarget();

    VCString vTargets;
    sTargets.Split(",", vTargets, false);

    for (CString& sTarget : vTargets) {
        Message.SetTarget(sTarget);
        if (m_pNetwork) {
            // May be nullptr.
            Message.SetChan(m_pNetwork->FindChan(sTarget));
        }

        bool bContinue = false;
        NETWORKMODULECALL(OnUserActionMessage(Message), m_pUser, m_pNetwork,
                          this, &bContinue);
        if (bContinue) continue;

        if (m_pNetwork) {
            AddBuffer(Message);
            EchoMessage(Message);
            PutIRC(Message.ToString(CMessage::ExcludePrefix |
                                    CMessage::ExcludeTags));
        }
    }

    return true;
}