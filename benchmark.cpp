#include <fstream>
#include <iostream>
#include <regex/sa_regex.hpp> // my_regex
#include <regex>
#include <string>

struct Timer {
  template <typename Callable>
  Timer(const std::string &label, Callable &&func) {
    std::cout << label << ": " << std::flush;

    // Start the timer
    auto start = std::chrono::high_resolution_clock::now();

    // Execute the callable
    std::invoke(std::forward<Callable>(func));

    // Stop the timer
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate and print the elapsed time
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << duration.count() << " microseconds" << std::endl;
  }
};

int main() {
  std::string filename = "../benchmarks/json.hpp";
  std::ifstream file(filename, std::ios::in | std::ios::binary);

  if (!file) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return 1;
  }

  // Move the file pointer to the end and get the file size
  file.seekg(0, std::ios::end);
  std::size_t fileSize = file.tellg();
  file.seekg(0, std::ios::beg);

  // Read the entire file into a string
  std::string fileContents(fileSize, '\0'); // Pre-allocate space
  file.read(&fileContents[0], fileSize);

  file.close();

  std::cout << "File Content size:\n" << fileContents.size() << std::endl;

  // std::string re = "([a-zA-z]+, )+(true(, )?)+true";
  std::string re = "([a-zA-z]+, )+(true(, )?)+true";
  // std::string re = "(([a-zA-z]+, )+(true(, )?)+true)|[a-z_]*<[a-g]+>";

  std::unique_ptr<sa_::Regex> engine;

  std::cout << "SA regex : " << '\n';

  Timer("Engine creation",
        [&engine, &re]() { engine = std::make_unique<sa_::Regex>(re); });

  bool res;
  std::vector<std::pair<size_t, size_t>> sa_matches;

  Timer("Match time", [&engine, &fileContents, &res, &sa_matches]() {
    res = engine->search(fileContents, &sa_matches);
  });

  std::cout << "Match result: " << (res ? "true" : "false") << std::endl;

  for (auto match : sa_matches) {
    std::cout << " match : " << fileContents.substr(match.first, match.second)
              << '\n';
  }

  std::cout << "STD regex : " << '\n';

  std::unique_ptr<std::regex> pattern;

  Timer("Engine creation",
        [&pattern, &re]() { pattern = std::make_unique<std::regex>(re); });

  std::smatch matches;
  Timer("Match time", [&pattern, &fileContents, &matches]() {
    std::regex_search(fileContents, matches, *pattern);
  });

  for (auto match : matches) {
    std::cout << " match : " << match << '\n';
  }

  return 0;
}