ParseNodePtr Parser::PnodeLabel(IdentPtr pid, ParseNodePtr pnodeLabels)
{
    AssertMem(pid);
    AssertNodeMemN(pnodeLabels);

    StmtNest *pstmt;
    ParseNodePtr pnodeT;

    // Look in the statement stack.
    for (pstmt = m_pstmtCur; nullptr != pstmt; pstmt = pstmt->pstmtOuter)
    {
        AssertNodeMem(pstmt->pnodeStmt);
        AssertNodeMemN(pstmt->pnodeLab);

        for (pnodeT = pstmt->pnodeLab; nullptr != pnodeT;
            pnodeT = pnodeT->sxLabel.pnodeNext)
        {
            Assert(knopLabel == pnodeT->nop);
            if (pid == pnodeT->sxLabel.pid)
                return pnodeT;
        }
    }

    // Also look in the pnodeLabels list.
    for (pnodeT = pnodeLabels; nullptr != pnodeT;
        pnodeT = pnodeT->sxLabel.pnodeNext)
    {
        Assert(knopLabel == pnodeT->nop);
        if (pid == pnodeT->sxLabel.pid)
            return pnodeT;
    }

    return nullptr;
}