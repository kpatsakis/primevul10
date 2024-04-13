BOOL Parser::DeferredParse(Js::LocalFunctionId functionId)
{
    if ((m_grfscr & fscrDeferFncParse) != 0)
    {
        if (m_stoppedDeferredParse)
        {
            return false;
        }
        if (PHASE_OFF_RAW(Js::DeferParsePhase, m_sourceContextInfo->sourceContextId, functionId))
        {
            return false;
        }
        if (PHASE_FORCE_RAW(Js::DeferParsePhase, m_sourceContextInfo->sourceContextId, functionId))
        {
            return true;
        }
#if ENABLE_PROFILE_INFO
#ifndef DISABLE_DYNAMIC_PROFILE_DEFER_PARSE
        if (m_sourceContextInfo->sourceDynamicProfileManager != nullptr)
        {
            Js::ExecutionFlags flags = m_sourceContextInfo->sourceDynamicProfileManager->IsFunctionExecuted(functionId);
            return flags != Js::ExecutionFlags_Executed;
        }
#endif
#endif
        return true;
    }

    return false;
}