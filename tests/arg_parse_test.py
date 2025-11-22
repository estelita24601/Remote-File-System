"""
    testExpectSuccess(
        "./rfs_test WRITE --local example_source.txt --remote example_destination.txt > output/write_local_remote.txt");

    // happy path received local and uses default remote
    testExpectSuccess("./rfs_test WRITE --local example_source.txt > output/write_local_only.txt");

    // edge case doesn't receive local path
    testExpectFailure("./rfs_test WRITE --remote example_destination.txt");

    // edge case doesn't receive local or remote path
    testExpectFailure("./rfs_test WRITE");
"""
import subprocess
from typing import List,Tuple

PROGRAM = "rfs_test"
WRITE = "WRITE"
GET = "GET"
RM = "RM"
LS = "LS"
LOCAL = "--local"
LOCAL_PATH = "local_file.txt"
REMOTE = "--remote"
REMOTE_PATH = "remote_file.txt"

OUTPUT_FORMAT = "COMMAND = %s\nLOCAL_PATH = %s\nREMOTE_PATH = %s\n"

def run(cmd: List[str]) -> Tuple[bool, str]:
    """runs the terminal command and returns the success of that command along with any output

    Args:
        terminal_cmd (List[str]): list of strings for the terminal command

    Returns:
        Tuple[bool, str]: (success, output)
    """

    result = subprocess.run(cmd, capture_output=True, text=True)

    if result.returncode !=0:
        return False, result.stderr.strip()
    else:
        return True, result.stdout.strip()

def test_expect_ok(cmd: List[str], expected_out: str) -> bool:
    """_summary_

    Args:
        cmd (List[str]): _description_
        expected_out (str): _description_

    Returns:
        bool: _description_
    """
    ok, output = run(cmd)
    assert ok, f"Expected success but failed: {cmd}\nOutput:\n{output}"
    assert output.strip() == expected_out.strip(), f"Incorrect output for: {cmd}"
    return True

def test_expect_not_ok(cmd: List[str]) -> bool:
    """_summary_

    Args:
        cmd (List[str]): _description_

    Returns:
        bool: _description_
    """
    ok, output = run(cmd)
    assert not ok, f"Expected error but succeeded: {cmd}\nOutput:\n{output}"
    return True

def cmd(*args) -> List[str]:
    return ["./"+ PROGRAM, *args]

def test_WRITE():
    def write_cmd(*args) -> List[str]:
        return cmd(WRITE, *args)

    # didnt' give local OR remote -> error
    test_expect_not_ok(write_cmd())

    # only gives local -> then default remote = local
    expected = OUTPUT_FORMAT % (WRITE, LOCAL_PATH, LOCAL_PATH)
    test_expect_ok(write_cmd(LOCAL, LOCAL_PATH), expected)

    # only gives remote -> error
    test_expect_not_ok(write_cmd(REMOTE, REMOTE_PATH))

    # gives local and remote in either order
    expected = OUTPUT_FORMAT % (WRITE, LOCAL_PATH, REMOTE_PATH)
    test_expect_ok(write_cmd(LOCAL, LOCAL_PATH, REMOTE, REMOTE_PATH), expected)
    test_expect_ok(write_cmd(REMOTE, REMOTE_PATH, LOCAL, LOCAL_PATH, ), expected)

def test_GET():
    def get_cmd(*args)-> List[str]:
        return cmd(GET, *args)

    # didnt' give local OR remote -> error
    test_expect_not_ok(get_cmd())

    # only gives remote -> then default local = remote
    expected = OUTPUT_FORMAT % (GET, REMOTE_PATH, REMOTE_PATH)
    test_expect_ok(get_cmd(REMOTE, REMOTE_PATH), expected)

    # gives local and remote in either order
    expected = OUTPUT_FORMAT % (GET, LOCAL_PATH, REMOTE_PATH)
    test_expect_ok(get_cmd(REMOTE, REMOTE_PATH, LOCAL, LOCAL_PATH), expected)
    test_expect_ok(get_cmd(LOCAL, LOCAL_PATH, REMOTE, REMOTE_PATH), expected)

    # only gives local -> error
    test_expect_not_ok(get_cmd(LOCAL, LOCAL_PATH))

def test_RM():
    def rm_cmd(*args)-> List[str]:
        return cmd(RM, *args)

    # didn't give local OR remote -> error
    test_expect_not_ok(rm_cmd())

    # only gives remote
    expected = OUTPUT_FORMAT % (RM, "", REMOTE_PATH)
    test_expect_ok(rm_cmd(REMOTE, REMOTE_PATH), expected)

    # gives the remote and local -> ignore the local
    test_expect_ok(rm_cmd(REMOTE, REMOTE_PATH, LOCAL, LOCAL_PATH), expected)
    test_expect_ok(rm_cmd(LOCAL, LOCAL_PATH,REMOTE, REMOTE_PATH), expected)

    # only gives local -> error
    test_expect_not_ok(rm_cmd(LOCAL, LOCAL_PATH))

def test_LS():
    pass # if I implement ls then write these tests out

# all of these should fail
def test_invalid_args():
    # invalid command with args
    test_expect_not_ok(cmd("INVALID", LOCAL, LOCAL_PATH, REMOTE, REMOTE_PATH))

    # invalid command with no args
    test_expect_not_ok(cmd("INVALID"))

    # literally no args
    test_expect_not_ok(cmd())

    # valid command but invalid flag
    test_expect_not_ok(cmd(WRITE, "--hello", REMOTE_PATH))

    # valid command with valid flag AND invalid flag
    test_expect_not_ok(cmd(WRITE, LOCAL, LOCAL_PATH, "--hello", REMOTE_PATH))

    # valid command and flag but nothing following the flag
    test_expect_not_ok(cmd(WRITE, LOCAL)) # index out of bounds
    test_expect_not_ok(cmd(WRITE, LOCAL, LOCAL_PATH, REMOTE)) # index out of bounds
    test_expect_not_ok(cmd(WRITE, LOCAL, REMOTE)) # flag following flag
    test_expect_not_ok(cmd(WRITE, REMOTE, LOCAL)) # flag following flag

    # valid command but theres no flag
    test_expect_not_ok(cmd(WRITE, LOCAL_PATH))

def main():
    # initial setup, create the custom executable called `rfs_test`
    make_cmd = f"gcc -Wall -DTESTING ../src/main.c -o {PROGRAM}".split()
    result = subprocess.run(make_cmd)
    if result.returncode != 0:
        print("ERROR creating test version of executable")
        return

    test_invalid_args()

    test_WRITE()

    test_GET()

    test_RM()

    test_LS()


if __name__ == '__main__':
    main()