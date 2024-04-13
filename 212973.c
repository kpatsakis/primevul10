CClient::~CClient() {
    if (m_spAuth) {
        CClientAuth* pAuth = (CClientAuth*)&(*m_spAuth);
        pAuth->Invalidate();
    }
    if (m_pUser != nullptr) {
        m_pUser->AddBytesRead(GetBytesRead());
        m_pUser->AddBytesWritten(GetBytesWritten());
    }
}