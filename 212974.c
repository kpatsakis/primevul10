unsigned int CClient::DetachChans(const std::set<CChan*>& sChans) {
    unsigned int uDetached = 0;
    for (CChan* pChan : sChans) {
        if (pChan->IsDetached()) continue;
        uDetached++;
        pChan->DetachUser();
    }
    return uDetached;
}