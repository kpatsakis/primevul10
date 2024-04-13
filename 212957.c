void CClient::AddBuffer(const T& Message) {
    const CString sTarget = Message.GetTarget();

    T Format;
    Format.Clone(Message);
    Format.SetNick(CNick(_NAMEDFMT(GetNickMask())));
    Format.SetTarget(_NAMEDFMT(sTarget));
    Format.SetText("{text}");

    CChan* pChan = m_pNetwork->FindChan(sTarget);
    if (pChan) {
        if (!pChan->AutoClearChanBuffer() || !m_pNetwork->IsUserOnline()) {
            pChan->AddBuffer(Format, Message.GetText());
        }
    } else if (Message.GetType() != CMessage::Type::Notice) {
        if (!m_pUser->AutoClearQueryBuffer() || !m_pNetwork->IsUserOnline()) {
            CQuery* pQuery = m_pNetwork->AddQuery(sTarget);
            if (pQuery) {
                pQuery->AddBuffer(Format, Message.GetText());
            }
        }
    }
}