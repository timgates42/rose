      SUBROUTINE MCFIT(LOUT)
      LOGICAL IERR
    ! KERR = KERR.OR.IERR
      IF (IERR) WRITE (LOUT,8000)
 8000 FORMAT (10X, 'ERROR IN CKXNUM READING FROM TRANSPORT DATA BASE')
      END
