/*
 *  Implementation of SAC standard module List
 */


#include "List.h"


void append( SAC_ND_KS_DEC_OUT_RC(list *, res),
             SAC_ND_KS_DEC_IN_RC(list *, elemsA), SAC_ND_KS_DEC_IN_RC(list *, elemsB))
{
/*
 * we do have now:
 * - list **res__p;
 * -  int **__res_rc__p;
 * - list *elemsA;
 * -  int *__elemsA_rc;
 * - list *elemsB;
 * -  int *__elemsB_rc;
 */
  list *new;


  if ( elemsA->rest == NULL) { /* elemsA == NIL! */
    *res__p = elemsB;
    *__res_rc__p = __elemsB_rc;
    if(--elemsA->rc == 0)
      free_list( elemsA);
  }
  else {

    if( elemsA->rc == 1) { /* re-use all elems while rc==1 ! */
      *res__p = elemsA;
      *__res_rc__p = __elemsA_rc;

      do {
        new = elemsA;
        elemsA = elemsA->rest;
      }
      while( (elemsA->rest != NULL) && ( elemsA->rc ==1));
      /*
       * Now, we decrement the "rest" of elemsA.
       * Although this may lead to a 0 rc in case of NIL,
       * we do NOT free that node, since we have to make
       * sure that it survives the copying while-loop.
       * After that loop we check, whether the rc is 0.
       * If that is the case, we know that there were
       * no copies to be done and we can free the NIL!
       */
      elemsA->rc--;
#if TRACE
    fprintf( stderr, "changing CONS at (%p)\n", new);
#endif
    }

    else { /* copy first elem & decrement rc of elemsA ! */
      new = (list *)SAC_MALLOC(sizeof(list));
      new->rc = 1;
      new->elem = elemsA->elem;
#if TRACE
    fprintf( stderr, "creating CONS at (%p)\n", new);
#endif
      *res__p = new;
      *__res_rc__p = &new->rc;
      elemsA->rc--;

      elemsA = elemsA->rest; /* elemsA has to be one in advance of new ! */
    }

    /* new points to the last elem reused/ copied
     * elemsA is one in front!
     */
    while( elemsA->rest != NULL) {
      new->rest = (list *)SAC_MALLOC(sizeof(list));
#if TRACE
    fprintf( stderr, "       [ %d   .   (%p)]\n", new->elem, new->rest);
    fprintf( stderr, "creating CONS at (%p)\n", new->rest);
#endif
      new = new->rest;
      new->rc = 1;
      new->elem = elemsA->elem;
      elemsA = elemsA->rest;
    }
    new->rest = elemsB;
#if TRACE
    fprintf( stderr, "       [ %d   .   (%p)]\n", new->elem, new->rest);
#endif
    /* Finally, we have to do some housekeeping! (see comment above!) */
    if( elemsA->rc == 0)
      free_list(elemsA);
  }
}
