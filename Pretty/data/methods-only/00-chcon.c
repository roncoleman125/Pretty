/* Given a security context, CONTEXT, derive a context_t (*RET),
   setting any portions selected via the global variables, specified_user,
   specified_role, etc.  */
static int
compute_context_from_mask (security_context_t context, context_t *ret)
{
  bool ok = true;
  context_t new_context = context_new (context);
  if (!new_context)
    {
      error (0, errno, _("failed to create security context: %s"),
             quotearg_colon (context));
      return 1;
    }

   do									\
     {									\
       if (specified_ ## comp						\
           && context_ ## comp ## _set ((C), specified_ ## comp))	\
         {								\
            error (0, errno,						\
                   _("failed to set %s security context component to %s"), \
                   #comp, quote (specified_ ## comp));			\
           ok = false;							\
         }								\
     }									\
   while (0)

  SET_COMPONENT (new_context, user);
  SET_COMPONENT (new_context, range);
  SET_COMPONENT (new_context, role);
  SET_COMPONENT (new_context, type);

  if (!ok)
    {
      int saved_errno = errno;
      context_free (new_context);
      errno = saved_errno;
      return 1;
    }

  *ret = new_context;
  return 0;
}

