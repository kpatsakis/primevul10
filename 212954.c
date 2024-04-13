const vector<CClient*>& CClient::GetClients() const {
    if (m_pNetwork) {
        return m_pNetwork->GetClients();
    }

    return m_pUser->GetUserClients();
}