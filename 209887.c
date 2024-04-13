static char *FindEndB(char *cp) {
    
    if (*cp == '\"'||*cp == '\'') {
        char endc = *cp;
        cp++;
        while (*cp && *cp != endc) {
            if (*cp == '\\' && cp[1]) cp++;
            cp++;
        }
        if (*cp == endc)
            cp++;
        if (*cp != ']')
            return NULL;
        return cp;
    } else
        return Jsi_Strchr(cp, ']');
}