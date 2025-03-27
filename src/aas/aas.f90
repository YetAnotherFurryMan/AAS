program aas
    use aas_parser

    implicit none

    character(len=20), target :: src
    type(aas_state) :: state
    type(aas_token) :: tok

    src = "This is a test"

    state = aas_state(pos = 1, src = null())

    state%src => src

    call aas_next_token(state, tok)
    print *, tok%tt
    if (allocated(tok%text)) then
        print *, "Text: ", tok%text
        deallocate(tok%text)
    end if

    do while (tok%tt /= AAS_TT_ERROR .and. tok%tt /= AAS_TT_EOF)
        call aas_next_token(state, tok)
        print *, tok%tt
        if (allocated(tok%text)) then
            print *, "Text: ", tok%text
            deallocate(tok%text)
        end if
    end do

end program aas
