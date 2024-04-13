BOOL Parser::PnodeLabelNoAST(IdentToken* pToken, LabelId* pLabelIdList)
{
    StmtNest* pStmt;
    LabelId* pLabelId;

    // Look in the label stack.
    for (pStmt = m_pstmtCur; pStmt != nullptr; pStmt = pStmt->pstmtOuter)
    {
        for (pLabelId = pStmt->pLabelId; pLabelId != nullptr; pLabelId = pLabelId->next)
        {
            if (pLabelId->pid == pToken->pid)
                return TRUE;
        }
    }

    // Also look in the pnodeLabels list.
    for (pLabelId = pLabelIdList; pLabelId != nullptr; pLabelId = pLabelId->next)
    {
        if (pLabelId->pid == pToken->pid)
            return TRUE;
    }

    return FALSE;
}