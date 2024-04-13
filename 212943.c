const CIRCSock* CClient::GetIRCSock() const {
    if (m_pNetwork) {
        return m_pNetwork->GetIRCSock();
    }

    return nullptr;
}