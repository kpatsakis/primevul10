CString CClient::GetNick(bool bAllowIRCNick) const {
    CString sRet;

    const CIRCSock* pSock = GetIRCSock();
    if (bAllowIRCNick && pSock && pSock->IsAuthed()) {
        sRet = pSock->GetNick();
    }

    return (sRet.empty()) ? m_sNick : sRet;
}