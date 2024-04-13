Jsi_RC jsi_RegExpMatches(Jsi_Interp *interp, Jsi_Value *pattern, const char *str, int n, Jsi_Value *ret, int *ofs, bool match)
{
    Jsi_Regex *re;
    int regexec_flags = 0;
    Jsi_Value *seq = pattern;

    if (seq == NULL || seq->vt != JSI_VT_OBJECT || seq->d.obj->ot != JSI_OT_REGEXP) {
        Jsi_ValueMakeNull(interp, &ret);
        return JSI_OK;
    }
    re = seq->d.obj->d.robj;
    regex_t *reg = &re->reg;
    
    regmatch_t pos[MAX_SUBREGEX] = {};
    int num_matches = 0, r;
    int isglob = (re->eflags&JSI_REG_GLOB);
    Jsi_Obj *obj;
    
    do {
        r = regexec(reg, str, MAX_SUBREGEX, pos, regexec_flags);

        if (r >= REG_BADPAT) {
            char buf[JSI_BUFSIZ];

            regerror(r, reg, buf, sizeof(buf));
            return Jsi_LogError("error while matching pattern: %s", buf);
        }
        if (r == REG_NOMATCH) {
            if (num_matches == 0) {
                Jsi_ValueMakeNull(interp, &ret);
                return JSI_OK;
            }
            break;
        }

        if (num_matches == 0) {
            obj = Jsi_ObjNewType(interp, JSI_OT_ARRAY);
            obj->__proto__ = interp->Array_prototype;
            Jsi_ValueMakeObject(interp, &ret, obj);
            Jsi_ObjSetLength(interp, ret->d.obj, 0);
        }
    
        int i;
        for (i = 0; i < MAX_SUBREGEX; ++i) {
            if (pos[i].rm_so <= 0 && pos[i].rm_eo <= 0)
                break;
            if (i && pos[i].rm_so == pos[i-1].rm_so && pos[i].rm_eo == pos[i-1].rm_eo)
                continue;
    
            int olen = -1;
            char *ostr = jsi_SubstrDup(str, -1, pos[i].rm_so, pos[i].rm_eo - pos[i].rm_so, &olen);
            Jsi_Value *val = Jsi_ValueMakeBlob(interp, NULL, (uchar*)ostr, olen);
            if (ofs)
                *ofs = pos[i].rm_eo;
            Jsi_ValueInsertArray(interp, ret, num_matches, val, 0);
            num_matches++;
            if ( match && isglob)
                break;
        }
        if (num_matches && match && !isglob)
            return JSI_OK;
        if (num_matches == 1 && (ofs || !isglob))
            break;
        
        str += pos[0].rm_eo;
        n -= pos[0].rm_eo;

        regexec_flags |= REG_NOTBOL;
    } while (n && pos[0].rm_eo>0);
    
    return JSI_OK;
}