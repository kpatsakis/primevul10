ParseNodePtr Parser::Parse(LPCUTF8 pszSrc, size_t offset, size_t length, charcount_t charOffset, ULONG grfscr, ULONG lineNumber, Js::LocalFunctionId * nextFunctionId, CompileScriptException *pse)
{
    ParseNodePtr pnodeProg;
    ParseNodePtr *lastNodeRef = nullptr;

    m_nextBlockId = 0;

    // Scanner should run in Running mode and not syntax coloring mode
    grfscr &= ~fscrSyntaxColor;

    if (this->m_scriptContext->IsScriptContextInDebugMode()
#ifdef ENABLE_PREJIT
         || Js::Configuration::Global.flags.Prejit
#endif
         || ((grfscr & fscrNoDeferParse) != 0)
        )
    {
        // Don't do deferred parsing if debugger is attached or feature is disabled
        // by command-line switch.
        grfscr &= ~fscrDeferFncParse;
    }
    else if (!(grfscr & fscrGlobalCode) &&
             (
                 PHASE_OFF1(Js::Phase::DeferEventHandlersPhase) ||
                 this->m_scriptContext->IsScriptContextInSourceRundownOrDebugMode()
             )
        )
    {
        // Don't defer event handlers in debug/rundown mode, because we need to register the document,
        // so we need to create a full FunctionBody for the script body.
        grfscr &= ~fscrDeferFncParse;
    }

    bool isDeferred = (grfscr & fscrDeferredFnc) != 0;
    bool isModuleSource = (grfscr & fscrIsModuleCode) != 0;

    m_grfscr = grfscr;
    m_length = length;
    m_originalLength = length;
    m_nextFunctionId = nextFunctionId;

    if(m_parseType != ParseType_Deferred)
    {
        JS_ETW(EventWriteJSCRIPT_PARSE_METHOD_START(m_sourceContextInfo->dwHostSourceContext, GetScriptContext(), *m_nextFunctionId, 0, m_parseType, Js::Constants::GlobalFunction));
        OUTPUT_TRACE(Js::DeferParsePhase, _u("Parsing function (%s) : %s (%d)\n"), GetParseType(), Js::Constants::GlobalFunction, *m_nextFunctionId);
    }

    // Give the scanner the source and get the first token
    m_pscan->SetText(pszSrc, offset, length, charOffset, grfscr, lineNumber);
    m_pscan->Scan();

    // Make the main 'knopProg' node
    int32 initSize = 0;
    m_pCurrentAstSize = &initSize;
    pnodeProg = CreateProgNodeWithScanner(isModuleSource);
    pnodeProg->grfpn = PNodeFlags::fpnNone;
    pnodeProg->sxFnc.pid = nullptr;
    pnodeProg->sxFnc.pnodeName = nullptr;
    pnodeProg->sxFnc.pnodeRest = nullptr;
    pnodeProg->sxFnc.ClearFlags();
    pnodeProg->sxFnc.SetNested(FALSE);
    pnodeProg->sxFnc.astSize = 0;
    pnodeProg->sxFnc.cbMin = m_pscan->IecpMinTok();
    pnodeProg->sxFnc.lineNumber = lineNumber;
    pnodeProg->sxFnc.columnNumber = 0;

    if (!isDeferred || (isDeferred && grfscr & fscrGlobalCode))
    {
        // In the deferred case, if the global function is deferred parse (which is in no-refresh case),
        // we will re-use the same function body, so start with the correct functionId.
        pnodeProg->sxFnc.functionId = (*m_nextFunctionId)++;
    }
    else
    {
        pnodeProg->sxFnc.functionId = Js::Constants::NoFunctionId;
    }

    if (isModuleSource)
    {
        Assert(m_scriptContext->GetConfig()->IsES6ModuleEnabled());

        pnodeProg->sxModule.localExportEntries = nullptr;
        pnodeProg->sxModule.indirectExportEntries = nullptr;
        pnodeProg->sxModule.starExportEntries = nullptr;
        pnodeProg->sxModule.importEntries = nullptr;
        pnodeProg->sxModule.requestedModules = nullptr;
    }

    m_pCurrentAstSize = & (pnodeProg->sxFnc.astSize);

    pnodeProg->sxFnc.hint = nullptr;
    pnodeProg->sxFnc.hintLength = 0;
    pnodeProg->sxFnc.hintOffset = 0;
    pnodeProg->sxFnc.isNameIdentifierRef = true;
    pnodeProg->sxFnc.nestedFuncEscapes = false;

    // initialize parsing variables
    pnodeProg->sxFnc.pnodeNext = nullptr;

    m_currentNodeFunc = nullptr;
    m_currentNodeDeferredFunc = nullptr;
    m_currentNodeProg = pnodeProg;
    m_cactIdentToNodeLookup = 1;

    pnodeProg->sxFnc.nestedCount = 0;
    m_pnestedCount = &pnodeProg->sxFnc.nestedCount;
    m_inDeferredNestedFunc = false;

    pnodeProg->sxFnc.pnodeParams = nullptr;
    pnodeProg->sxFnc.pnodeVars = nullptr;
    pnodeProg->sxFnc.pnodeRest = nullptr;
    m_ppnodeVar = &pnodeProg->sxFnc.pnodeVars;
    SetCurrentStatement(nullptr);
    AssertMsg(m_pstmtCur == nullptr, "Statement stack should be empty when we start parse global code");

    // Create block for const's and let's
    ParseNodePtr pnodeGlobalBlock = StartParseBlock<true>(PnodeBlockType::Global, ScopeType_Global);
    pnodeProg->sxProg.scope = pnodeGlobalBlock->sxBlock.scope;
    ParseNodePtr pnodeGlobalEvalBlock = nullptr;

    // Don't track function expressions separately from declarations at global scope.
    m_ppnodeExprScope = nullptr;

    // This synthetic block scope will contain all the nested scopes.
    pnodeProg->sxFnc.pnodeBodyScope = nullptr;
    pnodeProg->sxFnc.pnodeScopes = pnodeGlobalBlock;
    m_ppnodeScope = &pnodeGlobalBlock->sxBlock.pnodeScopes;

    if ((this->m_grfscr & fscrEvalCode) &&
        !(this->m_functionBody && this->m_functionBody->GetScopeInfo()))
    {
        pnodeGlobalEvalBlock = StartParseBlock<true>(PnodeBlockType::Regular, ScopeType_GlobalEvalBlock);
        pnodeProg->sxFnc.pnodeScopes = pnodeGlobalEvalBlock;
        m_ppnodeScope = &pnodeGlobalEvalBlock->sxBlock.pnodeScopes;
    }

    Js::ScopeInfo *scopeInfo = nullptr;
    if (m_parseType == ParseType_Deferred && m_functionBody)
    {
        // this->m_functionBody can be cleared during parsing, but we need access to the scope info later.
        scopeInfo = m_functionBody->GetScopeInfo();
        if (scopeInfo)
        {
            // Create an enclosing function context.
            m_currentNodeFunc = CreateNode(knopFncDecl);
            m_currentNodeFunc->sxFnc.pnodeName = nullptr;
            m_currentNodeFunc->sxFnc.functionId = m_functionBody->GetLocalFunctionId();
            m_currentNodeFunc->sxFnc.nestedCount = m_functionBody->GetNestedCount();
            m_currentNodeFunc->sxFnc.SetStrictMode(!!this->m_fUseStrictMode);

            this->RestoreScopeInfo(scopeInfo->GetParent());
        }
    }

    // It's possible for the module global to be defer-parsed in debug scenarios.
    if (isModuleSource && (!isDeferred || (isDeferred && grfscr & fscrGlobalCode)))
    {
        ParseNodePtr moduleFunction = GenerateModuleFunctionWrapper<true>();
        pnodeProg->sxFnc.pnodeBody = nullptr;
        AddToNodeList(&pnodeProg->sxFnc.pnodeBody, &lastNodeRef, moduleFunction);
    }
    else
    {
        // Process a sequence of statements/declarations
        ParseStmtList<true>(
            &pnodeProg->sxFnc.pnodeBody,
            &lastNodeRef,
            SM_OnGlobalCode,
            !(m_grfscr & fscrDeferredFncExpression) /* isSourceElementList */);
    }

    if (m_parseType == ParseType_Deferred)
    {
        if (scopeInfo)
        {
            this->FinishScopeInfo(scopeInfo->GetParent());
        }
    }

    pnodeProg->sxProg.m_UsesArgumentsAtGlobal = m_UsesArgumentsAtGlobal;

    if (IsStrictMode())
    {
        pnodeProg->sxFnc.SetStrictMode();
    }

#if DEBUG
    if(m_grfscr & fscrEnforceJSON && !IsJSONValid(pnodeProg->sxFnc.pnodeBody))
    {
        Error(ERRsyntax);
    }
#endif

    if (tkEOF != m_token.tk)
        Error(ERRsyntax);

    // Append an EndCode node.
    AddToNodeList(&pnodeProg->sxFnc.pnodeBody, &lastNodeRef,
        CreateNodeWithScanner<knopEndCode>());
    AssertMem(lastNodeRef);
    AssertNodeMem(*lastNodeRef);
    Assert((*lastNodeRef)->nop == knopEndCode);
    (*lastNodeRef)->ichMin = 0;
    (*lastNodeRef)->ichLim = 0;

    // Get the extent of the code.
    pnodeProg->ichLim = m_pscan->IchLimTok();
    pnodeProg->sxFnc.cbLim = m_pscan->IecpLimTok();

    // Terminate the local list
    *m_ppnodeVar = nullptr;

    Assert(nullptr == *m_ppnodeScope);
    Assert(nullptr == pnodeProg->sxFnc.pnodeNext);

#ifdef ENABLE_DEBUG_CONFIG_OPTIONS
    if (Js::Configuration::Global.flags.IsEnabled(Js::ForceUndoDeferFlag))
    {
        m_stoppedDeferredParse = true;
    }
#endif

    if (m_stoppedDeferredParse)
    {
        if (this->m_hasParallelJob)
        {
#if ENABLE_BACKGROUND_PARSING
            BackgroundParser *bgp = static_cast<BackgroundParser*>(m_scriptContext->GetBackgroundParser());
            Assert(bgp);
            this->WaitForBackgroundJobs(bgp, pse);
#endif
        }

        // Finally, see if there are any function bodies we now want to generate because we
        // decided to stop deferring.
        FinishDeferredFunction(pnodeProg->sxFnc.pnodeScopes);
    }

    if (pnodeGlobalEvalBlock)
    {
        FinishParseBlock(pnodeGlobalEvalBlock);
    }
    // Append block as body of pnodeProg
    FinishParseBlock(pnodeGlobalBlock);

    m_scriptContext->AddSourceSize(m_length);

    if (m_parseType != ParseType_Deferred)
    {
        JS_ETW(EventWriteJSCRIPT_PARSE_METHOD_STOP(m_sourceContextInfo->dwHostSourceContext, GetScriptContext(), pnodeProg->sxFnc.functionId, *m_pCurrentAstSize, false, Js::Constants::GlobalFunction));
    }
    return pnodeProg;
}