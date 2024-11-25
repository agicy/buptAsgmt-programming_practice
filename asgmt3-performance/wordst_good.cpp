#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <format>
#include <iostream>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

static constexpr std::size_t buffer_size = 1 << 20;

struct FileReader {
    std::FILE *file;
    char *buffer;
    char *p1, *p2;
    std::size_t line;

    /**
     * Constructor for FileReader.
     *
     * @param fp std::FILE to read from.
     *
     * Initializes the FileReader object by setting the file pointer, allocating a
     * buffer and setting the line number to 1. The file pointer is not checked for
     * validity.
     */
    FileReader(std::FILE *fp) {
        file = fp;
        buffer = new char[buffer_size];
        p1 = p2 = buffer;
        line = 1;
    }

    /**
     * Destructor for FileReader.
     *
     * Deallocates the buffer allocated in the constructor. The file pointer is not
     * closed.
     */
    ~FileReader(void) {
        delete[] buffer;
    }

    /**
     * Gets a character from the file.
     *
     * If the current buffer is exhausted, reads a new one from the file.
     * If the end of the file is reached, returns EOF.
     *
     * @return The character read, or EOF if the end of the file is reached.
     */
    auto get_char(void) -> char {
        return (p1 == p2 && (p2 = (p1 = buffer) + fread(buffer, 1, buffer_size, file), p1 == p2) ? EOF : *p1++);
    }

    /**
     * Extracts and returns the next word from the file.
     *
     * Skips non-alphabetic characters until a letter is found, then collects
     * characters to form a word until a non-alphabetic character is encountered.
     * Converts the word to lowercase before returning.
     * Increments the line counter when a newline character is encountered.
     *
     * @return The next word in lowercase, or an empty string if EOF is reached.
     */
    auto get_word(void) -> std::string {
        std::string result;
        char ch = get_char();
        while (!std::isalpha(ch)) {
            if (ch == EOF)
                return "";
            if (ch == '\n')
                ++line;
            ch = get_char();
        }
        while (std::isalpha(ch))
            result.push_back(ch), ch = get_char();
        if (ch == '\n')
            ++line;
        for (char &c : result)
            c = std::tolower(c);
        return result;
    }
};

struct FileWriter {
    std::FILE *file;
    char *buffer;
    char *ptr, *end;

    /**
     * Constructs a FileWriter object that writes to the specified file.
     *
     * @param fp std::FILE to write to.
     *
     * Initializes the FileWriter object by setting the file pointer, allocating a
     * buffer and setting the buffer pointers to the beginning of the buffer.
     * The file pointer is not checked for validity.
     */
    FileWriter(std::FILE *fp) {
        file = fp;
        buffer = new char[buffer_size];
        ptr = buffer;
        end = buffer + buffer_size;
    }

    /**
     * Destructor for FileWriter.
     *
     * Ensures any buffered data is written to the file by calling flush() and
     * deallocates the buffer memory. This is called automatically when the
     * FileWriter object goes out of scope.
     */
    ~FileWriter(void) {
        flush();
        delete[] buffer;
    }

    /**
     * Flushes the buffer by writing any buffered data to the file.
     *
     * Ensures any data stored in the buffer is written to the file by calling
     * fwrite() and resets the buffer pointer to the beginning of the buffer.
     */
    auto flush(void) -> void {
        fwrite(buffer, 1, ptr - buffer, file);
        ptr = buffer;
    }

    /**
     * Puts a single character into the output stream.
     *
     * The character is placed in the buffer. If the buffer is full, it is flushed
     * automatically.
     */
    auto put_char(char ch) -> void {
        if (ptr == end)
            flush();
        *ptr++ = ch;
    }

    /**
     * Writes a string to the output stream.
     *
     * The string is copied into the buffer and if the buffer is full, it is
     * flushed automatically.
     */
    auto put_string(const std::string &str) -> void {
        for (size_t i = 0; i < str.length();)
            if (static_cast<ssize_t>(str.length() - i) >= end - ptr) {
                std::memcpy(ptr, str.data() + i, end - ptr);
                flush();
                i += end - ptr;
            } else {
                std::memcpy(ptr, str.data() + i, str.length() - i);
                ptr += str.length() - i;
                i = str.length();
            }
    }
};

constexpr std::size_t limit = 20;

struct WordInfo {
    std::size_t count;
    std::array<size_t, limit> lines;
};

class Trie {
  private:
    struct unit_t {
        std::array<size_t, 26> ch;
        std::optional<WordInfo> info;
    };
    size_t root;
    std::vector<unit_t> units;

  public:
    /**
     * Constructor for the Trie class.
     *
     * Initializes the Trie with a single root node. The root node is
     * represented by index 0 in the units vector, and the vector is
     * resized to accommodate this root node.
     */
    Trie() {
        root = 0;
        units.resize(1);
    }

