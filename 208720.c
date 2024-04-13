void Parser::CheckForDuplicateExportEntry(ModuleImportOrExportEntryList* exportEntryList, IdentPtr exportName)
{
    ModuleImportOrExportEntry* findResult = exportEntryList->Find([&](ModuleImportOrExportEntry exportEntry)
    {
        if (exportName == exportEntry.exportName)
        {
            return true;
        }
        return false;
    });

    if (findResult != nullptr)
    {
        Error(ERRsyntax);
    }
}