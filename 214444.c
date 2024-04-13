static module *find_module(server_rec *s, const char *name)
{
    module *found = ap_find_linked_module(name);

    /* search prelinked stuff */
    if (!found) {
        ap_module_symbol_t *current = ap_prelinked_module_symbols;

        for (; current->name; ++current) {
            if (!strcmp(current->name, name)) {
                found = current->modp;
                break;
            }
        }
    }

    /* search dynamic stuff */
    if (!found) {
        APR_OPTIONAL_FN_TYPE(ap_find_loaded_module_symbol) *check_symbol =
            APR_RETRIEVE_OPTIONAL_FN(ap_find_loaded_module_symbol);

        if (check_symbol) {
            /*
             * There are two phases where calling ap_find_loaded_module_symbol
             * is problematic:
             *
             * During reading of the config, ap_server_conf is invalid but s
             * points to the main server config, if passed from cmd->server
             * of an EXEC_ON_READ directive.
             *
             * During config parsing, s may be a virtual host that would cause
             * a segfault in mod_so if passed to ap_find_loaded_module_symbol,
             * because mod_so's server config for vhosts is initialized later.
             * But ap_server_conf is already set at this time.
             *
             * Therefore we use s if it is not virtual and ap_server_conf if
             * s is virtual.
             */
            found = check_symbol(s->is_virtual ? ap_server_conf : s, name);
        }
    }

    return found;
}