    /**
     * Inserts a word into the Trie and records its occurrence line.
     *
     * Traverses or creates nodes corresponding to each character in the word.
     * If the word does not exist in the Trie, it creates a path for it.
     * Once the word is inserted, it updates the WordInfo for the terminal node
     * to increase the occurrence count and store the line number, if within limits.
     *
     * @param word The string to be inserted into the Trie.
     * @param line The line number where the word occurs.
     */
    auto insert(const std::string &word, const size_t line) -> void {
        size_t p = root;
        for (char ch : word) {
            size_t c = ch - 'a';
            if (!units[p].ch[c]) {
                units.emplace_back();
                units[p].ch[c] = units.size() - 1;
            }
            p = units[p].ch[c];
        }
        if (!units[p].info)
            units[p].info = WordInfo{0, {}};
        WordInfo &info = units[p].info.value();
        ++info.count;
        if (info.count <= limit)
            info.lines[info.count - 1] = line;
    }

    /**
     * Retrieves all words stored in the Trie along with their associated WordInfo.
     *
     * Performs a depth-first search of the Trie to collect all words and their
     * WordInfo data, including occurrence count and line numbers. Each word is
     * reconstructed from the path in the Trie and paired with its WordInfo.
     *
     * @return A vector of pairs containing each word and its WordInfo.
     */
    auto get_all(void) -> std::vector<std::pair<std::string, WordInfo>> {
        std::vector<std::pair<std::string, WordInfo>> result;

        std::string current;
        auto search = [&](size_t p, auto &&search) -> void {
            if (units[p].info) {
                WordInfo &info = units[p].info.value();
                result.emplace_back(current, info);
            }
            for (size_t i = 0; i < 26; ++i)
                if (units[p].ch[i]) {
                    current.push_back(i + 'a');
                    search(units[p].ch[i], search);
                    current.pop_back();
                }
        };

        search(root, search);
        return result;
    }
};

/**
 * Prints the statistics of words stored in a vector of pairs.
 *
 * This function takes a vector containing pairs of words and their associated
 * WordInfo, and prints the word statistics in a formatted table. The statistics
 * include the word, its occurrence count, and the line numbers where it appears.
 * The words are sorted by their occurrence count in descending order.
 *
 * @param wordsVector A vector of pairs, where each pair consists of a word
 *                    (std::string) and its corresponding WordInfo.
 * @details
 * This function first creates an index vector, then divides the words into buckets
 * based on their occurrence counts. The words in each bucket are sorted, and then
 * the words in the buckets are concatenated in descending order of occurrence
 * counts. Finally, the statistics are printed in a formatted table.
 */
static inline auto printStatis(const std::vector<std::pair<std::string, WordInfo>> &wordsVector) -> void {
    assert(!wordsVector.empty());

    std::vector<size_t> index(wordsVector.size());
    std::iota(index.begin(), index.end(), 0);

    constexpr size_t buckets_size = 1e3;

    std::vector<std::vector<size_t>> buckets(buckets_size);
    std::vector<size_t> residual;
    for (const auto i : index) {
        const WordInfo &info = wordsVector[i].second;
        if (info.count < buckets_size)
            buckets[info.count].push_back(i);
        else
            residual.push_back(i);
    }

    std::sort(residual.begin(), residual.end(), [&](const size_t a, const size_t b) -> bool {
        return wordsVector[a].second.count > wordsVector[b].second.count;
    });

    std::vector<size_t> result;
    result.insert(result.end(), residual.begin(), residual.end());
    for (size_t i = buckets_size - 1; i < buckets_size; --i)
        result.insert(result.end(), buckets[i].begin(), buckets[i].end());

    FileWriter writer(stdout);

    writer.put_string(std::format("WORD                 COUNT APPEARS-LINES\n"));
    for (const auto i : result) {
        const auto &p = wordsVector[i];
        const std::string &word = p.first;
        const WordInfo &info = p.second;

        writer.put_string(std::format("{:<20} {:<5} ", word.data(), info.count));
        writer.put_string(std::format("{}", info.lines.front()));
        for (size_t j = 1; j < std::min(info.lines.size(), info.count); ++j)
            writer.put_string(std::format(",{}", info.lines[j]));

        writer.put_char('\n');
    }
}

/**
 * The main entry point of the program.
 *
 * This function takes a filename as its argument, reads the file line by line,
 * and inserts each word into a Trie. The line number of each word is stored
 * in the Trie. Finally, the word statistics are printed in a formatted table.
 *
 * @param argc The number of arguments passed to the program.
 * @param argv The array of arguments passed to the program. The first element
 *             is the program name, and the second element is the filename.
 * @return 0 if the program runs successfully, 1 otherwise.
 */
int main(int argc, const char *argv[]) {
    if (argc != 2) {
        std::cerr
            << std::format("Usage: {} filename", argv[0])
            << std::endl;
        return 1;
    }

    std::FILE *fp = fopen(argv[1], "r");
    FileReader file_reader(fp);

    Trie trie;

    std::string word;
    while (word = file_reader.get_word(), word.length())
        trie.insert(word, file_reader.line);
    fclose(fp);

    printStatis(trie.get_all());
    return 0;
}
