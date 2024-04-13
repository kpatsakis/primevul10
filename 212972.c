void CClient::NotifyServerDependentCaps(const SCString& ssCaps) {
    for (const CString& sCap : ssCaps) {
        const auto& it = m_mCoreCaps.find(sCap);
        if (m_mCoreCaps.end() != it) {
            bool bServerDependent = std::get<0>(it->second);
            if (bServerDependent) {
                m_ssServerDependentCaps.insert(sCap);
            }
        }
    }

    if (HasCapNotify() && !m_ssServerDependentCaps.empty()) {
        CString sCaps = CString(" ").Join(m_ssServerDependentCaps.begin(),
                                          m_ssServerDependentCaps.end());
        PutClient(":irc.znc.in CAP " + GetNick() + " NEW :" + sCaps);
    }
}