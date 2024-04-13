Field *Item::tmp_table_field_from_field_type(TABLE *table, bool fixed_length)
{
  /*
    The field functions defines a field to be not null if null_ptr is not 0
  */
  uchar *null_ptr= maybe_null ? (uchar*) "" : 0;
  Field *field;

  switch (field_type()) {
  case MYSQL_TYPE_DECIMAL:
  case MYSQL_TYPE_NEWDECIMAL:
    field= Field_new_decimal::create_from_item(this);
    break;
  case MYSQL_TYPE_TINY:
    field= new Field_tiny((uchar*) 0, max_length, null_ptr, 0, Field::NONE,
			  name, 0, unsigned_flag);
    break;
  case MYSQL_TYPE_SHORT:
    field= new Field_short((uchar*) 0, max_length, null_ptr, 0, Field::NONE,
			   name, 0, unsigned_flag);
    break;
  case MYSQL_TYPE_LONG:
    field= new Field_long((uchar*) 0, max_length, null_ptr, 0, Field::NONE,
			  name, 0, unsigned_flag);
    break;
#ifdef HAVE_LONG_LONG
  case MYSQL_TYPE_LONGLONG:
    field= new Field_longlong((uchar*) 0, max_length, null_ptr, 0, Field::NONE,
			      name, 0, unsigned_flag);
    break;
#endif
  case MYSQL_TYPE_FLOAT:
    field= new Field_float((uchar*) 0, max_length, null_ptr, 0, Field::NONE,
			   name, decimals, 0, unsigned_flag);
    break;
  case MYSQL_TYPE_DOUBLE:
    field= new Field_double((uchar*) 0, max_length, null_ptr, 0, Field::NONE,
			    name, decimals, 0, unsigned_flag);
    break;
  case MYSQL_TYPE_INT24:
    field= new Field_medium((uchar*) 0, max_length, null_ptr, 0, Field::NONE,
			    name, 0, unsigned_flag);
    break;
  case MYSQL_TYPE_NEWDATE:
  case MYSQL_TYPE_DATE:
    field= new Field_newdate(0, null_ptr, 0, Field::NONE, name, &my_charset_bin);
    break;
  case MYSQL_TYPE_TIME:
    field= new_Field_time(0, null_ptr, 0, Field::NONE, name,
                              decimals, &my_charset_bin);
    break;
  case MYSQL_TYPE_TIMESTAMP:
    field= new_Field_timestamp(0, null_ptr, 0,
                               Field::NONE, name, 0, decimals, &my_charset_bin);
    break;
  case MYSQL_TYPE_DATETIME:
    field= new_Field_datetime(0, null_ptr, 0, Field::NONE, name,
                              decimals, &my_charset_bin);
    break;
  case MYSQL_TYPE_YEAR:
    field= new Field_year((uchar*) 0, max_length, null_ptr, 0, Field::NONE,
			  name);
    break;
  case MYSQL_TYPE_BIT:
    field= new Field_bit_as_char(NULL, max_length, null_ptr, 0,
                                 Field::NONE, name);
    break;
  default:
    /* This case should never be chosen */
    DBUG_ASSERT(0);
    /* If something goes awfully wrong, it's better to get a string than die */
  case MYSQL_TYPE_STRING:
  case MYSQL_TYPE_NULL:
    if (fixed_length && !too_big_for_varchar())
    {
      field= new Field_string(max_length, maybe_null, name,
                              collation.collation);
      break;
    }
    /* fall through */
  case MYSQL_TYPE_ENUM:
  case MYSQL_TYPE_SET:
  case MYSQL_TYPE_VAR_STRING:
  case MYSQL_TYPE_VARCHAR:
    return make_string_field(table);
  case MYSQL_TYPE_TINY_BLOB:
  case MYSQL_TYPE_MEDIUM_BLOB:
  case MYSQL_TYPE_LONG_BLOB:
  case MYSQL_TYPE_BLOB:
    if (this->type() == Item::TYPE_HOLDER)
      field= new Field_blob(max_length, maybe_null, name, collation.collation,
                            1);
    else
      field= new Field_blob(max_length, maybe_null, name, collation.collation);
    break;					// Blob handled outside of case
#ifdef HAVE_SPATIAL
  case MYSQL_TYPE_GEOMETRY:
    field= new Field_geom(max_length, maybe_null,
                          name, table->s, get_geometry_type());
#endif /* HAVE_SPATIAL */
  }
  if (field)
    field->init(table);
  return field;
}