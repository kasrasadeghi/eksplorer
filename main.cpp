#include <ncurses.h>
#include <experimental/filesystem>
#include <iostream>
#include <sstream>
#include <algorithm>

using std::cout;
using std::endl;
using std::stringstream;
using std::ostringstream;
using std::begin;
using std::end;
using std::vector;
using std::ostream;
using std::string;
using namespace std::experimental::filesystem::v1;

struct printer {
  // ostream& operator <<(ostream& out) {
    // std::stringstream ss;
    // ss << out.rdbuf();
    // addstr(ss.str().c_str());
    // return out;
  // }
  template <typename T>
  printer& operator<<(const T& anything) {
    std::stringstream ss;
    ss << anything;
    addstr(ss.str().c_str());
    return *this;
  }

  template <typename T>
  printer& operator<<(T&& anything) {
    std::stringstream ss;
    ss << anything;
    addstr(ss.str().c_str());
    return *this;
  }
};

void print_files(const path& cwd, unsigned int cursorline) {
  printer p;
  directory_iterator iter(cwd);
  vector<directory_entry> entries;
  std::copy(begin(iter), end(iter), back_inserter(entries));

  if (cursorline == 0) attron(A_BOLD);
  p << cwd.string() << "\n";
  attroff(A_BOLD);
  
  for (unsigned int i = 0; i < entries.size(); ++i) {
    const auto& v = entries[i];
    if (i + 1 == entries.size())
      break;
    p << "├── ";
    if (cursorline - 1 == i)
      attron(A_BOLD);
    p << v.path().filename().string();
    attroff(A_BOLD);
    p << "\n";
  }
  p << "└── ";
  if (cursorline == entries.size())
    attron(A_BOLD);
  p << entries[entries.size() - 1].path().filename().string();
  attroff(A_BOLD);
  p << "\n";
}

int main(int argc, char** argv) {
  setlocale(LC_ALL,"");
  initscr();
  noecho();
  keypad(stdscr, TRUE);
  raw();
  bool done = false;
  unsigned int cursorline = 0;
  printer p;  
  path cwd(current_path()); // current working directory
  
  while (!done) {
    clear();
    move(0, 0);

    try {
      if (exists(cwd)) {
        if (is_regular_file(cwd)) { 
          p << cwd << " size is " << file_size(cwd) << '\n';
        } else if (is_directory(cwd)) {
          print_files(cwd, cursorline);
        } else {
          p << cwd << " exists, but is not a regular file or directory\n";
        }
      } else {
        p << cwd << " does not exist\n";
      }
    } catch (const filesystem_error& ex) {
      addstr(ex.what() + '\n');
    }
    
    int c = getch();

    move(15, 0);
    p << "input code: " << c << "\n";

    switch(c) {
    case 113: //q
      done = true;
      break;
    case 259: // arrow up
      p << "UP\n";
      if (cursorline != 0) --cursorline;
      break;
    case 258: // arrow down
      p << "DOWN\n";
      ++cursorline;
      break;
    case 260: // arrow left
      cwd = cwd.parent_path();
      p << cwd.string() << "\n";
      break;
    case 261: // arrow right
      path child = 
      break;
    }

    refresh();
  }

  endwin();
}
