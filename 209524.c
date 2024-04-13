Jsi_HashDelete(Jsi_Hash *tablePtr)
{
    Jsi_HashClear(tablePtr);
    Jsi_Free(tablePtr);
}