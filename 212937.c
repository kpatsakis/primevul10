bool CClient::OnOtherMessage(CMessage& Message) {
    const CString& sCommand = Message.GetCommand();

    if (sCommand.Equals("ZNC")) {
        CString sTarget = Message.GetParam(0);
        CString sModCommand;

        if (sTarget.TrimPrefix(m_pUser->GetStatusPrefix())) {
            sModCommand = Message.GetParamsColon(1);
        } else {
            sTarget = "status";
            sModCommand = Message.GetParamsColon(0);
        }

        if (sTarget.Equals("status")) {
            if (sModCommand.empty())
                PutStatus(t_s("Hello. How may I help you?"));
            else
                UserCommand(sModCommand);
        } else {
            if (sModCommand.empty())
                CALLMOD(sTarget, this, m_pUser, m_pNetwork,
                        PutModule(t_s("Hello. How may I help you?")))
            else
                CALLMOD(sTarget, this, m_pUser, m_pNetwork,
                        OnModCommand(sModCommand))
        }
        return true;
    } else if (sCommand.Equals("ATTACH")) {
        if (!m_pNetwork) {
            return true;
        }

        CString sPatterns = Message.GetParamsColon(0);

        if (sPatterns.empty()) {
            PutStatusNotice(t_s("Usage: /attach <#chans>"));
            return true;
        }

        set<CChan*> sChans = MatchChans(sPatterns);
        unsigned int uAttachedChans = AttachChans(sChans);

        PutStatusNotice(t_p("There was {1} channel matching [{2}]",
                            "There were {1} channels matching [{2}]",
                            sChans.size())(sChans.size(), sPatterns));
        PutStatusNotice(t_p("Attached {1} channel", "Attached {1} channels",
                            uAttachedChans)(uAttachedChans));

        return true;
    } else if (sCommand.Equals("DETACH")) {
        if (!m_pNetwork) {
            return true;
        }

        CString sPatterns = Message.GetParamsColon(0);

        if (sPatterns.empty()) {
            PutStatusNotice(t_s("Usage: /detach <#chans>"));
            return true;
        }

        set<CChan*> sChans = MatchChans(sPatterns);
        unsigned int uDetached = DetachChans(sChans);

        PutStatusNotice(t_p("There was {1} channel matching [{2}]",
                            "There were {1} channels matching [{2}]",
                            sChans.size())(sChans.size(), sPatterns));
        PutStatusNotice(t_p("Detached {1} channel", "Detached {1} channels",
                            uDetached)(uDetached));

        return true;
    } else if (sCommand.Equals("PROTOCTL")) {
        for (const CString& sParam : Message.GetParams()) {
            if (sParam == "NAMESX") {
                m_bNamesx = true;
            } else if (sParam == "UHNAMES") {
                m_bUHNames = true;
            }
        }
        return true;  // If the server understands it, we already enabled namesx
                      // / uhnames
    }

    return false;
}