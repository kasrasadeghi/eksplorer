#include <ncurses.h>
#include <unistd.h>
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
using std::size_t;
using namespace std::experimental::filesystem::v1;

struct printer {
  // printer& operator <<(const basic_ostream& out) {
    // std::stringstream ss;
    // ss << out.rdbuf();
    // addstr(ss.str().c_str());
    // return *this;
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

class User {
  vector<directory_entry> _entries;
  unsigned int _cursorline;

  void _reset_entries() {
    _entries.clear();
    directory_iterator iter(current_path());
    std::copy(begin(iter), end(iter), back_inserter(_entries));
    _reset_cursorline();
  }

  void _reset_cursorline() {
    _cursorline = 0;
  }
  
public:
  User(): _entries(), _cursorline() {
    _reset_entries();
  }
  
  void display() {
    printer p;
    
    if (_cursorline == 0) attron(A_BOLD);
    p << current_path().string() << "\n";
    attroff(A_BOLD);

    size_t size = _entries.size();
    
    for (unsigned int i = 0; i < size; ++i) {
      const auto& v = _entries[i];
      if (i + 1 == size)
        break;
      p << "├── ";
      if (_cursorline - 1 == i)
        attron(A_BOLD);
      p << v.path().filename().string();
      if (is_directory(v)) p << "/";
      attroff(A_BOLD);
      p << "\n";
    }
    p << "└── ";
    if (_cursorline == size)
      attron(A_BOLD);
    p << _entries[size - 1].path().filename().string();
    attroff(A_BOLD);
    p << "\n";
  }

  void folder_up() {
    chdir("..");
    _reset_entries();
    
  }

  void folder_down() {
    chdir(current().c_str());
    _reset_entries();
  }

  void cursor_up() {
    if (_cursorline != 0) --_cursorline;
  }

  void cursor_down() {
    if (_cursorline != _entries.size()) ++_cursorline;
  }

  string current() {
    if (_cursorline == 0) return ".";
    return _entries[_cursorline - 1].path().filename().string();
  }
};

int main(int argc, char** argv) {
  setlocale(LC_ALL,"");
  initscr();
  noecho();
  keypad(stdscr, TRUE);
  raw();
  bool done = false;
  printer p;  
  User user;
  
  while (!done) {
    clear();
    move(0, 0);

    try {
      user.display();
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
      user.cursor_up();
      break;
    case 258: // arrow down
      p << "DOWN\n";
      user.cursor_down();
      break;
    case 260: // arrow left
      user.folder_up();
      break;
    case 261: // arrow right
      user.folder_down();
      break;
    }

    refresh();
  }

  endwin();
}
