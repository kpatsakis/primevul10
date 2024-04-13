sync_is_active(Slapi_Entry *e, Slapi_PBlock *pb)
{
    if (pb == NULL) {
        /* not yet initialized */
        return (0);
    } else {
        /* check id entry is in scope of sync request */
        return (sync_is_active_scope(slapi_entry_get_sdn_const(e), pb));
    }
}