void CClient::PutClient(const CString& sLine) {
    PutClient(CMessage(sLine));
}