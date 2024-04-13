lynx_put(struct pcilynx *lynx)
{
	kref_put(&lynx->kref, lynx_release);
}