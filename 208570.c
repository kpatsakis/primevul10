ModuleImportOrExportEntry* Parser::AddModuleImportOrExportEntry(ModuleImportOrExportEntryList* importOrExportEntryList, IdentPtr importName, IdentPtr localName, IdentPtr exportName, IdentPtr moduleRequest)
{
    ModuleImportOrExportEntry* importOrExportEntry = Anew(&m_nodeAllocator, ModuleImportOrExportEntry);

    importOrExportEntry->importName = importName;
    importOrExportEntry->localName = localName;
    importOrExportEntry->exportName = exportName;
    importOrExportEntry->moduleRequest = moduleRequest;

    return AddModuleImportOrExportEntry(importOrExportEntryList, importOrExportEntry);
}