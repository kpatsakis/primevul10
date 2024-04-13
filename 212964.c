CString CAuthBase::GetRemoteIP() const {
    if (m_pSock) return m_pSock->GetRemoteIP();
    return "";
}