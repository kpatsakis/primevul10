void CClient::PutModule(const CString& sModule, const CString& sLine) {
    if (!m_pUser) {
        return;
    }

    DEBUG("(" << GetFullName()
              << ") ZNC -> CLI [:" + m_pUser->GetStatusPrefix() +
                     ((sModule.empty()) ? "status" : sModule) +
                     "!znc@znc.in PRIVMSG " << GetNick() << " :" << sLine
              << "]");

    VCString vsLines;
    sLine.Split("\n", vsLines);
    for (const CString& s : vsLines) {
        Write(":" + m_pUser->GetStatusPrefix() +
              ((sModule.empty()) ? "status" : sModule) +
              "!znc@znc.in PRIVMSG " + GetNick() + " :" + s + "\r\n");
    }
}