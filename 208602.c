ParseNodePtr Parser::ParseStatement()
{
    ParseNodePtr *ppnodeT;
    ParseNodePtr pnodeT;
    ParseNodePtr pnode = nullptr;
    LabelId* pLabelIdList = nullptr;
    charcount_t ichMin = 0;
    size_t iecpMin = 0;
    StmtNest stmt;
    StmtNest *pstmt;
    BOOL fForInOrOfOkay;
    BOOL fCanAssign;
    IdentPtr pid;
    uint fnop;
    ParseNodePtr pnodeLabel = nullptr;
    bool expressionStmt = false;
    bool isAsyncMethod = false;
    tokens tok;
#if EXCEPTION_RECOVERY
    ParseNodePtr pParentTryCatch = nullptr;
    ParseNodePtr pTryBlock = nullptr;
    ParseNodePtr pTry = nullptr;
    ParseNodePtr pParentTryCatchBlock = nullptr;

    StmtNest stmtTryCatchBlock;
    StmtNest stmtTryCatch;
    StmtNest stmtTry;
    StmtNest stmtTryBlock;
#endif

    if (buildAST)
    {
#if EXCEPTION_RECOVERY
        if(Js::Configuration::Global.flags.SwallowExceptions)
        {
            // If we're swallowing exceptions, surround this statement with a try/catch block:
            //
            //   Before: x.y = 3;
            //   After:  try { x.y = 3; } catch(__ehobj) { }
            //
            // This is done to force the runtime to recover from exceptions at the most granular
            // possible point.  Recovering from EH dramatically improves coverage of testing via
            // fault injection.


            // create and push the try-catch node
            pParentTryCatchBlock = CreateBlockNode();
            PushStmt<buildAST>(&stmtTryCatchBlock, pParentTryCatchBlock, knopBlock, nullptr, nullptr);
            pParentTryCatch = CreateNodeWithScanner<knopTryCatch>();
            PushStmt<buildAST>(&stmtTryCatch, pParentTryCatch, knopTryCatch, nullptr, nullptr);

            // create and push a try node
            pTry = CreateNodeWithScanner<knopTry>();
            PushStmt<buildAST>(&stmtTry, pTry, knopTry, nullptr, nullptr);
            pTryBlock = CreateBlockNode();
            PushStmt<buildAST>(&stmtTryBlock, pTryBlock, knopBlock, nullptr, nullptr);
            // these nodes will be closed after the statement is parsed.
        }
#endif // EXCEPTION_RECOVERY
    }

    EnsureStackAvailable();

LRestart:
    tok = m_token.tk;

    switch (tok)
    {
    case tkEOF:
        if (buildAST)
        {
            pnode = nullptr;
        }
        break;

    case tkFUNCTION:
    {
LFunctionStatement:
        if (m_grfscr & fscrDeferredFncExpression)
        {
            // The top-level deferred function body was defined by a function expression whose parsing was deferred. We are now
            // parsing it, so unset the flag so that any nested functions are parsed normally. This flag is only applicable the
            // first time we see it.
            m_grfscr &= ~fscrDeferredFncExpression;
            pnode = ParseFncDecl<buildAST>(isAsyncMethod ? fFncAsync : fFncNoFlgs, nullptr);
        }
        else
        {
            pnode = ParseFncDecl<buildAST>(fFncDeclaration | (isAsyncMethod ? fFncAsync : fFncNoFlgs), nullptr);
        }
        if (isAsyncMethod)
        {
            pnode->sxFnc.cbMin = iecpMin;
            pnode->ichMin = ichMin;
        }
        break;
    }

    case tkCLASS:
        if (m_scriptContext->GetConfig()->IsES6ClassAndExtendsEnabled())
        {
            pnode = ParseClassDecl<buildAST>(TRUE, nullptr, nullptr, nullptr);
        }
        else
        {
            goto LDefaultToken;
        }
        break;

    case tkID:
        if (m_token.GetIdentifier(m_phtbl) == wellKnownPropertyPids.let)
        {
            // We see "let" at the start of a statement. This could either be a declaration or an identifier
            // reference. The next token determines which.
            RestorePoint parsedLet;
            m_pscan->Capture(&parsedLet);
            ichMin = m_pscan->IchMinTok();

            m_pscan->Scan();
            if (this->NextTokenConfirmsLetDecl())
            {
                pnode = ParseVariableDeclaration<buildAST>(tkLET, ichMin);
                goto LNeedTerminator;
            }
            m_pscan->SeekTo(parsedLet);
        }
        else if (m_token.GetIdentifier(m_phtbl) == wellKnownPropertyPids.async && m_scriptContext->GetConfig()->IsES7AsyncAndAwaitEnabled())
        {
            RestorePoint parsedAsync;
            m_pscan->Capture(&parsedAsync);
            ichMin = m_pscan->IchMinTok();
            iecpMin = m_pscan->IecpMinTok();

            m_pscan->Scan();
            if (m_token.tk == tkFUNCTION && !m_pscan->FHadNewLine())
            {
                isAsyncMethod = true;
                goto LFunctionStatement;
            }
            m_pscan->SeekTo(parsedAsync);
        }
        goto LDefaultToken;

    case tkCONST:
    case tkLET:
        ichMin = m_pscan->IchMinTok();

        m_pscan->Scan();
        pnode = ParseVariableDeclaration<buildAST>(tok, ichMin);
        goto LNeedTerminator;

    case tkVAR:
        ichMin = m_pscan->IchMinTok();

        m_pscan->Scan();
        pnode = ParseVariableDeclaration<buildAST>(tok, ichMin);
        goto LNeedTerminator;

    case tkFOR:
    {
        ParseNodePtr pnodeBlock = nullptr;
        ParseNodePtr *ppnodeScopeSave = nullptr;
        ParseNodePtr *ppnodeExprScopeSave = nullptr;

        ichMin = m_pscan->IchMinTok();
        ChkNxtTok(tkLParen, ERRnoLparen);
        pnodeBlock = StartParseBlock<buildAST>(PnodeBlockType::Regular, ScopeType_Block);
        if (buildAST)
        {
            PushFuncBlockScope(pnodeBlock, &ppnodeScopeSave, &ppnodeExprScopeSave);
        }

        RestorePoint startExprOrIdentifier;
        fForInOrOfOkay = TRUE;
        fCanAssign = TRUE;
        tok = m_token.tk;
        BOOL nativeForOkay = TRUE;

        switch (tok)
        {
        case tkID:
            if (m_token.GetIdentifier(m_phtbl) == wellKnownPropertyPids.let)
            {
                // We see "let" in the init part of a for loop. This could either be a declaration or an identifier
                // reference. The next token determines which.
                RestorePoint parsedLet;
                m_pscan->Capture(&parsedLet);
                auto ichMinInner = m_pscan->IchMinTok();

                m_pscan->Scan();
                if (IsPossiblePatternStart())
                {
                    m_pscan->Capture(&startExprOrIdentifier);
                }
                if (this->NextTokenConfirmsLetDecl() && m_token.tk != tkIN)
                {
                    pnodeT = ParseVariableDeclaration<buildAST>(tkLET, ichMinInner
                                                                , /*fAllowIn = */FALSE
                                                                , /*pfForInOk = */&fForInOrOfOkay
                                                                , /*singleDefOnly*/FALSE
                                                                , /*allowInit*/TRUE
                                                                , /*isTopVarParse*/TRUE
                                                                , /*isFor*/TRUE
                                                                , &nativeForOkay);
                    break;
                }
                m_pscan->SeekTo(parsedLet);
            }
            goto LDefaultTokenFor;
        case tkLET:
        case tkCONST:
        case tkVAR:
            {
                auto ichMinInner = m_pscan->IchMinTok();

                m_pscan->Scan();
                if (IsPossiblePatternStart())
                {
                    m_pscan->Capture(&startExprOrIdentifier);
                }
                pnodeT = ParseVariableDeclaration<buildAST>(tok, ichMinInner
                                                            , /*fAllowIn = */FALSE
                                                            , /*pfForInOk = */&fForInOrOfOkay
                                                            , /*singleDefOnly*/FALSE
                                                            , /*allowInit*/TRUE
                                                            , /*isTopVarParse*/TRUE
                                                            , /*isFor*/TRUE
                                                            , &nativeForOkay);
            }
            break;
        case tkSColon:
            pnodeT = nullptr;
            fForInOrOfOkay = FALSE;
            break;
        default:
            {
LDefaultTokenFor:
                RestorePoint exprStart;
                tokens beforeToken = tok;
                m_pscan->Capture(&exprStart);
                if (IsPossiblePatternStart())
                {
                    m_pscan->Capture(&startExprOrIdentifier);
                }
                bool fLikelyPattern = false;
                if (IsES6DestructuringEnabled() && (beforeToken == tkLBrack || beforeToken == tkLCurly))
                {
                    pnodeT = ParseExpr<buildAST>(koplNo,
                        &fCanAssign,
                        /*fAllowIn = */FALSE,
                        /*fAllowEllipsis*/FALSE,
                        /*pHint*/nullptr,
                        /*pHintLength*/nullptr,
                        /*pShortNameOffset*/nullptr,
                        /*pToken*/nullptr,
                        /**fUnaryOrParen*/false,
                        &fLikelyPattern);
                }
                else
                {
                    pnodeT = ParseExpr<buildAST>(koplNo, &fCanAssign, /*fAllowIn = */FALSE);
                }

                // We would veryfiy the grammar as destructuring grammar only when  for..in/of case. As in the native for loop case the above ParseExpr call
                // has already converted them appropriately.
                if (fLikelyPattern && TokIsForInOrForOf())
                {
                    m_pscan->SeekTo(exprStart);
                    ParseDestructuredLiteralWithScopeSave(tkNone, false/*isDecl*/, false /*topLevel*/, DIC_None, false /*allowIn*/);

                    if (buildAST)
                    {
                        pnodeT = ConvertToPattern(pnodeT);
                    }
                }
                if (buildAST)
                {
                    Assert(pnodeT);
                    pnodeT->isUsed = false;
                }
            }
            break;
        }

        if (TokIsForInOrForOf())
        {
            bool isForOf = (m_token.tk != tkIN);
            Assert(!isForOf || (m_token.tk == tkID && m_token.GetIdentifier(m_phtbl) == wellKnownPropertyPids.of));

            if ((buildAST && nullptr == pnodeT) || !fForInOrOfOkay)
            {
                if (isForOf)
                {
                    Error(ERRForOfNoInitAllowed);
                }
                else
                {
                    Error(ERRForInNoInitAllowed);
                }
            }
            if (!fCanAssign && PHASE_ON1(Js::EarlyReferenceErrorsPhase))
            {
                Error(JSERR_CantAssignTo);
            }

            m_pscan->Scan();
            ParseNodePtr pnodeObj = ParseExpr<buildAST>(isForOf ? koplCma : koplNo);
            charcount_t ichLim = m_pscan->IchLimTok();
            ChkCurTok(tkRParen, ERRnoRparen);

            if (buildAST)
            {
                if (isForOf)
                {
                    pnode = CreateNodeWithScanner<knopForOf>(ichMin);
                }
                else
                {
                    pnode = CreateNodeWithScanner<knopForIn>(ichMin);
                }
                pnode->sxForInOrForOf.pnodeBlock = pnodeBlock;
                pnode->sxForInOrForOf.pnodeLval = pnodeT;
                pnode->sxForInOrForOf.pnodeObj = pnodeObj;
                pnode->ichLim = ichLim;
                
                TrackAssignment<true>(pnodeT, nullptr);
            }
            PushStmt<buildAST>(&stmt, pnode, isForOf ? knopForOf : knopForIn, pnodeLabel, pLabelIdList);
            ParseNodePtr pnodeBody = ParseStatement<buildAST>();

            if (buildAST)
            {
                pnode->sxForInOrForOf.pnodeBody = pnodeBody;
            }
            PopStmt(&stmt);
        }
        else
        {
            if (!nativeForOkay)
            {
                Error(ERRDestructInit);
            }

            ChkCurTok(tkSColon, ERRnoSemic);
            ParseNodePtr pnodeCond = nullptr;
            if (m_token.tk != tkSColon)
            {
                pnodeCond = ParseExpr<buildAST>();
                if (m_token.tk != tkSColon)
                {
                    Error(ERRnoSemic);
                }
            }

            tokens tk;
            tk = m_pscan->Scan();

            ParseNodePtr pnodeIncr = nullptr;
            if (tk != tkRParen)
            {
                pnodeIncr = ParseExpr<buildAST>();
                if(pnodeIncr)
                {
                    pnodeIncr->isUsed = false;
                }
            }

            charcount_t ichLim = m_pscan->IchLimTok();

            ChkCurTok(tkRParen, ERRnoRparen);

            if (buildAST)
            {
                pnode = CreateNodeWithScanner<knopFor>(ichMin);
                pnode->sxFor.pnodeBlock = pnodeBlock;
                pnode->sxFor.pnodeInverted= nullptr;
                pnode->sxFor.pnodeInit = pnodeT;
                pnode->sxFor.pnodeCond = pnodeCond;
                pnode->sxFor.pnodeIncr = pnodeIncr;
                pnode->ichLim = ichLim;
            }
            PushStmt<buildAST>(&stmt, pnode, knopFor, pnodeLabel, pLabelIdList);
            ParseNodePtr pnodeBody = ParseStatement<buildAST>();
            if (buildAST)
            {
                pnode->sxFor.pnodeBody = pnodeBody;
            }
            PopStmt(&stmt);
        }

        if (buildAST)
        {
            PopFuncBlockScope(ppnodeScopeSave, ppnodeExprScopeSave);
        }

        FinishParseBlock(pnodeBlock);

        break;
    }

    case tkSWITCH:
    {
        BOOL fSeenDefault = FALSE;
        ParseNodePtr pnodeBlock = nullptr;
        ParseNodePtr *ppnodeScopeSave = nullptr;
        ParseNodePtr *ppnodeExprScopeSave = nullptr;

        ichMin = m_pscan->IchMinTok();
        ChkNxtTok(tkLParen, ERRnoLparen);
        ParseNodePtr pnodeVal = ParseExpr<buildAST>();
        charcount_t ichLim = m_pscan->IchLimTok();

        ChkCurTok(tkRParen, ERRnoRparen);
        ChkCurTok(tkLCurly, ERRnoLcurly);

        if (buildAST)
        {
            pnode = CreateNodeWithScanner<knopSwitch>(ichMin);
        }
        PushStmt<buildAST>(&stmt, pnode, knopSwitch, pnodeLabel, pLabelIdList);
        pnodeBlock = StartParseBlock<buildAST>(PnodeBlockType::Regular, ScopeType_Block, nullptr, pLabelIdList);

        if (buildAST)
        {
            pnode->sxSwitch.pnodeVal = pnodeVal;
            pnode->sxSwitch.pnodeBlock = pnodeBlock;
            pnode->ichLim = ichLim;
            PushFuncBlockScope(pnode->sxSwitch.pnodeBlock, &ppnodeScopeSave, &ppnodeExprScopeSave);

            pnode->sxSwitch.pnodeDefault = nullptr;
            ppnodeT = &pnode->sxSwitch.pnodeCases;
        }

        for (;;)
        {
            ParseNodePtr pnodeBody = nullptr;
            switch (m_token.tk)
            {
            default:
                goto LEndSwitch;
            case tkCASE:
            {
                pnodeT = this->ParseCase<buildAST>(&pnodeBody);
                break;
            }
            case tkDEFAULT:
                if (fSeenDefault)
                {
                    Error(ERRdupDefault);
                    // No recovery necessary since this is a semantic, not structural, error
                }
                fSeenDefault = TRUE;
                charcount_t ichMinT = m_pscan->IchMinTok();
                m_pscan->Scan();
                charcount_t ichMinInner = m_pscan->IchLimTok();
                ChkCurTok(tkColon, ERRnoColon);
                if (buildAST)
                {
                    pnodeT = CreateNodeWithScanner<knopCase>(ichMinT);
                    pnode->sxSwitch.pnodeDefault = pnodeT;
                    pnodeT->ichLim = ichMinInner;
                    pnodeT->sxCase.pnodeExpr = nullptr;
                }
                ParseStmtList<buildAST>(&pnodeBody);
                break;
            }
            if (buildAST)
            {
                if (pnodeBody)
                {
                    // Create a block node to contain the statement list for this case.
                    // This helps us insert byte code to return the right value from
                    // global/eval code.
                    pnodeT->sxCase.pnodeBody = CreateBlockNode(pnodeT->ichMin, pnodeT->ichLim);
                    pnodeT->sxCase.pnodeBody->grfpn |= PNodeFlags::fpnSyntheticNode; // block is not a user specifier block
                    pnodeT->sxCase.pnodeBody->sxBlock.pnodeStmt = pnodeBody;
                }
                else
                {
                    pnodeT->sxCase.pnodeBody = nullptr;
                }
                *ppnodeT = pnodeT;
                ppnodeT = &pnodeT->sxCase.pnodeNext;
            }
        }
LEndSwitch:
        ChkCurTok(tkRCurly, ERRnoRcurly);
        if (buildAST)
        {
            *ppnodeT = nullptr;
            PopFuncBlockScope(ppnodeScopeSave, ppnodeExprScopeSave);
            FinishParseBlock(pnode->sxSwitch.pnodeBlock);
        }
        else
        {
            FinishParseBlock(pnodeBlock);
        }
        PopStmt(&stmt);

        break;
    }

    case tkWHILE:
    {
        ichMin = m_pscan->IchMinTok();
        ChkNxtTok(tkLParen, ERRnoLparen);
        ParseNodePtr pnodeCond = ParseExpr<buildAST>();
        charcount_t ichLim = m_pscan->IchLimTok();
        ChkCurTok(tkRParen, ERRnoRparen);

        if (buildAST)
        {
            pnode = CreateNodeWithScanner<knopWhile>(ichMin);
            pnode->sxWhile.pnodeCond = pnodeCond;
            pnode->ichLim = ichLim;
        }
        PushStmt<buildAST>(&stmt, pnode, knopWhile, pnodeLabel, pLabelIdList);
        ParseNodePtr pnodeBody = ParseStatement<buildAST>();
        PopStmt(&stmt);

        if (buildAST)
        {
            pnode->sxWhile.pnodeBody = pnodeBody;
        }
        break;
    }

    case tkDO:
    {
        if (buildAST)
        {
            pnode = CreateNodeWithScanner<knopDoWhile>();
        }
        PushStmt<buildAST>(&stmt, pnode, knopDoWhile, pnodeLabel, pLabelIdList);
        m_pscan->Scan();
        ParseNodePtr pnodeBody = ParseStatement<buildAST>();
        PopStmt(&stmt);
        charcount_t ichMinT = m_pscan->IchMinTok();

        ChkCurTok(tkWHILE, ERRnoWhile);
        ChkCurTok(tkLParen, ERRnoLparen);

        ParseNodePtr pnodeCond = ParseExpr<buildAST>();
        charcount_t ichLim = m_pscan->IchLimTok();
        ChkCurTok(tkRParen, ERRnoRparen);

        if (buildAST)
        {
            pnode->sxWhile.pnodeBody = pnodeBody;
            pnode->sxWhile.pnodeCond = pnodeCond;
            pnode->ichLim = ichLim;
            pnode->ichMin = ichMinT;
        }

        // REVIEW: Allow do...while statements to be embedded in other compound statements like if..else, or do..while?
        //      goto LNeedTerminator;

        // For now just eat the trailing semicolon if present.
        if (m_token.tk == tkSColon)
        {
            if (pnode)
            {
                pnode->grfpn |= PNodeFlags::fpnExplicitSemicolon;
            }
            m_pscan->Scan();
        }
        else if (pnode)
        {
            pnode->grfpn |= PNodeFlags::fpnAutomaticSemicolon;
        }

        break;
    }

    case tkIF:
    {
        ichMin = m_pscan->IchMinTok();
        ChkNxtTok(tkLParen, ERRnoLparen);
        ParseNodePtr pnodeCond = ParseExpr<buildAST>();
        if (buildAST)
        {
            pnode = CreateNodeWithScanner<knopIf>(ichMin);
            pnode->ichLim = m_pscan->IchLimTok();
            pnode->sxIf.pnodeCond = pnodeCond;
        }
        ChkCurTok(tkRParen, ERRnoRparen);

        PushStmt<buildAST>(&stmt, pnode, knopIf, pnodeLabel, pLabelIdList);
        ParseNodePtr pnodeTrue = ParseStatement<buildAST>();
        ParseNodePtr pnodeFalse = nullptr;
        if (m_token.tk == tkELSE)
        {
            m_pscan->Scan();
            pnodeFalse = ParseStatement<buildAST>();
        }
        if (buildAST)
        {
            pnode->sxIf.pnodeTrue = pnodeTrue;
            pnode->sxIf.pnodeFalse = pnodeFalse;
        }
        PopStmt(&stmt);
        break;
    }

    case tkTRY:
    {
        if (buildAST)
        {
            pnode = CreateBlockNode();
            pnode->grfpn |= PNodeFlags::fpnSyntheticNode; // block is not a user specifier block
        }
        PushStmt<buildAST>(&stmt, pnode, knopBlock, pnodeLabel, pLabelIdList);
        ParseNodePtr pnodeStmt = ParseTryCatchFinally<buildAST>();
        if (buildAST)
        {
            pnode->sxBlock.pnodeStmt = pnodeStmt;
        }
        PopStmt(&stmt);
        break;
    }

    case tkWITH:
    {
        if ( IsStrictMode() )
        {
            Error(ERRES5NoWith);
        }
        if (m_currentNodeFunc)
        {
            GetCurrentFunctionNode()->sxFnc.SetHasWithStmt(); // Used by DeferNested
        }

        ichMin = m_pscan->IchMinTok();
        ChkNxtTok(tkLParen, ERRnoLparen);
        ParseNodePtr pnodeObj = ParseExpr<buildAST>();
        if (!buildAST)
        {
            m_scopeCountNoAst++;
        }
        charcount_t ichLim = m_pscan->IchLimTok();
        ChkCurTok(tkRParen, ERRnoRparen);

        if (buildAST)
        {
            pnode = CreateNodeWithScanner<knopWith>(ichMin);
        }
        PushStmt<buildAST>(&stmt, pnode, knopWith, pnodeLabel, pLabelIdList);

        ParseNodePtr *ppnodeExprScopeSave = nullptr;
        if (buildAST)
        {
            pnode->sxWith.pnodeObj = pnodeObj;
            this->CheckArguments(pnode->sxWith.pnodeObj);

            if (m_ppnodeExprScope)
            {
                Assert(*m_ppnodeExprScope == nullptr);
                *m_ppnodeExprScope = pnode;
                m_ppnodeExprScope = &pnode->sxWith.pnodeNext;
            }
            else
            {
                Assert(m_ppnodeScope);
                Assert(*m_ppnodeScope == nullptr);
                *m_ppnodeScope = pnode;
                m_ppnodeScope = &pnode->sxWith.pnodeNext;
            }
            pnode->sxWith.pnodeNext = nullptr;
            pnode->sxWith.scope = nullptr;

            ppnodeExprScopeSave = m_ppnodeExprScope;
            m_ppnodeExprScope = &pnode->sxWith.pnodeScopes;
            pnode->sxWith.pnodeScopes = nullptr;

            pnode->ichLim = ichLim;
        }

        PushBlockInfo(CreateBlockNode());
        PushDynamicBlock();

        ParseNodePtr pnodeBody = ParseStatement<buildAST>();
        if (buildAST)
        {
            pnode->sxWith.pnodeBody = pnodeBody;
            m_ppnodeExprScope = ppnodeExprScopeSave;
        }
        else
        {
            m_scopeCountNoAst--;
        }

        // The dynamic block is not stored in the actual parse tree and so will not
        // be visited by the byte code generator.  Grab the callsEval flag off it and
        // pass on to outer block in case of:
        // with (...) eval(...); // i.e. blockless form of with
        bool callsEval = GetCurrentBlock()->sxBlock.GetCallsEval();
        PopBlockInfo();
        if (callsEval)
        {
            // be careful not to overwrite an existing true with false
            GetCurrentBlock()->sxBlock.SetCallsEval(true);
        }

        PopStmt(&stmt);
        break;
    }

    case tkLCurly:
        pnode = ParseBlock<buildAST>(pnodeLabel, pLabelIdList);
        break;

    case tkSColon:
        pnode = nullptr;
        m_pscan->Scan();
        break;

    case tkBREAK:
        if (buildAST)
        {
            pnode = CreateNodeWithScanner<knopBreak>();
        }
        fnop = fnopBreak;
        goto LGetJumpStatement;

    case tkCONTINUE:
        if (buildAST)
        {
            pnode = CreateNode(knopContinue);
        }
        fnop = fnopContinue;

LGetJumpStatement:
        m_pscan->ScanForcingPid();
        if (tkID == m_token.tk && !m_pscan->FHadNewLine())
        {
            // Labeled break or continue.
            pid = m_token.GetIdentifier(m_phtbl);
            AssertMem(pid);
            if (buildAST)
            {
                pnode->sxJump.hasExplicitTarget=true;
                pnode->ichLim = m_pscan->IchLimTok();

                m_pscan->Scan();
                PushStmt<buildAST>(&stmt, pnode, pnode->nop, pnodeLabel, nullptr);
                Assert(pnode->sxStmt.grfnop == 0);
                for (pstmt = m_pstmtCur; nullptr != pstmt; pstmt = pstmt->pstmtOuter)
                {
                    AssertNodeMem(pstmt->pnodeStmt);
                    AssertNodeMemN(pstmt->pnodeLab);
                    for (pnodeT = pstmt->pnodeLab; nullptr != pnodeT;
                         pnodeT = pnodeT->sxLabel.pnodeNext)
                    {
                        Assert(knopLabel == pnodeT->nop);
                        if (pid == pnodeT->sxLabel.pid)
                        {
                            // Found the label. Make sure we can use it. We can
                            // break out of any statement, but we can only
                            // continue loops.
                            if (fnop == fnopContinue &&
                                !(pstmt->pnodeStmt->Grfnop() & fnop))
                            {
                                Error(ERRbadContinue);
                            }
                            else
                            {
                                pstmt->pnodeStmt->sxStmt.grfnop |= fnop;
                                pnode->sxJump.pnodeTarget = pstmt->pnodeStmt;
                            }
                            PopStmt(&stmt);
                            goto LNeedTerminator;
                        }
                    }
                    pnode->sxStmt.grfnop |=
                        (pstmt->pnodeStmt->Grfnop() & fnopCleanup);
                }
            }
            else
            {
                m_pscan->Scan();
                for (pstmt = m_pstmtCur; pstmt; pstmt = pstmt->pstmtOuter)
                {
                    LabelId* pLabelId;
                    for (pLabelId = pstmt->pLabelId; pLabelId; pLabelId = pLabelId->next)
                    {

                        if (pid == pLabelId->pid)
                        {
                            // Found the label. Make sure we can use it. We can
                            // break out of any statement, but we can only
                            // continue loops.
                            if (fnop == fnopContinue &&
                                !(ParseNode::Grfnop(pstmt->op) & fnop))
                            {
                                Error(ERRbadContinue);
                            }
                            goto LNeedTerminator;
                        }
                    }
                }
            }
            Error(ERRnoLabel);
        }
        else
        {
            // If we're doing a fast scan, we're not tracking labels, so we can't accurately do this analysis.
            // Let the thread that's doing the full parse detect the error, if there is one.
            if (!this->m_doingFastScan)
            {
                // Unlabeled break or continue.
                if (buildAST)
                {
                    pnode->sxJump.hasExplicitTarget=false;
                    PushStmt<buildAST>(&stmt, pnode, pnode->nop, pnodeLabel, nullptr);
                    Assert(pnode->sxStmt.grfnop == 0);
                }

                for (pstmt = m_pstmtCur; nullptr != pstmt; pstmt = pstmt->pstmtOuter)
                {
                    if (buildAST)
                    {
                        AnalysisAssert(pstmt->pnodeStmt);
                        if (pstmt->pnodeStmt->Grfnop() & fnop)
                        {
                            pstmt->pnodeStmt->sxStmt.grfnop |= fnop;
                            pnode->sxJump.pnodeTarget = pstmt->pnodeStmt;
                            PopStmt(&stmt);
                            goto LNeedTerminator;
                        }
                        pnode->sxStmt.grfnop |=
                            (pstmt->pnodeStmt->Grfnop() & fnopCleanup);
                    }
                    else
                    {
                        if (ParseNode::Grfnop(pstmt->GetNop()) & fnop)
                        {
                            if (!pstmt->isDeferred)
                            {
                                AnalysisAssert(pstmt->pnodeStmt);
                                pstmt->pnodeStmt->sxStmt.grfnop |= fnop;
                            }
                            goto LNeedTerminator;
                        }
                    }
                }
                Error(fnop == fnopBreak ? ERRbadBreak : ERRbadContinue);
            }
            goto LNeedTerminator;
        }

    case tkRETURN:
    {
        if (buildAST)
        {
            if (nullptr == m_currentNodeFunc)
            {
                Error(ERRbadReturn);
            }
            pnode = CreateNodeWithScanner<knopReturn>();
        }
        m_pscan->Scan();
        ParseNodePtr pnodeExpr = nullptr;
        ParseOptionalExpr<buildAST>(&pnodeExpr, true);
        if (buildAST)
        {
            pnode->sxReturn.pnodeExpr = pnodeExpr;
            if (pnodeExpr)
            {
                this->CheckArguments(pnode->sxReturn.pnodeExpr);
                pnode->ichLim = pnode->sxReturn.pnodeExpr->ichLim;
            }
            // See if return should call finally
            PushStmt<buildAST>(&stmt, pnode, knopReturn, pnodeLabel, nullptr);
            Assert(pnode->sxStmt.grfnop == 0);
            for (pstmt = m_pstmtCur; nullptr != pstmt; pstmt = pstmt->pstmtOuter)
            {
                AssertNodeMem(pstmt->pnodeStmt);
                AssertNodeMemN(pstmt->pnodeLab);
                if (pstmt->pnodeStmt->Grfnop() & fnopCleanup)
                {
                    pnode->sxStmt.grfnop |= fnopCleanup;
                    break;
                }
            }
            PopStmt(&stmt);
        }
        goto LNeedTerminator;
    }

    case tkTHROW:
    {
        if (buildAST)
        {
            pnode = CreateUniNode(knopThrow, nullptr);
        }
        m_pscan->Scan();
        ParseNodePtr pnode1 = nullptr;
        if (m_token.tk != tkSColon &&
            m_token.tk != tkRCurly &&
            !m_pscan->FHadNewLine())
        {
            pnode1 = ParseExpr<buildAST>();
        }
        else
        {
            Error(ERRdanglingThrow);
        }

        if (buildAST)
        {
            pnode->sxUni.pnode1 = pnode1;
            if (pnode1)
            {
                this->CheckArguments(pnode->sxUni.pnode1);
                pnode->ichLim = pnode->sxUni.pnode1->ichLim;
            }
        }
        goto LNeedTerminator;
    }

    case tkDEBUGGER:
        if (buildAST)
        {
            pnode = CreateNodeWithScanner<knopDebugger>();
        }
        m_pscan->Scan();
        goto LNeedTerminator;

    case tkIMPORT:
        if (!(m_grfscr & fscrIsModuleCode))
        {
            goto LDefaultToken;
        }

        pnode = ParseImportDeclaration<buildAST>();

        goto LNeedTerminator;

    case tkEXPORT:
        if (!(m_grfscr & fscrIsModuleCode))
        {
            goto LDefaultToken;
        }

        pnode = ParseExportDeclaration<buildAST>();

        goto LNeedTerminator;

LDefaultToken:
    default:
    {
        // First check for a label via lookahead. If not found,
        // rewind and reparse as expression statement.
        if (m_token.tk == tkLParen || m_token.tk == tkID)
        {
            RestorePoint idStart;
            m_pscan->Capture(&idStart);

            // Support legacy behavior of allowing parentheses around label identifiers.
            // Require balanced parentheses for correcting parsing.  Note unbalanced cases
            // take care of themselves correctly by resulting in rewind and parsing as
            // an expression statement.
            // REVIEW[ianhall]: Can this legacy functionality be removed? Chrome does not support this parsing behavior.
            uint parenCount = 0;
            while (m_token.tk == tkLParen)
            {
                parenCount += 1;
                m_pscan->Scan();
            }

            if (m_token.tk == tkID)
            {
                IdentToken tokInner;
                tokInner.tk = tkID;
                tokInner.ichMin = m_pscan->IchMinTok();
                tokInner.ichLim = m_pscan->IchLimTok();
                tokInner.pid = m_token.GetIdentifier(m_phtbl);

                m_pscan->Scan();

                while (parenCount > 0 && m_token.tk == tkRParen)
                {
                    parenCount -= 1;
                    m_pscan->Scan();
                }

                if (parenCount == 0 && m_token.tk == tkColon)
                {
                    // We have a label.
                    // TODO[ianhall]: Refactor to eliminate separate code paths for buildAST and !buildAST
                    if (buildAST)
                    {
                        // See if the label is already defined.
                        if (nullptr != PnodeLabel(tokInner.pid, pnodeLabel))
                        {
                            Error(ERRbadLabel);
                        }
                        pnodeT = CreateNodeWithScanner<knopLabel>();
                        pnodeT->sxLabel.pid = tokInner.pid;
                        pnodeT->sxLabel.pnodeNext = pnodeLabel;
                        pnodeLabel = pnodeT;
                    }
                    else
                    {
                        // See if the label is already defined.
                        if (PnodeLabelNoAST(&tokInner, pLabelIdList))
                        {
                            Error(ERRbadLabel);
                        }
                        LabelId* pLabelId = CreateLabelId(&tokInner);
                        pLabelId->next = pLabelIdList;
                        pLabelIdList = pLabelId;
                    }
                    m_pscan->Scan();
                    goto LRestart;
                }
            }

            // No label, rewind back to the tkID and parse an expression
            m_pscan->SeekTo(idStart);
        }

        // Must be an expression statement.
        pnode = ParseExpr<buildAST>();

        if (m_hasDeferredShorthandInitError)
        {
            Error(ERRnoColon);
        }

        if (buildAST)
        {
            expressionStmt = true;

            AnalysisAssert(pnode);
            pnode->isUsed = false;
        }
    }

LNeedTerminator:
        // Need a semicolon, new-line, } or end-of-file.
        // We digest a semicolon if it's there.
        switch (m_token.tk)
        {
        case tkSColon:
            m_pscan->Scan();
            if (pnode!= nullptr) pnode->grfpn |= PNodeFlags::fpnExplicitSemicolon;
            break;
        case tkEOF:
        case tkRCurly:
            if (pnode!= nullptr) pnode->grfpn |= PNodeFlags::fpnAutomaticSemicolon;
            break;
        default:
            if (!m_pscan->FHadNewLine())
            {
                Error(ERRnoSemic);
            }
            else
            {
                if (pnode!= nullptr) pnode->grfpn |= PNodeFlags::fpnAutomaticSemicolon;
            }
            break;
        }
        break;
    }

    if (m_hasDeferredShorthandInitError)
    {
        Error(ERRnoColon);
    }

    if (buildAST)
    {
        // All non expression statements excluded from the "this.x" optimization
        // Another check while parsing expressions
        if (!expressionStmt)
        {
            if (m_currentNodeFunc)
            {
                m_currentNodeFunc->sxFnc.SetHasNonThisStmt();
            }
            else if (m_currentNodeProg)
            {
                m_currentNodeProg->sxFnc.SetHasNonThisStmt();
            }
        }

#if EXCEPTION_RECOVERY
        // close the try/catch block
        if(Js::Configuration::Global.flags.SwallowExceptions)
        {
            // pop the try block and fill in the body
            PopStmt(&stmtTryBlock);
            pTryBlock->sxBlock.pnodeStmt = pnode;
            PopStmt(&stmtTry);
            if(pnode != nullptr)
            {
                pTry->ichLim = pnode->ichLim;
            }
            pTry->sxTry.pnodeBody = pTryBlock;


            // create a catch block with an empty body
            StmtNest stmtCatch;
            ParseNodePtr pCatch;
            pCatch = CreateNodeWithScanner<knopCatch>();
            PushStmt<buildAST>(&stmtCatch, pCatch, knopCatch, nullptr, nullptr);
            pCatch->sxCatch.pnodeBody = nullptr;
            if(pnode != nullptr)
            {
                pCatch->ichLim = pnode->ichLim;
            }
            pCatch->sxCatch.grfnop = 0;
            pCatch->sxCatch.pnodeNext = nullptr;

            // create a fake name for the catch var.
            const WCHAR *uniqueNameStr = _u("__ehobj");
            IdentPtr uniqueName = m_phtbl->PidHashNameLen(uniqueNameStr, static_cast<int32>(wcslen(uniqueNameStr)));

            pCatch->sxCatch.pnodeParam = CreateNameNode(uniqueName);

            // Add this catch to the current list. We don't bother adjusting the catch and function expression
            // lists here because the catch is just an empty statement.

            if (m_ppnodeExprScope)
            {
                Assert(*m_ppnodeExprScope == nullptr);
                *m_ppnodeExprScope = pCatch;
                m_ppnodeExprScope = &pCatch->sxCatch.pnodeNext;
            }
            else
            {
                Assert(m_ppnodeScope);
                Assert(*m_ppnodeScope == nullptr);
                *m_ppnodeScope = pCatch;
                m_ppnodeScope = &pCatch->sxCatch.pnodeNext;
            }

            pCatch->sxCatch.pnodeScopes = nullptr;

            PopStmt(&stmtCatch);

            // fill in and pop the try-catch
            pParentTryCatch->sxTryCatch.pnodeTry = pTry;
            pParentTryCatch->sxTryCatch.pnodeCatch = pCatch;
            PopStmt(&stmtTryCatch);
            PopStmt(&stmtTryCatchBlock);

            // replace the node that's being returned
            pParentTryCatchBlock->sxBlock.pnodeStmt = pParentTryCatch;
            pnode = pParentTryCatchBlock;
        }
#endif // EXCEPTION_RECOVERY

    }

    return pnode;
}