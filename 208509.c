IdentPtrList* Parser::EnsureRequestedModulesList()
{
    if (m_currentNodeProg->sxModule.requestedModules == nullptr)
    {
        m_currentNodeProg->sxModule.requestedModules = Anew(&m_nodeAllocator, IdentPtrList, &m_nodeAllocator);
    }
    return m_currentNodeProg->sxModule.requestedModules;
}