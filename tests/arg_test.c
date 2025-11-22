// special executable for args test is called `rfs_test`

#include <assert.h>
#include <stdlib.h>

void testExpectFailure(char* sys_command) {
    int exitStatus = system(sys_command);
    assert(exitStatus == 1);
}

void testExpectSuccess(char* sys_command) {
    int exitStatus = system(sys_command);
    assert(exitStatus == 0);
}

void test_WRITE() {
    // happy path received both
    testExpectSuccess(
        "./rfs_test WRITE --local example_source.txt --remote example_destination.txt > output/write_local_remote.txt");

    // happy path received local and uses default remote
    testExpectSuccess("./rfs_test WRITE --local example_source.txt > output/write_local_only.txt");

    // edge case doesn't receive local path
    testExpectFailure("./rfs_test WRITE --remote example_destination.txt");

    // edge case doesn't receive local or remote path
    testExpectFailure("./rfs_test WRITE");
}

void test_GET() {}

void test_RM() {}

void test_LS() {}

int main() {
    // edge case invalid command
    testExpectFailure("./rfs_test INVALID");

    // edge case empty args
    testExpectFailure("./rfs");

    // WRITE command requires local path but remote path is optional
    test_WRITE();

    // GET command requires remote path and local path is optional
    test_GET();

    // RM and LS command requires remote path
    test_RM();
    test_LS();

    return 0;
}