bool CClient::SendMotd() {
    const VCString& vsMotd = CZNC::Get().GetMotd();

    if (!vsMotd.size()) {
        return false;
    }

    for (const CString& sLine : vsMotd) {
        if (m_pNetwork) {
            PutStatusNotice(m_pNetwork->ExpandString(sLine));
        } else {
            PutStatusNotice(m_pUser->ExpandString(sLine));
        }
    }

    return true;
}