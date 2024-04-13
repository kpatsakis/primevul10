find_light_source_in_children (RsvgNode *node)
{
    struct find_light_source_closure closure;
    RsvgNodeLightSource *source;

    closure.found_node = NULL;
    rsvg_node_foreach_child (node, is_light_source, &closure);
    if (closure.found_node == NULL)
        return NULL;

    g_assert (rsvg_node_get_type (closure.found_node) == RSVG_NODE_TYPE_LIGHT_SOURCE);

    source = rsvg_rust_cnode_get_impl (closure.found_node);
    closure.found_node = rsvg_node_unref (closure.found_node);

    return source;
}