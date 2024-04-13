void CAuthBase::AcceptLogin(CUser& User) {
    if (m_pSock) {
        AcceptedLogin(User);
        Invalidate();
    }
}