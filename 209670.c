int jsiPopArgs(Jsi_OpCodes *argCodes, int i)
{
    int m=i-1, n = (uintptr_t)argCodes->codes[i].data, cnt = 0;
    if (argCodes->codes[i].op == OP_OBJECT)
        n *= 2;
    for (; m>=0 && cnt<n; m--, cnt++) {
        int op = argCodes->codes[m].op;
        if (op == OP_ARRAY || op == OP_OBJECT)
            m = jsiPopArgs(argCodes, m);
    }
    return m+1;
}