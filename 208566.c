ModuleImportOrExportEntryList* Parser::EnsureModuleImportEntryList()
{
    if (m_currentNodeProg->sxModule.importEntries == nullptr)
    {
        m_currentNodeProg->sxModule.importEntries = Anew(&m_nodeAllocator, ModuleImportOrExportEntryList, &m_nodeAllocator);
    }
    return m_currentNodeProg->sxModule.importEntries;
}