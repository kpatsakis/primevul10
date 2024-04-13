void CClient::ParseIdentifier(const CString& sAuthLine) {
    // user[@identifier]

    const size_t uAt = sAuthLine.rfind("@");
    if (uAt != CString::npos) {
        const CString sId = sAuthLine.substr(uAt + 1);

        if (IsValidIdentifier(sId)) {
            m_sIdentifier = sId;
            m_sUser = sAuthLine.substr(0, uAt);
        } else {
            m_sUser = sAuthLine;
        }
    } else {
        m_sUser = sAuthLine;
    }
}