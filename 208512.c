void Parser::SetNestedFuncEscapes() const
{
    if (m_sourceContextInfo ? 
            !PHASE_OFF_RAW(Js::DisableStackFuncOnDeferredEscapePhase, m_sourceContextInfo->sourceContextId, m_currentNodeFunc->sxFnc.functionId) :
            !PHASE_OFF1(Js::DisableStackFuncOnDeferredEscapePhase))
    {
        m_currentNodeFunc->sxFnc.SetNestedFuncEscapes();
    }
}