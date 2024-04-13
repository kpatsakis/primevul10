bool CClient::PutClientRaw(const CString& sLine) {
    CString sCopy = sLine;
    bool bReturn = false;
    NETWORKMODULECALL(OnSendToClient(sCopy, *this), m_pUser, m_pNetwork, this,
                      &bReturn);
    if (bReturn) return false;

    DEBUG("(" << GetFullName() << ") ZNC -> CLI ["
        << CDebug::Filter(sCopy) << "]");
    Write(sCopy + "\r\n");
    return true;
}