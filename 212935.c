void CClient::RespondCap(const CString& sResponse) {
    PutClient(":irc.znc.in CAP " + GetNick() + " " + sResponse);
}