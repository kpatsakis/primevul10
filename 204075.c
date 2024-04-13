zfree (voidpf opaque, voidpf address)
{
  (void) opaque;
  xfree (address);
}