isEllipse(node_t* n)
{
    polygon_t *p;

    if ((p = ND_shape(n)->polygon)) {
	return (p->sides <= 2);
    }
    else
	return 0;
}