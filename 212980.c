set<CChan*> CClient::MatchChans(const CString& sPatterns) const {
    VCString vsPatterns;
    sPatterns.Replace_n(",", " ")
        .Split(" ", vsPatterns, false, "", "", true, true);

    set<CChan*> sChans;
    for (const CString& sPattern : vsPatterns) {
        vector<CChan*> vChans = m_pNetwork->FindChans(sPattern);
        sChans.insert(vChans.begin(), vChans.end());
    }
    return sChans;
}