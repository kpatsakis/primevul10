is_light_source (RsvgNode *node, gpointer data)
{
    struct find_light_source_closure *closure = data;

    if (rsvg_node_get_type (node) == RSVG_NODE_TYPE_LIGHT_SOURCE) {
        closure->found_node = rsvg_node_ref (node);
    }

    return TRUE;
}