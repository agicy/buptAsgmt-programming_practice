#include <cctype>
#include <cstdio>

/// Maximum number of arguments that can be parsed.
#define MAX_ARGC 30
/// Maximum length of each argument (including \0).
#define MAX_ARG_LEN 32

/**
 * @brief Splits a given string into an array of arguments.
 *
 * This function parses the input string and separates it into individual arguments
 * based on whitespace. It stores the arguments in the provided argv array.
 *
 * @param argv Array to store the pointers to the arguments.
 * @param buffer The string to be split into arguments.
 * @return The number of arguments parsed, or 0 if an error occurs.
 */
int split_str(char *const argv[], const char *buffer) {
    int argc = 0;
    const char *p = buffer;
    while (*p) {
        while (std::isblank(*p))
            ++p;

        if (!(*p))
            break;

        if (argc == MAX_ARGC - 1) {
            std::fprintf(stderr, "[ERROR] more than %d arguments.\n", MAX_ARGC);
            return 0;
        }

        int i = 0;
        while (*p && !std::isblank(*p)) {
            argv[argc][i++] = *p++;
            if (i + 1 >= MAX_ARG_LEN) {
                std::fprintf(stderr,
                             "[ERROR] argument longer than %d.\n", MAX_ARG_LEN);
                return 0;
            }
        }
        argv[argc][i] = '\0';
        ++argc;
    }

    return argc;
}

/**
 * @brief Main function demonstrating the usage of split_str.
 *
 * This function initializes an array of char pointers, calls split_str to parse a string,
 * and then prints out the parsed arguments. It also cleans up the allocated memory.
 *
 * @return Always returns 0.
 */
int main() {
    char *argv[MAX_ARGC];
    for (int i = 0; i < MAX_ARGC; i++)
        argv[i] = new char[MAX_ARG_LEN];

    int argc = split_str(
        argv,
        " kjsf  ks  ks dhf ksdjh ksdjfh skdjf skdf skdjf sdkjf kjsdhf   ");

    for (int i = 0; i < argc; i++)
        std::printf("argv[%d] = %s\n", i, argv[i]);

    for (int i = 0; i < MAX_ARGC; i++) {
        delete[] argv[i];
        argv[i] = nullptr;
    }

    return 0;
}
