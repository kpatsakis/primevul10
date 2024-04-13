void Parser::FinishFncNode(ParseNodePtr pnodeFnc)
{
    AnalysisAssert(pnodeFnc);

    // Finish the AST for a function that was deferred earlier, but which we decided
    // to finish after the fact.
    // We assume that the name(s) and arg(s) have already got parse nodes, so
    // we just have to do the function body.

    // Save the current next function Id, and resume from the old one.
    Js::LocalFunctionId * nextFunctionIdSave = m_nextFunctionId;
    Js::LocalFunctionId tempNextFunctionId = pnodeFnc->sxFnc.functionId + 1;
    this->m_nextFunctionId = &tempNextFunctionId;

    ParseNodePtr pnodeFncSave = m_currentNodeFunc;
    uint *pnestedCountSave = m_pnestedCount;
    int32* pAstSizeSave = m_pCurrentAstSize;

    m_currentNodeFunc = pnodeFnc;
    m_pCurrentAstSize = & (pnodeFnc->sxFnc.astSize);

    pnodeFnc->sxFnc.nestedCount = 0;
    m_pnestedCount = &pnodeFnc->sxFnc.nestedCount;

    // Cue up the parser to the start of the function body.
    if (pnodeFnc->sxFnc.pnodeName)
    {
        // Skip the name(s).
        m_pscan->SetCurrentCharacter(pnodeFnc->sxFnc.pnodeName->ichLim, pnodeFnc->sxFnc.lineNumber);
    }
    else
    {
        m_pscan->SetCurrentCharacter(pnodeFnc->ichMin, pnodeFnc->sxFnc.lineNumber);
        if (pnodeFnc->sxFnc.IsAccessor())
        {
            // Getter/setter. The node text starts with the name, so eat that.
            m_pscan->ScanNoKeywords();
        }
        else
        {
            // Anonymous function. Skip any leading "("'s and "function".
            for (;;)
            {
                m_pscan->Scan();
                if (m_token.GetIdentifier(m_phtbl) == wellKnownPropertyPids.async)
                {
                    Assert(pnodeFnc->sxFnc.IsAsync());
                    continue;
                }
                if (m_token.tk == tkFUNCTION)
                {
                    break;
                }
                Assert(m_token.tk == tkLParen || m_token.tk == tkStar);
            }
        }
    }

    // switch scanner to treat 'yield' as keyword in generator functions
    // or as an identifier in non-generator functions
    bool fPreviousYieldIsKeyword = m_pscan->SetYieldIsKeyword(pnodeFnc && pnodeFnc->sxFnc.IsGenerator());

    bool fPreviousAwaitIsKeyword = m_pscan->SetAwaitIsKeyword(pnodeFnc && pnodeFnc->sxFnc.IsAsync());

    // Skip the arg list.
    m_pscan->ScanNoKeywords();
    if (m_token.tk == tkStar)
    {
        Assert(pnodeFnc->sxFnc.IsGenerator());
        m_pscan->ScanNoKeywords();
    }
    Assert(m_token.tk == tkLParen);
    m_pscan->ScanNoKeywords();

    if (m_token.tk != tkRParen)
    {
        for (;;)
        {
            if (m_token.tk == tkEllipsis)
            {
                m_pscan->ScanNoKeywords();
            }

            if (m_token.tk == tkID)
            {
                m_pscan->ScanNoKeywords();

                if (m_token.tk == tkAsg)
                {
                    // Eat the default expression
                    m_pscan->Scan();
                    ParseExpr<false>(koplCma);
                }
            }
            else if (IsPossiblePatternStart())
            {
                ParseDestructuredLiteralWithScopeSave(tkLET, false/*isDecl*/, false /*topLevel*/);
            }
            else
            {
                AssertMsg(false, "Unexpected identifier prefix while fast-scanning formals");
            }

            if (m_token.tk != tkComma)
            {
                break;
            }
            m_pscan->ScanNoKeywords();

            if (m_token.tk == tkRParen && m_scriptContext->GetConfig()->IsES7TrailingCommaEnabled())
            {
                break;
            }
        }
    }

    if (m_token.tk == tkRParen) // This might be false due to a lambda => token.
    {
        m_pscan->Scan();
    }

    // Finish the function body.
    {
        // Note that in IE8- modes, surrounding parentheses are considered part of function body. e.g. "( function x(){} )".
        // We lose that context here since we start from middle of function body. So save and restore source range info.
        ParseNodePtr* lastNodeRef = NULL;
        const charcount_t ichLim = pnodeFnc->ichLim;
        const size_t cbLim = pnodeFnc->sxFnc.cbLim;
        this->FinishFncDecl(pnodeFnc, NULL, lastNodeRef);

#if DBG
        // The pnode extent may not match the original extent.
        // We expect this to happen only when there are trailing ")"'s.
        // Consume them and make sure that's all we've got.
        if (pnodeFnc->ichLim != ichLim)
        {
            Assert(pnodeFnc->ichLim < ichLim);
            m_pscan->SetCurrentCharacter(pnodeFnc->ichLim);
            while (m_pscan->IchLimTok() != ichLim)
            {
                m_pscan->ScanNoKeywords();
                Assert(m_token.tk == tkRParen);
            }
        }
#endif
        pnodeFnc->ichLim = ichLim;
        pnodeFnc->sxFnc.cbLim = cbLim;
    }

    m_currentNodeFunc = pnodeFncSave;
    m_pCurrentAstSize = pAstSizeSave;
    m_pnestedCount = pnestedCountSave;
    Assert(m_pnestedCount);

    Assert(tempNextFunctionId == pnodeFnc->sxFnc.deferredParseNextFunctionId);
    this->m_nextFunctionId = nextFunctionIdSave;

    m_pscan->SetYieldIsKeyword(fPreviousYieldIsKeyword);
    m_pscan->SetAwaitIsKeyword(fPreviousAwaitIsKeyword);
}