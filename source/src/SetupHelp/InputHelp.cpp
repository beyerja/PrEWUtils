#include <SetupHelp/InputHelp.h>

// Standard library
#include <dirent.h>
#include <regex>

namespace PrEWUtils {
namespace SetupHelp {

//------------------------------------------------------------------------------

std::vector<std::string>
InputHelp::regex_search(const std::string &dir, const std::string &regex_name) {
  /** Search the given directory for files that fit the given regular
   *expression. Returns a vector of the full file paths.
   **/

  // Find all files in the given directory, copy-pasted from:
  // https://www.linuxquestions.org/questions/programming-9/c-list-files-in-directory-379323/
  DIR *dp;
  struct dirent *dirp;
  if ((dp = opendir(dir.c_str())) == NULL) {
    throw std::invalid_argument("Error(" + std::to_string(errno) + ") opening " + dir);
  }

  std::vector<std::string> all_files {};
  while ((dirp = readdir(dp)) != NULL) {
    all_files.push_back(std::string(dirp->d_name));
  }
  closedir(dp);

  // Try to find files that fit the given regex name
  std::vector<std::string> fitting_files {};
  std::regex regular_expression(regex_name);
  for (auto const &file_name : all_files) {
    if (regex_match(file_name, regular_expression)) {
      fitting_files.push_back(dir + "/" + file_name);
    }
  }
  
  return fitting_files;
}

//------------------------------------------------------------------------------

} // namespace SetupHelp
} // namespace PrEWUtils