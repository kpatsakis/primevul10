file_id_comparator (void const *entry1, void const *entry2)
{
  file_id const *e1 = entry1;
  file_id const *e2 = entry2;
  return (e1->ino == e2->ino && e1->dev == e2->dev);
}