bool Parser::DoParallelParse(ParseNodePtr pnodeFnc) const
{
#if ENABLE_BACKGROUND_PARSING
    if (!PHASE_ON_RAW(Js::ParallelParsePhase, m_sourceContextInfo->sourceContextId, pnodeFnc->sxFnc.functionId))
    {
        return false;
    }

    BackgroundParser *bgp = m_scriptContext->GetBackgroundParser();
    return bgp != nullptr;
#else
    return false;
#endif
}