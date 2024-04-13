void Parser::PopStmt(StmtNest *pStmt)
{
    Assert(pStmt == m_pstmtCur);
    SetCurrentStatement(m_pstmtCur->pstmtOuter);
}