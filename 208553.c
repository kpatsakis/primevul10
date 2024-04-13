void Parser::AddModuleSpecifier(IdentPtr moduleRequest)
{
    IdentPtrList* requestedModulesList = EnsureRequestedModulesList();

    if (!requestedModulesList->Has(moduleRequest))
    {
        requestedModulesList->Prepend(moduleRequest);
    }
}