void CClient::StatusCTCP(const CString& sLine) {
    CString sCommand = sLine.Token(0);

    if (sCommand.Equals("PING")) {
        PutStatusNotice("\001PING " + sLine.Token(1, true) + "\001");
    } else if (sCommand.Equals("VERSION")) {
        PutStatusNotice("\001VERSION " + CZNC::GetTag() + "\001");
    }
}