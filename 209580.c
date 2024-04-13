void jsi_ScopeStrsPush(Jsi_Interp *interp, Jsi_ScopeStrs *ss, const char *string, int argType)
{
    if (ss->count >= ss->_size) {
        int osz = ss->_size, isold = (ss->args!=NULL);
        ss->_size += ALLOC_MOD_SIZE;
        if (!isold)
            ss->args = (jsi_ArgValue*)Jsi_Calloc(ss->_size,  sizeof(ss->args[0]));
        else {
            ss->args = (jsi_ArgValue*)Jsi_Realloc(ss->args, (ss->_size) * sizeof(ss->args[0]));
            memset(ss->args+osz, 0, (ss->_size-osz)* sizeof(ss->args[0]));
        }
    }
    ss->args[ss->count].name = (char*)Jsi_KeyAdd(interp, string);
    ss->args[ss->count].type = argType;
    if (argType)
        ss->typeCnt++;
    ss->count++;
}