ecma_op_container_set_noramlize_zero (ecma_value_t this_arg) /*< this arg */
{
  if (ecma_is_value_number (this_arg))
  {
    ecma_number_t number_value = ecma_get_number_from_value (this_arg);

    if (JERRY_UNLIKELY (ecma_number_is_zero (number_value) && ecma_number_is_negative (number_value)))
    {
      return ecma_make_integer_value (0);
    }
  }

  return this_arg;
} /* ecma_op_container_set_noramlize_zero */