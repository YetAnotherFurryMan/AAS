program aas
    use aas_parser

    implicit none

    character(len=20), target :: src
    character(len=:), pointer :: srcp
    type(aas_state) :: state
    type(aas_token) :: tok

    srcp => src

    state = aas_state(pos = 0, src = srcp)

    src = "This is a test"

    call aas_next_token(state, tok)

    do while (tok%tt /= AAS_TT_ERROR .and. tok%tt /= AAS_TT_EOF)
        print *, tok%tt, " ", tok%text
        call aas_next_token(state, tok)
    end do

end program aas
