import subprocess
from typing import List, Tuple

# name of executable
PROGRAM = "rfs_test"

# commands
WRITE = "WRITE"
GET = "GET"
RM = "RM"
LS = "LS"

# flags
LOCAL = "--local"
REMOTE = "--remote"

# arg values for the flags
LOCAL_PATH = "local_file.txt"
REMOTE_PATH = "remote_file.txt"

# what the executable should print out
OUTPUT_FORMAT = "COMMAND = %s\nLOCAL_PATH = %s\nREMOTE_PATH = %s\n"


def run(cmd: List[str]) -> Tuple[bool, str]:
    """runs the terminal command and returns the success of that command along with any output

    Args:
        terminal_cmd (List[str]): list of strings for the terminal command

    Returns:
        Tuple[bool, str]: (success, output)
    """

    result = subprocess.run(cmd, capture_output=True, text=True)

    if result.returncode != 0:
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
    print(f"testing {cmd}")

    ok, output = run(cmd)
    assert ok, f"Expected success but failed: {cmd}\nOutput:\n{output}"
    assert output.strip() == expected_out.strip(
    ), f"{cmd}\nexpected:\n{expected_out}\nactual:\n{output}"
    return True


def test_expect_not_ok(cmd: List[str]) -> bool:
    """_summary_

    Args:
        cmd (List[str]): _description_

    Returns:
        bool: _description_
    """
    print(f"testing {cmd}")

    ok, output = run(cmd)
    assert not ok, f"Expected error but succeeded: {cmd}\nOutput:\n{output}"
    return True


def cmd(*args) -> List[str]:
    return ["./" + PROGRAM, *args]


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
    test_expect_ok(write_cmd(REMOTE, REMOTE_PATH,
                             LOCAL, LOCAL_PATH, ), expected)


def test_GET():
    def get_cmd(*args) -> List[str]:
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
    def rm_cmd(*args) -> List[str]:
        return cmd(RM, *args)

    # didn't give local OR remote -> error
    test_expect_not_ok(rm_cmd())

    # only gives remote
    expected = OUTPUT_FORMAT % (RM, "", REMOTE_PATH)
    test_expect_ok(rm_cmd(REMOTE, REMOTE_PATH), expected)

    # gives the remote and local -> ignore the local
    test_expect_ok(rm_cmd(REMOTE, REMOTE_PATH, LOCAL, LOCAL_PATH), expected)
    test_expect_ok(rm_cmd(LOCAL, LOCAL_PATH, REMOTE, REMOTE_PATH), expected)

    # only gives local -> error
    test_expect_not_ok(rm_cmd(LOCAL, LOCAL_PATH))


def test_LS():
    pass  # if I implement ls then write these tests out


def test_invalid_args():
    # invalid command with args
    test_expect_not_ok(cmd("INVALID", LOCAL, LOCAL_PATH, REMOTE, REMOTE_PATH))

    # invalid command with no args
    test_expect_not_ok(cmd("INVALID"))

    # literally no args
    test_expect_not_ok(cmd())

    # valid command but invalid flag
    test_expect_not_ok(cmd(WRITE, "--hello", LOCAL_PATH))

    # valid command and flag but nothing following the flag
    test_expect_not_ok(cmd(WRITE, LOCAL))  # index out of bounds
    test_expect_not_ok(cmd(WRITE, LOCAL, LOCAL_PATH, REMOTE)
                       )  # index out of bounds
    test_expect_not_ok(cmd(WRITE, LOCAL, REMOTE))  # flag following flag
    test_expect_not_ok(cmd(WRITE, REMOTE, LOCAL))  # flag following flag

    # valid command but theres no flag
    test_expect_not_ok(cmd(WRITE, LOCAL_PATH))


def main():
    # initial setup, create the custom executable called `rfs_test`
    result = subprocess.run(["make", "test_args"])

    # make sure we created exectuable for the test
    if result.returncode != 0:
        print("ERROR creating test version of executable")
        return

    # edge case: ignore the invalid flag
    expected = OUTPUT_FORMAT % (WRITE, LOCAL_PATH, LOCAL_PATH)
    test_expect_ok(cmd(WRITE, LOCAL, LOCAL_PATH,
                       "--hello", REMOTE_PATH), expected)

    test_invalid_args()

    test_WRITE()

    test_GET()

    test_RM()

    test_LS()


if __name__ == '__main__':
    main()
