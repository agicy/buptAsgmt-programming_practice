#include "hmac.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

void test_hmac(const char *const algorithm) {
    const char *const src = "kjhdskfhdskfjhdskjfdskfdskfjsdkfjds";
    const char *const key = "kjshfkds";

    const std::size_t buffer_size = 256;
    char *const buffer = new char[buffer_size];

    const char *const result = hmac(
        src, std::strlen(src),
        key, std::strlen(key),
        algorithm,
        buffer, buffer_size);

    if (result)
        printf("%s result = [%s]\n", algorithm, result);
    else
        printf("ERROR: %s\n", buffer);

    delete[] buffer;

    return;
}

int main() {
    // 测试存在的算法
    test_hmac("md5");
    test_hmac("sha1");
    test_hmac("sha224");
    test_hmac("sha256");
    test_hmac("sha384");
    test_hmac("sha512");

    // xxx 是不存在的算法
    test_hmac("xxx");
    return 0;
}
