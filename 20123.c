void flatview_unref(FlatView *view)
{
    view->ref--;
    if (view->ref <= 0) {
        flatview_destroy(view);
    }
}