isBox (node_t* n)
{
    polygon_t *p;

    if ((p = ND_shape(n)->polygon)) {
	return (p->sides == 4 && (ROUND(p->orientation) % 90) == 0 && p->distortion == 0. && p->skew == 0.);
    }
    else
	return 0;
}