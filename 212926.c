void CClient::PutModNotice(const CString& sModule, const CString& sLine) {
    if (!m_pUser) {
        return;
    }

    DEBUG("(" << GetFullName()
              << ") ZNC -> CLI [:" + m_pUser->GetStatusPrefix() +
                     ((sModule.empty()) ? "status" : sModule) +
                     "!znc@znc.in NOTICE " << GetNick() << " :" << sLine
              << "]");
    Write(":" + m_pUser->GetStatusPrefix() +
          ((sModule.empty()) ? "status" : sModule) + "!znc@znc.in NOTICE " +
          GetNick() + " :" + sLine + "\r\n");
}