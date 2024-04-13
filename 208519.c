ParseNodePtr Parser::GenerateEmptyConstructor(bool extends)
{
    ParseNodePtr pnodeFnc;

    // Create the node.
    pnodeFnc = CreateNode(knopFncDecl);
    pnodeFnc->sxFnc.ClearFlags();
    pnodeFnc->sxFnc.SetNested(NULL != m_currentNodeFunc);
    pnodeFnc->sxFnc.SetStrictMode();
    pnodeFnc->sxFnc.SetDeclaration(TRUE);
    pnodeFnc->sxFnc.SetIsMethod(TRUE);
    pnodeFnc->sxFnc.SetIsClassMember(TRUE);
    pnodeFnc->sxFnc.SetIsClassConstructor(TRUE);
    pnodeFnc->sxFnc.SetIsBaseClassConstructor(!extends);
    pnodeFnc->sxFnc.SetHasNonThisStmt();
    pnodeFnc->sxFnc.SetIsGeneratedDefault(TRUE);

    pnodeFnc->ichLim = m_pscan->IchLimTok();
    pnodeFnc->ichMin = m_pscan->IchMinTok();
    pnodeFnc->sxFnc.cbLim = m_pscan->IecpLimTok();
    pnodeFnc->sxFnc.cbMin = m_pscan->IecpMinTok();
    pnodeFnc->sxFnc.astSize = 0;
    pnodeFnc->sxFnc.lineNumber = m_pscan->LineCur();

    pnodeFnc->sxFnc.functionId          = (*m_nextFunctionId);
    pnodeFnc->sxFnc.pid                 = nullptr;
    pnodeFnc->sxFnc.hint                = nullptr;
    pnodeFnc->sxFnc.hintOffset          = 0;
    pnodeFnc->sxFnc.hintLength          = 0;
    pnodeFnc->sxFnc.isNameIdentifierRef = true;
    pnodeFnc->sxFnc.nestedFuncEscapes   = false;
    pnodeFnc->sxFnc.pnodeName           = nullptr;
    pnodeFnc->sxFnc.pnodeScopes         = nullptr;
    pnodeFnc->sxFnc.pnodeParams         = nullptr;
    pnodeFnc->sxFnc.pnodeVars           = nullptr;
    pnodeFnc->sxFnc.pnodeBody           = nullptr;
    pnodeFnc->sxFnc.nestedCount         = 0;
    pnodeFnc->sxFnc.pnodeNext           = nullptr;
    pnodeFnc->sxFnc.pnodeRest           = nullptr;
    pnodeFnc->sxFnc.deferredStub        = nullptr;
    pnodeFnc->sxFnc.funcInfo            = nullptr;

    // In order to (re-)defer the default constructor, we need to, for instance, track
    // deferred class expression the way we track function expression, since we lose the part of the source
    // that tells us which we have.
    pnodeFnc->sxFnc.canBeDeferred       = false;

#ifdef DBG
    pnodeFnc->sxFnc.deferredParseNextFunctionId = *(this->m_nextFunctionId);
#endif

    AppendFunctionToScopeList(true, pnodeFnc);

    if (m_nextFunctionId)
    {
        (*m_nextFunctionId)++;
    }

    // Update the count of functions nested in the current parent.
    if (m_pnestedCount)
    {
        (*m_pnestedCount)++;
    }

    if (!buildAST)
    {
        return NULL;
    }

    if (m_pscan->IchMinTok() >= m_pscan->IchMinLine())
    {
        // In scenarios involving defer parse IchMinLine() can be incorrect for the first line after defer parse
        pnodeFnc->sxFnc.columnNumber = m_pscan->IchMinTok() - m_pscan->IchMinLine();
    }
    else if (m_currentNodeFunc)
    {
        // For the first line after defer parse, compute the column relative to the column number
        // of the lexically parent function.
        ULONG offsetFromCurrentFunction = m_pscan->IchMinTok() - m_currentNodeFunc->ichMin;
        pnodeFnc->sxFnc.columnNumber = m_currentNodeFunc->sxFnc.columnNumber + offsetFromCurrentFunction;
    }
    else
    {
        // if there is no current function, lets give a default of 0.
        pnodeFnc->sxFnc.columnNumber = 0;
    }

    int32 * pAstSizeSave = m_pCurrentAstSize;
    m_pCurrentAstSize = &(pnodeFnc->sxFnc.astSize);

    // Make this the current function.
    ParseNodePtr pnodeFncSave = m_currentNodeFunc;
    m_currentNodeFunc = pnodeFnc;

    ParseNodePtr argsId = nullptr;
    ParseNodePtr *lastNodeRef = nullptr;
    ParseNodePtr pnodeBlock = StartParseBlock<buildAST>(PnodeBlockType::Parameter, ScopeType_Parameter);

    if (extends)
    {
        // constructor(...args) { super(...args); }
        //             ^^^^^^^
        ParseNodePtr *const ppnodeVarSave = m_ppnodeVar;
        m_ppnodeVar = &pnodeFnc->sxFnc.pnodeVars;

        IdentPtr pidargs = m_phtbl->PidHashNameLen(_u("args"), sizeof("args") - 1);
        ParseNodePtr pnodeT = CreateVarDeclNode(pidargs, STFormal);
        pnodeT->sxVar.sym->SetIsNonSimpleParameter(true);
        pnodeFnc->sxFnc.pnodeRest = pnodeT;
        PidRefStack *ref = this->PushPidRef(pidargs);

        argsId = CreateNameNode(pidargs, pnodeFnc->ichMin, pnodeFnc->ichLim);

        argsId->sxPid.symRef = ref->GetSymRef();
        m_ppnodeVar = ppnodeVarSave;
    }

    ParseNodePtr pnodeInnerBlock = StartParseBlock<buildAST>(PnodeBlockType::Function, ScopeType_FunctionBody);
    pnodeBlock->sxBlock.pnodeScopes = pnodeInnerBlock;
    pnodeFnc->sxFnc.pnodeBodyScope = pnodeInnerBlock;
    pnodeFnc->sxFnc.pnodeScopes = pnodeBlock;

    if (extends)
    {
        // constructor(...args) { super(...args); }
        //                        ^^^^^^^^^^^^^^^
        Assert(argsId);
        ParseNodePtr spreadArg = CreateUniNode(knopEllipsis, argsId, pnodeFnc->ichMin, pnodeFnc->ichLim);

        ParseNodePtr superRef = CreateNodeWithScanner<knopSuper>();
        pnodeFnc->sxFnc.SetHasSuperReference(TRUE);

        ParseNodePtr callNode = CreateCallNode(knopCall, superRef, spreadArg);
        callNode->sxCall.spreadArgCount = 1;
        AddToNodeList(&pnodeFnc->sxFnc.pnodeBody, &lastNodeRef, callNode);
    }

    AddToNodeList(&pnodeFnc->sxFnc.pnodeBody, &lastNodeRef, CreateNodeWithScanner<knopEndCode>());

    FinishParseBlock(pnodeInnerBlock);
    FinishParseBlock(pnodeBlock);

    m_currentNodeFunc = pnodeFncSave;
    m_pCurrentAstSize = pAstSizeSave;

    return pnodeFnc;
}