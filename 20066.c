static void flatview_destroy(FlatView *view)
{
    if (view->dispatch) {
        address_space_dispatch_free(view->dispatch);
    }
    g_free(view->ranges);
    g_free(view);
}