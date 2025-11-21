# Question 1 (20 points)
- [ ] expand the code to allow you to send a command from a client to the server for writing a file from the client's file system to the server. 
  - The request needs include:
    - some kind of command (e.g., WRITE)
    - a path to a local file name (e.g., folder/foo.txt)
    - the path and name of the remote file on the server (default to local file path name)
  - cl format: `% rfs WRITE local-file-path remote-file-path `
  - example:   `% rfs WRITE data/localfoo.txt folder/foo.txt`
- [ ] You need to figure out how to get data from the client to the server. 
  - Assume that the client and server run on different systems. That means that you cannot use the file system to send the file as the client and server would be running on different computer systems in practice.
- [ ] Create tests that have the client and server on different physical (or virtual) machines. 
  - You may use physical computers, local virtual machines, or virtual machines in the cloud.
- [ ] The server keeps running continuously until its process is killed. The rfs client connects, does its (one) command, closes the socket, and exits, but the server keeps running.

# Question 2 (20 points)
- [ ] implement a command that retrieves a new file from the remote file system and writes the data read from the socket to a local file. 
  - If the local file path or name (the third command line argument) is omitted, use current folder.
  - cl format: `% rfs GET remote_folder/remote_file.html local_folder/local_file.html`

# Question 3 (20 points)
- [ ] implement a command that deletes a file from the remote file system. 
  - cl format: `% rfs RM remote_folder/remote_file.html`

# Question 4 (25 points)
- [ ] now create a new thread or process for every client so you can service multiple clients simultaneously
  - Be sure to carefully consider your critical regions and ensure that shared data structures are not corrupted. 
  - Consider of multiple clients write to the same file at the same time -- how do you ensure integrity of the file?

# Question 5 (15 points)
- [ ] Modify the code so that when a WRITE request writes to the same remote file name more than once, that the prior version is not simply overwritten but saved as an older version, i.e., implement a versioning file system.

# Question 6 (20 BONUS points)
- [ ] Add a command that gets all versioning information about a file, 
  - i.e., the name of the file and all timestamps when the versions were last written to.
  - `% rfs LS folder/somefile.txt`

# Question 7 (10 BONUS points)
- [ ] Modify the code so that a GET allows you to request a specific version of a file.

# Unofficial Task
- account for the situation where someone stops the process with CTRL-C (SIGINT) so that there is a safe exit (https://www.delftstack.com/howto/c/sigint-in-c/)

# Intended Workflow

1. start the server program, i.e., rfserver &
2. server waits on a port (whatever you set it, let's say 7768) for clients to connect
3. you do: rfs LS foo/bar.c
4. in the multi-threaded version, a thread is spawned to handle the client's request
5. rfs (the client) connects to the server on port 7768 and send it the string LS foo/bar.c 
6. server reads from the socket and gets the string LS foo/bar.c 
7. it interprets the command: LS and then calls some handling function, perhaps via a table of pointers to handler functions
8. in that function you get the info and versions for the file (where and how is up to you)
9. server returns the information to the client by writing to the socket; format in whatever way you like 
10. client gets the information, parses it, and then printf's it to the console 
11. clients (get) disconnect/closes the socket 
12. server waits for next client 
13. You turn off the server by killing the process... or you could have another command that stops the server, e.g., fget STOP -- if the server gets that command it calls exit(0) to exit the process after closing the socket.