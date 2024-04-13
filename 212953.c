void CClient::HandleCap(const CMessage& Message) {
    CString sSubCmd = Message.GetParam(0);

    if (sSubCmd.Equals("LS")) {
        SCString ssOfferCaps;
        for (const auto& it : m_mCoreCaps) {
            bool bServerDependent = std::get<0>(it.second);
            if (!bServerDependent ||
                m_ssServerDependentCaps.count(it.first) > 0)
                ssOfferCaps.insert(it.first);
        }
        GLOBALMODULECALL(OnClientCapLs(this, ssOfferCaps), NOTHING);
        CString sRes =
            CString(" ").Join(ssOfferCaps.begin(), ssOfferCaps.end());
        RespondCap("LS :" + sRes);
        m_bInCap = true;
        if (Message.GetParam(1).ToInt() >= 302) {
            m_bCapNotify = true;
        }
    } else if (sSubCmd.Equals("END")) {
        m_bInCap = false;
        if (!IsAttached()) {
            if (!m_pUser && m_bGotUser && !m_bGotPass) {
                SendRequiredPasswordNotice();
            } else {
                AuthUser();
            }
        }
    } else if (sSubCmd.Equals("REQ")) {
        VCString vsTokens;
        Message.GetParam(1).Split(" ", vsTokens, false);

        for (const CString& sToken : vsTokens) {
            bool bVal = true;
            CString sCap = sToken;
            if (sCap.TrimPrefix("-")) bVal = false;

            bool bAccepted = false;
            const auto& it = m_mCoreCaps.find(sCap);
            if (m_mCoreCaps.end() != it) {
                bool bServerDependent = std::get<0>(it->second);
                bAccepted = !bServerDependent ||
                            m_ssServerDependentCaps.count(sCap) > 0;
            }
            GLOBALMODULECALL(IsClientCapSupported(this, sCap, bVal),
                             &bAccepted);

            if (!bAccepted) {
                // Some unsupported capability is requested
                RespondCap("NAK :" + Message.GetParam(1));
                return;
            }
        }

        // All is fine, we support what was requested
        for (const CString& sToken : vsTokens) {
            bool bVal = true;
            CString sCap = sToken;
            if (sCap.TrimPrefix("-")) bVal = false;

            auto handler_it = m_mCoreCaps.find(sCap);
            if (m_mCoreCaps.end() != handler_it) {
                const auto& handler = std::get<1>(handler_it->second);
                handler(bVal);
            }
            GLOBALMODULECALL(OnClientCapRequest(this, sCap, bVal), NOTHING);

            if (bVal) {
                m_ssAcceptedCaps.insert(sCap);
            } else {
                m_ssAcceptedCaps.erase(sCap);
            }
        }

        RespondCap("ACK :" + Message.GetParam(1));
    } else if (sSubCmd.Equals("LIST")) {
        CString sList =
            CString(" ").Join(m_ssAcceptedCaps.begin(), m_ssAcceptedCaps.end());
        RespondCap("LIST :" + sList);
    } else {
        PutClient(":irc.znc.in 410 " + GetNick() + " " + sSubCmd +
                  " :Invalid CAP subcommand");
    }
}