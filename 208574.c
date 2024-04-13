PidRefStack* Parser::PushPidRef(IdentPtr pid)
{
    if (PHASE_ON1(Js::ParallelParsePhase))
    {
        // NOTE: the phase check is here to protect perf. See OSG 1020424.
        // In some LS AST-rewrite cases we lose a lot of perf searching the PID ref stack rather
        // than just pushing on the top. This hasn't shown up as a perf issue in non-LS benchmarks.
        return pid->FindOrAddPidRef(&m_nodeAllocator, GetCurrentBlock()->sxBlock.blockId, GetCurrentFunctionNode()->sxFnc.functionId);
    }

    Assert(GetCurrentBlock() != nullptr);
    AssertMsg(pid != nullptr, "PID should be created");
    PidRefStack *ref = pid->GetTopRef();
    int blockId = GetCurrentBlock()->sxBlock.blockId;
    int funcId = GetCurrentFunctionNode()->sxFnc.functionId;
    if (!ref || (ref->GetScopeId() < blockId))
    {
        ref = Anew(&m_nodeAllocator, PidRefStack);
        if (ref == nullptr)
        {
            Error(ERRnoMemory);
        }
        pid->PushPidRef(blockId, funcId, ref);
    }
    else if (m_reparsingLambdaParams)
    {
        // If we're reparsing params, then we may have pid refs left behind from the first pass. Make sure we're
        // working with the right ref at this point.
        ref = this->FindOrAddPidRef(pid, blockId, funcId);
        // Fix up the function ID if we're reparsing lambda parameters.
        ref->funcId = funcId;
    }

    return ref;
}