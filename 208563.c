ModuleImportOrExportEntryList* Parser::EnsureModuleIndirectExportEntryList()
{
    if (m_currentNodeProg->sxModule.indirectExportEntries == nullptr)
    {
        m_currentNodeProg->sxModule.indirectExportEntries = Anew(&m_nodeAllocator, ModuleImportOrExportEntryList, &m_nodeAllocator);
    }
    return m_currentNodeProg->sxModule.indirectExportEntries;
}