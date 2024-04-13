ParseNodePtr Parser::ParseExpr(int oplMin,
    BOOL *pfCanAssign,
    BOOL fAllowIn,
    BOOL fAllowEllipsis,
    LPCOLESTR pNameHint,
    uint32 *pHintLength,
    uint32 *pShortNameOffset,
    _Inout_opt_ IdentToken* pToken,
    bool fUnaryOrParen,
    _Inout_opt_ bool* pfLikelyPattern,
    _Inout_opt_ charcount_t *plastRParen)
{
    Assert(pToken == nullptr || pToken->tk == tkNone); // Must be empty initially
    int opl;
    OpCode nop;
    charcount_t ichMin;
    ParseNodePtr pnode = nullptr;
    ParseNodePtr pnodeT = nullptr;
    BOOL fCanAssign = TRUE;
    bool assignmentStmt = false;
    bool fIsDotOrIndex = false;
    IdentToken term;
    RestorePoint termStart;
    uint32 hintLength = 0;
    uint32 hintOffset = 0;

    ParserState parserState;

    if (pHintLength != nullptr)
    {
        hintLength = *pHintLength;
    }

    if (pShortNameOffset != nullptr)
    {
        hintOffset = *pShortNameOffset;
    }

    EnsureStackAvailable();

    // Storing the state here as we need to restore this state back when we need to reparse the grammar under lambda syntax.
    CaptureState(&parserState);

    m_pscan->Capture(&termStart);

    bool deferredErrorFoundOnLeftSide = false;
    bool savedDeferredInitError = m_hasDeferredShorthandInitError;
    m_hasDeferredShorthandInitError = false;

    // Is the current token a unary operator?
    if (m_phtbl->TokIsUnop(m_token.tk, &opl, &nop) && nop != knopNone)
    {
        IdentToken operandToken;
        ichMin = m_pscan->IchMinTok();

        if (nop == knopYield)
        {
            if (!m_pscan->YieldIsKeyword() || oplMin > opl)
            {
                // The case where 'yield' is scanned as a keyword (tkYIELD) but the scanner
                // is not treating yield as a keyword (!m_pscan->YieldIsKeyword()) occurs
                // in strict mode non-generator function contexts.
                //
                // That is, 'yield' is a keyword because of strict mode, but YieldExpression
                // is not a grammar production outside of generator functions.
                //
                // Otherwise it is an error for a yield to appear in the context of a higher level
                // binding operator, be it unary or binary.
                Error(ERRsyntax);
            }
            if (m_currentScope->GetScopeType() == ScopeType_Parameter)
            {
                Error(ERRsyntax);
            }
        }
        else if (nop == knopAwait)
        {
            if (!m_pscan->AwaitIsKeyword() ||
                m_currentScope->GetScopeType() == ScopeType_Parameter)
            {
                // As with the 'yield' keyword, the case where 'await' is scanned as a keyword (tkAWAIT)
                // but the scanner is not treating await as a keyword (!m_pscan->AwaitIsKeyword())
                // occurs in strict mode non-async function contexts.
                //
                // That is, 'await' is a keyword because of strict mode, but AwaitExpression
                // is not a grammar production outside of async functions.
                //
                // Further, await expressions are disallowed within parameter scopes.
                Error(ERRBadAwait);
            }
        }

        m_pscan->Scan();

        if (m_token.tk == tkEllipsis) {
            // ... cannot have a unary prefix.
            Error(ERRUnexpectedEllipsis);
        }

        if (nop == knopYield && !m_pscan->FHadNewLine() && m_token.tk == tkStar)
        {
            m_pscan->Scan();
            nop = knopYieldStar;
        }

        if (nop == knopYield)
        {
            if (!ParseOptionalExpr<buildAST>(&pnodeT, false, opl, NULL, TRUE, fAllowEllipsis))
            {
                nop = knopYieldLeaf;
                if (buildAST)
                {
                    pnode = CreateNodeT<knopYieldLeaf>(ichMin, m_pscan->IchLimTok());
                }
            }
        }
        else
        {
            // Disallow spread after a unary operator.
            pnodeT = ParseExpr<buildAST>(opl, &fCanAssign, TRUE, FALSE, nullptr /*hint*/, nullptr /*hintLength*/, nullptr /*hintOffset*/, &operandToken, true);
        }

        if (nop != knopYieldLeaf)
        {
            if (nop == knopIncPre || nop == knopDecPre)
            {
                if (!fCanAssign && PHASE_ON1(Js::EarlyReferenceErrorsPhase))
                {
                    Error(JSERR_CantAssignTo);
                }
                TrackAssignment<buildAST>(pnodeT, &operandToken);
                if (buildAST)
                {
                    if (IsStrictMode() && pnodeT->nop == knopName)
                    {
                        CheckStrictModeEvalArgumentsUsage(pnodeT->sxPid.pid);
                    }
                }
                else
                {
                    if (IsStrictMode() && operandToken.tk == tkID)
                    {
                        CheckStrictModeEvalArgumentsUsage(operandToken.pid);
                    }
                }
            }
            else if (nop == knopEllipsis)
            {
                if (!fAllowEllipsis)
                {
                    DeferOrEmitPotentialSpreadError(pnodeT);
                }
            }
            else if (m_token.tk == tkExpo)
            {
                //Unary operator on the left hand-side of ** is unexpected, except ++, -- or ...
                Error(ERRInvalidUseofExponentiationOperator);
            }

            if (buildAST)
            {
                //Do not do the folding for Asm in case of KnopPos as we need this to determine the type
                if (nop == knopPos && (pnodeT->nop == knopInt || pnodeT->nop == knopFlt) && !this->m_InAsmMode)
                {
                    // Fold away a unary '+' on a number.
                    pnode = pnodeT;
                }
                else if (nop == knopNeg &&
                    ((pnodeT->nop == knopInt && pnodeT->sxInt.lw != 0) ||
                    (pnodeT->nop == knopFlt && (pnodeT->sxFlt.dbl != 0 || this->m_InAsmMode))))
                {
                    // Fold a unary '-' on a number into the value of the number itself.
                    pnode = pnodeT;
                    if (pnode->nop == knopInt)
                    {
                        pnode->sxInt.lw = -pnode->sxInt.lw;
                    }
                    else
                    {
                        pnode->sxFlt.dbl = -pnode->sxFlt.dbl;
                    }
                }
                else
                {
                    pnode = CreateUniNode(nop, pnodeT);
                    this->CheckArguments(pnode->sxUni.pnode1);
                }
                pnode->ichMin = ichMin;
            }

            if (nop == knopDelete)
            {
                if (IsStrictMode())
                {
                    if ((buildAST && pnode->sxUni.pnode1->nop == knopName) ||
                        (!buildAST && operandToken.tk == tkID))
                    {
                        Error(ERRInvalidDelete);
                    }
                }

                if (buildAST)
                {
                    ParseNodePtr pnode1 = pnode->sxUni.pnode1;
                    if (m_currentNodeFunc)
                    {
                        if (pnode1->nop == knopDot || pnode1->nop == knopIndex)
                        {
                            // If we delete an arguments property, use the conservative,
                            // heap-allocated arguments object.
                            this->CheckArguments(pnode1->sxBin.pnode1);
                        }
                    }
                }
            }
        }

        fCanAssign = FALSE;
    }
    else
    {
        ichMin = m_pscan->IchMinTok();
        BOOL fLikelyPattern = FALSE;
        pnode = ParseTerm<buildAST>(TRUE, pNameHint, &hintLength, &hintOffset, &term, fUnaryOrParen, &fCanAssign, IsES6DestructuringEnabled() ? &fLikelyPattern : nullptr, &fIsDotOrIndex, plastRParen);
        if (pfLikelyPattern != nullptr)
        {
            *pfLikelyPattern = !!fLikelyPattern;
        }

        if (m_token.tk == tkDArrow)
        {
            m_hasDeferredShorthandInitError = false;
        }

        if (m_token.tk == tkAsg && oplMin <= koplAsg && fLikelyPattern)
        {
            m_pscan->SeekTo(termStart);

            // As we are reparsing from the beginning of the destructured literal we need to reset the Block IDs as well to make sure the Block IDs
            // on the pidref stack match.
            int saveNextBlockId = m_nextBlockId;
            m_nextBlockId = parserState.m_nextBlockId;

            ParseDestructuredLiteralWithScopeSave(tkLCurly, false/*isDecl*/, false /*topLevel*/, DIC_ShouldNotParseInitializer);

            // Restore the Block ID at the end of the reparsing so it matches the one at the end of the first pass. We need to do this 
            // because we don't parse initializers during reparse and there may be additional blocks (e.g. a class declaration)
            // in the initializers that will cause the next Block ID at the end of the reparsing to be different.
            m_nextBlockId = saveNextBlockId;

            if (buildAST)
            {
                pnode = ConvertToPattern(pnode);
            }

            // The left-hand side is found to be destructuring pattern - so the shorthand can have initializer.
            m_hasDeferredShorthandInitError = false;
        }

        if (buildAST)
        {
            pNameHint = NULL;
            if (pnode->nop == knopName)
            {
                pNameHint = pnode->sxPid.pid->Psz();
                hintLength = pnode->sxPid.pid->Cch();
                hintOffset = 0;
            }
            else if (pnode->nop == knopDot || pnode->nop == knopIndex)
            {
                if (CONFIG_FLAG(UseFullName))
                {
                    pNameHint = ConstructNameHint(pnode, &hintLength, &hintOffset);
                }
                else
                {
                    ParseNodePtr pnodeName = pnode;
                    while (pnodeName->nop == knopDot)
                    {
                        pnodeName = pnodeName->sxBin.pnode2;
                    }

                    if (pnodeName->nop == knopName)
                    {
                        pNameHint = pnodeName->sxPid.pid->Psz();
                        hintLength = pnodeName->sxPid.pid->Cch();
                        hintOffset = 0;
                    }
                }
            }
        }

        // Check for postfix unary operators.
        if (!m_pscan->FHadNewLine() &&
            (tkInc == m_token.tk || tkDec == m_token.tk))
        {
            if (!fCanAssign && PHASE_ON1(Js::EarlyReferenceErrorsPhase))
            {
                Error(JSERR_CantAssignTo);
            }
            TrackAssignment<buildAST>(pnode, &term);
            fCanAssign = FALSE;
            if (buildAST)
            {
                if (IsStrictMode() && pnode->nop == knopName)
                {
                    CheckStrictModeEvalArgumentsUsage(pnode->sxPid.pid);
                }
                this->CheckArguments(pnode);
                pnode = CreateUniNode(tkInc == m_token.tk ? knopIncPost : knopDecPost, pnode);
                pnode->ichLim = m_pscan->IchLimTok();
            }
            else
            {
                if (IsStrictMode() && term.tk == tkID)
                {
                    CheckStrictModeEvalArgumentsUsage(term.pid);
                }
                // This expression is not an identifier
                term.tk = tkNone;
            }
            m_pscan->Scan();
        }
    }

    deferredErrorFoundOnLeftSide = m_hasDeferredShorthandInitError;

    // Process a sequence of operators and operands.
    for (;;)
    {
        if (!m_phtbl->TokIsBinop(m_token.tk, &opl, &nop) || nop == knopNone)
        {
            break;
        }
        if ( ! fAllowIn && nop == knopIn )
        {
            break;
        }
        Assert(opl != koplNo);

        if (opl == koplAsg)
        {
            if (m_token.tk != tkDArrow)
            {
                // Assignment operator. These are the only right associative
                // binary operators. We also need to special case the left
                // operand - it should only be a LeftHandSideExpression.
                Assert(ParseNode::Grfnop(nop) & fnopAsg || nop == knopFncDecl);
                TrackAssignment<buildAST>(pnode, &term);
                if (buildAST)
                {
                    if (IsStrictMode() && pnode->nop == knopName)
                    {
                        CheckStrictModeEvalArgumentsUsage(pnode->sxPid.pid);
                    }

                    // Assignment stmt of the form "this.<id> = <expr>"
                    if (nop == knopAsg && pnode->nop == knopDot && pnode->sxBin.pnode1->nop == knopThis && pnode->sxBin.pnode2->nop == knopName)
                    {
                        if (pnode->sxBin.pnode2->sxPid.pid != wellKnownPropertyPids.__proto__)
                        {
                            assignmentStmt = true;
                        }
                    }
                }
                else
                {
                    if (IsStrictMode() && term.tk == tkID)
                    {
                        CheckStrictModeEvalArgumentsUsage(term.pid);
                    }
                }
            }

            if (opl < oplMin)
            {
                break;
            }
            if (m_token.tk != tkDArrow && !fCanAssign && PHASE_ON1(Js::EarlyReferenceErrorsPhase))
            {
                Error(JSERR_CantAssignTo);
                // No recovery necessary since this is a semantic, not structural, error.
            }
        }
        else if (opl == koplExpo)
        {
            // ** operator is right associative
            if (opl < oplMin)
            {
                break;
            }

        }
        else if (opl <= oplMin)
        {
            break;
        }

        // This expression is not an identifier
        term.tk = tkNone;

        // Precedence is high enough. Consume the operator token.
        m_pscan->Scan();
        fCanAssign = FALSE;

        // Special case the "?:" operator
        if (nop == knopQmark)
        {
            pnodeT = ParseExpr<buildAST>(koplAsg, NULL, fAllowIn);
            ChkCurTok(tkColon, ERRnoColon);
            ParseNodePtr pnodeT2 = ParseExpr<buildAST>(koplAsg, NULL, fAllowIn);
            if (buildAST)
            {
                pnode = CreateTriNode(nop, pnode, pnodeT, pnodeT2);
                this->CheckArguments(pnode->sxTri.pnode2);
                this->CheckArguments(pnode->sxTri.pnode3);
            }
        }
        else if (nop == knopFncDecl)
        {
            ushort flags = fFncLambda;
            size_t iecpMin = 0;
            bool isAsyncMethod = false;

            RestoreStateFrom(&parserState);

            m_pscan->SeekTo(termStart);
            if (m_token.tk == tkID && m_token.GetIdentifier(m_phtbl) == wellKnownPropertyPids.async && m_scriptContext->GetConfig()->IsES7AsyncAndAwaitEnabled())
            {
                ichMin = m_pscan->IchMinTok();
                iecpMin = m_pscan->IecpMinTok();

                m_pscan->Scan();
                if ((m_token.tk == tkID || m_token.tk == tkLParen) && !m_pscan->FHadNewLine())
                {
                    flags |= fFncAsync;
                    isAsyncMethod = true;
                }
                else
                {
                    m_pscan->SeekTo(termStart);
                }
            }
            pnode = ParseFncDecl<buildAST>(flags, nullptr, /* needsPIDOnRCurlyScan = */false, /* resetParsingSuperRestrictionState = */false);
            if (isAsyncMethod)
            {
                pnode->sxFnc.cbMin = iecpMin;
                pnode->ichMin = ichMin;
            }
        }
        else
        {
            // Parse the operand, make a new node, and look for more
            IdentToken token;
            pnodeT = ParseExpr<buildAST>(opl, NULL, fAllowIn, FALSE, pNameHint, &hintLength, &hintOffset, &token);

            // Detect nested function escapes of the pattern "o.f = function(){...}" or "o[s] = function(){...}".
            // Doing so in the parser allows us to disable stack-nested-functions in common cases where an escape
            // is not detected at byte code gen time because of deferred parsing.
            if (fIsDotOrIndex && nop == knopAsg)
            {
                this->MarkEscapingRef(pnodeT, &token);
            }

            if (buildAST)
            {
                pnode = CreateBinNode(nop, pnode, pnodeT);
                Assert(pnode->sxBin.pnode2 != NULL);
                if (pnode->sxBin.pnode2->nop == knopFncDecl)
                {
                    Assert(hintLength >= hintOffset);
                    pnode->sxBin.pnode2->sxFnc.hint = pNameHint;
                    pnode->sxBin.pnode2->sxFnc.hintLength = hintLength;
                    pnode->sxBin.pnode2->sxFnc.hintOffset = hintOffset;

                    if (pnode->sxBin.pnode1->nop == knopDot)
                    {
                        pnode->sxBin.pnode2->sxFnc.isNameIdentifierRef  = false;
                    }
                    else if (pnode->sxBin.pnode1->nop == knopName)
                    {
                        PidRefStack *pidRef = pnode->sxBin.pnode1->sxPid.pid->GetTopRef();
                        pidRef->isFuncAssignment = true;
                    }
                }
                if (pnode->sxBin.pnode2->nop == knopClassDecl && pnode->sxBin.pnode1->nop == knopDot)
                {
                    Assert(pnode->sxBin.pnode2->sxClass.pnodeConstructor);
                    pnode->sxBin.pnode2->sxClass.pnodeConstructor->sxFnc.isNameIdentifierRef  = false;
                }
            }
            pNameHint = NULL;
        }
    }

    if (buildAST)
    {
        if (!assignmentStmt)
        {
            // Don't set the flag for following nodes
            switch (pnode->nop)
            {
            case knopName:
            case knopInt:
            case knopFlt:
            case knopStr:
            case knopRegExp:
            case knopNull:
            case knopFalse:
            case knopTrue:
                break;
            default:
                if (m_currentNodeFunc)
                {
                    m_currentNodeFunc->sxFnc.SetHasNonThisStmt();
                }
                else if (m_currentNodeProg)
                {
                    m_currentNodeProg->sxFnc.SetHasNonThisStmt();
                }
            }
        }
    }

    if (m_hasDeferredShorthandInitError && !deferredErrorFoundOnLeftSide)
    {
        // Raise error only if it is found not on the right side of the expression.
        // such as  <expr> = {x = 1}
        Error(ERRnoColon);
    }

    m_hasDeferredShorthandInitError = m_hasDeferredShorthandInitError || savedDeferredInitError;

    if (NULL != pfCanAssign)
    {
        *pfCanAssign = fCanAssign;
    }

    // Pass back identifier if requested
    if (pToken && term.tk == tkID)
    {
        *pToken = term;
    }

    //Track "obj.a" assignment patterns here - Promote the Assignment state for the property's PID.
    // This includes =, += etc.
    if (pnode != NULL)
    {
        uint nodeType = ParseNode::Grfnop(pnode->nop);
        if (nodeType & fnopAsg)
        {
            if (nodeType & fnopBin)
            {
                ParseNodePtr lhs = pnode->sxBin.pnode1;

                Assert(lhs);
                if (lhs->nop == knopDot)
                {
                    ParseNodePtr propertyNode = lhs->sxBin.pnode2;
                    if (propertyNode->nop == knopName)
                    {
                        propertyNode->sxPid.pid->PromoteAssignmentState();
                    }
                }
            }
            else if (nodeType & fnopUni)
            {
                // cases like obj.a++, ++obj.a
                ParseNodePtr lhs = pnode->sxUni.pnode1;
                if (lhs->nop == knopDot)
                {
                    ParseNodePtr propertyNode = lhs->sxBin.pnode2;
                    if (propertyNode->nop == knopName)
                    {
                        propertyNode->sxPid.pid->PromoteAssignmentState();
                    }
                }
            }
        }
    }
    return pnode;
}