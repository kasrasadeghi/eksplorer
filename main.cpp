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
    std::sort(_entries.begin(), _entries.end());
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
    move(1, 0);
    printer p;

    try {
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
      if (is_directory(_entries[size - 1])) p << "/";
      attroff(A_BOLD);
      p << "\n";
    } catch(const filesystem_error& err) {
      addstr(err.what() + '\n');
    }
    
  }

  void folder_up() {
    path prev_wd = current_path();
    chdir("..");
    _reset_entries();
    size_t size = _entries.size();
    for (size_t i = 0; i < size; ++i) {
      if (_entries[i] == prev_wd) {
        _cursorline = i + 1;
        break;
      }
    }
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
  initscr();             // initialize ncurses
  noecho();              // don't actually write the characters received from input
  keypad(stdscr, TRUE);  // special characters like F1
  raw();                 // print things immediately
  curs_set(0);           // turn off cursor
  bool done = false;
  printer p;  
  User user; 
  int c = 0;

  user.display();

  do {
    c = getch();
    clear();
    
    move(0, 0);
    p << "input code: " << c << ", ";
    
    move(0, 17);
    switch(c) {
    case 113: //q
      done = true;
      break;

    case 115: // s - search query
      p << "SEARCH QUERY (unimplemented)\n";
      break;
      
    case 10:  // enter - search close
      p << "SEARCH CLOSE (unimplemented)\n";
      break;
      
    case 102: // f - history forward
      p << "HISTORY FORWARD (unimplemented)\n";
      break;
      
    case 98:  // b - history back
      p << "HISTORY BACK (unimplemented)\n";
      break;
      
      // navigation
    case 104: // h
    case 260: // arrow left
      p << "FOLDER UP\n";
      user.folder_up();
      break;

    case 106: // j
    case 258: // arrow down
      p << "DOWN\n";
      user.cursor_down();
      break;
      
    case 107: // k
    case 259: // arrow up
      p << "UP\n";
      user.cursor_up();
      break;
      
      
    case 108:  // l
    case 261: // arrow right
      p << "FOLDER DOWN\n";
      user.folder_down();
      break;
    }
    
    user.display();
    refresh();
    
  } while (!done);

  endwin();
}
