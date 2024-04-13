Jsi_Func *jsi_FuncMake(jsi_Pstate *pstate, Jsi_ScopeStrs *args, Jsi_OpCodes *ops, jsi_Pline* line, const char *name, int isArrow)
{
    Jsi_Interp *interp = pstate->interp;
    Jsi_ScopeStrs *localvar = jsi_ScopeGetVarlist(pstate);
    Jsi_Func *f = jsi_FuncNew(interp);
    jsi_Lexer *l = pstate->lexer;
    if (isArrow)
        f->isArrow = isArrow;
    f->type = FC_NORMAL;
    f->opcodes = ops;
    f->argnames = args;
    f->localnames = localvar;
    f->script = interp->curFile;
    f->bodyline = *line;
    f->retType = (Jsi_otype)args->retType;
    if (!pstate->eval_flag) {
        f->scriptFile = f->script;
    }
    if (l->ltype == LT_STRING)
        f->bodyStr = l->d.str;
    f->endPos = l->cur;
    f->startPos = -1; // Have to get these from newline count.
    if (f->retType & JSI_TT_UNDEFINED)
        Jsi_LogWarn("illegal use of 'undefined' in a return type: %s", name?name:"");
    
    //f->strict = (jsi_GetDirective(interp, ops, "use strict") != NULL);
    pstate->argType = 0;
    if (localvar && args && (interp->strict)) {
        int i, j;
        for (i=0; i<args->count; i++) {
            for (j=0; j<args->count; j++) {
                if (i != j && !Jsi_Strcmp(args->args[i].name, args->args[j].name)) {
                        if (line)
                            interp->parseLine = line;
                        Jsi_LogWarn("function %s():  duplicate parameter name '%s'", name?name:"", args->args[i].name);
                        if (line)
                            interp->parseLine = NULL;
                        jsi_TypeMismatch(interp);
                        if (interp->typeCheck.error)
                            pstate->err_count++;
                }
            }
            for (j=0; j<localvar->count; j++) {
                if (!Jsi_Strcmp(localvar->args[j].name, args->args[i].name)) {
                        if (line)
                            interp->parseLine = line;
                        Jsi_LogWarn("function %s():  parameter name conflicts with 'var %s'", name?name:"", localvar->args[j].name);
                        if (line)
                            interp->parseLine = NULL;
                        jsi_TypeMismatch(interp);
                        if (interp->typeCheck.error)
                            pstate->err_count++;
                }
            }
        }
    }
    if (name) {
        f->name = Jsi_KeyAdd(interp, name);
        if ((interp->typeCheck.run|interp->typeCheck.parse|interp->typeCheck.all|interp->typeCheck.funcsig)) {
            
            if (f->retType && !(f->retType&JSI_TT_VOID) && ops && ops->code_len && ops->codes[ops->code_len-1].op != OP_RET) {
                if (line)
                    interp->parseLine = line;
                Jsi_LogWarn("missing return at end of function '%s'", name);
                if (line)
                    interp->parseLine = NULL;
                //if (interp->typeCheck.error)
                 //   pstate->err_count++;
            }
             
            if (interp->staticFuncsTbl) {
                Jsi_Func *fo = (Jsi_Func*)Jsi_HashGet(interp->staticFuncsTbl, (void*)name, 0);
                
                // Forward declaration signature compare (indicated by an empty body).
                if (interp->typeCheck.funcsig && fo && fo->opcodes && fo->opcodes->code_len == 1 && fo->opcodes->codes->op == OP_NOP) {
                    if (!jsi_FuncSigsMatch(pstate, f, fo)) {
                        if (line)
                            interp->parseLine = line;
                        Jsi_LogWarn("possible signature mismatch for function '%s' at %.120s:%d", name, fo->script, fo->bodyline.first_line);
                        if (line)
                            interp->parseLine = NULL;
                        jsi_TypeMismatch(interp);
                    }
                    //printf("OLD: %s\n", name);
                }
                Jsi_HashSet(interp->staticFuncsTbl, name, f);
            }
        }
    }
    return f;
}