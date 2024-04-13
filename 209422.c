static void code_reserved_replace(Jsi_OpCodes *ops, int step_len, int break_only,
                           const char *desire_label, int topop)
{
    int i;
    for (i = 0; i < ops->code_len; ++i) {
        if (ops->codes[i].op != OP_RESERVED) continue;
        jsi_ReservedInfo *ri = (jsi_ReservedInfo *)ops->codes[i].data;

        if (ri->label) {
            if (!desire_label || Jsi_Strcmp(ri->label, desire_label) != 0) {
                ri->topop += topop;
                continue;
            }
        }
        
        if (ri->type == RES_CONTINUE) {
            if (break_only) {
                ri->topop += topop;
                continue;
            } else {
                int topop = ri->topop;
                Jsi_Free(ri);       /* kill reserved Warn, replace with other opcode */
 /*               if (ops->codes[i].data && ops->codes[i].alloc) //TODO: memory leak?
                    Jsi_Free(ops->codes[i].data);*/
                if (topop) {
                    ops->codes[i].data = jpinfo_new(ops->code_len - i, topop);
                    ops->codes[i].op = OP_JMPPOP;
                    ops->codes[i].alloc = 1;
                } else {
                    ops->codes[i].data = (void *)(uintptr_t)(ops->code_len - i);
                    ops->codes[i].op = OP_JMP;
                    ops->codes[i].alloc = 0;
                }
            }
        } else if (ri->type == RES_BREAK) {
            int topop = ri->topop;
            Jsi_Free(ri);
/*           if (ops->codes[i].data && ops->codes[i].alloc)
                Jsi_Free(ops->codes[i].data); */
            if (topop) {
                ops->codes[i].data = jpinfo_new(step_len + ops->code_len - i, topop);
                ops->codes[i].op = OP_JMPPOP;
                ops->codes[i].alloc = 1;
            } else {
                ops->codes[i].data = (void *)(uintptr_t)(step_len + ops->code_len - i);
                ops->codes[i].op = OP_JMP;
                ops->codes[i].alloc = 0;
            }
        }
    }
}