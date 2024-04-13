kScreenSaver::~kScreenSaver()
{
	XFreeGC( qt_xdisplay(), gc );
}