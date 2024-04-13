ParseNodePtr Parser::ParseVariableDeclaration(
    tokens declarationType, charcount_t ichMin,
    BOOL fAllowIn/* = TRUE*/,
    BOOL* pfForInOk/* = nullptr*/,
    BOOL singleDefOnly/* = FALSE*/,
    BOOL allowInit/* = TRUE*/,
    BOOL isTopVarParse/* = TRUE*/,
    BOOL isFor/* = FALSE*/,
    BOOL* nativeForOk /*= nullptr*/)
{
    ParseNodePtr pnodeThis = nullptr;
    ParseNodePtr pnodeInit;
    ParseNodePtr pnodeList = nullptr;
    ParseNodePtr *lastNodeRef = nullptr;
    LPCOLESTR pNameHint = nullptr;
    uint32     nameHintLength = 0;
    uint32     nameHintOffset = 0;
    Assert(declarationType == tkVAR || declarationType == tkCONST || declarationType == tkLET);

    for (;;)
    {
        if (IsES6DestructuringEnabled() && IsPossiblePatternStart())
        {
            pnodeThis = ParseDestructuredLiteral<buildAST>(declarationType, true, !!isTopVarParse, DIC_None, !!fAllowIn, pfForInOk, nativeForOk);
            if (pnodeThis != nullptr)
            {
                pnodeThis->ichMin = ichMin;
            }
        }
        else
        {
            if (m_token.tk != tkID)
            {
                IdentifierExpectedError(m_token);
            }

            IdentPtr pid = m_token.GetIdentifier(m_phtbl);
            Assert(pid);
            pNameHint = pid->Psz();
            nameHintLength = pid->Cch();
            nameHintOffset = 0;

            if (pid == wellKnownPropertyPids.let && (declarationType == tkCONST || declarationType == tkLET))
            {
                Error(ERRLetIDInLexicalDecl, pnodeThis);
            }

            if (declarationType == tkVAR)
            {
                pnodeThis = CreateVarDeclNode(pid, STVariable);
            }
            else if (declarationType == tkCONST)
            {
                pnodeThis = CreateBlockScopedDeclNode(pid, knopConstDecl);
                CHAKRATEL_LANGSTATS_INC_LANGFEATURECOUNT(Const, m_scriptContext);
            }
            else
            {
                pnodeThis = CreateBlockScopedDeclNode(pid, knopLetDecl);
                CHAKRATEL_LANGSTATS_INC_LANGFEATURECOUNT(Let, m_scriptContext);
            }

            if (pid == wellKnownPropertyPids.arguments && m_currentNodeFunc)
            {
                // This var declaration may change the way an 'arguments' identifier in the function is resolved
                if (declarationType == tkVAR)
                {
                    m_currentNodeFunc->grfpn |= PNodeFlags::fpnArguments_varDeclaration;
                }
                else
                {
                    if (GetCurrentBlockInfo()->pnodeBlock->sxBlock.blockType == Function)
                    {
                        // Only override arguments if we are at the function block level.
                        m_currentNodeFunc->grfpn |= PNodeFlags::fpnArguments_overriddenByDecl;
                    }
                }
            }

            if (pnodeThis)
            {
                pnodeThis->ichMin = ichMin;
            }

            m_pscan->Scan();

            if (m_token.tk == tkAsg)
            {
                if (!allowInit)
                {
                    Error(ERRUnexpectedDefault);
                }
                if (pfForInOk && (declarationType == tkLET || declarationType == tkCONST || IsStrictMode()))
                {
                    *pfForInOk = FALSE;
                }

                m_pscan->Scan();
                pnodeInit = ParseExpr<buildAST>(koplCma, nullptr, fAllowIn, FALSE, pNameHint, &nameHintLength, &nameHintOffset);
                if (buildAST)
                {
                    AnalysisAssert(pnodeThis);
                    pnodeThis->sxVar.pnodeInit = pnodeInit;
                    pnodeThis->ichLim = pnodeInit->ichLim;

                    if (pnodeInit->nop == knopFncDecl)
                    {
                        Assert(nameHintLength >= nameHintOffset);
                        pnodeInit->sxFnc.hint = pNameHint;
                        pnodeInit->sxFnc.hintLength = nameHintLength;
                        pnodeInit->sxFnc.hintOffset = nameHintOffset;
                        pnodeThis->sxVar.pid->GetTopRef()->isFuncAssignment = true;
                    }
                    else
                    {
                        this->CheckArguments(pnodeInit);
                    }
                    pNameHint = nullptr;
                }

                //Track var a =, let a= , const a =
                // This is for FixedFields Constant Heuristics
                if (pnodeThis && pnodeThis->sxVar.pnodeInit != nullptr)
                {
                    pnodeThis->sxVar.sym->PromoteAssignmentState();
                    if (m_currentNodeFunc && pnodeThis->sxVar.sym->GetIsFormal())
                    {
                        m_currentNodeFunc->sxFnc.SetHasAnyWriteToFormals(true);
                    }
                }
            }
            else if (declarationType == tkCONST /*pnodeThis->nop == knopConstDecl*/
                     && !singleDefOnly
                     && !(isFor && TokIsForInOrForOf()))
            {
                Error(ERRUninitializedConst);
            }
        }

        if (singleDefOnly)
        {
            return pnodeThis;
        }

        if (buildAST)
        {
            AddToNodeListEscapedUse(&pnodeList, &lastNodeRef, pnodeThis);
        }

        if (m_token.tk != tkComma)
        {
            return pnodeList;
        }

        if (pfForInOk)
        {
            // don't allow "for (var a, b in c)"
            *pfForInOk = FALSE;
        }
        m_pscan->Scan();
        ichMin = m_pscan->IchMinTok();
    }
}