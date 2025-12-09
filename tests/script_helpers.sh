print_header(){
    message=$1 # first arg = header

    echo ""
    echo "================================================"
    echo "$1"
    echo "================================================"
}

run_cmd(){
    # print to console
    echo "$@"

    # actually execute it
    "$@"
}

run_cmd_and_save(){
    save_location=$1
    shift # discard arg after saving it into variable

    echo "$@"
    "$@" &> "$save_location"
}

setup(){
    # assuming we're in same directory as makefile
    print_header "Building Project"
    make clean
    make || exit 1

    print_header "Cleaning up old processes"
    pkill -9 rfs_server
    sleep 1

    print_header "Resetting Files"
    rm -rf server/data/*
    rm -rf server/data/.versions/*
    rm -rf client/data/*
}

display_client_data(){
    # assuming we're inside of tests
    echo ""
    echo "================================================"
    ls -R -A ../client/data | sed 's/^/  /'
}

display_server_data(){
    # assuming we're inside of tests
    echo ""
    echo "================================================"
    ls -R -A ../server/data | sed 's/^/  /'
    echo ""
}