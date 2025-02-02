#include "lapacke_utils.h"

lapack_int API_SUFFIX(LAPACKE_zunhr_col_work)( int matrix_layout, lapack_int m, lapack_int n,
                                   lapack_int nb, lapack_complex_double* a,
                                   lapack_int lda, lapack_complex_double* t,
                                   lapack_int ldt, lapack_complex_double* d )
{
    lapack_int info = 0;
    if( matrix_layout == LAPACK_COL_MAJOR ) {
        /* Call LAPACK function and adjust info */
        LAPACK_zunhr_col( &m, &n, &nb, a, &lda, t, &ldt, d, &info );
        if( info < 0 ) {
            info = info - 1;
        }
    } else if( matrix_layout == LAPACK_ROW_MAJOR ) {
        lapack_int lda_t = MAX(1,m);
        lapack_int ldt_t = MAX(1,MIN(nb,n));
        lapack_complex_double* a_t = NULL;
        lapack_complex_double* t_t = NULL;
        /* Check leading dimension(s) */
        if( lda < n ) {
            info = -6;
            API_SUFFIX(LAPACKE_xerbla)( "LAPACKE_zunhr_col_work", info );
            return info;
        }
        if( ldt < n ) {
            info = -8;
            API_SUFFIX(LAPACKE_xerbla)( "LAPACKE_zunhr_col_work", info );
            return info;
        }
        /* Allocate memory for temporary array(s) */
        a_t = (lapack_complex_double*)
            LAPACKE_malloc( sizeof(lapack_complex_double) * lda_t * MAX(1,n) );
        if( a_t == NULL ) {
            info = LAPACK_TRANSPOSE_MEMORY_ERROR;
            goto exit_level_0;
        }
        t_t = (lapack_complex_double*)
            LAPACKE_malloc( sizeof(lapack_complex_double) *
                            ldt_t * MAX(1,n) );
        if( t_t == NULL ) {
            info = LAPACK_TRANSPOSE_MEMORY_ERROR;
            goto exit_level_1;
        }
        /* Transpose input matrices */
        API_SUFFIX(LAPACKE_zge_trans)( matrix_layout, m, n, a, lda, a_t, lda_t );
        /* Call LAPACK function and adjust info */
        LAPACK_zunhr_col( &m, &n, &nb, a_t, &lda_t, t_t, &ldt_t, d, &info );
        if( info < 0 ) {
            info = info - 1;
        }
        /* Transpose output matrices */
        API_SUFFIX(LAPACKE_zge_trans)( LAPACK_COL_MAJOR, m, n, a_t, lda_t, a, lda );
        API_SUFFIX(LAPACKE_zge_trans)( LAPACK_COL_MAJOR, ldt, n, t_t, ldt_t, t,
                           ldt );
        /* Release memory and exit */
        LAPACKE_free( t_t );
exit_level_1:
        LAPACKE_free( a_t );
exit_level_0:
        if( info == LAPACK_TRANSPOSE_MEMORY_ERROR ) {
            API_SUFFIX(LAPACKE_xerbla)( "LAPACKE_zunhr_col_work", info );
        }
    } else {
        info = -1;
        API_SUFFIX(LAPACKE_xerbla)( "LAPACKE_zunhr_col_work", info );
    }
    return info;
}
