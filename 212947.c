bool CClient::OnCTCPMessage(CCTCPMessage& Message) {
    CString sTargets = Message.GetTarget();

    VCString vTargets;
    sTargets.Split(",", vTargets, false);

    if (Message.IsReply()) {
        CString sCTCP = Message.GetText();
        if (sCTCP.Token(0) == "VERSION") {
            // There are 2 different scenarios:
            //
            // a) CTCP reply for VERSION is not set.
            // 1. ZNC receives CTCP VERSION from someone
            // 2. ZNC forwards CTCP VERSION to client
            // 3. Client replies with something
            // 4. ZNC adds itself to the reply
            // 5. ZNC sends the modified reply to whoever asked
            //
            // b) CTCP reply for VERSION is set.
            // 1. ZNC receives CTCP VERSION from someone
            // 2. ZNC replies with the configured reply (or just drops it if
            //    empty), without forwarding anything to client
            // 3. Client does not see any CTCP request, and does not reply
            //
            // So, if user doesn't want "via ZNC" in CTCP VERSION reply, they
            // can set custom reply.
            //
            // See more bikeshedding at github issues #820 and #1012
            Message.SetText(sCTCP + " via " + CZNC::GetTag(false));
        }
    }

    for (CString& sTarget : vTargets) {
        Message.SetTarget(sTarget);
        if (m_pNetwork) {
            // May be nullptr.
            Message.SetChan(m_pNetwork->FindChan(sTarget));
        }

        bool bContinue = false;
        if (Message.IsReply()) {
            NETWORKMODULECALL(OnUserCTCPReplyMessage(Message), m_pUser,
                              m_pNetwork, this, &bContinue);
        } else {
            NETWORKMODULECALL(OnUserCTCPMessage(Message), m_pUser, m_pNetwork,
                              this, &bContinue);
        }
        if (bContinue) continue;

        if (!GetIRCSock()) {
            // Some lagmeters do a NOTICE to their own nick, ignore those.
            if (!sTarget.Equals(m_sNick))
                PutStatus(t_f(
                    "Your CTCP to {1} got lost, you are not connected to IRC!")(
                    Message.GetTarget()));
            continue;
        }

        if (m_pNetwork) {
            PutIRC(Message.ToString(CMessage::ExcludePrefix |
                                    CMessage::ExcludeTags));
        }
    }

    return true;
}