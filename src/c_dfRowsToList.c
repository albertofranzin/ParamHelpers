#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

SEXP c_dfRowsToList(SEXP s_df, SEXP s_pars, SEXP s_types, SEXP s_parnames, SEXP s_lens) {
  int *types = INTEGER(s_types);
  int npars = LENGTH(s_lens);
  int *lens = INTEGER(s_lens);
  int nrow_df = LENGTH(VECTOR_ELT(s_df, 0));
  int row, par, k; /* loop counters for rows, cols, params, vector param elements */
  int type; /* type of column we are currently handling */
  int parlen; /* length of param we are currently handling */
  int colcount = 0; /* when we iterate params, what is the (first) column of s_df that corresponds? */
  SEXP s_res, s_rowlist, s_parval, s_call;
  int all_missing;

  /* we iterate thru rows then params. */
  s_res = PROTECT(NEW_LIST(nrow_df));
  s_call = PROTECT(lang3(install("discreteNameToValue"), R_NilValue, R_NilValue));
  for (row = 0; row < nrow_df; row++) {
    s_rowlist = PROTECT(NEW_LIST(npars));
    /* convert row to R objects and define them in envir s_env */
    colcount = 0;
    for (par = 0; par < npars; par++) { /* iter thru params */
      parlen = lens[par];
      type = types[colcount];
      /* copy vector-param block of row to s_parval */
      if (type == 1) { /* numerics */
        s_parval = PROTECT(NEW_NUMERIC(parlen));
        for (k = 0; k < parlen; k++)
          REAL(s_parval)[k] = REAL(VECTOR_ELT(s_df, colcount+k))[row];
      } else if (type == 2 || type == 4) { /* integers, logical */
        s_parval = PROTECT(NEW_INTEGER(parlen));
        for (k = 0; k < parlen; k++)
          INTEGER(s_parval)[k] = INTEGER(VECTOR_ELT(s_df, colcount+k))[row];
      } else { /* factors */
        s_parval = PROTECT(NEW_CHARACTER(parlen));
        for (k = 0; k < parlen; k++)
          SET_STRING_ELT(s_parval, k, STRING_ELT(VECTOR_ELT(s_df, colcount+k), row));
      }
      
      /* are all entries in s_parval NA ? */
      all_missing = 1;
      for (k = 0; k < parlen; k++) { 
        if(!MISSING(s_parval[k]))
          all_missing = 0;
      }
      if (all_missing) 
        s_parval = NA_LOGICAL;

      /* convert discrete names to values */ 
      if (!all_missing && type == 4) {
        SETCADR(s_call, VECTOR_ELT(s_pars, par));
        SETCADDR(s_call, s_parval);
        s_parval = PROTECT(eval(s_call, R_GlobalEnv)); 
        UNPROTECT(1); /* eval */
      }
     
      SET_VECTOR_ELT(s_rowlist, par, s_parval);
      SET_NAMES(s_rowlist, s_parnames);
      colcount += parlen;
      UNPROTECT(1); /* s_parval  */
    }
    SET_VECTOR_ELT(s_res, row, s_rowlist);
    UNPROTECT(1); /* s_rowlist */
  }
  UNPROTECT(2); /* s_res, s_call */
  return s_res;
}
