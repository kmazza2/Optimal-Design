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
* Contents: Native high-level C interface to LAPACK function dgeevx
* Author: Intel Corporation
*****************************************************************************/

#include "lapacke_utils.h"

lapack_int API_SUFFIX(LAPACKE_dgeevx)( int matrix_layout, char balanc, char jobvl,
                           char jobvr, char sense, lapack_int n, double* a,
                           lapack_int lda, double* wr, double* wi, double* vl,
                           lapack_int ldvl, double* vr, lapack_int ldvr,
                           lapack_int* ilo, lapack_int* ihi, double* scale,
                           double* abnrm, double* rconde, double* rcondv )
{
    lapack_int info = 0;
    lapack_int lwork = -1;
    lapack_int* iwork = NULL;
    double* work = NULL;
    double work_query;
    if( matrix_layout != LAPACK_COL_MAJOR && matrix_layout != LAPACK_ROW_MAJOR ) {
        API_SUFFIX(LAPACKE_xerbla)( "LAPACKE_dgeevx", -1 );
        return -1;
    }
#ifndef LAPACK_DISABLE_NAN_CHECK
    if( LAPACKE_get_nancheck() ) {
        /* Optionally check input matrices for NaNs */
        if( API_SUFFIX(LAPACKE_dge_nancheck)( matrix_layout, n, n, a, lda ) ) {
            return -7;
        }
    }
#endif
    /* Allocate memory for working array(s) */
    if( API_SUFFIX(LAPACKE_lsame)( sense, 'b' ) || API_SUFFIX(LAPACKE_lsame)( sense, 'v' ) ) {
        iwork = (lapack_int*)
            LAPACKE_malloc( sizeof(lapack_int) * MAX(1,2*n-2) );
        if( iwork == NULL ) {
            info = LAPACK_WORK_MEMORY_ERROR;
            goto exit_level_0;
        }
    }
    /* Query optimal working array(s) size */
    info = API_SUFFIX(LAPACKE_dgeevx_work)( matrix_layout, balanc, jobvl, jobvr, sense, n, a,
                                lda, wr, wi, vl, ldvl, vr, ldvr, ilo, ihi,
                                scale, abnrm, rconde, rcondv, &work_query,
                                lwork, iwork );
    if( info != 0 ) {
        goto exit_level_1;
    }
    lwork = (lapack_int)work_query;
    /* Allocate memory for work arrays */
    work = (double*)LAPACKE_malloc( sizeof(double) * lwork );
    if( work == NULL ) {
        info = LAPACK_WORK_MEMORY_ERROR;
        goto exit_level_1;
    }
    /* Call middle-level interface */
    info = API_SUFFIX(LAPACKE_dgeevx_work)( matrix_layout, balanc, jobvl, jobvr, sense, n, a,
                                lda, wr, wi, vl, ldvl, vr, ldvr, ilo, ihi,
                                scale, abnrm, rconde, rcondv, work, lwork,
                                iwork );
    /* Release memory and exit */
    LAPACKE_free( work );
exit_level_1:
    if( API_SUFFIX(LAPACKE_lsame)( sense, 'b' ) || API_SUFFIX(LAPACKE_lsame)( sense, 'v' ) ) {
        LAPACKE_free( iwork );
    }
exit_level_0:
    if( info == LAPACK_WORK_MEMORY_ERROR ) {
        API_SUFFIX(LAPACKE_xerbla)( "LAPACKE_dgeevx", info );
    }
    return info;
}
