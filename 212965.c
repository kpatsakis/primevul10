void CAuthBase::RefuseLogin(const CString& sReason) {
    if (!m_pSock) return;

    CUser* pUser = CZNC::Get().FindUser(GetUsername());

    // If the username is valid, notify that user that someone tried to
    // login. Use sReason because there are other reasons than "wrong
    // password" for a login to be rejected (e.g. fail2ban).
    if (pUser) {
        pUser->PutStatusNotice(t_f(
            "A client from {1} attempted to login as you, but was rejected: "
            "{2}")(GetRemoteIP(), sReason));
    }

    GLOBALMODULECALL(OnFailedLogin(GetUsername(), GetRemoteIP()), NOTHING);
    RefusedLogin(sReason);
    Invalidate();
}