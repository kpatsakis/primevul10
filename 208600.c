void Parser::ParseFncFormals(ParseNodePtr pnodeFnc, ParseNodePtr pnodeParentFnc, ushort flags)
{
    bool fLambda = (flags & fFncLambda) != 0;
    bool fMethod = (flags & fFncMethod) != 0;
    bool fNoArg = (flags & fFncNoArg) != 0;
    bool fOneArg = (flags & fFncOneArg) != 0;
    bool fAsync = (flags & fFncAsync) != 0;

    bool fPreviousYieldIsKeyword = false;
    bool fPreviousAwaitIsKeyword = false;

    if (fLambda)
    {
        fPreviousYieldIsKeyword = m_pscan->SetYieldIsKeyword(pnodeParentFnc != nullptr && pnodeParentFnc->sxFnc.IsGenerator());
        fPreviousAwaitIsKeyword = m_pscan->SetAwaitIsKeyword(fAsync || (pnodeParentFnc != nullptr && pnodeParentFnc->sxFnc.IsAsync()));
    }

    Assert(!fNoArg || !fOneArg); // fNoArg and fOneArg can never be true at the same time.

    // strictFormals corresponds to the StrictFormalParameters grammar production
    // in the ES spec which just means duplicate names are not allowed
    bool fStrictFormals = IsStrictMode() || fLambda || fMethod;

    // When detecting duplicated formals pids are needed so force PID creation (unless the function should take 0 or 1 arg).
    bool forcePid = fStrictFormals && !fNoArg && !fOneArg;
    AutoTempForcePid autoForcePid(m_pscan, forcePid);

    // Lambda's allow single formal specified by a single binding identifier without parentheses, special case it.
    if (fLambda && m_token.tk == tkID)
    {
        IdentPtr pid = m_token.GetIdentifier(m_phtbl);

        CreateVarDeclNode(pid, STFormal, false, nullptr, false);
        CheckPidIsValid(pid);

        m_pscan->Scan();

        if (m_token.tk != tkDArrow)
        {
            Error(ERRsyntax, m_pscan->IchMinTok(), m_pscan->IchLimTok());
        }

        if (fLambda)
        {
            m_pscan->SetYieldIsKeyword(fPreviousYieldIsKeyword);
            m_pscan->SetAwaitIsKeyword(fPreviousAwaitIsKeyword);
        }

        return;
    }
    else if (fLambda && m_token.tk == tkAWAIT)
    {
        // async await => {}
        IdentifierExpectedError(m_token);
    }

    // Otherwise, must have a parameter list within parens.
    ChkCurTok(tkLParen, ERRnoLparen);

    // Now parse the list of arguments, if present
    if (m_token.tk == tkRParen)
    {
        if (fOneArg)
        {
            Error(ERRSetterMustHaveOneParameter);
        }
    }
    else
    {
        if (fNoArg)
        {
            Error(ERRGetterMustHaveNoParameters);
        }
        SList<IdentPtr> formals(&m_nodeAllocator);
        ParseNodePtr pnodeT = nullptr;
        bool seenRestParameter = false;
        bool isNonSimpleParameterList = false;
        for (Js::ArgSlot argPos = 0; ; ++argPos)
        {
            bool isBindingPattern = false;
            if (m_scriptContext->GetConfig()->IsES6RestEnabled() && m_token.tk == tkEllipsis)
            {
                // Possible rest parameter
                m_pscan->Scan();
                seenRestParameter = true;
            }
            if (m_token.tk != tkID)
            {
                if (IsES6DestructuringEnabled() && IsPossiblePatternStart())
                {
                    // Mark that the function has a non simple parameter list before parsing the pattern since the pattern can have function definitions.
                    this->GetCurrentFunctionNode()->sxFnc.SetHasNonSimpleParameterList();

                    ParseNodePtr *const ppnodeVarSave = m_ppnodeVar;
                    m_ppnodeVar = &pnodeFnc->sxFnc.pnodeVars;

                    ParseNodePtr * ppNodeLex = m_currentBlockInfo->m_ppnodeLex;
                    Assert(ppNodeLex != nullptr);

                    ParseNodePtr paramPattern = nullptr;
                    ParseNodePtr pnodePattern = ParseDestructuredLiteral<buildAST>(tkLET, true /*isDecl*/, false /*topLevel*/);

                    // Instead of passing the STFormal all the way on many methods, it seems it is better to change the symbol type afterward.
                    for (ParseNodePtr lexNode = *ppNodeLex; lexNode != nullptr; lexNode = lexNode->sxVar.pnodeNext)
                    {
                        Assert(lexNode->IsVarLetOrConst());
                        UpdateOrCheckForDuplicateInFormals(lexNode->sxVar.pid, &formals);
                        lexNode->sxVar.sym->SetSymbolType(STFormal);
                        if (m_currentNodeFunc != nullptr && lexNode->sxVar.pid == wellKnownPropertyPids.arguments)
                        {
                            m_currentNodeFunc->grfpn |= PNodeFlags::fpnArguments_overriddenByDecl;
                        }
                    }

                    m_ppnodeVar = ppnodeVarSave;
                    if (buildAST)
                    {
                        paramPattern = CreateParamPatternNode(pnodePattern);

                        // Linking the current formal parameter (which is pattern parameter) with other formals.
                        *m_ppnodeVar = paramPattern;
                        paramPattern->sxParamPattern.pnodeNext = nullptr;
                        m_ppnodeVar = &paramPattern->sxParamPattern.pnodeNext;
                    }

                    isBindingPattern = true;
                    isNonSimpleParameterList = true;
                }
                else
                {
                    IdentifierExpectedError(m_token);
                }
            }

            if (!isBindingPattern)
            {
                IdentPtr pid = m_token.GetIdentifier(m_phtbl);
                LPCOLESTR pNameHint = pid->Psz();
                uint32 nameHintLength = pid->Cch();
                uint32 nameHintOffset = 0;

                if (seenRestParameter)
                {
                    this->GetCurrentFunctionNode()->sxFnc.SetHasNonSimpleParameterList();
                    if (flags & fFncOneArg)
                    {
                        // The parameter of a setter cannot be a rest parameter.
                        Error(ERRUnexpectedEllipsis);
                    }
                    pnodeT = CreateDeclNode(knopVarDecl, pid, STFormal, false);
                    pnodeT->sxVar.sym->SetIsNonSimpleParameter(true);
                    if (buildAST)
                    {
                        // When only validating formals, we won't have a function node.
                        pnodeFnc->sxFnc.pnodeRest = pnodeT;
                        if (!isNonSimpleParameterList)
                        {
                            // This is the first non-simple parameter we've seen. We need to go back
                            // and set the Symbols of all previous parameters.
                            MapFormalsWithoutRest(m_currentNodeFunc, [&](ParseNodePtr pnodeArg) { pnodeArg->sxVar.sym->SetIsNonSimpleParameter(true); });
                        }
                    }

                    isNonSimpleParameterList = true;
                }
                else
                {
                    pnodeT = CreateVarDeclNode(pid, STFormal, false, nullptr, false);
                    if (isNonSimpleParameterList)
                    {
                        pnodeT->sxVar.sym->SetIsNonSimpleParameter(true);
                    }
                }

                if (buildAST && pid == wellKnownPropertyPids.arguments)
                {
                    // This formal parameter overrides the built-in 'arguments' object
                    m_currentNodeFunc->grfpn |= PNodeFlags::fpnArguments_overriddenByDecl;
                }

                if (fStrictFormals)
                {
                    UpdateOrCheckForDuplicateInFormals(pid, &formals);
                }

                m_pscan->Scan();

                if (seenRestParameter && m_token.tk != tkRParen && m_token.tk != tkAsg)
                {
                    Error(ERRRestLastArg);
                }

                if (m_token.tk == tkAsg && m_scriptContext->GetConfig()->IsES6DefaultArgsEnabled())
                {
                    if (seenRestParameter && m_scriptContext->GetConfig()->IsES6RestEnabled())
                    {
                        Error(ERRRestWithDefault);
                    }

                    // In defer parse mode we have to flag the function node to indicate that it has default arguments
                    // so that it will be considered for any syntax error scenario.
                    // Also mark it before parsing the expression as it may contain functions.
                    ParseNode* currentFncNode = GetCurrentFunctionNode();
                    if (!currentFncNode->sxFnc.HasDefaultArguments())
                    {
                        currentFncNode->sxFnc.SetHasDefaultArguments();
                        currentFncNode->sxFnc.SetHasNonSimpleParameterList();
                        currentFncNode->sxFnc.firstDefaultArg = argPos;
                    }

                    m_pscan->Scan();
                    ParseNodePtr pnodeInit = ParseExpr<buildAST>(koplCma, nullptr, TRUE, FALSE, pNameHint, &nameHintLength, &nameHintOffset);

                    if (buildAST && pnodeInit->nop == knopFncDecl)
                    {
                        Assert(nameHintLength >= nameHintOffset);
                        pnodeInit->sxFnc.hint = pNameHint;
                        pnodeInit->sxFnc.hintLength = nameHintLength;
                        pnodeInit->sxFnc.hintOffset = nameHintOffset;
                    }

                    AnalysisAssert(pnodeT);
                    pnodeT->sxVar.sym->SetIsNonSimpleParameter(true);
                    if (!isNonSimpleParameterList)
                    {
                        if (buildAST)
                        {
                            // This is the first non-simple parameter we've seen. We need to go back
                            // and set the Symbols of all previous parameters.
                            MapFormalsWithoutRest(m_currentNodeFunc, [&](ParseNodePtr pnodeArg) { pnodeArg->sxVar.sym->SetIsNonSimpleParameter(true); });
                        }

                        // There may be previous parameters that need to be checked for duplicates.
                        isNonSimpleParameterList = true;
                    }

                    if (buildAST)
                    {
                        if (!m_currentNodeFunc->sxFnc.HasDefaultArguments())
                        {
                            CHAKRATEL_LANGSTATS_INC_LANGFEATURECOUNT(DefaultArgFunction, m_scriptContext);
                        }
                        pnodeT->sxVar.pnodeInit = pnodeInit;
                        pnodeT->ichLim = m_pscan->IchLimTok();
                    }
                }
            }

            if (isNonSimpleParameterList && m_currentScope->GetHasDuplicateFormals())
            {
                Error(ERRFormalSame);
            }

            if (flags & fFncOneArg)
            {
                if (m_token.tk != tkRParen)
                {
                    Error(ERRSetterMustHaveOneParameter);
                }
                break; //enforce only one arg
            }

            if (m_token.tk != tkComma)
            {
                break;
            }

            m_pscan->Scan();

            if (m_token.tk == tkRParen && m_scriptContext->GetConfig()->IsES7TrailingCommaEnabled())
            {
                break;
            }
        }

        if (seenRestParameter)
        {
            CHAKRATEL_LANGSTATS_INC_LANGFEATURECOUNT(Rest, m_scriptContext);
        }

        if (m_token.tk != tkRParen)
        {
            Error(ERRnoRparen);
        }

        if (this->GetCurrentFunctionNode()->sxFnc.CallsEval() || this->GetCurrentFunctionNode()->sxFnc.ChildCallsEval())
        {
            if (!m_scriptContext->GetConfig()->IsES6DefaultArgsSplitScopeEnabled())
            {
                Error(ERREvalNotSupportedInParamScope);
            }
            else
            {
                Assert(pnodeFnc->sxFnc.HasNonSimpleParameterList());
                pnodeFnc->sxFnc.pnodeScopes->sxBlock.scope->SetCannotMergeWithBodyScope();
            }
        }
    }
    Assert(m_token.tk == tkRParen);

    if (fLambda)
    {
        m_pscan->SetYieldIsKeyword(fPreviousYieldIsKeyword);
        m_pscan->SetAwaitIsKeyword(fPreviousAwaitIsKeyword);
    }
}