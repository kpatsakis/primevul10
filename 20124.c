static FlatView *flatview_new(MemoryRegion *mr_root)
{
    FlatView *view;

    view = g_new0(FlatView, 1);
    view->ref = 1;
    view->root = mr_root;

    return view;
}