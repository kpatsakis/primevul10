ossl_x509name_eql(VALUE self, VALUE other)
{
    if (!rb_obj_is_kind_of(other, cX509Name))
	return Qfalse;

    return ossl_x509name_cmp0(self, other) == 0 ? Qtrue : Qfalse;
}