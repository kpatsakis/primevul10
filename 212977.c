void CClientAuth::AcceptedLogin(CUser& User) {
    if (m_pClient) {
        m_pClient->AcceptLogin(User);
    }
}