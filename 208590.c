void Parser::FinishFncDecl(ParseNodePtr pnodeFnc, LPCOLESTR pNameHint, ParseNodePtr *lastNodeRef, bool skipCurlyBraces)
{
    LPCOLESTR name = NULL;
    JS_ETW(int32 startAstSize = *m_pCurrentAstSize);
    if (IS_JS_ETW(EventEnabledJSCRIPT_PARSE_METHOD_START()) || PHASE_TRACE1(Js::DeferParsePhase))
    {
        name = GetFunctionName(pnodeFnc, pNameHint);
        m_functionBody = NULL;  // for nested functions we do not want to get the name of the top deferred function return name;
        JS_ETW(EventWriteJSCRIPT_PARSE_METHOD_START(m_sourceContextInfo->dwHostSourceContext, GetScriptContext(), pnodeFnc->sxFnc.functionId, 0, m_parseType, name));
        OUTPUT_TRACE(Js::DeferParsePhase, _u("Parsing function (%s) : %s (%d)\n"), GetParseType(), name, pnodeFnc->sxFnc.functionId);
    }

    JS_ETW_INTERNAL(EventWriteJSCRIPT_PARSE_FUNC(GetScriptContext(), pnodeFnc->sxFnc.functionId, /*Undefer*/FALSE));


    // Do the work of creating an AST for a function body.
    // This is common to the un-deferred case and the case in which we un-defer late in the game.

    Assert(pnodeFnc->nop == knopFncDecl);

    if (!skipCurlyBraces)
    {
        ChkCurTok(tkLCurly, ERRnoLcurly);
    }

    ParseStmtList<true>(&pnodeFnc->sxFnc.pnodeBody, &lastNodeRef, SM_OnFunctionCode, true /* isSourceElementList */);
    // Append an EndCode node.
    AddToNodeList(&pnodeFnc->sxFnc.pnodeBody, &lastNodeRef, CreateNodeWithScanner<knopEndCode>());

    if (!skipCurlyBraces)
    {
        ChkCurTokNoScan(tkRCurly, ERRnoRcurly);
    }

    pnodeFnc->ichLim = m_pscan->IchLimTok();
    pnodeFnc->sxFnc.cbLim = m_pscan->IecpLimTok();

#ifdef ENABLE_JS_ETW
    int32 astSize = *m_pCurrentAstSize - startAstSize;
    EventWriteJSCRIPT_PARSE_METHOD_STOP(m_sourceContextInfo->dwHostSourceContext, GetScriptContext(), pnodeFnc->sxFnc.functionId, astSize, m_parseType, name);
#endif
}