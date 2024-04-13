static Header headerUnlink(Header h)
{
    if (h != NULL)
	h->nrefs--;
    return NULL;
}