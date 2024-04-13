scanner_scan_statement_end (parser_context_t *context_p, /**< context */
                            scanner_context_t *scanner_context_p, /**< scanner context */
                            lexer_token_type_t type) /**< current token type */
{
  bool terminator_found = false;

  if (type == LEXER_SEMICOLON)
  {
    lexer_next_token (context_p);
    terminator_found = true;
  }

  while (true)
  {
    type = (lexer_token_type_t) context_p->token.type;

    switch (context_p->stack_top_uint8)
    {
      case SCAN_STACK_SCRIPT:
      case SCAN_STACK_SCRIPT_FUNCTION:
      {
        if (type == LEXER_EOS)
        {
          return SCAN_NEXT_TOKEN;
        }
        break;
      }
      case SCAN_STACK_BLOCK_STATEMENT:
#if ENABLED (JERRY_ES2015)
      case SCAN_STACK_CLASS_STATEMENT:
#endif /* ENABLED (JERRY_ES2015) */
      case SCAN_STACK_FUNCTION_STATEMENT:
      {
        if (type != LEXER_RIGHT_BRACE)
        {
          break;
        }

#if ENABLED (JERRY_ES2015)
        if (context_p->stack_top_uint8 != SCAN_STACK_CLASS_STATEMENT)
        {
          scanner_pop_literal_pool (context_p, scanner_context_p);
        }
#else /* !ENABLED (JERRY_ES2015) */
        if (context_p->stack_top_uint8 == SCAN_STACK_FUNCTION_STATEMENT)
        {
          scanner_pop_literal_pool (context_p, scanner_context_p);
        }
#endif /* ENABLED (JERRY_ES2015) */

        terminator_found = true;
        parser_stack_pop_uint8 (context_p);
        lexer_next_token (context_p);
        continue;
      }
      case SCAN_STACK_FUNCTION_EXPRESSION:
#if ENABLED (JERRY_ES2015)
      case SCAN_STACK_FUNCTION_ARROW:
#endif /* ENABLED (JERRY_ES2015) */
      {
        if (type != LEXER_RIGHT_BRACE)
        {
          break;
        }

        scanner_context_p->mode = SCAN_MODE_POST_PRIMARY_EXPRESSION;
#if ENABLED (JERRY_ES2015)
        if (context_p->stack_top_uint8 == SCAN_STACK_FUNCTION_ARROW)
        {
          scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION_END;
        }
#endif /* ENABLED (JERRY_ES2015) */

        scanner_pop_literal_pool (context_p, scanner_context_p);
        parser_stack_pop_uint8 (context_p);
        return SCAN_NEXT_TOKEN;
      }
      case SCAN_STACK_FUNCTION_PROPERTY:
      {
        if (type != LEXER_RIGHT_BRACE)
        {
          break;
        }

        scanner_pop_literal_pool (context_p, scanner_context_p);
        parser_stack_pop_uint8 (context_p);

#if ENABLED (JERRY_ES2015)
        if (context_p->stack_top_uint8 == SCAN_STACK_EXPLICIT_CLASS_CONSTRUCTOR
            || context_p->stack_top_uint8 == SCAN_STACK_IMPLICIT_CLASS_CONSTRUCTOR)
        {
          scanner_context_p->mode = SCAN_MODE_CLASS_METHOD;
          return SCAN_KEEP_TOKEN;
        }
#endif /* ENABLED (JERRY_ES2015) */

        JERRY_ASSERT (context_p->stack_top_uint8 == SCAN_STACK_OBJECT_LITERAL);

        lexer_next_token (context_p);

        if (context_p->token.type == LEXER_RIGHT_BRACE)
        {
          scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION_END;
          return SCAN_KEEP_TOKEN;
        }

        if (context_p->token.type != LEXER_COMMA)
        {
          scanner_raise_error (context_p);
        }

        scanner_context_p->mode = SCAN_MODE_PROPERTY_NAME;
        return SCAN_KEEP_TOKEN;
      }
      case SCAN_STACK_SWITCH_BLOCK:
      {
        if (type != LEXER_RIGHT_BRACE)
        {
          break;
        }

        scanner_switch_statement_t switch_statement;

        parser_stack_pop_uint8 (context_p);
        parser_stack_pop (context_p, &switch_statement, sizeof (scanner_switch_statement_t));

        scanner_context_p->active_switch_statement = switch_statement;

#if ENABLED (JERRY_ES2015)
        scanner_pop_literal_pool (context_p, scanner_context_p);
#endif /* ENABLED (JERRY_ES2015) */

        terminator_found = true;
        lexer_next_token (context_p);
        continue;
      }
      case SCAN_STACK_IF_STATEMENT:
      {
        parser_stack_pop_uint8 (context_p);

        if (type == LEXER_KEYW_ELSE
            && (terminator_found || (context_p->token.flags & LEXER_WAS_NEWLINE)))
        {
#if ENABLED (JERRY_ES2015)
          scanner_check_function_after_if (context_p, scanner_context_p);
          return SCAN_KEEP_TOKEN;
#else /* !ENABLED (JERRY_ES2015) */
          scanner_context_p->mode = SCAN_MODE_STATEMENT;
          return SCAN_NEXT_TOKEN;
#endif /* ENABLED (JERRY_ES2015) */
        }
        continue;
      }
      case SCAN_STACK_WITH_STATEMENT:
      {
        scanner_literal_pool_t *literal_pool_p = scanner_context_p->active_literal_pool_p;

        JERRY_ASSERT (literal_pool_p->status_flags & SCANNER_LITERAL_POOL_IN_WITH);

        parser_stack_pop_uint8 (context_p);

        if (context_p->stack_top_uint8 == 0)
        {
          literal_pool_p->status_flags &= (uint16_t) ~SCANNER_LITERAL_POOL_IN_WITH;
        }

        parser_stack_pop_uint8 (context_p);
        continue;
      }
      case SCAN_STACK_DO_STATEMENT:
      {
        parser_stack_pop_uint8 (context_p);

        if (type != LEXER_KEYW_WHILE
            || (!terminator_found && !(context_p->token.flags & LEXER_WAS_NEWLINE)))
        {
          scanner_raise_error (context_p);
        }

        lexer_next_token (context_p);
        if (context_p->token.type != LEXER_LEFT_PAREN)
        {
          scanner_raise_error (context_p);
        }

        parser_stack_push_uint8 (context_p, SCAN_STACK_DO_EXPRESSION);
        scanner_context_p->mode = SCAN_MODE_PRIMARY_EXPRESSION;
        return SCAN_NEXT_TOKEN;
      }
      case SCAN_STACK_DO_EXPRESSION:
      {
        parser_stack_pop_uint8 (context_p);
        terminator_found = true;
        continue;
      }
#if ENABLED (JERRY_ES2015)
      case SCAN_STACK_PRIVATE_BLOCK_EARLY:
      {
        parser_list_iterator_t literal_iterator;
        lexer_lit_location_t *literal_p;

        parser_list_iterator_init (&scanner_context_p->active_literal_pool_p->literal_pool, &literal_iterator);

        while ((literal_p = (lexer_lit_location_t *) parser_list_iterator_next (&literal_iterator)) != NULL)
        {
          if ((literal_p->type & (SCANNER_LITERAL_IS_LET | SCANNER_LITERAL_IS_CONST))
              && literal_p->type & SCANNER_LITERAL_NO_REG)
          {
            literal_p->type |= SCANNER_LITERAL_EARLY_CREATE;
          }
        }
        /* FALLTHRU */
      }
      case SCAN_STACK_PRIVATE_BLOCK:
      {
        parser_stack_pop_uint8 (context_p);
        scanner_pop_literal_pool (context_p, scanner_context_p);
        continue;
      }
#endif /* ENABLED (JERRY_ES2015) */
      default:
      {
        JERRY_ASSERT (context_p->stack_top_uint8 == SCAN_STACK_TRY_STATEMENT
                      || context_p->stack_top_uint8 == SCAN_STACK_CATCH_STATEMENT);

        if (type != LEXER_RIGHT_BRACE)
        {
          break;
        }

        uint8_t stack_top = context_p->stack_top_uint8;
        parser_stack_pop_uint8 (context_p);
        lexer_next_token (context_p);

#if ENABLED (JERRY_ES2015)
        scanner_pop_literal_pool (context_p, scanner_context_p);
#else /* !ENABLED (JERRY_ES2015) */
        if (stack_top == SCAN_STACK_CATCH_STATEMENT)
        {
          scanner_pop_literal_pool (context_p, scanner_context_p);
        }
#endif /* ENABLED (JERRY_ES2015) */

        /* A finally statement is optional after a try or catch statement. */
        if (context_p->token.type == LEXER_KEYW_FINALLY)
        {
          lexer_next_token (context_p);

          if (context_p->token.type != LEXER_LEFT_BRACE)
          {
            scanner_raise_error (context_p);
          }

#if ENABLED (JERRY_ES2015)
          scanner_literal_pool_t *literal_pool_p;
          literal_pool_p = scanner_push_literal_pool (context_p,
                                                      scanner_context_p,
                                                      SCANNER_LITERAL_POOL_BLOCK);
          literal_pool_p->source_p = context_p->source_p;
#endif /* ENABLED (JERRY_ES2015) */

          parser_stack_push_uint8 (context_p, SCAN_STACK_BLOCK_STATEMENT);
          scanner_context_p->mode = SCAN_MODE_STATEMENT_OR_TERMINATOR;
          return SCAN_NEXT_TOKEN;
        }

        if (stack_top == SCAN_STACK_CATCH_STATEMENT)
        {
          terminator_found = true;
          continue;
        }

        /* A catch statement must be present after a try statement unless a finally is provided. */
        if (context_p->token.type != LEXER_KEYW_CATCH)
        {
          scanner_raise_error (context_p);
        }

        lexer_next_token (context_p);

        if (context_p->token.type != LEXER_LEFT_PAREN)
        {
          scanner_raise_error (context_p);
        }

        scanner_literal_pool_t *literal_pool_p;
        literal_pool_p = scanner_push_literal_pool (context_p, scanner_context_p, SCANNER_LITERAL_POOL_BLOCK);
        literal_pool_p->source_p = context_p->source_p;

        lexer_next_token (context_p);
        parser_stack_push_uint8 (context_p, SCAN_STACK_CATCH_STATEMENT);

#if ENABLED (JERRY_ES2015)
        if (context_p->token.type == LEXER_LEFT_SQUARE || context_p->token.type == LEXER_LEFT_BRACE)
        {
          scanner_push_destructuring_pattern (context_p, scanner_context_p, SCANNER_BINDING_CATCH, false);

          if (context_p->token.type == LEXER_LEFT_SQUARE)
          {
            parser_stack_push_uint8 (context_p, SCAN_STACK_ARRAY_LITERAL);
            scanner_context_p->mode = SCAN_MODE_BINDING;
            return SCAN_NEXT_TOKEN;
          }

          parser_stack_push_uint8 (context_p, SCAN_STACK_OBJECT_LITERAL);
          scanner_context_p->mode = SCAN_MODE_PROPERTY_NAME;
          return SCAN_KEEP_TOKEN;
        }
#endif /* ENABLED (JERRY_ES2015) */

        if (context_p->token.type != LEXER_LITERAL
            || context_p->token.lit_location.type != LEXER_IDENT_LITERAL)
        {
          scanner_raise_error (context_p);
        }

        lexer_lit_location_t *lit_location_p = scanner_add_literal (context_p, scanner_context_p);
        lit_location_p->type |= SCANNER_LITERAL_IS_LOCAL;

        lexer_next_token (context_p);

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
    }

    if (!terminator_found && !(context_p->token.flags & LEXER_WAS_NEWLINE))
    {
      scanner_raise_error (context_p);
    }

    scanner_context_p->mode = SCAN_MODE_STATEMENT;
    return SCAN_KEEP_TOKEN;
  }
} /* scanner_scan_statement_end */