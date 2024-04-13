static int reorder_sorter(const void *va, const void *vb)
{
    const struct reorder_sort_rec *a = va;
    const struct reorder_sort_rec *b = vb;
    core_dir_config *core_a;
    core_dir_config *core_b;

    core_a = ap_get_core_module_config(a->elt);
    core_b = ap_get_core_module_config(b->elt);

    /* a regex always sorts after a non-regex
     */
    if (!core_a->r && core_b->r) {
        return -1;
    }
    else if (core_a->r && !core_b->r) {
        return 1;
    }

    /* we always sort next by the number of components
     */
    if (core_a->d_components < core_b->d_components) {
        return -1;
    }
    else if (core_a->d_components > core_b->d_components) {
        return 1;
    }

    /* They have the same number of components, we now have to compare
     * the minor key to maintain the original order (from the config.)
     */
    return a->orig_index - b->orig_index;
}