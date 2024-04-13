void CClient::SendRequiredPasswordNotice() {
    PutClient(":irc.znc.in 464 " + GetNick() + " :Password required");
    PutClient(
        ":irc.znc.in NOTICE " + GetNick() + " :*** "
        "You need to send your password. "
        "Configure your client to send a server password.");
    PutClient(
        ":irc.znc.in NOTICE " + GetNick() + " :*** "
        "To connect now, you can use /quote PASS <username>:<password>, "
        "or /quote PASS <username>/<network>:<password> to connect to a "
        "specific network.");
}