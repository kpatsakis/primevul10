bool CClient::OnQuitMessage(CQuitMessage& Message) {
    bool bReturn = false;
    NETWORKMODULECALL(OnUserQuitMessage(Message), m_pUser, m_pNetwork, this,
                      &bReturn);
    if (!bReturn) {
        Close(Csock::CLT_AFTERWRITE);  // Treat a client quit as a detach
    }
    // Don't forward this msg.  We don't want the client getting us
    // disconnected.
    return true;
}