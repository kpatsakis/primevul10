void CClient::PutIRC(const CString& sLine) {
    if (m_pNetwork) {
        m_pNetwork->PutIRC(sLine);
    }
}