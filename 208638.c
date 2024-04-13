void Parser::RemovePrevPidRef(IdentPtr pid, PidRefStack *ref)
{
    PidRefStack *prevRef = pid->RemovePrevPidRef(ref);
    Assert(prevRef);
    if (prevRef->GetSym() == nullptr)
    {
        AllocatorDelete(ArenaAllocator, &m_nodeAllocator, prevRef);
    }
}