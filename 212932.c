bool CClient::OnPongMessage(CMessage& Message) {
    // Block PONGs, we already responded to the pings
    return true;
}