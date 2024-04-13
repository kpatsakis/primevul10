ParseNodePtr Parser::GenerateModuleFunctionWrapper()
{
    ParseNodePtr pnodeFnc = ParseFncDecl<buildAST>(fFncModule, nullptr, false, true, true);
    ParseNodePtr callNode = CreateCallNode(knopCall, pnodeFnc, nullptr);

    return callNode;
}