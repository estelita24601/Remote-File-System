# Practicum II - CS 5600

**Fall 2025**

**Estelita Chen**

## Build Commands

**`make` or `make all`** - Build client and server  
**`make client`** - Build client only (`client/rfs`)  
**`make server`** - Build server only (`server/rfs_server`)  
**`make test_args`** - Build argument parser unit tests (`tests/test_args`)  
**`make test_protocol`** - Build protocol unit tests (`tests/test_protocol`)  
**`make clean`** - Remove all executables

## Usage

**Start the server:**

```bash
cd server
./rfs_server
```

**Client commands:**

```bash
cd client
./rfs <COMMAND> [ARGUMENTS]
```

**Commands:**

- `WRITE` - Upload a local file to the remote file system
- `GET` - Download a file from the remote file system
- `RM` - Remove a file from the remote file system

**Arguments:**

- `--server IP` - Server IP address (default: localhost)
- `--local PATH` - Local file path (required for WRITE, optional for GET)
- `--remote PATH` - Remote file path (required for GET/RM/LS, optional for WRITE)

**Examples:**

```bash
./rfs WRITE --local myfile.txt
./rfs WRITE --local myfile.txt --remote folder/file.txt
./rfs GET --remote file.txt
./rfs GET --remote file.txt --local download.txt
./rfs RM --remote file.txt
./rfs RM --remote file.txt --server 12.123.123.12
```

## Testing

**Integration tests:**

```bash
cd tests
./write_test.sh    # output saved in tests/output_write
./get_test.sh      # output saved in tests/output_get
./rm_test.sh       # output saved in tests/output_remove
./version_test.sh  # output saved in tests/output_version
```



**Unit tests:**

```bash
cd tests
python3 arg_parse_test.py  # Test argument parsing for client
./test_protocol            # Test protocol for client/server communication
```

# 

## References

Code, L. (2023a, October 30). *Using the gettimeofday() Function for Precise Timing in C*. TheLinuxCode. https://thelinuxcode.com/gettimeofday_c_language/

Code, L. (2023b, November). *The sockaddr_in Structure in C - A Detailed Guide*. TheLinuxCode. https://thelinuxcode.com/sockaddr-in-structure-usage-c/

Code, L. (2023c, November 8). *Mastering the recv() System Call for Socket Programming in C*. TheLinuxCode. https://thelinuxcode.com/c-recv-function-usage/

Code, L. (2023d, November 13). *Gethostbyname() Function in C - An In-Depth Guide*. TheLinuxCode. https://thelinuxcode.com/gethostbyname-function-c/

GeeksforGeeks. (2022, July 15). *Converting String to Long in C*. GeeksforGeeks. https://www.geeksforgeeks.org/c/converting-string-to-long-in-c/

GeeksforGeeks, & Semwal, S. (2017, November 24). *C Program to display hostname and IP address*. GeeksforGeeks. https://www.geeksforgeeks.org/c/c-program-display-hostname-ip-address/

GeeksforGeeks, & Sinha, A. (2016, December 6). *Socket Programming in C*. GeeksforGeeks. https://www.geeksforgeeks.org/c/socket-programming-cc/

*How large should my recv buffer be when calling recv in the socket library*. (2010, June). Stack Overflow. https://stackoverflow.com/questions/2862071/how-large-should-my-recv-buffer-be-when-calling-recv-in-the-socket-library

Hu, J. (2021, February 14). *How to Create a New Directory in C*. Delft Stack. https://www.delftstack.com/howto/c/mkdir-in-c/

Lawrence, B. (2025). *Creating and Destroying Mutexes*. LLNL HPC Tutorials. https://hpc-tutorials.llnl.gov/posix/creating_and_destroying_mutexes/

*Maximum Path Length Limitation*. (2024, July 16). Learn.microsoft.com. https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation?tabs=registry

*mkdir(2) - Linux manual page*. (2025, May 17). Man7.org. https://man7.org/linux/man-pages/man2/mkdir.2.html

OpenAI. (2025). *GPT-5.1*. https://chatgpt.com/share/69294bfe-a954-800e-8d48-fd0a1c3636e6

*pthread_mutex_destroy(3p) - Linux manual page*. (2017). Man7.org. https://www.man7.org/linux/man-pages/man3/pthread_mutex_destroy.3p.html

Sorber, J. (2024). Using read write locks (example in C). In *YouTube*. https://youtu.be/3iwTk24dZOU?si=E08VbPDgox6i75jS

*Understanding `/dev/null` in Linux*. (2025, November 14). LinuxVox. https://linuxvox.com/blog/dev-null-in-linux/
