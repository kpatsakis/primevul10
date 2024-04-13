bool CClient::PutClient(const CMessage& Message) {
    if (!m_bAwayNotify && Message.GetType() == CMessage::Type::Away) {
        return false;
    } else if (!m_bAccountNotify &&
               Message.GetType() == CMessage::Type::Account) {
        return false;
    }

    CMessage Msg(Message);

    const CIRCSock* pIRCSock = GetIRCSock();
    if (pIRCSock) {
        if (Msg.GetType() == CMessage::Type::Numeric) {
            unsigned int uCode = Msg.As<CNumericMessage>().GetCode();

            if (uCode == 352) {  // RPL_WHOREPLY
                if (!m_bNamesx && pIRCSock->HasNamesx()) {
                    // The server has NAMESX, but the client doesn't, so we need
                    // to remove extra prefixes
                    CString sNick = Msg.GetParam(6);
                    if (sNick.size() > 1 && pIRCSock->IsPermChar(sNick[1])) {
                        CString sNewNick = sNick;
                        size_t pos =
                            sNick.find_first_not_of(pIRCSock->GetPerms());
                        if (pos >= 2 && pos != CString::npos) {
                            sNewNick = sNick[0] + sNick.substr(pos);
                        }
                        Msg.SetParam(6, sNewNick);
                    }
                }
            } else if (uCode == 353) {  // RPL_NAMES
                if ((!m_bNamesx && pIRCSock->HasNamesx()) ||
                    (!m_bUHNames && pIRCSock->HasUHNames())) {
                    // The server has either UHNAMES or NAMESX, but the client
                    // is missing either or both
                    CString sNicks = Msg.GetParam(3);
                    VCString vsNicks;
                    sNicks.Split(" ", vsNicks, false);

                    for (CString& sNick : vsNicks) {
                        if (sNick.empty()) break;

                        if (!m_bNamesx && pIRCSock->HasNamesx() &&
                            pIRCSock->IsPermChar(sNick[0])) {
                            // The server has NAMESX, but the client doesn't, so
                            // we just use the first perm char
                            size_t pos =
                                sNick.find_first_not_of(pIRCSock->GetPerms());
                            if (pos >= 2 && pos != CString::npos) {
                                sNick = sNick[0] + sNick.substr(pos);
                            }
                        }

                        if (!m_bUHNames && pIRCSock->HasUHNames()) {
                            // The server has UHNAMES, but the client doesn't,
                            // so we strip away ident and host
                            sNick = sNick.Token(0, false, "!");
                        }
                    }

                    Msg.SetParam(
                        3, CString(" ").Join(vsNicks.begin(), vsNicks.end()));
                }
            }
        } else if (Msg.GetType() == CMessage::Type::Join) {
            if (!m_bExtendedJoin && pIRCSock->HasExtendedJoin()) {
                Msg.SetParams({Msg.As<CJoinMessage>().GetTarget()});
            }
        }
    }

    MCString mssTags;

    for (const auto& it : Msg.GetTags()) {
        if (IsTagEnabled(it.first)) {
            mssTags[it.first] = it.second;
        }
    }

    if (HasServerTime()) {
        // If the server didn't set the time tag, manually set it
        mssTags.emplace("time", CUtils::FormatServerTime(Msg.GetTime()));
    }

    Msg.SetTags(mssTags);
    Msg.SetClient(this);
    Msg.SetNetwork(m_pNetwork);

    bool bReturn = false;
    NETWORKMODULECALL(OnSendToClientMessage(Msg), m_pUser, m_pNetwork, this,
                      &bReturn);
    if (bReturn) return false;

    return PutClientRaw(Msg.ToString());
}