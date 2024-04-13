ParseNodePtr Parser::ParseCatch()
{
    ParseNodePtr rootNode = nullptr;
    ParseNodePtr* ppnode = &rootNode;
    ParseNodePtr *ppnodeExprScopeSave = nullptr;
    ParseNodePtr pnode = nullptr;
    ParseNodePtr pnodeCatchScope = nullptr;
    StmtNest stmt;
    IdentPtr pidCatch = nullptr;
    //while (tkCATCH == m_token.tk)
    if (tkCATCH == m_token.tk)
    {
        charcount_t ichMin;
        if (buildAST)
        {
            ichMin = m_pscan->IchMinTok();
        }
        m_pscan->Scan(); //catch
        ChkCurTok(tkLParen, ERRnoLparen); //catch(

        bool isPattern = false;
        if (tkID != m_token.tk)
        {
            isPattern = IsES6DestructuringEnabled() && IsPossiblePatternStart();
            if (!isPattern)
            {
                IdentifierExpectedError(m_token);
            }
        }

        if (buildAST)
        {
            pnode = CreateNodeWithScanner<knopCatch>(ichMin);
            PushStmt<buildAST>(&stmt, pnode, knopCatch, nullptr, nullptr);
            *ppnode = pnode;
            ppnode = &pnode->sxCatch.pnodeNext;
            *ppnode = nullptr;
        }

        pnodeCatchScope = StartParseBlock<buildAST>(PnodeBlockType::Regular, isPattern ? ScopeType_CatchParamPattern : ScopeType_Catch);

        if (buildAST)
        {
            // Add this catch to the current scope list.

            if (m_ppnodeExprScope)
            {
                Assert(*m_ppnodeExprScope == nullptr);
                *m_ppnodeExprScope = pnode;
                m_ppnodeExprScope = &pnode->sxCatch.pnodeNext;
            }
            else
            {
                Assert(m_ppnodeScope);
                Assert(*m_ppnodeScope == nullptr);
                *m_ppnodeScope = pnode;
                m_ppnodeScope = &pnode->sxCatch.pnodeNext;
            }

            // Keep a list of function expressions (not declarations) at this scope.

            ppnodeExprScopeSave = m_ppnodeExprScope;
            m_ppnodeExprScope = &pnode->sxCatch.pnodeScopes;
            pnode->sxCatch.pnodeScopes = nullptr;
        }

        if (isPattern)
        {
            ParseNodePtr pnodePattern = ParseDestructuredLiteral<buildAST>(tkLET, true /*isDecl*/, true /*topLevel*/, DIC_ForceErrorOnInitializer);
            if (buildAST)
            {
                pnode->sxCatch.pnodeParam = CreateParamPatternNode(pnodePattern);
                Scope *scope = pnodeCatchScope->sxBlock.scope;
                pnode->sxCatch.scope = scope;
            }
        }
        else
        {
            if (IsStrictMode())
            {
                IdentPtr pid = m_token.GetIdentifier(m_phtbl);
                if (pid == wellKnownPropertyPids.eval)
                {
                    Error(ERREvalUsage);
                }
                else if (pid == wellKnownPropertyPids.arguments)
                {
                    Error(ERRArgsUsage);
                }
            }

            pidCatch = m_token.GetIdentifier(m_phtbl);
            PidRefStack *ref = this->PushPidRef(pidCatch);

            ParseNodePtr pnodeParam = CreateNameNode(pidCatch);
            pnodeParam->sxPid.symRef = ref->GetSymRef();

            const char16 *name = reinterpret_cast<const char16*>(pidCatch->Psz());
            int nameLength = pidCatch->Cch();
            SymbolName const symName(name, nameLength);
            Symbol *sym = Anew(&m_nodeAllocator, Symbol, symName, pnodeParam, STVariable);
            sym->SetPid(pidCatch);
            if (sym == nullptr)
            {
                Error(ERRnoMemory);
            }
            Assert(ref->GetSym() == nullptr);
            ref->SetSym(sym);

            Scope *scope = pnodeCatchScope->sxBlock.scope;
            scope->AddNewSymbol(sym);

            if (buildAST)
            {
                pnode->sxCatch.pnodeParam = pnodeParam;
                pnode->sxCatch.scope = scope;
            }

            m_pscan->Scan();
        }

        charcount_t ichLim;
        if (buildAST)
        {
            ichLim = m_pscan->IchLimTok();
        }
        ChkCurTok(tkRParen, ERRnoRparen); //catch(id[:expr])

        if (tkLCurly != m_token.tk)
        {
            Error(ERRnoLcurly);
        }

        ParseNodePtr pnodeBody = ParseStatement<buildAST>();  //catch(id[:expr]) {block}
        if (buildAST)
        {
            pnode->sxCatch.pnodeBody = pnodeBody;
            pnode->ichLim = ichLim;
        }

        if (pnodeCatchScope != nullptr)
        {
            FinishParseBlock(pnodeCatchScope);
        }

        if (buildAST)
        {
            PopStmt(&stmt);

            // Restore the lists of function expression scopes.

            AssertMem(m_ppnodeExprScope);
            Assert(*m_ppnodeExprScope == nullptr);
            m_ppnodeExprScope = ppnodeExprScopeSave;
        }
    }
    return rootNode;
}