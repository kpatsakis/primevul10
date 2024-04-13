void PrintPnodeWIndent(ParseNode *pnode,int indentAmt) {
    if (pnode==NULL)
        return;

    Output::Print(_u("[%4d, %4d): "), pnode->ichMin, pnode->ichLim);
    switch (pnode->nop) {
        //PTNODE(knopName       , "name"        ,None    ,Pid  ,fnopLeaf)
  case knopName:
      Indent(indentAmt);
      if (pnode->sxPid.pid!=NULL) {
        Output::Print(_u("id: %s\n"),pnode->sxPid.pid->Psz());
      }
      else {
        Output::Print(_u("name node\n"));
      }
      break;
      //PTNODE(knopInt        , "int const"    ,None    ,Int  ,fnopLeaf|fnopConst)
  case knopInt:
      Indent(indentAmt);
      Output::Print(_u("%d\n"),pnode->sxInt.lw);
      break;
      //PTNODE(knopFlt        , "flt const"    ,None    ,Flt  ,fnopLeaf|fnopConst)
  case knopFlt:
      Indent(indentAmt);
      Output::Print(_u("%lf\n"),pnode->sxFlt.dbl);
      break;
      //PTNODE(knopStr        , "str const"    ,None    ,Pid  ,fnopLeaf|fnopConst)
  case knopStr:
      Indent(indentAmt);
      Output::Print(_u("\"%s\"\n"),pnode->sxPid.pid->Psz());
      break;
      //PTNODE(knopRegExp     , "reg expr"    ,None    ,Pid  ,fnopLeaf|fnopConst)
  case knopRegExp:
      Indent(indentAmt);
      Output::Print(_u("/%x/\n"),pnode->sxPid.regexPattern);
      break;
      //PTNODE(knopThis       , "this"        ,None    ,None ,fnopLeaf)
  case knopThis:
      Indent(indentAmt);
      Output::Print(_u("this\n"));
      break;
      //PTNODE(knopSuper      , "super"       ,None    ,None ,fnopLeaf)
  case knopSuper:
      Indent(indentAmt);
      Output::Print(_u("super\n"));
      break;
      //PTNODE(knopNewTarget  , "new.target"  ,None    ,None ,fnopLeaf)
  case knopNewTarget:
      Indent(indentAmt);
      Output::Print(_u("new.target\n"));
      break;
      //PTNODE(knopNull       , "null"        ,Null    ,None ,fnopLeaf)
  case knopNull:
      Indent(indentAmt);
      Output::Print(_u("null\n"));
      break;
      //PTNODE(knopFalse      , "false"        ,False   ,None ,fnopLeaf)
  case knopFalse:
      Indent(indentAmt);
      Output::Print(_u("false\n"));
      break;
      //PTNODE(knopTrue       , "true"        ,True    ,None ,fnopLeaf)
  case knopTrue:
      Indent(indentAmt);
      Output::Print(_u("true\n"));
      break;
      //PTNODE(knopEmpty      , "empty"        ,Empty   ,None ,fnopLeaf)
  case knopEmpty:
      Indent(indentAmt);
      Output::Print(_u("empty\n"));
      break;
      // Unary operators.
      //PTNODE(knopNot        , "~"            ,BitNot  ,Uni  ,fnopUni)
  case knopNot:
      Indent(indentAmt);
      Output::Print(_u("~\n"));
      PrintPnodeWIndent(pnode->sxUni.pnode1,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopNeg        , "unary -"    ,Neg     ,Uni  ,fnopUni)
  case knopNeg:
      Indent(indentAmt);
      Output::Print(_u("U-\n"));
      PrintPnodeWIndent(pnode->sxUni.pnode1,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopPos        , "unary +"    ,Pos     ,Uni  ,fnopUni)
  case knopPos:
      Indent(indentAmt);
      Output::Print(_u("U+\n"));
      PrintPnodeWIndent(pnode->sxUni.pnode1,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopLogNot     , "!"            ,LogNot  ,Uni  ,fnopUni)
  case knopLogNot:
      Indent(indentAmt);
      Output::Print(_u("!\n"));
      PrintPnodeWIndent(pnode->sxUni.pnode1,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopEllipsis     , "..."       ,Spread  ,Uni    , fnopUni)
  case knopEllipsis:
      Indent(indentAmt);
      Output::Print(_u("...<expr>\n"));
      PrintPnodeWIndent(pnode->sxUni.pnode1,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopIncPost    , "++ post"    ,Inc     ,Uni  ,fnopUni|fnopAsg)
  case knopIncPost:
      Indent(indentAmt);
      Output::Print(_u("<expr>++\n"));
      PrintPnodeWIndent(pnode->sxUni.pnode1,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopDecPost    , "-- post"    ,Dec     ,Uni  ,fnopUni|fnopAsg)
  case knopDecPost:
      Indent(indentAmt);
      Output::Print(_u("<expr>--\n"));
      PrintPnodeWIndent(pnode->sxUni.pnode1,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopIncPre     , "++ pre"    ,Inc     ,Uni  ,fnopUni|fnopAsg)
  case knopIncPre:
      Indent(indentAmt);
      Output::Print(_u("++<expr>\n"));
      PrintPnodeWIndent(pnode->sxUni.pnode1,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopDecPre     , "-- pre"    ,Dec     ,Uni  ,fnopUni|fnopAsg)
  case knopDecPre:
      Indent(indentAmt);
      Output::Print(_u("--<expr>\n"));
      PrintPnodeWIndent(pnode->sxUni.pnode1,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopTypeof     , "typeof"    ,None    ,Uni  ,fnopUni)
  case knopTypeof:
      Indent(indentAmt);
      Output::Print(_u("typeof\n"));
      PrintPnodeWIndent(pnode->sxUni.pnode1,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopVoid       , "void"        ,Void    ,Uni  ,fnopUni)
  case knopVoid:
      Indent(indentAmt);
      Output::Print(_u("void\n"));
      PrintPnodeWIndent(pnode->sxUni.pnode1,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopDelete     , "delete"    ,None    ,Uni  ,fnopUni)
  case knopDelete:
      Indent(indentAmt);
      Output::Print(_u("delete\n"));
      PrintPnodeWIndent(pnode->sxUni.pnode1,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopArray      , "arr cnst"    ,None    ,Uni  ,fnopUni)

  case knopArrayPattern:
      Indent(indentAmt);
      Output::Print(_u("Array Pattern\n"));
      PrintPnodeListWIndent(pnode->sxUni.pnode1, indentAmt + INDENT_SIZE);
      break;

  case knopObjectPattern:
      Indent(indentAmt);
      Output::Print(_u("Object Pattern\n"));
      PrintPnodeListWIndent(pnode->sxUni.pnode1, indentAmt + INDENT_SIZE);
      break;

  case knopArray:
      Indent(indentAmt);
      Output::Print(_u("Array Literal\n"));
      PrintPnodeListWIndent(pnode->sxUni.pnode1,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopObject     , "obj cnst"    ,None    ,Uni  ,fnopUni)
  case knopObject:
      Indent(indentAmt);
      Output::Print(_u("Object Literal\n"));
      PrintPnodeListWIndent(pnode->sxUni.pnode1,indentAmt+INDENT_SIZE);
      break;
      // Binary and Ternary Operators
      //PTNODE(knopAdd        , "+"            ,Add     ,Bin  ,fnopBin)
  case knopAdd:
      Indent(indentAmt);
      Output::Print(_u("+\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopSub        , "-"            ,Sub     ,Bin  ,fnopBin)
  case knopSub:
      Indent(indentAmt);
      Output::Print(_u("-\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopMul        , "*"            ,Mul     ,Bin  ,fnopBin)
  case knopMul:
      Indent(indentAmt);
      Output::Print(_u("*\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopDiv        , "/"            ,Div     ,Bin  ,fnopBin)
  case knopExpo:
      Indent(indentAmt);
      Output::Print(_u("**\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1, indentAmt + INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2, indentAmt + INDENT_SIZE);
      break;
      //PTNODE(knopExpo        , "**"            ,Expo     ,Bin  ,fnopBin)

  case knopDiv:
      Indent(indentAmt);
      Output::Print(_u("/\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopMod        , "%"            ,Mod     ,Bin  ,fnopBin)
  case knopMod:
      Indent(indentAmt);
      Output::Print(_u("%\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopOr         , "|"            ,BitOr   ,Bin  ,fnopBin)
  case knopOr:
      Indent(indentAmt);
      Output::Print(_u("|\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopXor        , "^"            ,BitXor  ,Bin  ,fnopBin)
  case knopXor:
      Indent(indentAmt);
      Output::Print(_u("^\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopAnd        , "&"            ,BitAnd  ,Bin  ,fnopBin)
  case knopAnd:
      Indent(indentAmt);
      Output::Print(_u("&\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopEq         , "=="        ,EQ      ,Bin  ,fnopBin|fnopRel)
  case knopEq:
      Indent(indentAmt);
      Output::Print(_u("==\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopNe         , "!="        ,NE      ,Bin  ,fnopBin|fnopRel)
  case knopNe:
      Indent(indentAmt);
      Output::Print(_u("!=\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopLt         , "<"            ,LT      ,Bin  ,fnopBin|fnopRel)
  case knopLt:
      Indent(indentAmt);
      Output::Print(_u("<\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopLe         , "<="        ,LE      ,Bin  ,fnopBin|fnopRel)
  case knopLe:
      Indent(indentAmt);
      Output::Print(_u("<=\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopGe         , ">="        ,GE      ,Bin  ,fnopBin|fnopRel)
  case knopGe:
      Indent(indentAmt);
      Output::Print(_u(">=\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopGt         , ">"            ,GT      ,Bin  ,fnopBin|fnopRel)
  case knopGt:
      Indent(indentAmt);
      Output::Print(_u(">\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopCall       , "()"        ,None    ,Bin  ,fnopBin)
  case knopCall:
      Indent(indentAmt);
      Output::Print(_u("Call\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeListWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopDot        , "."            ,None    ,Bin  ,fnopBin)
  case knopDot:
      Indent(indentAmt);
      Output::Print(_u(".\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopAsg        , "="            ,None    ,Bin  ,fnopBin|fnopAsg)
  case knopAsg:
      Indent(indentAmt);
      Output::Print(_u("=\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopInstOf     , "instanceof",InstOf  ,Bin  ,fnopBin|fnopRel)
  case knopInstOf:
      Indent(indentAmt);
      Output::Print(_u("instanceof\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopIn         , "in"        ,In      ,Bin  ,fnopBin|fnopRel)
  case knopIn:
      Indent(indentAmt);
      Output::Print(_u("in\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopEqv        , "==="        ,Eqv     ,Bin  ,fnopBin|fnopRel)
  case knopEqv:
      Indent(indentAmt);
      Output::Print(_u("===\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopNEqv       , "!=="        ,NEqv    ,Bin  ,fnopBin|fnopRel)
  case knopNEqv:
      Indent(indentAmt);
      Output::Print(_u("!==\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopComma      , ","            ,None    ,Bin  ,fnopBin)
  case knopComma:
      Indent(indentAmt);
      Output::Print(_u(",\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopLogOr      , "||"        ,None    ,Bin  ,fnopBin)
  case knopLogOr:
      Indent(indentAmt);
      Output::Print(_u("||\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopLogAnd     , "&&"        ,None    ,Bin  ,fnopBin)
  case knopLogAnd:
      Indent(indentAmt);
      Output::Print(_u("&&\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopLsh        , "<<"        ,Lsh     ,Bin  ,fnopBin)
  case knopLsh:
      Indent(indentAmt);
      Output::Print(_u("<<\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopRsh        , ">>"        ,Rsh     ,Bin  ,fnopBin)
  case knopRsh:
      Indent(indentAmt);
      Output::Print(_u(">>\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopRs2        , ">>>"        ,Rs2     ,Bin  ,fnopBin)
  case knopRs2:
      Indent(indentAmt);
      Output::Print(_u(">>>\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopNew        , "new"        ,None    ,Bin  ,fnopBin)
  case knopNew:
      Indent(indentAmt);
      Output::Print(_u("new\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeListWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopIndex      , "[]"        ,None    ,Bin  ,fnopBin)
  case knopIndex:
      Indent(indentAmt);
      Output::Print(_u("[]\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeListWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopQmark      , "?"            ,None    ,Tri  ,fnopBin)
  case knopQmark:
      Indent(indentAmt);
      Output::Print(_u("?:\n"));
      PrintPnodeWIndent(pnode->sxTri.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxTri.pnode2,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxTri.pnode3,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopAsgAdd     , "+="        ,Add     ,Bin  ,fnopBin|fnopAsg)
  case knopAsgAdd:
      Indent(indentAmt);
      Output::Print(_u("+=\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopAsgSub     , "-="        ,Sub     ,Bin  ,fnopBin|fnopAsg)
  case knopAsgSub:
      Indent(indentAmt);
      Output::Print(_u("-=\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopAsgMul     , "*="        ,Mul     ,Bin  ,fnopBin|fnopAsg)
  case knopAsgMul:
      Indent(indentAmt);
      Output::Print(_u("*=\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopAsgDiv     , "/="        ,Div     ,Bin  ,fnopBin|fnopAsg)
  case knopAsgExpo:
      Indent(indentAmt);
      Output::Print(_u("**=\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1, indentAmt + INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2, indentAmt + INDENT_SIZE);
      break;
      //PTNODE(knopAsgExpo     , "**="       ,Expo     ,Bin  ,fnopBin|fnopAsg)

  case knopAsgDiv:
      Indent(indentAmt);
      Output::Print(_u("/=\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopAsgMod     , "%="        ,Mod     ,Bin  ,fnopBin|fnopAsg)
  case knopAsgMod:
      Indent(indentAmt);
      Output::Print(_u("%=\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopAsgAnd     , "&="        ,BitAnd  ,Bin  ,fnopBin|fnopAsg)
  case knopAsgAnd:
      Indent(indentAmt);
      Output::Print(_u("&=\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopAsgXor     , "^="        ,BitXor  ,Bin  ,fnopBin|fnopAsg)
  case knopAsgXor:
      Indent(indentAmt);
      Output::Print(_u("^=\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopAsgOr      , "|="        ,BitOr   ,Bin  ,fnopBin|fnopAsg)
  case knopAsgOr:
      Indent(indentAmt);
      Output::Print(_u("|=\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopAsgLsh     , "<<="        ,Lsh     ,Bin  ,fnopBin|fnopAsg)
  case knopAsgLsh:
      Indent(indentAmt);
      Output::Print(_u("<<=\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopAsgRsh     , ">>="        ,Rsh     ,Bin  ,fnopBin|fnopAsg)
  case knopAsgRsh:
      Indent(indentAmt);
      Output::Print(_u(">>=\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopAsgRs2     , ">>>="        ,Rs2     ,Bin  ,fnopBin|fnopAsg)
  case knopAsgRs2:
      Indent(indentAmt);
      Output::Print(_u(">>>=\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;

  case knopComputedName:
      Indent(indentAmt);
      Output::Print(_u("ComputedProperty\n"));
      PrintPnodeWIndent(pnode->sxUni.pnode1, indentAmt + INDENT_SIZE);
      break;

      //PTNODE(knopMember     , ":"            ,None    ,Bin  ,fnopBin)
  case knopMember:
  case knopMemberShort:
  case knopObjectPatternMember:
      Indent(indentAmt);
      Output::Print(_u(":\n"));
      PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxBin.pnode2,indentAmt+INDENT_SIZE);
      break;
      // General nodes.
      //PTNODE(knopList       , "<list>"    ,None    ,Bin  ,fnopNone)
  case knopList:
      Indent(indentAmt);
      Output::Print(_u("List\n"));
      PrintPnodeListWIndent(pnode,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopVarDecl    , "varDcl"    ,None    ,Var  ,fnopNone)
  case knopVarDecl:
      Indent(indentAmt);
      Output::Print(_u("var %s\n"),pnode->sxVar.pid->Psz());
      if (pnode->sxVar.pnodeInit!=NULL)
          PrintPnodeWIndent(pnode->sxVar.pnodeInit,indentAmt+INDENT_SIZE);
      break;
  case knopConstDecl:
      Indent(indentAmt);
      Output::Print(_u("const %s\n"),pnode->sxVar.pid->Psz());
      if (pnode->sxVar.pnodeInit!=NULL)
          PrintPnodeWIndent(pnode->sxVar.pnodeInit,indentAmt+INDENT_SIZE);
      break;
  case knopLetDecl:
      Indent(indentAmt);
      Output::Print(_u("let %s\n"),pnode->sxVar.pid->Psz());
      if (pnode->sxVar.pnodeInit!=NULL)
          PrintPnodeWIndent(pnode->sxVar.pnodeInit,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopFncDecl    , "fncDcl"    ,None    ,Fnc  ,fnopLeaf)
  case knopFncDecl:
      Indent(indentAmt);
      if (pnode->sxFnc.pid!=NULL)
      {
          Output::Print(_u("fn decl %d nested %d name %s (%d-%d)\n"),pnode->sxFnc.IsDeclaration(),pnode->sxFnc.IsNested(),
              pnode->sxFnc.pid->Psz(), pnode->ichMin, pnode->ichLim);
      }
      else
      {
          Output::Print(_u("fn decl %d nested %d anonymous (%d-%d)\n"),pnode->sxFnc.IsDeclaration(),pnode->sxFnc.IsNested(),pnode->ichMin,pnode->ichLim);
      }
      PrintScopesWIndent(pnode, indentAmt+INDENT_SIZE);
      PrintFormalsWIndent(pnode->sxFnc.pnodeParams, indentAmt + INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxFnc.pnodeRest, indentAmt + INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxFnc.pnodeBody, indentAmt + INDENT_SIZE);
      if (pnode->sxFnc.pnodeBody == nullptr)
      {
          Output::Print(_u("[%4d, %4d): "), pnode->ichMin, pnode->ichLim);
          Indent(indentAmt + INDENT_SIZE);
          Output::Print(_u("<parse deferred body>\n"));
      }
      break;
      //PTNODE(knopProg       , "program"    ,None    ,Fnc  ,fnopNone)
  case knopProg:
      Indent(indentAmt);
      Output::Print(_u("program\n"));
      PrintScopesWIndent(pnode, indentAmt+INDENT_SIZE);
      PrintPnodeListWIndent(pnode->sxFnc.pnodeBody,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopEndCode    , "<endcode>"    ,None    ,None ,fnopNone)
  case knopEndCode:
      Indent(indentAmt);
      Output::Print(_u("<endcode>\n"));
      break;
      //PTNODE(knopDebugger   , "debugger"    ,None    ,None ,fnopNone)
  case knopDebugger:
      Indent(indentAmt);
      Output::Print(_u("<debugger>\n"));
      break;
      //PTNODE(knopFor        , "for"        ,None    ,For  ,fnopBreak|fnopContinue)
  case knopFor:
      Indent(indentAmt);
      Output::Print(_u("for\n"));
      PrintScopesWIndent(pnode, indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxFor.pnodeInit,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxFor.pnodeCond,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxFor.pnodeIncr,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxFor.pnodeBody,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopIf         , "if"        ,None    ,If   ,fnopNone)
  case knopIf:
      Indent(indentAmt);
      Output::Print(_u("if\n"));
      PrintPnodeWIndent(pnode->sxIf.pnodeCond,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxIf.pnodeTrue,indentAmt+INDENT_SIZE);
      if (pnode->sxIf.pnodeFalse!=NULL)
          PrintPnodeWIndent(pnode->sxIf.pnodeFalse,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopWhile      , "while"        ,None    ,While,fnopBreak|fnopContinue)
  case knopWhile:
      Indent(indentAmt);
      Output::Print(_u("while\n"));
      PrintPnodeWIndent(pnode->sxWhile.pnodeCond,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxWhile.pnodeBody,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopDoWhile    , "do-while"    ,None    ,While,fnopBreak|fnopContinue)
  case knopDoWhile:
      Indent(indentAmt);
      Output::Print(_u("do\n"));
      PrintPnodeWIndent(pnode->sxWhile.pnodeCond,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxWhile.pnodeBody,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopForIn      , "for in"    ,None    ,ForIn,fnopBreak|fnopContinue|fnopCleanup)
  case knopForIn:
      Indent(indentAmt);
      Output::Print(_u("forIn\n"));
      PrintScopesWIndent(pnode, indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxForInOrForOf.pnodeLval,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxForInOrForOf.pnodeObj,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxForInOrForOf.pnodeBody,indentAmt+INDENT_SIZE);
      break;
  case knopForOf:
      Indent(indentAmt);
      Output::Print(_u("forOf\n"));
      PrintScopesWIndent(pnode, indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxForInOrForOf.pnodeLval,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxForInOrForOf.pnodeObj,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxForInOrForOf.pnodeBody,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopReturn     , "return"    ,None    ,Uni  ,fnopNone)
  case knopReturn:
      Indent(indentAmt);
      Output::Print(_u("return\n"));
      if (pnode->sxReturn.pnodeExpr!=NULL)
          PrintPnodeWIndent(pnode->sxReturn.pnodeExpr,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopBlock      , "{}"        ,None    ,Block,fnopNone)
  case knopBlock:
      Indent(indentAmt);
      Output::Print(_u("block "));
      if (pnode->grfpn & fpnSyntheticNode)
          Output::Print(_u("synthetic "));
      PrintBlockType(pnode->sxBlock.blockType);
      Output::Print(_u("(%d-%d)\n"),pnode->ichMin,pnode->ichLim);
      PrintScopesWIndent(pnode, indentAmt+INDENT_SIZE);
      if (pnode->sxBlock.pnodeStmt!=NULL)
          PrintPnodeWIndent(pnode->sxBlock.pnodeStmt,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopWith       , "with"        ,None    ,With ,fnopCleanup)
  case knopWith:
      Indent(indentAmt);
      Output::Print(_u("with (%d-%d)\n"), pnode->ichMin,pnode->ichLim);
      PrintScopesWIndent(pnode, indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxWith.pnodeObj,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxWith.pnodeBody,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopBreak      , "break"        ,None    ,Jump ,fnopNone)
  case knopBreak:
      Indent(indentAmt);
      Output::Print(_u("break\n"));
      // TODO: some representation of target
      break;
      //PTNODE(knopContinue   , "continue"    ,None    ,Jump ,fnopNone)
  case knopContinue:
      Indent(indentAmt);
      Output::Print(_u("continue\n"));
      // TODO: some representation of target
      break;
      //PTNODE(knopLabel      , "label"        ,None    ,Label,fnopNone)
  case knopLabel:
      Indent(indentAmt);
      Output::Print(_u("label %s"),pnode->sxLabel.pid->Psz());
      // TODO: print labeled statement
      break;
      //PTNODE(knopSwitch     , "switch"    ,None    ,Switch,fnopBreak)
  case knopSwitch:
      Indent(indentAmt);
      Output::Print(_u("switch\n"));
      PrintScopesWIndent(pnode, indentAmt+INDENT_SIZE);
      for (ParseNode *pnodeT = pnode->sxSwitch.pnodeCases; NULL != pnodeT;pnodeT = pnodeT->sxCase.pnodeNext) {
          PrintPnodeWIndent(pnodeT,indentAmt+2);
      }
      break;
      //PTNODE(knopCase       , "case"        ,None    ,Case ,fnopNone)
  case knopCase:
      Indent(indentAmt);
      Output::Print(_u("case\n"));
      PrintPnodeWIndent(pnode->sxCase.pnodeExpr,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxCase.pnodeBody,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopTryFinally,"try-finally",None,TryFinally,fnopCleanup)
  case knopTryFinally:
      PrintPnodeWIndent(pnode->sxTryFinally.pnodeTry,indentAmt);
      PrintPnodeWIndent(pnode->sxTryFinally.pnodeFinally,indentAmt);
      break;
  case knopFinally:
      Indent(indentAmt);
      Output::Print(_u("finally\n"));
      PrintPnodeWIndent(pnode->sxFinally.pnodeBody,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopCatch      , "catch"     ,None    ,Catch,fnopNone)
  case knopCatch:
      Indent(indentAmt);
      Output::Print(_u("catch (%d-%d)\n"), pnode->ichMin,pnode->ichLim);
      PrintScopesWIndent(pnode, indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxCatch.pnodeParam,indentAmt+INDENT_SIZE);
//      if (pnode->sxCatch.pnodeGuard!=NULL)
//          PrintPnodeWIndent(pnode->sxCatch.pnodeGuard,indentAmt+INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxCatch.pnodeBody,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopTryCatch      , "try-catch" ,None    ,TryCatch  ,fnopCleanup)
  case knopTryCatch:
      PrintPnodeWIndent(pnode->sxTryCatch.pnodeTry,indentAmt);
      PrintPnodeWIndent(pnode->sxTryCatch.pnodeCatch,indentAmt);
      break;
      //PTNODE(knopTry        , "try"       ,None    ,Try  ,fnopCleanup)
  case knopTry:
      Indent(indentAmt);
      Output::Print(_u("try\n"));
      PrintPnodeWIndent(pnode->sxTry.pnodeBody,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopThrow      , "throw"     ,None    ,Uni  ,fnopNone)
  case knopThrow:
      Indent(indentAmt);
      Output::Print(_u("throw\n"));
      PrintPnodeWIndent(pnode->sxUni.pnode1,indentAmt+INDENT_SIZE);
      break;
      //PTNODE(knopClassDecl, "classDecl", None , Class, fnopLeaf)
  case knopClassDecl:
      Indent(indentAmt);
      Output::Print(_u("class %s"), pnode->sxClass.pnodeName->sxVar.pid->Psz());
      if (pnode->sxClass.pnodeExtends != nullptr)
      {
          Output::Print(_u(" extends "));
          PrintPnodeWIndent(pnode->sxClass.pnodeExtends, 0);
      }
      else {
          Output::Print(_u("\n"));
      }

      PrintPnodeWIndent(pnode->sxClass.pnodeConstructor,   indentAmt + INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxClass.pnodeMembers,       indentAmt + INDENT_SIZE);
      PrintPnodeWIndent(pnode->sxClass.pnodeStaticMembers, indentAmt + INDENT_SIZE);
      break;
  case knopStrTemplate:
      Indent(indentAmt);
      Output::Print(_u("string template\n"));
      PrintPnodeListWIndent(pnode->sxStrTemplate.pnodeSubstitutionExpressions, indentAmt + INDENT_SIZE);
      break;
  case knopYieldStar:
      Indent(indentAmt);
      Output::Print(_u("yield*\n"));
      PrintPnodeListWIndent(pnode->sxUni.pnode1, indentAmt + INDENT_SIZE);
      break;
  case knopYield:
  case knopYieldLeaf:
      Indent(indentAmt);
      Output::Print(_u("yield\n"));
      PrintPnodeListWIndent(pnode->sxUni.pnode1, indentAmt + INDENT_SIZE);
      break;
  case knopAwait:
      Indent(indentAmt);
      Output::Print(_u("await\n"));
      PrintPnodeListWIndent(pnode->sxUni.pnode1, indentAmt + INDENT_SIZE);
      break;
  case knopExportDefault:
      Indent(indentAmt);
      Output::Print(_u("export default\n"));
      PrintPnodeListWIndent(pnode->sxExportDefault.pnodeExpr, indentAmt + INDENT_SIZE);
      break;
  default:
      Output::Print(_u("unhandled pnode op %d\n"),pnode->nop);
      break;
    }
}