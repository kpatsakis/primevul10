void Parser::FinishDeferredFunction(ParseNodePtr pnodeScopeList)
{
    VisitFunctionsInScope(pnodeScopeList,
        [this](ParseNodePtr pnodeFnc)
    {
        Assert(pnodeFnc->nop == knopFncDecl);

        // Non-simple params (such as default) require a good amount of logic to put vars on appriopriate scopes. ParseFncDecl handles it
        // properly (both on defer and non-defer case). This is to avoid write duplicated logic here as well. Function with non-simple-param
        // will remain deferred untill they are called.
        if (pnodeFnc->sxFnc.pnodeBody == nullptr && !pnodeFnc->sxFnc.HasNonSimpleParameterList())
        {
            // Go back and generate an AST for this function.
            JS_ETW_INTERNAL(EventWriteJSCRIPT_PARSE_FUNC(this->GetScriptContext(), pnodeFnc->sxFnc.functionId, /*Undefer*/TRUE));

            ParseNodePtr pnodeFncSave = this->m_currentNodeFunc;
            this->m_currentNodeFunc = pnodeFnc;

            ParseNodePtr pnodeFncExprBlock = nullptr;
            if (pnodeFnc->sxFnc.pnodeName &&
                !pnodeFnc->sxFnc.IsDeclaration())
            {
                // Set up the named function expression symbol so references inside the function can be bound.
                ParseNodePtr pnodeName = pnodeFnc->sxFnc.pnodeName;
                Assert(pnodeName->nop == knopVarDecl);
                Assert(pnodeName->sxVar.pnodeNext == nullptr);

                pnodeFncExprBlock = this->StartParseBlock<true>(PnodeBlockType::Function, ScopeType_FuncExpr);
                PidRefStack *ref = this->PushPidRef(pnodeName->sxVar.pid);
                pnodeName->sxVar.symRef = ref->GetSymRef();
                ref->SetSym(pnodeName->sxVar.sym);

                Scope *fncExprScope = pnodeFncExprBlock->sxBlock.scope;
                fncExprScope->AddNewSymbol(pnodeName->sxVar.sym);
                pnodeFnc->sxFnc.scope = fncExprScope;
            }

            ParseNodePtr pnodeBlock = this->StartParseBlock<true>(PnodeBlockType::Parameter, ScopeType_Parameter);
            pnodeFnc->sxFnc.pnodeScopes = pnodeBlock;
            m_ppnodeScope = &pnodeBlock->sxBlock.pnodeScopes;
            pnodeBlock->sxBlock.pnodeStmt = pnodeFnc;

            // Add the args to the scope, since we won't re-parse those.
            Scope *scope = pnodeBlock->sxBlock.scope;
            auto addArgsToScope = [&](ParseNodePtr pnodeArg) {
                if (pnodeArg->IsVarLetOrConst())
                {
                    PidRefStack *ref = this->PushPidRef(pnodeArg->sxVar.pid);
                    pnodeArg->sxVar.symRef = ref->GetSymRef();
                    if (ref->GetSym() != nullptr)
                    {
                        // Duplicate parameter in a configuration that allows them.
                        // The symbol is already in the scope, just point it to the right declaration.
                        Assert(ref->GetSym() == pnodeArg->sxVar.sym);
                        ref->GetSym()->SetDecl(pnodeArg);
                    }
                    else
                    {
                        ref->SetSym(pnodeArg->sxVar.sym);
                        scope->AddNewSymbol(pnodeArg->sxVar.sym);
                    }
                }
            };
            MapFormals(pnodeFnc, addArgsToScope);
            MapFormalsFromPattern(pnodeFnc, addArgsToScope);

            ParseNodePtr pnodeInnerBlock = this->StartParseBlock<true>(PnodeBlockType::Function, ScopeType_FunctionBody);
            pnodeFnc->sxFnc.pnodeBodyScope = pnodeInnerBlock;

            // Set the parameter block's child to the function body block.
            *m_ppnodeScope = pnodeInnerBlock;

            ParseNodePtr *ppnodeScopeSave = nullptr;
            ParseNodePtr *ppnodeExprScopeSave = nullptr;

            ppnodeScopeSave = m_ppnodeScope;

            // This synthetic block scope will contain all the nested scopes.
            m_ppnodeScope = &pnodeInnerBlock->sxBlock.pnodeScopes;
            pnodeInnerBlock->sxBlock.pnodeStmt = pnodeFnc;

            // Keep nested function declarations and expressions in the same list at function scope.
            // (Indicate this by nulling out the current function expressions list.)
            ppnodeExprScopeSave = m_ppnodeExprScope;
            m_ppnodeExprScope = nullptr;

            // Shouldn't be any temps in the arg list.
            Assert(*m_ppnodeVar == nullptr);

            // Start the var list.
            pnodeFnc->sxFnc.pnodeVars = nullptr;
            m_ppnodeVar = &pnodeFnc->sxFnc.pnodeVars;

            if (scope != nullptr && !pnodeFnc->sxFnc.IsAsync())
            {
                if (scope->GetCanMergeWithBodyScope())
                {
                    scope->ForEachSymbol([this](Symbol* paramSym)
                    {
                        PidRefStack* ref = PushPidRef(paramSym->GetPid());
                        ref->SetSym(paramSym);
                    });
                }
                else
                {
                    OUTPUT_TRACE_DEBUGONLY(Js::ParsePhase, _u("The param and body scope of the function %s cannot be merged\n"), pnodeFnc->sxFnc.pnodeName ? pnodeFnc->sxFnc.pnodeName->sxVar.pid->Psz() : _u("Anonymous function"));
                    // Add a new symbol reference for each formal in the param scope to the body scope.
                    scope->ForEachSymbol([this](Symbol* param) {
                        OUTPUT_TRACE_DEBUGONLY(Js::ParsePhase, _u("Creating a duplicate symbol for the parameter %s in the body scope\n"), param->GetPid()->Psz());
                        ParseNodePtr paramNode = this->CreateVarDeclNode(param->GetPid(), STVariable, false, nullptr, false);
                        Assert(paramNode && paramNode->sxVar.sym->GetScope()->GetScopeType() == ScopeType_FunctionBody);
                        paramNode->sxVar.sym->SetHasInit(true);
                    });
                }
            }

            Assert(m_currentNodeNonLambdaFunc == nullptr);
            m_currentNodeNonLambdaFunc = pnodeFnc;

            this->FinishFncNode(pnodeFnc);

            Assert(pnodeFnc == m_currentNodeNonLambdaFunc);
            m_currentNodeNonLambdaFunc = nullptr;

            m_ppnodeExprScope = ppnodeExprScopeSave;

            AssertMem(m_ppnodeScope);
            Assert(nullptr == *m_ppnodeScope);
            m_ppnodeScope = ppnodeScopeSave;

            this->FinishParseBlock(pnodeInnerBlock);

            this->AddArgumentsNodeToVars(pnodeFnc);

            this->FinishParseBlock(pnodeBlock);
            if (pnodeFncExprBlock)
            {
                this->FinishParseBlock(pnodeFncExprBlock);
            }

            this->m_currentNodeFunc = pnodeFncSave;
        }
    });
}