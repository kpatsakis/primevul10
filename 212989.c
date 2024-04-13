unsigned int CClient::AttachChans(const std::set<CChan*>& sChans) {
    unsigned int uAttached = 0;
    for (CChan* pChan : sChans) {
        if (!pChan->IsDetached()) continue;
        uAttached++;
        pChan->AttachUser();
    }
    return uAttached;
}