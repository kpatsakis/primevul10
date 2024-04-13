void Parser::ParseTopLevelDeferredFunc(ParseNodePtr pnodeFnc, ParseNodePtr pnodeFncParent, LPCOLESTR pNameHint)
{
    // Parse a function body that is a transition point from building AST to doing fast syntax check.

    pnodeFnc->sxFnc.pnodeVars = nullptr;
    pnodeFnc->sxFnc.pnodeBody = nullptr;

    this->m_deferringAST = TRUE;

    // Put the scanner into "no hashing" mode.
    BYTE deferFlags = m_pscan->SetDeferredParse(TRUE);

    m_pscan->Scan();

    ChkCurTok(tkLCurly, ERRnoLcurly);

    ParseNodePtr *ppnodeVarSave = m_ppnodeVar;

    m_ppnodeVar = &pnodeFnc->sxFnc.pnodeVars;

    if (pnodeFncParent != nullptr
        && m_currDeferredStub != nullptr
        // We don't create stubs for function bodies in parameter scope.
        && pnodeFnc->sxFnc.pnodeScopes->sxBlock.blockType != PnodeBlockType::Parameter)
    {
        // We've already parsed this function body for syntax errors on the initial parse of the script.
        // We have information that allows us to skip it, so do so.

        DeferredFunctionStub *stub = m_currDeferredStub + (pnodeFncParent->sxFnc.nestedCount - 1);
        Assert(pnodeFnc->ichMin == stub->ichMin);
        if (stub->fncFlags & kFunctionCallsEval)
        {
            this->MarkEvalCaller();
        }
        if (stub->fncFlags & kFunctionChildCallsEval)
        {
            pnodeFnc->sxFnc.SetChildCallsEval(true);
        }
        if (stub->fncFlags & kFunctionHasWithStmt)
        {
            pnodeFnc->sxFnc.SetHasWithStmt(true);
        }

        PHASE_PRINT_TRACE1(
            Js::SkipNestedDeferredPhase,
            _u("Skipping nested deferred function %d. %s: %d...%d\n"),
            pnodeFnc->sxFnc.functionId, GetFunctionName(pnodeFnc, pNameHint), pnodeFnc->ichMin, stub->restorePoint.m_ichMinTok);

        m_pscan->SeekTo(stub->restorePoint, m_nextFunctionId);
        pnodeFnc->sxFnc.nestedCount = stub->nestedCount;
        pnodeFnc->sxFnc.deferredStub = stub->deferredStubs;
        if (stub->fncFlags & kFunctionStrictMode)
        {
            pnodeFnc->sxFnc.SetStrictMode(true);
        }
    }
    else
    {
        ParseStmtList<false>(nullptr, nullptr, SM_DeferredParse, true /* isSourceElementList */);
    }

    pnodeFnc->ichLim = m_pscan->IchLimTok();
    pnodeFnc->sxFnc.cbLim = m_pscan->IecpLimTok();

    m_ppnodeVar = ppnodeVarSave;

    // Restore the scanner's default hashing mode.
    // Do this before we consume the next token.
    m_pscan->SetDeferredParseFlags(deferFlags);

    ChkCurTokNoScan(tkRCurly, ERRnoRcurly);

#if DBG
    pnodeFnc->sxFnc.deferredParseNextFunctionId = *this->m_nextFunctionId;
#endif
    this->m_deferringAST = FALSE;
}