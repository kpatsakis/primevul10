int ScaleX(rfbScreenInfoPtr from, rfbScreenInfoPtr to, int x)
{
    if ((from==to) || (from==NULL) || (to==NULL)) return x;
    return ((int)(((double) x / (double)from->width) * (double)to->width ));
}