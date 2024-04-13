void Parser::ParseStmtList(ParseNodePtr *ppnodeList, ParseNodePtr **pppnodeLast, StrictModeEnvironment smEnvironment, const bool isSourceElementList, bool* strictModeOn)
{
    BOOL doneDirectives = !isSourceElementList; // directives may only exist in a SourceElementList, not a StatementList
    BOOL seenDirectiveContainingOctal = false; // Have we seen an octal directive before a use strict directive?

    BOOL old_UseStrictMode = m_fUseStrictMode;

    ParseNodePtr pnodeStmt;
    ParseNodePtr *lastNodeRef = nullptr;

    if (buildAST)
    {
        AssertMem(ppnodeList);
        AssertMemN(pppnodeLast);
        *ppnodeList = nullptr;
    }

    if(CONFIG_FLAG(ForceStrictMode))
    {
        m_fUseStrictMode = TRUE;
    }

    for (;;)
    {
        switch (m_token.tk)
        {
        case tkCASE:
        case tkDEFAULT:
        case tkRCurly:
        case tkEOF:
            if (buildAST && nullptr != pppnodeLast)
            {
                *pppnodeLast = lastNodeRef;
            }
            if (!buildAST)
            {
                m_fUseStrictMode = old_UseStrictMode;
            }
            return;
        }

        if (doneDirectives == FALSE)
        {
            bool isOctalInString = false;
            bool isUseStrictDirective = false;
            bool isUseAsmDirective = false;
            if (smEnvironment != SM_NotUsed && CheckForDirective(&isUseStrictDirective, &isUseAsmDirective, &isOctalInString))
            {
                // Ignore "use asm" statement when not building the AST
                isUseAsmDirective &= buildAST;

                if (isUseStrictDirective)
                {
                    // Functions with non-simple parameter list cannot be made strict mode
                    if (GetCurrentFunctionNode()->sxFnc.HasNonSimpleParameterList())
                    {
                        Error(ERRNonSimpleParamListInStrictMode);
                    }

                    if (seenDirectiveContainingOctal)
                    {
                        // Directives seen before a "use strict" cannot contain an octal.
                        Error(ERRES5NoOctal);
                    }
                    if (!buildAST)
                    {
                        // Turning on strict mode in deferred code.
                        m_fUseStrictMode = TRUE;
                        if (!m_inDeferredNestedFunc)
                        {
                            // Top-level deferred function, so there's a parse node
                            Assert(m_currentNodeFunc != nullptr);
                            m_currentNodeFunc->sxFnc.SetStrictMode();
                        }
                        else if (strictModeOn)
                        {
                            // This turns on strict mode in a deferred function, we need to go back
                            // and re-check duplicated formals.
                            *strictModeOn = true;
                        }
                    }
                    else
                    {
                        if (smEnvironment == SM_OnGlobalCode)
                        {
                            // Turning on strict mode at the top level
                            m_fUseStrictMode = TRUE;
                        }
                        else
                        {
                            // i.e. smEnvironment == SM_OnFunctionCode
                            Assert(m_currentNodeFunc != nullptr);
                            m_currentNodeFunc->sxFnc.SetStrictMode();
                        }
                    }
                }
                else if (isUseAsmDirective)
                {
                    if (smEnvironment != SM_OnGlobalCode) //Top level use asm doesn't mean anything.
                    {
                        // i.e. smEnvironment == SM_OnFunctionCode
                        Assert(m_currentNodeFunc != nullptr);
                        m_currentNodeFunc->sxFnc.SetAsmjsMode();
                        m_currentNodeFunc->sxFnc.SetCanBeDeferred(false);
                        m_InAsmMode = true;

                        CHAKRATEL_LANGSTATS_INC_LANGFEATURECOUNT(AsmJSFunction, m_scriptContext);
                    }
                }
                else if (isOctalInString)
                {
                    seenDirectiveContainingOctal = TRUE;
                }
            }
            else
            {
                // The first time we see anything other than a directive we can have no more directives.
                doneDirectives = TRUE;
            }
        }

        if (nullptr != (pnodeStmt = ParseStatement<buildAST>()))
        {
            if (buildAST)
            {
                AddToNodeList(ppnodeList, &lastNodeRef, pnodeStmt);
            }
        }
    }
}