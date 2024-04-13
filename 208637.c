ParseNodePtr Parser::AddVarDeclNode(IdentPtr pid, ParseNodePtr pnodeFnc)
{
    AnalysisAssert(pnodeFnc);

    ParseNodePtr *const ppnodeVarSave = m_ppnodeVar;

    m_ppnodeVar = &pnodeFnc->sxFnc.pnodeVars;
    while (*m_ppnodeVar != nullptr)
    {
        m_ppnodeVar = &(*m_ppnodeVar)->sxVar.pnodeNext;
    }

    ParseNodePtr pnode = CreateVarDeclNode(pid, STUnknown, false, 0, /* checkReDecl = */ false);

    m_ppnodeVar = ppnodeVarSave;

    return pnode;
}