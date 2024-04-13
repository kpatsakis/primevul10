void CClient::AuthUser() {
    if (!m_bGotNick || !m_bGotUser || !m_bGotPass || m_bInCap || IsAttached())
        return;

    m_spAuth = std::make_shared<CClientAuth>(this, m_sUser, m_sPass);

    CZNC::Get().AuthUser(m_spAuth);
}