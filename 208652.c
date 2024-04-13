void Parser::ReduceDeferredScriptLength(size_t chars)
{
    // If we're in deferred mode, subtract the given char count from the total length,
    // and see if this puts us under the deferral threshold.
    if ((m_grfscr & fscrDeferFncParse) &&
        (
            PHASE_OFF1(Js::DeferEventHandlersPhase) ||
            (m_grfscr & fscrGlobalCode)
        )
    )
    {
        if (m_length > chars)
        {
            m_length -= chars;
        }
        else
        {
            m_length = 0;
        }
        if (m_length < Parser::GetDeferralThreshold(this->m_sourceContextInfo->IsSourceProfileLoaded()))
        {
            // Stop deferring.
            m_grfscr &= ~fscrDeferFncParse;
            m_stoppedDeferredParse = TRUE;
        }
    }
}