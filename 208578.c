ParseNodePtr Parser::ParseFncDecl(ushort flags, LPCOLESTR pNameHint, const bool needsPIDOnRCurlyScan, bool resetParsingSuperRestrictionState, bool fUnaryOrParen)
{
    AutoParsingSuperRestrictionStateRestorer restorer(this);
    if (resetParsingSuperRestrictionState)
    {
        //  ParseFncDecl will always reset m_parsingSuperRestrictionState to super disallowed unless explicitly disabled
        this->m_parsingSuperRestrictionState = ParsingSuperRestrictionState_SuperDisallowed;
    }

    ParseNodePtr pnodeFnc = nullptr;
    ParseNodePtr *ppnodeVarSave = nullptr;
    ParseNodePtr pnodeFncBlockScope = nullptr;
    ParseNodePtr *ppnodeScopeSave = nullptr;
    ParseNodePtr *ppnodeExprScopeSave = nullptr;
    bool funcHasName = false;
    bool fDeclaration = flags & fFncDeclaration;
    bool fModule = (flags & fFncModule) != 0;
    bool fLambda = (flags & fFncLambda) != 0;
    charcount_t ichMin = this->m_pscan->IchMinTok();
    bool wasInDeferredNestedFunc = false;

    uint tryCatchOrFinallyDepthSave = this->m_tryCatchOrFinallyDepth;
    this->m_tryCatchOrFinallyDepth = 0;

    if (this->m_arrayDepth)
    {
        this->m_funcInArrayDepth++; // Count function depth within array literal
    }

    // Update the count of functions nested in the current parent.
    Assert(m_pnestedCount || !buildAST);
    uint *pnestedCountSave = m_pnestedCount;
    if (buildAST || m_pnestedCount)
    {
        (*m_pnestedCount)++;
    }

    uint scopeCountNoAstSave = m_scopeCountNoAst;
    m_scopeCountNoAst = 0;

    bool noStmtContext = false;

    if (fDeclaration)
    {
        noStmtContext = m_pstmtCur->GetNop() != knopBlock;

        if (noStmtContext)
        {
            // We have a function declaration like "if (a) function f() {}". We didn't see
            // a block scope on the way in, so we need to pretend we did. Note that this is a syntax error
            // in strict mode.
            if (!this->FncDeclAllowedWithoutContext(flags))
            {
                Error(ERRsyntax);
            }
            pnodeFncBlockScope = StartParseBlock<buildAST>(PnodeBlockType::Regular, ScopeType_Block);
            if (buildAST)
            {
                PushFuncBlockScope(pnodeFncBlockScope, &ppnodeScopeSave, &ppnodeExprScopeSave);
            }
        }
    }

    // Create the node.
    pnodeFnc = CreateNode(knopFncDecl);
    pnodeFnc->sxFnc.ClearFlags();
    pnodeFnc->sxFnc.SetDeclaration(fDeclaration);
    pnodeFnc->sxFnc.astSize             = 0;
    pnodeFnc->sxFnc.pnodeName           = nullptr;
    pnodeFnc->sxFnc.pnodeScopes         = nullptr;
    pnodeFnc->sxFnc.pnodeRest           = nullptr;
    pnodeFnc->sxFnc.pid                 = nullptr;
    pnodeFnc->sxFnc.hint                = nullptr;
    pnodeFnc->sxFnc.hintOffset          = 0;
    pnodeFnc->sxFnc.hintLength          = 0;
    pnodeFnc->sxFnc.isNameIdentifierRef = true;
    pnodeFnc->sxFnc.nestedFuncEscapes   = false;
    pnodeFnc->sxFnc.pnodeNext           = nullptr;
    pnodeFnc->sxFnc.pnodeParams         = nullptr;
    pnodeFnc->sxFnc.pnodeVars           = nullptr;
    pnodeFnc->sxFnc.funcInfo            = nullptr;
    pnodeFnc->sxFnc.deferredStub        = nullptr;
    pnodeFnc->sxFnc.nestedCount         = 0;
    pnodeFnc->sxFnc.cbMin = m_pscan->IecpMinTok();
    pnodeFnc->sxFnc.functionId = (*m_nextFunctionId)++;

    // Push new parser state with this new function node

    AppendFunctionToScopeList(fDeclaration, pnodeFnc);

    // Start the argument list.
    ppnodeVarSave = m_ppnodeVar;

    if (buildAST)
    {
        pnodeFnc->sxFnc.lineNumber = m_pscan->LineCur();
        pnodeFnc->sxFnc.columnNumber = CalculateFunctionColumnNumber();
        pnodeFnc->sxFnc.SetNested(m_currentNodeFunc != nullptr); // If there is a current function, then we're a nested function.
        pnodeFnc->sxFnc.SetStrictMode(IsStrictMode()); // Inherit current strict mode -- may be overridden by the function itself if it contains a strict mode directive.
        pnodeFnc->sxFnc.firstDefaultArg = 0;

        m_pCurrentAstSize = &pnodeFnc->sxFnc.astSize;
    }
    else // if !buildAST
    {
        wasInDeferredNestedFunc = m_inDeferredNestedFunc;
        m_inDeferredNestedFunc = true;
    }

    m_pnestedCount = &pnodeFnc->sxFnc.nestedCount;

    AnalysisAssert(pnodeFnc);
    pnodeFnc->sxFnc.SetIsAsync((flags & fFncAsync) != 0);
    pnodeFnc->sxFnc.SetIsLambda(fLambda);
    pnodeFnc->sxFnc.SetIsMethod((flags & fFncMethod) != 0);
    pnodeFnc->sxFnc.SetIsClassMember((flags & fFncClassMember) != 0);
    pnodeFnc->sxFnc.SetIsModule(fModule);

    bool needScanRCurly = true;
    bool result = ParseFncDeclHelper<buildAST>(pnodeFnc, pNameHint, flags, &funcHasName, fUnaryOrParen, noStmtContext, &needScanRCurly, fModule);
    if (!result)
    {
        Assert(!pnodeFncBlockScope);

        return pnodeFnc;
    }

    AnalysisAssert(pnodeFnc);

    *m_ppnodeVar = nullptr;
    m_ppnodeVar = ppnodeVarSave;

    if (m_currentNodeFunc && (pnodeFnc->sxFnc.CallsEval() || pnodeFnc->sxFnc.ChildCallsEval()))
    {
        GetCurrentFunctionNode()->sxFnc.SetChildCallsEval(true);
    }

    ParseNodePtr pnodeFncParent = buildAST ? m_currentNodeFunc : m_currentNodeDeferredFunc;

    // Lambdas do not have "arguments" and instead capture their parent's
    // binding of "arguments.  To ensure the arguments object of the enclosing
    // non-lambda function is loaded propagate the UsesArguments flag up to
    // the parent function
    if ((flags & fFncLambda) != 0 && pnodeFnc->sxFnc.UsesArguments())
    {
        if (pnodeFncParent != nullptr)
        {
            pnodeFncParent->sxFnc.SetUsesArguments();
        }
        else
        {
            m_UsesArgumentsAtGlobal = true;
        }
    }

    if (needScanRCurly && !fModule)
    {
        // Consume the next token now that we're back in the enclosing function (whose strictness may be
        // different from the function we just finished).
#if DBG
        bool expectedTokenValid = m_token.tk == tkRCurly;
        AssertMsg(expectedTokenValid, "Invalid token expected for RCurly match");
#endif
        // The next token may need to have a PID created in !buildAST mode, as we may be parsing a method with a string name.
        if (needsPIDOnRCurlyScan)
        {
            m_pscan->ScanForcingPid();
        }
        else
        {
            m_pscan->Scan();
        }
    }

    m_pnestedCount = pnestedCountSave;
    Assert(!buildAST || !wasInDeferredNestedFunc);
    m_inDeferredNestedFunc = wasInDeferredNestedFunc;

    if (this->m_arrayDepth)
    {
        this->m_funcInArrayDepth--;
        if (this->m_funcInArrayDepth == 0)
        {
            // We disable deferred parsing if array literals dominate.
            // But don't do this if the array literal is dominated by function bodies.
            if (flags & (fFncMethod | fFncClassMember) && m_token.tk != tkSColon)
            {
                // Class member methods have optional separators. We need to check whether we are
                // getting the IchLim of the correct token.
                Assert(m_pscan->m_tkPrevious == tkRCurly && needScanRCurly);

                this->m_funcInArray += m_pscan->IchMinTok() - /*tkRCurly*/ 1 - ichMin;
            }
            else
            {
                this->m_funcInArray += m_pscan->IchLimTok() - ichMin;
            }
        }
    }

    m_scopeCountNoAst = scopeCountNoAstSave;

    if (buildAST && fDeclaration && !IsStrictMode())
    {
        if (pnodeFnc->sxFnc.pnodeName != nullptr && pnodeFnc->sxFnc.pnodeName->nop == knopVarDecl &&
            GetCurrentBlock()->sxBlock.blockType == PnodeBlockType::Regular)
        {
            // Add a function-scoped VarDecl with the same name as the function for
            // back compat with pre-ES6 code that declares functions in blocks. The
            // idea is that the last executed declaration wins at the function scope
            // level and we accomplish this by having each block scoped function
            // declaration assign to both the block scoped "let" binding, as well
            // as the function scoped "var" binding.
            bool isRedecl = false;
            ParseNodePtr vardecl = CreateVarDeclNode(pnodeFnc->sxFnc.pnodeName->sxVar.pid, STVariable, false, nullptr, false, &isRedecl);
            vardecl->sxVar.isBlockScopeFncDeclVar = true;
            if (isRedecl)
            {
                vardecl->sxVar.sym->SetHasBlockFncVarRedecl();
            }
        }
    }

    if (pnodeFncBlockScope)
    {
        Assert(pnodeFncBlockScope->sxBlock.pnodeStmt == nullptr);
        pnodeFncBlockScope->sxBlock.pnodeStmt = pnodeFnc;
        if (buildAST)
        {
            PopFuncBlockScope(ppnodeScopeSave, ppnodeExprScopeSave);
        }
        FinishParseBlock(pnodeFncBlockScope);
        return pnodeFncBlockScope;
    }

    this->m_tryCatchOrFinallyDepth = tryCatchOrFinallyDepthSave;

    return pnodeFnc;
}