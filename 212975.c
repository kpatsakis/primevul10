void CClient::BouncedOff() {
    PutStatusNotice(
        t_s("You are being disconnected because another user just "
            "authenticated as you."));
    Close(Csock::CLT_AFTERWRITE);
}