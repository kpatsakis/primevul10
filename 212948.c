CString CClient::GetNickMask() const {
    if (GetIRCSock() && GetIRCSock()->IsAuthed()) {
        return GetIRCSock()->GetNickMask();
    }

    CString sHost =
        m_pNetwork ? m_pNetwork->GetBindHost() : m_pUser->GetBindHost();
    if (sHost.empty()) {
        sHost = "irc.znc.in";
    }

    return GetNick() + "!" +
           (m_pNetwork ? m_pNetwork->GetIdent() : m_pUser->GetIdent()) + "@" +
           sHost;
}