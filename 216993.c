scanner_scan_primary_expression_end (parser_context_t *context_p, /**< context */
                                     scanner_context_t *scanner_context_p, /**< scanner context */
                                     lexer_token_type_t type, /**< current token type */
                                     scan_stack_modes_t stack_top) /**< current stack top */
{
  if (type == LEXER_COMMA)
  {
    switch (stack_top)
    {
      case SCAN_STACK_VAR:
#if ENABLED (JERRY_ES2015)
      case SCAN_STACK_LET:
      case SCAN_STACK_CONST:
#endif /* ENABLED (JERRY_ES2015) */
      case SCAN_STACK_FOR_VAR_START:
#if ENABLED (JERRY_ES2015)
      case SCAN_STACK_FOR_LET_START:
      case SCAN_STACK_FOR_CONST_START:
#endif /* ENABLED (JERRY_ES2015) */
      {
        scanner_context_p->mode = SCAN_MODE_VAR_STATEMENT;
        return SCAN_NEXT_TOKEN;
      }
      case SCAN_STACK_COLON_EXPRESSION:
      {
        scanner_raise_error (context_p);
        break;
      }
#if ENABLED (JERRY_ES2015)
      case SCAN_STACK_BINDING_INIT:
      case SCAN_STACK_BINDING_LIST_INIT:
      {
        break;
      }
      case SCAN_STACK_ARROW_ARGUMENTS:
      {
        lexer_next_token (context_p);
        scanner_check_arrow_arg (context_p, scanner_context_p);
        return SCAN_KEEP_TOKEN;
      }
      case SCAN_STACK_ARROW_EXPRESSION:
      {
        break;
      }
      case SCAN_STACK_FUNCTION_PARAMETERS:
      {
        scanner_context_p->mode = SCAN_MODE_CONTINUE_FUNCTION_ARGUMENTS;
        parser_stack_pop_uint8 (context_p);
        return SCAN_NEXT_TOKEN;
      }
      case SCAN_STACK_ARRAY_LITERAL:
      {
        scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;

        if (scanner_context_p->binding_type != SCANNER_BINDING_NONE)
        {
          scanner_context_p->mode = SCAN_MODE_BINDING;
        }

        return SCAN_NEXT_TOKEN;
      }
#endif /* ENABLED (JERRY_ES2015) */
      case SCAN_STACK_OBJECT_LITERAL:
      {
        scanner_context_p->mode = SCAN_MODE_PROPERTY_NAME;
        return SCAN_KEEP_TOKEN;
      }
      default:
      {
        scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;
        return SCAN_NEXT_TOKEN;
      }
    }
  }

  switch (stack_top)
  {
    case SCAN_STACK_WITH_EXPRESSION:
    {
      if (type != LEXER_RIGHT_PAREN)
      {
        break;
      }

      parser_stack_pop_uint8 (context_p);

      uint16_t status_flags = scanner_context_p->active_literal_pool_p->status_flags;
      parser_stack_push_uint8 (context_p, (status_flags & SCANNER_LITERAL_POOL_IN_WITH) ? 1 : 0);
      parser_stack_push_uint8 (context_p, SCAN_STACK_WITH_STATEMENT);
      status_flags |= SCANNER_LITERAL_POOL_IN_WITH;
      scanner_context_p->active_literal_pool_p->status_flags = status_flags;

      scanner_context_p->mode = SCAN_MODE_STATEMENT;
      return SCAN_NEXT_TOKEN;
    }
    case SCAN_STACK_DO_EXPRESSION:
    {
      if (type != LEXER_RIGHT_PAREN)
      {
        break;
      }

      scanner_context_p->mode = SCAN_MODE_STATEMENT_END;
      return SCAN_NEXT_TOKEN;
    }
    case SCAN_STACK_WHILE_EXPRESSION:
    {
      if (type != LEXER_RIGHT_PAREN)
      {
        break;
      }

      scanner_source_start_t source_start;

      parser_stack_pop_uint8 (context_p);
      parser_stack_pop (context_p, &source_start, sizeof (scanner_source_start_t));

      scanner_location_info_t *location_info_p;
      location_info_p = (scanner_location_info_t *) scanner_insert_info (context_p,
                                                                         source_start.source_p,
                                                                         sizeof (scanner_location_info_t));
      location_info_p->info.type = SCANNER_TYPE_WHILE;

      scanner_get_location (&location_info_p->location, context_p);

      scanner_context_p->mode = SCAN_MODE_STATEMENT;
      return SCAN_NEXT_TOKEN;
    }
    case SCAN_STACK_PAREN_EXPRESSION:
    {
      if (type != LEXER_RIGHT_PAREN)
      {
        break;
      }

      parser_stack_pop_uint8 (context_p);

#if ENABLED (JERRY_ES2015)
      if (context_p->stack_top_uint8 == SCAN_STACK_USE_ASYNC)
      {
        scanner_add_async_literal (context_p, scanner_context_p);
      }
#endif /* ENABLED (JERRY_ES2015) */

      scanner_context_p->mode = SCAN_MODE_POST_PRIMARY_EXPRESSION;
      return SCAN_NEXT_TOKEN;
    }
    case SCAN_STACK_STATEMENT_WITH_EXPR:
    {
      if (type != LEXER_RIGHT_PAREN)
      {
        break;
      }

      parser_stack_pop_uint8 (context_p);

#if ENABLED (JERRY_ES2015)
      if (context_p->stack_top_uint8 == SCAN_STACK_IF_STATEMENT)
      {
        scanner_check_function_after_if (context_p, scanner_context_p);
        return SCAN_KEEP_TOKEN;
      }
#endif /* ENABLED (JERRY_ES2015) */

      scanner_context_p->mode = SCAN_MODE_STATEMENT;
      return SCAN_NEXT_TOKEN;
    }
#if ENABLED (JERRY_ES2015)
    case SCAN_STACK_BINDING_LIST_INIT:
    {
      parser_stack_pop_uint8 (context_p);

      JERRY_ASSERT (context_p->stack_top_uint8 == SCAN_STACK_ARRAY_LITERAL
                    || context_p->stack_top_uint8 == SCAN_STACK_OBJECT_LITERAL
                    || context_p->stack_top_uint8 == SCAN_STACK_LET
                    || context_p->stack_top_uint8 == SCAN_STACK_CONST
                    || context_p->stack_top_uint8 == SCAN_STACK_FOR_LET_START
                    || context_p->stack_top_uint8 == SCAN_STACK_FOR_CONST_START
                    || context_p->stack_top_uint8 == SCAN_STACK_FUNCTION_PARAMETERS
                    || context_p->stack_top_uint8 == SCAN_STACK_ARROW_ARGUMENTS);

      scanner_binding_item_t *item_p = scanner_context_p->active_binding_list_p->items_p;

      while (item_p != NULL)
      {
        if (item_p->literal_p->type & SCANNER_LITERAL_IS_USED)
        {
          item_p->literal_p->type |= SCANNER_LITERAL_EARLY_CREATE;
        }
        item_p = item_p->next_p;
      }

      scanner_pop_binding_list (scanner_context_p);
      scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION_END;
      return SCAN_KEEP_TOKEN;
    }
    case SCAN_STACK_BINDING_INIT:
    {
      scanner_binding_literal_t binding_literal;

      parser_stack_pop_uint8 (context_p);
      parser_stack_pop (context_p, &binding_literal, sizeof (scanner_binding_literal_t));

      JERRY_ASSERT (context_p->stack_top_uint8 == SCAN_STACK_ARRAY_LITERAL
                    || context_p->stack_top_uint8 == SCAN_STACK_OBJECT_LITERAL
                    || context_p->stack_top_uint8 == SCAN_STACK_LET
                    || context_p->stack_top_uint8 == SCAN_STACK_CONST
                    || context_p->stack_top_uint8 == SCAN_STACK_FOR_LET_START
                    || context_p->stack_top_uint8 == SCAN_STACK_FOR_CONST_START
                    || context_p->stack_top_uint8 == SCAN_STACK_FUNCTION_PARAMETERS
                    || context_p->stack_top_uint8 == SCAN_STACK_ARROW_ARGUMENTS);

      JERRY_ASSERT ((stack_top != SCAN_STACK_ARRAY_LITERAL && stack_top != SCAN_STACK_OBJECT_LITERAL)
                    || SCANNER_NEEDS_BINDING_LIST (scanner_context_p->binding_type));

      if (binding_literal.literal_p->type & SCANNER_LITERAL_IS_USED)
      {
        binding_literal.literal_p->type |= SCANNER_LITERAL_EARLY_CREATE;
      }

      scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION_END;
      return SCAN_KEEP_TOKEN;
    }
#endif /* ENABLED (JERRY_ES2015) */
    case SCAN_STACK_VAR:
#if ENABLED (JERRY_ES2015)
    case SCAN_STACK_LET:
    case SCAN_STACK_CONST:
#endif /* ENABLED (JERRY_ES2015) */
    {
#if ENABLED (JERRY_ES2015_MODULE_SYSTEM)
      scanner_context_p->active_literal_pool_p->status_flags &= (uint16_t) ~SCANNER_LITERAL_POOL_IN_EXPORT;
#endif /* ENABLED (JERRY_ES2015_MODULE_SYSTEM) */

      parser_stack_pop_uint8 (context_p);
      return SCAN_KEEP_TOKEN;
    }
    case SCAN_STACK_FOR_VAR_START:
#if ENABLED (JERRY_ES2015)
    case SCAN_STACK_FOR_LET_START:
    case SCAN_STACK_FOR_CONST_START:
#endif /* ENABLED (JERRY_ES2015) */
    case SCAN_STACK_FOR_START:
    {
      if (type == LEXER_KEYW_IN || SCANNER_IDENTIFIER_IS_OF ())
      {
        scanner_for_statement_t for_statement;

        parser_stack_pop_uint8 (context_p);
        parser_stack_pop (context_p, &for_statement, sizeof (scanner_for_statement_t));

        scanner_location_info_t *location_info;
        location_info = (scanner_location_info_t *) scanner_insert_info (context_p,
                                                                         for_statement.u.source_p,
                                                                         sizeof (scanner_location_info_t));
#if ENABLED (JERRY_ES2015)
        location_info->info.type = (type == LEXER_KEYW_IN) ? SCANNER_TYPE_FOR_IN : SCANNER_TYPE_FOR_OF;

        if (stack_top == SCAN_STACK_FOR_LET_START || stack_top == SCAN_STACK_FOR_CONST_START)
        {
          parser_stack_push_uint8 (context_p, SCAN_STACK_PRIVATE_BLOCK_EARLY);
        }
#else /* !ENABLED (JERRY_ES2015) */
        location_info->info.type = SCANNER_TYPE_FOR_IN;
#endif /* ENABLED (JERRY_ES2015) */

        scanner_get_location (&location_info->location, context_p);

        parser_stack_push_uint8 (context_p, SCAN_STACK_STATEMENT_WITH_EXPR);
        scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;
        return SCAN_NEXT_TOKEN;
      }

      if (type != LEXER_SEMICOLON)
      {
        break;
      }

      scanner_for_statement_t for_statement;

      parser_stack_pop_uint8 (context_p);
      parser_stack_pop (context_p, NULL, sizeof (scanner_for_statement_t));

#if ENABLED (JERRY_ES2015)
      if (stack_top == SCAN_STACK_FOR_LET_START || stack_top == SCAN_STACK_FOR_CONST_START)
      {
        parser_stack_push_uint8 (context_p, SCAN_STACK_PRIVATE_BLOCK);
      }
#endif /* ENABLED (JERRY_ES2015) */

      for_statement.u.source_p = context_p->source_p;
      parser_stack_push (context_p, &for_statement, sizeof (scanner_for_statement_t));
      parser_stack_push_uint8 (context_p, SCAN_STACK_FOR_CONDITION);

      lexer_next_token (context_p);

      if (context_p->token.type != LEXER_SEMICOLON)
      {
        scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;
        return SCAN_KEEP_TOKEN;
      }

      type = LEXER_SEMICOLON;
      /* FALLTHRU */
    }
    case SCAN_STACK_FOR_CONDITION:
    {
      if (type != LEXER_SEMICOLON)
      {
        break;
      }

      scanner_for_statement_t for_statement;

      parser_stack_pop_uint8 (context_p);
      parser_stack_pop (context_p, &for_statement, sizeof (scanner_for_statement_t));

      scanner_for_info_t *for_info_p;
      for_info_p = (scanner_for_info_t *) scanner_insert_info (context_p,
                                                               for_statement.u.source_p,
                                                               sizeof (scanner_for_info_t));
      for_info_p->info.type = SCANNER_TYPE_FOR;

      scanner_get_location (&for_info_p->expression_location, context_p);
      for_info_p->end_location.source_p = NULL;

      for_statement.u.for_info_p = for_info_p;

      parser_stack_push (context_p, &for_statement, sizeof (scanner_for_statement_t));
      parser_stack_push_uint8 (context_p, SCAN_STACK_FOR_EXPRESSION);

      lexer_next_token (context_p);

      if (context_p->token.type != LEXER_RIGHT_PAREN)
      {
        scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;
        return SCAN_KEEP_TOKEN;
      }

      type = LEXER_RIGHT_PAREN;
      /* FALLTHRU */
    }
    case SCAN_STACK_FOR_EXPRESSION:
    {
      if (type != LEXER_RIGHT_PAREN)
      {
        break;
      }

      scanner_for_statement_t for_statement;

      parser_stack_pop_uint8 (context_p);
      parser_stack_pop (context_p, &for_statement, sizeof (scanner_for_statement_t));

      scanner_get_location (&for_statement.u.for_info_p->end_location, context_p);

      scanner_context_p->mode = SCAN_MODE_STATEMENT;
      return SCAN_NEXT_TOKEN;
    }
    case SCAN_STACK_SWITCH_EXPRESSION:
    {
      if (type != LEXER_RIGHT_PAREN)
      {
        break;
      }

      lexer_next_token (context_p);

      if (context_p->token.type != LEXER_LEFT_BRACE)
      {
        break;
      }

#if ENABLED (JERRY_ES2015)
      scanner_literal_pool_t *literal_pool_p;
      literal_pool_p = scanner_push_literal_pool (context_p, scanner_context_p, SCANNER_LITERAL_POOL_BLOCK);
      literal_pool_p->source_p = context_p->source_p - 1;
#endif /* ENABLED (JERRY_ES2015) */

      parser_stack_pop_uint8 (context_p);

      scanner_switch_statement_t switch_statement = scanner_context_p->active_switch_statement;
      parser_stack_push (context_p, &switch_statement, sizeof (scanner_switch_statement_t));
      parser_stack_push_uint8 (context_p, SCAN_STACK_SWITCH_BLOCK);

      scanner_switch_info_t *switch_info_p;
      switch_info_p = (scanner_switch_info_t *) scanner_insert_info (context_p,
                                                                     context_p->source_p,
                                                                     sizeof (scanner_switch_info_t));
      switch_info_p->info.type = SCANNER_TYPE_SWITCH;
      switch_info_p->case_p = NULL;
      scanner_context_p->active_switch_statement.last_case_p = &switch_info_p->case_p;

      lexer_next_token (context_p);

      if (context_p->token.type != LEXER_RIGHT_BRACE
          && context_p->token.type != LEXER_KEYW_CASE
          && context_p->token.type != LEXER_KEYW_DEFAULT)
      {
        break;
      }

      scanner_context_p->mode = SCAN_MODE_STATEMENT_OR_TERMINATOR;
      return SCAN_KEEP_TOKEN;
    }
    case SCAN_STACK_CASE_STATEMENT:
    {
      if (type != LEXER_COLON)
      {
        break;
      }

      scanner_source_start_t source_start;

      parser_stack_pop_uint8 (context_p);
      parser_stack_pop (context_p, &source_start, sizeof (scanner_source_start_t));

      scanner_location_info_t *location_info_p;
      location_info_p = (scanner_location_info_t *) scanner_insert_info (context_p,
                                                                         source_start.source_p,
                                                                         sizeof (scanner_location_info_t));
      location_info_p->info.type = SCANNER_TYPE_CASE;

      scanner_get_location (&location_info_p->location, context_p);

      scanner_context_p->mode = SCAN_MODE_STATEMENT_OR_TERMINATOR;
      return SCAN_NEXT_TOKEN;
    }
    case SCAN_STACK_COLON_EXPRESSION:
    {
      if (type != LEXER_COLON)
      {
        break;
      }

      scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;
      parser_stack_pop_uint8 (context_p);
      return SCAN_NEXT_TOKEN;
    }
#if ENABLED (JERRY_ES2015)
    case SCAN_STACK_ARRAY_LITERAL:
    case SCAN_STACK_OBJECT_LITERAL:
    {
      if (((stack_top == SCAN_STACK_ARRAY_LITERAL) && (type != LEXER_RIGHT_SQUARE))
          || ((stack_top == SCAN_STACK_OBJECT_LITERAL) && (type != LEXER_RIGHT_BRACE)))
      {
        break;
      }

      scanner_source_start_t source_start;
      uint8_t binding_type = scanner_context_p->binding_type;

      parser_stack_pop_uint8 (context_p);
      scanner_context_p->binding_type = context_p->stack_top_uint8;
      parser_stack_pop_uint8 (context_p);
      parser_stack_pop (context_p, &source_start, sizeof (scanner_source_start_t));

      lexer_next_token (context_p);

      if (binding_type == SCANNER_BINDING_CATCH && context_p->stack_top_uint8 == SCAN_STACK_CATCH_STATEMENT)
      {
        scanner_pop_binding_list (scanner_context_p);

        if (context_p->token.type != LEXER_RIGHT_PAREN)
        {
          scanner_raise_error (context_p);
        }

        lexer_next_token (context_p);

        if (context_p->token.type != LEXER_LEFT_BRACE)
        {
          scanner_raise_error (context_p);
        }

        scanner_context_p->mode = SCAN_MODE_STATEMENT_OR_TERMINATOR;
        return SCAN_NEXT_TOKEN;
      }

      if (context_p->token.type != LEXER_ASSIGN)
      {
        if (SCANNER_NEEDS_BINDING_LIST (binding_type))
        {
          scanner_pop_binding_list (scanner_context_p);
        }

        scanner_context_p->mode = SCAN_MODE_POST_PRIMARY_EXPRESSION;
        return SCAN_KEEP_TOKEN;
      }

      scanner_location_info_t *location_info_p;
      location_info_p = (scanner_location_info_t *) scanner_insert_info (context_p,
                                                                         source_start.source_p,
                                                                         sizeof (scanner_location_info_t));
      location_info_p->info.type = SCANNER_TYPE_INITIALIZER;
      scanner_get_location (&location_info_p->location, context_p);
      scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;

      if (SCANNER_NEEDS_BINDING_LIST (binding_type))
      {
        scanner_binding_item_t *item_p = scanner_context_p->active_binding_list_p->items_p;

        while (item_p != NULL)
        {
          item_p->literal_p->type &= (uint8_t) ~SCANNER_LITERAL_IS_USED;
          item_p = item_p->next_p;
        }

        parser_stack_push_uint8 (context_p, SCAN_STACK_BINDING_LIST_INIT);
      }
      return SCAN_NEXT_TOKEN;
    }
#else /* !ENABLED (JERRY_ES2015) */
    case SCAN_STACK_ARRAY_LITERAL:
#endif /* ENABLED (JERRY_ES2015) */
    case SCAN_STACK_PROPERTY_ACCESSOR:
    {
      if (type != LEXER_RIGHT_SQUARE)
      {
        break;
      }

      scanner_context_p->mode = SCAN_MODE_POST_PRIMARY_EXPRESSION;
      parser_stack_pop_uint8 (context_p);
      return SCAN_NEXT_TOKEN;
    }
#if !ENABLED (JERRY_ES2015)
    case SCAN_STACK_OBJECT_LITERAL:
    {
      if (type != LEXER_RIGHT_BRACE)
      {
        break;
      }

      scanner_context_p->mode = SCAN_MODE_POST_PRIMARY_EXPRESSION;
      parser_stack_pop_uint8 (context_p);
      return SCAN_NEXT_TOKEN;
    }
#endif /* !ENABLED (JERRY_ES2015) */
#if ENABLED (JERRY_ES2015)
    case SCAN_STACK_COMPUTED_PROPERTY:
    {
      if (type != LEXER_RIGHT_SQUARE)
      {
        break;
      }

      lexer_next_token (context_p);

      parser_stack_pop_uint8 (context_p);
      stack_top = (scan_stack_modes_t) context_p->stack_top_uint8;

      if (stack_top == SCAN_STACK_FUNCTION_PROPERTY)
      {
        scanner_push_literal_pool (context_p, scanner_context_p, SCANNER_LITERAL_POOL_FUNCTION);

        scanner_context_p->mode = SCAN_MODE_FUNCTION_ARGUMENTS;
        return SCAN_KEEP_TOKEN;
      }

      JERRY_ASSERT (stack_top == SCAN_STACK_OBJECT_LITERAL);

      if (context_p->token.type == LEXER_LEFT_PAREN)
      {
        scanner_push_literal_pool (context_p, scanner_context_p, SCANNER_LITERAL_POOL_FUNCTION);

        parser_stack_push_uint8 (context_p, SCAN_STACK_FUNCTION_PROPERTY);
        scanner_context_p->mode = SCAN_MODE_FUNCTION_ARGUMENTS;
        return SCAN_KEEP_TOKEN;
      }

      if (context_p->token.type != LEXER_COLON)
      {
        scanner_raise_error (context_p);
      }

      scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;

      if (scanner_context_p->binding_type != SCANNER_BINDING_NONE)
      {
        scanner_context_p->mode = SCAN_MODE_BINDING;
      }
      return SCAN_NEXT_TOKEN;
    }
    case SCAN_STACK_COMPUTED_GENERATOR:
    case SCAN_STACK_COMPUTED_ASYNC:
    case SCAN_STACK_COMPUTED_ASYNC_GENERATOR:
    {
      if (type != LEXER_RIGHT_SQUARE)
      {
        break;
      }

      lexer_next_token (context_p);
      parser_stack_pop_uint8 (context_p);

      JERRY_ASSERT (context_p->stack_top_uint8 == SCAN_STACK_OBJECT_LITERAL
                    || context_p->stack_top_uint8 == SCAN_STACK_FUNCTION_PROPERTY);

      uint16_t status_flags = (uint16_t) (SCANNER_LITERAL_POOL_FUNCTION
                                          | SCANNER_LITERAL_POOL_GENERATOR
                                          | SCANNER_FROM_COMPUTED_TO_LITERAL_POOL (stack_top));

      scanner_push_literal_pool (context_p, scanner_context_p, status_flags);

      scanner_context_p->mode = SCAN_MODE_FUNCTION_ARGUMENTS;
      return SCAN_KEEP_TOKEN;
    }
    case SCAN_STACK_TEMPLATE_STRING:
    case SCAN_STACK_TAGGED_TEMPLATE_LITERAL:
    {
      if (type != LEXER_RIGHT_BRACE)
      {
        break;
      }

      context_p->source_p--;
      context_p->column--;
      lexer_parse_string (context_p, LEXER_STRING_NO_OPTS);

      if (context_p->source_p[-1] != LIT_CHAR_GRAVE_ACCENT)
      {
        scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;
      }
      else
      {
        parser_stack_pop_uint8 (context_p);
        scanner_context_p->mode = SCAN_MODE_POST_PRIMARY_EXPRESSION;
      }
      return SCAN_NEXT_TOKEN;
    }
    case SCAN_STACK_ARROW_ARGUMENTS:
    {
      if (type != LEXER_RIGHT_PAREN)
      {
        break;
      }

      scanner_check_arrow (context_p, scanner_context_p);
      return SCAN_KEEP_TOKEN;
    }
    case SCAN_STACK_ARROW_EXPRESSION:
    {
      scanner_pop_literal_pool (context_p, scanner_context_p);
      parser_stack_pop_uint8 (context_p);
      lexer_update_await_yield (context_p, context_p->status_flags);
      scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION_END;
      return SCAN_KEEP_TOKEN;
    }
    case SCAN_STACK_CLASS_EXTENDS:
    {
      if (type != LEXER_LEFT_BRACE)
      {
        break;
      }

      scanner_context_p->mode = SCAN_MODE_CLASS_METHOD;
      parser_stack_pop_uint8 (context_p);

      return SCAN_KEEP_TOKEN;
    }
    case SCAN_STACK_FUNCTION_PARAMETERS:
    {
      parser_stack_pop_uint8 (context_p);

      if (type != LEXER_RIGHT_PAREN
          && (type != LEXER_EOS || context_p->stack_top_uint8 != SCAN_STACK_SCRIPT_FUNCTION))
      {
        break;
      }

      scanner_context_p->mode = SCAN_MODE_CONTINUE_FUNCTION_ARGUMENTS;
      return SCAN_KEEP_TOKEN;
    }
#endif /* ENABLED (JERRY_ES2015) */
    default:
    {
      scanner_context_p->mode = SCAN_MODE_STATEMENT_END;
      return SCAN_KEEP_TOKEN;
    }
  }

  scanner_raise_error (context_p);
  return SCAN_NEXT_TOKEN;
} /* scanner_scan_primary_expression_end */