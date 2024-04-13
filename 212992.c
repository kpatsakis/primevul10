void CClientAuth::RefusedLogin(const CString& sReason) {
    if (m_pClient) {
        m_pClient->RefuseLogin(sReason);
    }
}