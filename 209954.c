void jsi_mark_local(Jsi_OpCodes *ops) // Mark variables as declared with "var"
{
    return;
    int i = 0;
    if (ops == NULL || ops->codes == NULL)
        return;
    while (i < ops->code_len) {
        if (ops->codes[i].op == OP_PUSHVAR)
            ops->codes[i].local = 1;
        i++;
    }
}