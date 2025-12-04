# Practicum_2_CS5600

# TODO

- [ ] standardize file paths to be inside client/data or server/data
- [x] serialize request object
- [x] send request object to server
- [x] send file contents to server
- [x] de-serialize request object
- [x] serialize response object
- [x] send response object to client
- [x] de-serialize response object
- [x] send file contents to client
- [ ] IMPORTANT: double check instructions for GET command
  - "If the local file path or name (the third command line argument) is omitted, use current folder."
  - currently I'm using remote path as the default for the local path
- [x] figure out how to get IP address of server
- [ ] handle server shut down
  - [ ] You turn off the server by killing the process... or you could have another command that stops the server, e.g., rfs STOP -- if the server gets that command it calls exit(0) to exit the process after closing the socket.
  - [ ] use article he gave us to handle ctrl+c (<https://www.delftstack.com/howto/c/sigint-in-c/>)
- [x] implement RM command
- [ ] implement multi-threading OR multi-processing
  - initial thought = threads
  - then I can just repurpose command handler functions
- [ ] WRITE command to existing file saves the old version
  - [ ] function that creates filename for the older versions, need to keep track of v1, v2 etc.

# references

<https://www.geeksforgeeks.org/c/socket-programming-cc/>
<https://thelinuxcode.com/sockaddr-in-structure-usage-c/>
<https://thelinuxcode.com/c-recv-function-usage/>
<https://stackoverflow.com/questions/2862071/how-large-should-my-recv-buffer-be-when-calling-recv-in-the-socket-library>
<https://www.geeksforgeeks.org/c/converting-string-to-long-in-c/>
<https://chatgpt.com/share/69294bfe-a954-800e-8d48-fd0a1c3636e6>
<https://linuxvox.com/blog/dev-null-in-linux/>
<https://sample-files.com/>
<https://www.geeksforgeeks.org/c/c-program-display-hostname-ip-address/#>
<https://www.man7.org/linux/man-pages/man2/gethostname.2.html>
<https://thelinuxcode.com/gethostbyname-function-c/>
<https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation?tabs=registry>
