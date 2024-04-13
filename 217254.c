lynx_get(struct pcilynx *lynx)
{
	kref_get(&lynx->kref);

	return lynx;
}