WCHAR * Parser::AllocateStringOfLength(ULONG length)
{
    Assert(length > 0);
    ULONG totalBytes;
    if (ULongMult(length, sizeof(OLECHAR), &totalBytes) != S_OK)
    {
        Error(ERRnoMemory);
    }
    WCHAR* finalName = (WCHAR*)m_phtbl->GetAllocator()->Alloc(totalBytes);
    if (finalName == nullptr)
    {
        Error(ERRnoMemory);
    }
    return finalName;
}