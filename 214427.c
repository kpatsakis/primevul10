static const ap_directive_t * find_parent(const ap_directive_t *dirp,
                                          const char *what)
{
    while (dirp->parent != NULL) {
        dirp = dirp->parent;

        /* ### it would be nice to have atom-ized directives */
        if (ap_cstr_casecmp(dirp->directive, what) == 0)
            return dirp;
    }

    return NULL;
}