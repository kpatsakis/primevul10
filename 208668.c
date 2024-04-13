ParseNodePtr Parser::ParseRegExp()
{
    ParseNodePtr pnode = nullptr;

    if (buildAST || m_doingFastScan)
    {
        m_pscan->RescanRegExp();

        BOOL saveDeferringAST = this->m_deferringAST;
        if (m_doingFastScan)
        {
            this->m_deferringAST = false;
        }
        pnode = CreateNodeWithScanner<knopRegExp>();
        pnode->sxPid.regexPattern = m_token.GetRegex();
        if (m_doingFastScan)
        {
            this->m_deferringAST = saveDeferringAST;
            this->AddFastScannedRegExpNode(pnode);
            if (!buildAST)
            {
                pnode = nullptr;
            }
        }
#if ENABLE_BACKGROUND_PARSING
        else if (this->IsBackgroundParser())
        {
            Assert(pnode->sxPid.regexPattern == nullptr);
            this->AddBackgroundRegExpNode(pnode);
        }
#endif
    }
    else
    {
        m_pscan->RescanRegExpNoAST();
    }
    Assert(m_token.tk == tkRegExp);

    return pnode;
}