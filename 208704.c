void Parser::BindPidRefsInScope(IdentPtr pid, Symbol *sym, int blockId, uint maxBlockId)
{
    PidRefStack *ref, *nextRef, *lastRef = nullptr;
    Js::LocalFunctionId funcId = GetCurrentFunctionNode()->sxFnc.functionId;
    Assert(sym);

    if (pid->GetIsModuleExport())
    {
        sym->SetIsModuleExportStorage(true);
    }

    bool hasFuncAssignment = sym->GetHasFuncAssignment();
    bool doesEscape = false;

    for (ref = pid->GetTopRef(); ref && ref->GetScopeId() >= blockId; ref = nextRef)
    {
        // Fix up sym* on PID ref.
        Assert(!ref->GetSym() || ref->GetSym() == sym);
        nextRef = ref->prev;
        Assert(ref->GetScopeId() >= 0);
        if ((uint)ref->GetScopeId() > maxBlockId)
        {
            lastRef = ref;
            continue;
        }
        ref->SetSym(sym);
        this->RemovePrevPidRef(pid, lastRef);

        if (ref->IsAssignment())
        {
            sym->PromoteAssignmentState();
            if (m_currentNodeFunc && sym->GetIsFormal())
            {
                m_currentNodeFunc->sxFnc.SetHasAnyWriteToFormals(true);                
            }
        }

        if (ref->GetFuncScopeId() != funcId && !sym->GetIsGlobal() && !sym->GetIsModuleExportStorage())
        {
            Assert(ref->GetFuncScopeId() > funcId);
            sym->SetHasNonLocalReference();
        }

        if (ref->IsFuncAssignment())
        {
            hasFuncAssignment = true;
        }

        if (ref->IsEscape())
        {
            doesEscape = true;
        }

        if (m_currentNodeFunc && doesEscape && hasFuncAssignment)
        {
            if (m_sourceContextInfo ? 
                    !PHASE_OFF_RAW(Js::DisableStackFuncOnDeferredEscapePhase, m_sourceContextInfo->sourceContextId, m_currentNodeFunc->sxFnc.functionId) :
                    !PHASE_OFF1(Js::DisableStackFuncOnDeferredEscapePhase))
            {
                m_currentNodeFunc->sxFnc.SetNestedFuncEscapes();
            }
        }

        if (ref->GetScopeId() == blockId)
        {
            break;
        }
    }
}