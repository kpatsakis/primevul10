bool Parser::IsCurBlockInLoop() const
{
    for (StmtNest *stmt = this->m_pstmtCur; stmt != nullptr; stmt = stmt->pstmtOuter)
    {
        OpCode nop = stmt->GetNop();
        if (ParseNode::Grfnop(nop) & fnopContinue)
        {
            return true;
        }
        if (nop == knopFncDecl)
        {
            return false;
        }
    }
    return false;
}