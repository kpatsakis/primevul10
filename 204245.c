getShortAt (const void *data)
{
  uint16_t p;

  memcpy (&p, data, sizeof (uint16_t));          /* ensure alignment! */
  return ntohs (p);
}