program aas
    use aas_parser

    implicit none

    integer :: argc, input_size
    logical :: file_exists
    character(len=128) :: program_name, input_path
    character(len=:), allocatable, target :: src
    type(aas_state) :: state
    type(aas_token) :: tok

    argc = command_argument_count()
    call get_command_argument(0, program_name)

    if (argc == 0) then
        ! TODO: Print help
        print *, "Asset Assembler Script help:"
        print *, "Usage: ", trim(program_name), " <input.aas>"
        stop
    end if

    ! TODO: parse arg to determinate is it is a flag, def or input
    call get_command_argument(1, input_path)

    print *, "Loading input file: ", input_path

    inquire(file=input_path, exist=file_exists, size=input_size)

    if (.not. file_exists) then
        print *, "Input file doesn't exists: ", input_path
        stop
    end if

    print *, "Input Size: ", input_size
    allocate(character(len=input_size) :: src)

    open(10, file=input_path, status="old", access="stream")
    read (10) src
    close(10)

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

    deallocate(src)

end program aas
