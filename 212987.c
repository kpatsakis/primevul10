void CClient::RefuseLogin(const CString& sReason) {
    PutStatus("Bad username and/or password.");
    PutClient(":irc.znc.in 464 " + GetNick() + " :" + sReason);
    Close(Csock::CLT_AFTERWRITE);
}