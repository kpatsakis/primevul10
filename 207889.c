void kScreenSaver::expose( int x, int y, int width, int height )
{
    XSetForeground(qt_xdisplay(), gc, BlackPixel(qt_xdisplay(), qt_xscreen()));
    XFillRectangle(qt_xdisplay(), d, gc, x, y, width, height);                  
}