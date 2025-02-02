/*****************************************************************************
  Copyright (c) 2014, Intel Corp.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************
* Contents: Native high-level C interface to LAPACK function zgbsvxx
* Author: Intel Corporation
*****************************************************************************/

#include "lapacke_utils.h"

lapack_int API_SUFFIX(LAPACKE_zgbsvxx)( int matrix_layout, char fact, char trans,
                            lapack_int n, lapack_int kl, lapack_int ku,
                            lapack_int nrhs, lapack_complex_double* ab,
                            lapack_int ldab, lapack_complex_double* afb,
                            lapack_int ldafb, lapack_int* ipiv, char* equed,
                            double* r, double* c, lapack_complex_double* b,
                            lapack_int ldb, lapack_complex_double* x,
                            lapack_int ldx, double* rcond, double* rpvgrw,
                            double* berr, lapack_int n_err_bnds,
                            double* err_bnds_norm, double* err_bnds_comp,
                            lapack_int nparams, double* params )
{
    lapack_int info = 0;
    double* rwork = NULL;
    lapack_complex_double* work = NULL;
    if( matrix_layout != LAPACK_COL_MAJOR && matrix_layout != LAPACK_ROW_MAJOR ) {
        API_SUFFIX(LAPACKE_xerbla)( "LAPACKE_zgbsvxx", -1 );
        return -1;
    }
#ifndef LAPACK_DISABLE_NAN_CHECK
    if( LAPACKE_get_nancheck() ) {
        /* Optionally check input matrices for NaNs */
        if( API_SUFFIX(LAPACKE_zgb_nancheck)( matrix_layout, n, n, kl, ku, ab, ldab ) ) {
            return -8;
        }
        if( API_SUFFIX(LAPACKE_lsame)( fact, 'f' ) ) {
            if( API_SUFFIX(LAPACKE_zgb_nancheck)( matrix_layout, n, n, kl, kl+ku, afb,
                ldafb ) ) {
                return -10;
            }
        }
        if( API_SUFFIX(LAPACKE_zge_nancheck)( matrix_layout, n, nrhs, b, ldb ) ) {
            return -16;
        }
        if( API_SUFFIX(LAPACKE_lsame)( fact, 'f' ) && ( API_SUFFIX(LAPACKE_lsame)( *equed, 'b' ) ||
            API_SUFFIX(LAPACKE_lsame)( *equed, 'c' ) ) ) {
            if( API_SUFFIX(LAPACKE_d_nancheck)( n, c, 1 ) ) {
                return -15;
            }
        }
        if( nparams>0 ) {
            if( API_SUFFIX(LAPACKE_d_nancheck)( nparams, params, 1 ) ) {
                return -27;
            }
        }
        if( API_SUFFIX(LAPACKE_lsame)( fact, 'f' ) && ( API_SUFFIX(LAPACKE_lsame)( *equed, 'b' ) ||
            API_SUFFIX(LAPACKE_lsame)( *equed, 'r' ) ) ) {
            if( API_SUFFIX(LAPACKE_d_nancheck)( n, r, 1 ) ) {
                return -14;
            }
        }
    }
#endif
    /* Allocate memory for working array(s) */
    rwork = (double*)LAPACKE_malloc( sizeof(double) * MAX(1,3*n) );
    if( rwork == NULL ) {
        info = LAPACK_WORK_MEMORY_ERROR;
        goto exit_level_0;
    }
    work = (lapack_complex_double*)
        LAPACKE_malloc( sizeof(lapack_complex_double) * MAX(1,2*n) );
    if( work == NULL ) {
        info = LAPACK_WORK_MEMORY_ERROR;
        goto exit_level_1;
    }
    /* Call middle-level interface */
    info = API_SUFFIX(LAPACKE_zgbsvxx_work)( matrix_layout, fact, trans, n, kl, ku, nrhs, ab,
                                 ldab, afb, ldafb, ipiv, equed, r, c, b, ldb, x,
                                 ldx, rcond, rpvgrw, berr, n_err_bnds,
                                 err_bnds_norm, err_bnds_comp, nparams, params,
                                 work, rwork );
    /* Release memory and exit */
    LAPACKE_free( work );
exit_level_1:
    LAPACKE_free( rwork );
exit_level_0:
    if( info == LAPACK_WORK_MEMORY_ERROR ) {
        API_SUFFIX(LAPACKE_xerbla)( "LAPACKE_zgbsvxx", info );
    }
    return info;
}
