PidRefStack* Parser::FindOrAddPidRef(IdentPtr pid, int scopeId, Js::LocalFunctionId funcId)
{
    PidRefStack *ref = pid->FindOrAddPidRef(&m_nodeAllocator, scopeId, funcId);
    if (ref == NULL)
    {
        Error(ERRnoMemory);
    }
    return ref;
}