CString CClient::GetFullName() const {
    if (!m_pUser) return GetRemoteIP();
    CString sFullName = m_pUser->GetUsername();
    if (!m_sIdentifier.empty()) sFullName += "@" + m_sIdentifier;
    if (m_pNetwork) sFullName += "/" + m_pNetwork->GetName();
    return sFullName;
}