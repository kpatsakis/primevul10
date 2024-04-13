void CClient::SetTagSupport(const CString& sTag, bool bState) {
    if (bState) {
        m_ssSupportedTags.insert(sTag);
    } else {
        m_ssSupportedTags.erase(sTag);
    }
}