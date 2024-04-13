bool Item_field::enumerate_field_refs_processor(uchar *arg)
{
  Field_enumerator *fe= (Field_enumerator*)arg;
  fe->visit_field(this);
  return FALSE;
}