#include <ncurses.h>
#include <experimental/filesystem>
#include <iostream>
#include <sstream>
#include <algorithm>

using std::cout;
using std::endl;
using std::wstringstream;
using std::ostringstream;
using std::begin;
using std::end;
using std::vector;
using namespace std::experimental::filesystem::v1;

ostringstream& print_entries(const directory_iterator& iter, ostringstream& out) {
  vector<directory_entry> entries;
  std::copy(begin(iter), end(iter), back_inserter(entries));
  
  for (unsigned int i = 0; i < entries.size(); ++i) {
    const auto& v = entries[i];
    if (i + 1 == entries.size())
      break;
    out << "├── " << v.path().filename() << endl;
  }
  out << "└── " << entries[entries.size() - 1].path().filename() << endl;
  return out;
}

int main(int argc, char** argv) {
  setlocale(LC_ALL,"");
  initscr();

  path cwd(current_path());

  try {
    ostringstream out;
    if (exists(cwd)) {
      if (is_regular_file(cwd)) {
        
        out << cwd << " size is " << file_size(cwd) << '\n';
        
      } else if (is_directory(cwd)) {
        out << cwd << "\n";
        print_entries(directory_iterator(cwd), out);
      } else {
        out << cwd << " exists, but is not a regular file or directory\n";
      }
    }
    else {
      out << cwd << " does not exist\n";
    }
    addstr(out.str().c_str());
    
  } catch (const filesystem_error& ex) {
    addstr(ex.what() + '\n');
  }
  
  refresh();
  getch();
  endwin();
}
