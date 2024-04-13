CClientAuth::CClientAuth(CClient* pClient, const CString& sUsername,
                         const CString& sPassword)
    : CAuthBase(sUsername, sPassword, pClient), m_pClient(pClient) {}