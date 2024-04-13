void Parser::PrepareScanner(bool fromExternal)
{
    // NOTE: HashTbl and Scanner are currently allocated from the CRT heap. If we want to allocate them from the
    // parser arena, then we also need to change the way the HashTbl allocates PID's from its underlying
    // allocator (which also currently uses the CRT heap). This is not trivial, because we still need to support
    // heap allocation for the colorizer interface.

    // create the hash table and init PID members
    if (nullptr == (m_phtbl = HashTbl::Create(HASH_TABLE_SIZE, &m_err)))
        Error(ERRnoMemory);
    InitPids();

    // create the scanner
    if (nullptr == (m_pscan = Scanner_t::Create(this, m_phtbl, &m_token, &m_err, m_scriptContext)))
        Error(ERRnoMemory);

    if (fromExternal)
        m_pscan->FromExternalSource();
}