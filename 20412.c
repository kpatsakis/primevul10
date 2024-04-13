comp_opt_exact_or_map(OptExact* e, OptMap* m)
{
#define COMP_EM_BASE  20
  int ae, am;

  if (m->value <= 0) return -1;

  ae = COMP_EM_BASE * e->len * (e->ignore_case ? 1 : 2);
  am = COMP_EM_BASE * 5 * 2 / m->value;
  return comp_distance_value(&e->mmd, &m->mmd, ae, am);
}