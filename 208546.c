ModuleImportOrExportEntry* Parser::AddModuleImportOrExportEntry(ModuleImportOrExportEntryList* importOrExportEntryList, ModuleImportOrExportEntry* importOrExportEntry)
{
    if (importOrExportEntry->exportName != nullptr)
    {
        CheckForDuplicateExportEntry(importOrExportEntryList, importOrExportEntry->exportName);
    }

    importOrExportEntryList->Prepend(*importOrExportEntry);

    return importOrExportEntry;
}