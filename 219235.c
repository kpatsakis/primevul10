int ScaleY(rfbScreenInfoPtr from, rfbScreenInfoPtr to, int y)
{
    if ((from==to) || (from==NULL) || (to==NULL)) return y;
    return ((int)(((double) y / (double)from->height) * (double)to->height ));
}