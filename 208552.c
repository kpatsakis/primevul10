ModuleImportOrExportEntryList* Parser::EnsureModuleLocalExportEntryList()
{
    if (m_currentNodeProg->sxModule.localExportEntries == nullptr)
    {
        m_currentNodeProg->sxModule.localExportEntries = Anew(&m_nodeAllocator, ModuleImportOrExportEntryList, &m_nodeAllocator);
    }
    return m_currentNodeProg->sxModule.localExportEntries;
}