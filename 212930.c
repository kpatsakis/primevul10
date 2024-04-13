unsigned int CClient::PutStatus(const CTable& table) {
    unsigned int idx = 0;
    CString sLine;
    while (table.GetLine(idx++, sLine)) PutStatus(sLine);
    return idx - 1;
}