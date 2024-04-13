bool jsi_ModBlacklisted(Jsi_Interp *interp, const char *mod) {
    if (!interp->subOpts.blacklist) return false;
    const char *blstr =Jsi_Strstr(interp->subOpts.blacklist, mod);
    if (!blstr) return false;
    if ((blstr==interp->subOpts.blacklist || !isalnum(blstr[-1])) && !isalnum(blstr[Jsi_Strlen(mod)]))
        return false;
    return true;
}