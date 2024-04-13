DriverModuleInitialized(const char *module)
{
    Driver *drvPtr;
    bool found = NS_FALSE;

    NS_NONNULL_ASSERT(module != NULL);

    for (drvPtr = firstDrvPtr; drvPtr != NULL;  drvPtr = drvPtr->nextPtr) {

        if (strcmp(drvPtr->moduleName, module) == 0) {
            found = NS_TRUE;
            Ns_Log(Notice, "Driver %s is already initialized", module);
            break;
        }
    }

    return found;
}