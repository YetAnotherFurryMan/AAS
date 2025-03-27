module aas_parser
    implicit none

    enum, bind(c) 
        enumerator AAS_TT_ERROR, AAS_TT_EOF, AAS_TT_ID, AAS_TT_STR, AAS_TT_INT
    end enum

    type :: aas_token
        integer :: tt
        character(len=:), allocatable :: text
    end type

    type :: aas_state
        integer :: pos
        character(len=:), pointer :: src
    end type

contains

    pure logical function is_valid_num_ch(ch)
        character, intent(in) :: ch

        is_valid_num_ch = ch >= '0' .and. ch <= '9'
    end function is_valid_num_ch

    pure logical function is_valid_id_ch(ch, strict)
        character, intent(in) :: ch
        logical, intent(in) :: strict

        if (ch == '_') then
            is_valid_id_ch = .true.
        else if (ch >= 'A' .and. ch <= 'Z') then
            is_valid_id_ch = .true.
        else if (ch >= 'a' .and. ch <= 'z') then
            is_valid_id_ch = .true.
        else if (.not. strict .and. is_valid_num_ch(ch)) then
            is_valid_id_ch = .true.
        else if (strict .and. ch == '-') then
            is_valid_id_ch = .false.
        else if ((ch >= '*' .and. ch <= '/') .or. (ch >= ':' .and. ch <= '?')) then
            is_valid_id_ch = .true.
        else
            is_valid_id_ch = .false.
        end if

    end function is_valid_id_ch

    subroutine aas_next_token(state, tok)
        type(aas_state), intent(inout) :: state
        type(aas_token), intent(out) :: tok
        integer :: begin

        do while(state%pos <= len_trim(state%src) .and. state%src(state%pos:state%pos) == ' ')
            state%pos = state%pos + 1
        end do

        if (state%pos > len_trim(state%src)) then
            tok = aas_token(tt = AAS_TT_EOF, text = null())
            return
        end if

        tok = aas_token(tt = AAS_TT_ERROR, text = null())

        if (is_valid_id_ch(state%src(state%pos:state%pos), .true.)) then
            begin = state%pos

            do while (state%pos <= len_trim(state%src) .and. is_valid_id_ch(state%src(state%pos:state%pos), .false.))
                state%pos = state%pos + 1
            end do
            
            tok%tt = AAS_TT_ID
            tok%text = state%src(begin:state%pos - 1)
        else if (state%src(state%pos:state%pos) == '-' .or. is_valid_num_ch(state%src(state%pos:state%pos))) then
            begin = state%pos
            state%pos = state%pos + 1

            do while (state%pos <= len_trim(state%src) .and. is_valid_num_ch(state%src(state%pos:state%pos)))
                state%pos = state%pos + 1
            end do

            tok%tt = AAS_TT_INT
            tok%text = state%src(begin:state%pos - 1)
        else if (state%src(state%pos:state%pos) == char(39)) then
            ! This is raw string inside ''
            state%pos = state%pos + 1
            begin = state%pos

            do while (state%pos <= len_trim(state%src) .and. state%src(state%pos:state%pos) /= char(39))
                state%pos = state%pos + 1
            end do

            if (state%pos > len_trim(state%src)) then
                tok%text = state%src(begin - 1:)
            else
                tok%tt = AAS_TT_STR
                tok%text = state%src(begin:state%pos - 1)
                state%pos = state%pos + 1
            end if
        else if (state%src(state%pos:state%pos) == char(34)) then
            ! This is a string that accepts escape-sequenses
            state%pos = state%pos + 1
            begin = state%pos

            tok%text = ""

            do while (state%pos <= len_trim(state%src) .and. state%src(state%pos:state%pos) /= char(34))
                if (state%src(state%pos:state%pos) == '\') then
                    tok%text = tok%text // state%src(begin:state%pos - 1)

                    state%pos = state%pos + 1
                    
                    if (state%pos > len_trim(state%src)) then
                        exit
                    end if

                    select case (state%src(state%pos:state%pos))
                        case (char(34))
                        case (char(39))
                        case ('\')
                            tok%text = tok%text // state%src(state%pos:state%pos)

                        case ('n')
                            tok%text = tok%text // char(10)

                        case ('r')
                            tok%text = tok%text // char(13)

                        case ('t')
                            tok%text = tok%text // char(9)

                        case ('b')
                            tok%text = tok%text // char(7)
                    end select
                end if

                state%pos = state%pos + 1
            end do

            tok%text = tok%text // state%src(begin:state%pos - 1)
            if (state%pos <= len_trim(state%src)) then
                tok%tt = AAS_TT_STR
                state%pos = state%pos + 1
            end if
        end if
    end subroutine aas_next_token

end module aas_parser
