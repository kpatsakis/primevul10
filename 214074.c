static void dist_cache_push(struct rar5* rar, int value) {
	int* q = rar->cstate.dist_cache;

	q[3] = q[2];
	q[2] = q[1];
	q[1] = q[0];
	q[0] = value;
}