ParseNodePtr Parser::CreateBlockScopedDeclNode(IdentPtr pid, OpCode nodeType)
{
    Assert(nodeType == knopConstDecl || nodeType == knopLetDecl);

    ParseNodePtr pnode = CreateDeclNode(nodeType, pid, STVariable, true);

    if (nullptr != pid)
    {
        AssertMem(pid);
        pid->SetIsLetOrConst();
        AddVarDeclToBlock(pnode);
        CheckPidIsValid(pid);
    }

    return pnode;
}