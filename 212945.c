CIRCSock* CClient::GetIRCSock() {
    if (m_pNetwork) {
        return m_pNetwork->GetIRCSock();
    }

    return nullptr;
}