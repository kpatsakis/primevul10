Symbol* Parser::AddDeclForPid(ParseNodePtr pnode, IdentPtr pid, SymbolType symbolType, bool errorOnRedecl, bool *isRedecl)
{
    Assert(pnode->IsVarLetOrConst());

    PidRefStack *refForUse = nullptr, *refForDecl = nullptr;

    if (isRedecl)
    {
        *isRedecl = false;
    }

    BlockInfoStack *blockInfo;
    bool fBlockScope = false;
    if (pnode->nop != knopVarDecl || symbolType == STFunction)
    {
        Assert(m_pstmtCur);
        if (m_pstmtCur->GetNop() != knopBlock)
        {
            // Let/const declared in a bare statement context.
            Error(ERRDeclOutOfStmt);
        }

        if (m_pstmtCur->pstmtOuter && m_pstmtCur->pstmtOuter->GetNop() == knopSwitch)
        {
            // Let/const declared inside a switch block (requiring conservative use-before-decl check).
            pnode->sxVar.isSwitchStmtDecl = true;
        }

        fBlockScope = pnode->nop != knopVarDecl ||
            (
                !GetCurrentBlockInfo()->pnodeBlock->sxBlock.scope ||
                GetCurrentBlockInfo()->pnodeBlock->sxBlock.scope->GetScopeType() != ScopeType_GlobalEvalBlock
                );
    }
    if (fBlockScope)
    {
        blockInfo = GetCurrentBlockInfo();
    }
    else
    {
        blockInfo = GetCurrentFunctionBlockInfo();
    }

    // If we are creating an 'arguments' Sym at function block scope, create it in
    // the parameter scope instead. That way, if we need to reuse the Sym for the
    // actual arguments object at the end of the function, we don't need to move it
    // into the parameter scope.
    if (pid == wellKnownPropertyPids.arguments
        && pnode->nop == knopVarDecl
        && blockInfo->pnodeBlock->sxBlock.blockType == PnodeBlockType::Function
        && blockInfo->pBlockInfoOuter != nullptr
        && blockInfo->pBlockInfoOuter->pnodeBlock->sxBlock.blockType == PnodeBlockType::Parameter
        && blockInfo->pnodeBlock->sxBlock.scope->GetScopeType() != ScopeType_FuncExpr
        && blockInfo->pBlockInfoOuter->pnodeBlock->sxBlock.scope->GetCanMergeWithBodyScope())
    {
        blockInfo = blockInfo->pBlockInfoOuter;
    }

    refForDecl = this->FindOrAddPidRef(pid, blockInfo->pnodeBlock->sxBlock.blockId, GetCurrentFunctionNode()->sxFnc.functionId);

    if (refForDecl == nullptr)
    {
        Error(ERRnoMemory);
    }

    if (refForDecl->funcId != GetCurrentFunctionNode()->sxFnc.functionId)
    {
        // Fix up the function id, which is incorrect if we're reparsing lambda parameters
        Assert(this->m_reparsingLambdaParams);
        refForDecl->funcId = GetCurrentFunctionNode()->sxFnc.functionId;
    }
    
    if (blockInfo == GetCurrentBlockInfo())
    {
        refForUse = refForDecl;
    }
    else
    {
        refForUse = this->PushPidRef(pid);
    }
    pnode->sxVar.symRef = refForUse->GetSymRef();
    Symbol *sym = refForDecl->GetSym();
    if (sym != nullptr)
    {
        if (isRedecl)
        {
            *isRedecl = true;
        }
        // Multiple declarations in the same scope. 3 possibilities: error, existing one wins, new one wins.
        switch (pnode->nop)
        {
        case knopLetDecl:
        case knopConstDecl:
            if (!sym->GetDecl()->sxVar.isBlockScopeFncDeclVar)
            {
                Assert(errorOnRedecl);
                // Redeclaration error.
                Error(ERRRedeclaration);
            }
            else
            {
                // (New) let/const hides the (old) var
                sym->SetSymbolType(symbolType);
                sym->SetDecl(pnode);
            }
            break;
        case knopVarDecl:
            if (m_currentScope->GetScopeType() == ScopeType_Parameter)
            {
                // If this is a parameter list, mark the scope to indicate that it has duplicate definition.
                // If later this turns out to be a non-simple param list (like function f(a, a, c = 1) {}) then it is a SyntaxError to have duplicate formals.
                m_currentScope->SetHasDuplicateFormals();
            }

            if (sym->GetDecl() == nullptr)
            {
                Assert(symbolType == STFunction);
                sym->SetDecl(pnode);
                break;
            }
            switch (sym->GetDecl()->nop)
            {
            case knopLetDecl:
            case knopConstDecl:
                // Destructuring made possible to have the formals to be the let bind. But that shouldn't throw the error.
                if (errorOnRedecl && (!IsES6DestructuringEnabled() || sym->GetSymbolType() != STFormal))
                {
                    Error(ERRRedeclaration);
                }
                // If !errorOnRedecl, (old) let/const hides the (new) var, so do nothing.
                break;
            case knopVarDecl:
                // Legal redeclaration. Who wins?
                if (errorOnRedecl || sym->GetDecl()->sxVar.isBlockScopeFncDeclVar)
                {
                    if (symbolType == STFormal ||
                        (symbolType == STFunction && sym->GetSymbolType() != STFormal) ||
                        sym->GetSymbolType() == STVariable)
                    {
                        // New decl wins.
                        sym->SetSymbolType(symbolType);
                        sym->SetDecl(pnode);
                    }
                }
                break;
            }
            break;
        }
    }
    else
    {
        Scope *scope = blockInfo->pnodeBlock->sxBlock.scope;
        if (scope == nullptr)
        {
            Assert(blockInfo->pnodeBlock->sxBlock.blockType == PnodeBlockType::Regular);
            scope = Anew(&m_nodeAllocator, Scope, &m_nodeAllocator, ScopeType_Block);
            if (this->IsCurBlockInLoop())
            {
                scope->SetIsBlockInLoop();
            }
            blockInfo->pnodeBlock->sxBlock.scope = scope;
            PushScope(scope);
        }

        if (scope->GetScopeType() == ScopeType_GlobalEvalBlock)
        {
            Assert(fBlockScope);
            Assert(scope->GetEnclosingScope() == m_currentNodeProg->sxProg.scope);
            // Check for same-named decl in Global scope.
            PidRefStack *pidRefOld = pid->GetPidRefForScopeId(0);
            if (pidRefOld && pidRefOld->GetSym())
            {
                Error(ERRRedeclaration);
            }
        }
        else if (scope->GetScopeType() == ScopeType_Global && (this->m_grfscr & fscrEvalCode) &&
                 !(m_functionBody && m_functionBody->GetScopeInfo()))
        {
            // Check for same-named decl in GlobalEvalBlock scope. Note that this is not necessary
            // if we're compiling a deferred nested function and the global scope was restored from cached info,
            // because in that case we don't need a GlobalEvalScope.
            Assert(!fBlockScope || (this->m_grfscr & fscrConsoleScopeEval) == fscrConsoleScopeEval);
            PidRefStack *pidRefOld = pid->GetPidRefForScopeId(1);
            if (pidRefOld && pidRefOld->GetSym())
            {
                Error(ERRRedeclaration);
            }
        }

        if ((scope->GetScopeType() == ScopeType_FunctionBody || scope->GetScopeType() == ScopeType_Parameter) && symbolType != STFunction)
        {
            ParseNodePtr pnodeFnc = GetCurrentFunctionNode();
            AnalysisAssert(pnodeFnc);
            if (pnodeFnc->sxFnc.pnodeName &&
                pnodeFnc->sxFnc.pnodeName->nop == knopVarDecl &&
                pnodeFnc->sxFnc.pnodeName->sxVar.pid == pid)
            {
                // Named function expression has its name hidden by a local declaration.
                // This is important to know if we don't know whether nested deferred functions refer to it,
                // because if the name has a non-local reference then we have to create a scope object.
                m_currentNodeFunc->sxFnc.SetNameIsHidden();
            }
        }

        if (!sym)
        {
            const char16 *name = reinterpret_cast<const char16*>(pid->Psz());
            int nameLength = pid->Cch();
            SymbolName const symName(name, nameLength);

            Assert(!scope->FindLocalSymbol(symName));
            sym = Anew(&m_nodeAllocator, Symbol, symName, pnode, symbolType);
            scope->AddNewSymbol(sym);
            sym->SetPid(pid);
        }
        refForDecl->SetSym(sym);
    }
    return sym;
}