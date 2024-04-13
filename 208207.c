test_choose_instruction (xd3_stream *stream, int ignore)
{
  int i;

  stream->code_table = (*stream->code_table_func) ();

  for (i = 0; i < 256; i += 1)
    {
      const xd3_dinst *d = stream->code_table + i;
      xd3_rinst prev, inst;

      CHECK(d->type1 > 0);

      memset (& prev, 0, sizeof (prev));
      memset (& inst, 0, sizeof (inst));

      if (d->type2 == 0)
	{
	  inst.type = d->type1;

	  if ((inst.size = d->size1) == 0)
	    {
	      inst.size = TESTBUFSIZE;
	    }

	  XD3_CHOOSE_INSTRUCTION (stream, NULL, & inst);

	  if (inst.code2 != 0 || inst.code1 != i)
	    {
	      stream->msg = "wrong single instruction";
	      return XD3_INTERNAL;
	    }
	}
      else
	{
	  prev.type = d->type1;
	  prev.size = d->size1;
	  inst.type = d->type2;
	  inst.size = d->size2;

	  XD3_CHOOSE_INSTRUCTION (stream, & prev, & inst);

	  if (prev.code2 != i)
	    {
	      stream->msg = "wrong double instruction";
	      return XD3_INTERNAL;
	    }
	}
    }

  return 0;
}