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

void print_entries(const directory_iterator& iter, int cursorline) {
  vector<directory_entry> entries;
  std::copy(begin(iter), end(iter), back_inserter(entries));
  
  p << cwd << "\n";
  
  for (unsigned int i = 0; i < entries.size(); ++i) {
    const auto& v = entries[i];
    if (i + 1 == entries.size())
      break;
    printer() << "├── ";
    if (cursorline)
    attron(A_BOLD);
    printer() << v.path().filename();
    attroff(A_BOLD);
    printer() << "\n";
  }
  printer() << "└── " << entries[entries.size() - 1].path().filename() << "\n";
}

void highlight(int cursorline) {
  if (cursorline == 0) move(cursorline, 0);
  else                 move(cursorline, 3);
  char str[100];
  innstr(str, 100);
  move(15, 0);
  printer() << cursorline << ": ";
  printer() << str << "\n";
}

int main(int argc, char** argv) {
  setlocale(LC_ALL,"");
  initscr();
  noecho();
  keypad(stdscr, TRUE);
  raw();
  bool done = false;
  int cursorline = 0;
  printer p;
  
  while (!done) {
    move(0, 0);
    path cwd(current_path());

    try {
      if (exists(cwd)) {
        if (is_regular_file(cwd)) { 
          p << cwd << " size is " << file_size(cwd) << '\n';
        } else if (is_directory(cwd)) {
          print_explorer(directory_iterator(cwd), cursorline);
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
    p << "input code: " << c << "\n";

    switch(c) {
    case 113: //q
      done = true;
      break;
    case 259:
      p << "UP\n";
      if (cursorline != 0) --cursorline;
      break;
    case 258:
      p << "DOWN\n";
      ++cursorline;
      break;
    }

    highlight(cursorline);

    refresh();
  }

  endwin();
}
