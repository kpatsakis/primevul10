HRESULT Parser::ValidateSyntax(LPCUTF8 pszSrc, size_t encodedCharCount, bool isGenerator, bool isAsync, CompileScriptException *pse, void (Parser::*validateFunction)())
{
    AssertPsz(pszSrc);
    AssertMemN(pse);

    if (this->IsBackgroundParser())
    {
        PROBE_STACK_NO_DISPOSE(m_scriptContext, Js::Constants::MinStackDefault);
    }
    else
    {
        PROBE_STACK(m_scriptContext, Js::Constants::MinStackDefault);
    }

    HRESULT hr;
    SmartFPUControl smartFpuControl;

    DebugOnly( m_err.fInited = TRUE; )
    BOOL fDeferSave = m_deferringAST;
    try
    {
        hr = NOERROR;

        this->PrepareScanner(false);

        m_length = encodedCharCount;
        m_originalLength = encodedCharCount;

        // make sure deferred parsing is turned off
        ULONG grfscr = fscrNil;

        // Give the scanner the source and get the first token
        m_pscan->SetText(pszSrc, 0, encodedCharCount, 0, grfscr);
        m_pscan->SetYieldIsKeyword(isGenerator);
        m_pscan->SetAwaitIsKeyword(isAsync);
        m_pscan->Scan();

        uint nestedCount = 0;
        m_pnestedCount = &nestedCount;

        ParseNodePtr pnodeScope = nullptr;
        m_ppnodeScope = &pnodeScope;
        m_ppnodeExprScope = nullptr;

        uint nextFunctionId = 0;
        m_nextFunctionId = &nextFunctionId;

        m_inDeferredNestedFunc = false;
        m_deferringAST = true;



        m_nextBlockId = 0;

        ParseNode *pnodeFnc = CreateNode(knopFncDecl);
        pnodeFnc->sxFnc.ClearFlags();
        pnodeFnc->sxFnc.SetDeclaration(false);
        pnodeFnc->sxFnc.functionId   = 0;
        pnodeFnc->sxFnc.astSize      = 0;
        pnodeFnc->sxFnc.pnodeVars    = nullptr;
        pnodeFnc->sxFnc.pnodeParams  = nullptr;
        pnodeFnc->sxFnc.pnodeBody    = nullptr;
        pnodeFnc->sxFnc.pnodeName    = nullptr;
        pnodeFnc->sxFnc.pnodeRest    = nullptr;
        pnodeFnc->sxFnc.deferredStub = nullptr;
        pnodeFnc->sxFnc.SetIsGenerator(isGenerator);
        pnodeFnc->sxFnc.SetIsAsync(isAsync);
        m_ppnodeVar = &pnodeFnc->sxFnc.pnodeVars;
        m_currentNodeFunc = pnodeFnc;
        m_currentNodeDeferredFunc = NULL;
        m_sourceContextInfo = nullptr;
        AssertMsg(m_pstmtCur == NULL, "Statement stack should be empty when we start parse function body");

        ParseNodePtr block = StartParseBlock<false>(PnodeBlockType::Function, ScopeType_FunctionBody);
        (this->*validateFunction)();
        FinishParseBlock(block);

        pnodeFnc->ichLim = m_pscan->IchLimTok();
        pnodeFnc->sxFnc.cbLim = m_pscan->IecpLimTok();
        pnodeFnc->sxFnc.pnodeVars = nullptr;

        // there should be nothing after successful parsing for a given construct
        if (m_token.tk != tkEOF)
            Error(ERRsyntax);

        RELEASEPTR(m_pscan);
        m_deferringAST = fDeferSave;
    }
    catch(ParseExceptionObject& e)
    {
        m_deferringAST = fDeferSave;
        m_err.m_hr = e.GetError();
        hr = pse->ProcessError( m_pscan,  m_err.m_hr, /* pnodeBase */ NULL);
    }

    return hr;
}