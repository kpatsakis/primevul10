static int dist_cache_touch(struct rar5* rar, int idx) {
	int* q = rar->cstate.dist_cache;
	int i, dist = q[idx];

	for(i = idx; i > 0; i--)
		q[i] = q[i - 1];

	q[0] = dist;
	return dist;
}