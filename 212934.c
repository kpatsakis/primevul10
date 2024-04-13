void CClient::ParsePass(const CString& sAuthLine) {
    // [user[@identifier][/network]:]password

    const size_t uColon = sAuthLine.find(":");
    if (uColon != CString::npos) {
        m_sPass = sAuthLine.substr(uColon + 1);

        ParseUser(sAuthLine.substr(0, uColon));
    } else {
        m_sPass = sAuthLine;
    }
}