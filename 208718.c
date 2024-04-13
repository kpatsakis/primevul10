ModuleImportOrExportEntryList* Parser::EnsureModuleStarExportEntryList()
{
    if (m_currentNodeProg->sxModule.starExportEntries == nullptr)
    {
        m_currentNodeProg->sxModule.starExportEntries = Anew(&m_nodeAllocator, ModuleImportOrExportEntryList, &m_nodeAllocator);
    }
    return m_currentNodeProg->sxModule.starExportEntries;
}