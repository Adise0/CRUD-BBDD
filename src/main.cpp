#include "Postgre/Postgre.h"

#include <cstdio>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

using namespace CRUD;
using namespace ftxui;


int GetInput() {
  system("cls");
  auto screen = ScreenInteractive::TerminalOutput();

  std::vector<std::string> options = {"Create Computer", "Read Computers", "Update Computer",
                                      "Delete Computer", "Exit"};

  int selected = 0;

  auto menu = Menu(&options, &selected);

  auto component = Renderer(menu, [&] {
    return vbox({
        text("CRUD - Classroom Computers") | bold | hcenter,
        separator(),
        menu->Render(),
    });
  });

  auto loop = CatchEvent(component, [&](Event event) {
    if (event == Event::Return) {
      screen.ExitLoopClosure()();
      return true;
    }
    return false;
  });

  screen.Loop(loop);
  return selected;
}



int main() {
  printf("Connecting to database...\n");
  Postgre::Connect();

  bool running = Postgre::connection.has_value();
  while (running) {
    int selected = GetInput();

    switch (selected) {
    case 0:
      Postgre::Create();
      break;
    case 1:
      Postgre::Read();
      break;
    case 2:
      Postgre::Update();
      break;
    case 3:
      Postgre::Delete();
      break;
    case 4:
      Postgre::Disconnect();
      running = false;
      break;
    default:
      printf("Invalid selection\n");
    }
  }

  printf("Bye!\n");
  return 0;
}
