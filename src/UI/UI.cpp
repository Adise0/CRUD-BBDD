#include "./UI.h"

namespace CRUD {
using namespace ftxui;

void UI::ShowComputers(const pqxx::result &result) {
  std::vector<std::vector<std::string>> table_data;
  table_data.push_back({"ID", "Name", "Users", "Active", "Type"});

  for (const auto &row : result) {
    table_data.push_back({
        row["id"].as<std::string>(),
        row["name"].as<std::string>(),
        row["n_of_users"].as<std::string>(),
        row["is_active"].as<bool>() ? "Yes" : "No",
        row["type"].as<std::string>(),
    });
  }

  auto table = Table(table_data);
  table.SelectAll().Border(LIGHT);
  table.SelectRow(0).Decorate(bold);
  table.SelectAll().DecorateCells([](Element e) { return hbox({text(" "), e, text(" ")}); });
  table.SelectRow(0).SeparatorVertical(LIGHT);
  table.SelectRow(0).Border(DOUBLE);

  auto document = vbox({
      text("READ - Classroom Computers") | bold | hcenter,
      separator(),
      table.Render(),
  });

  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);
  system("cls");
  screen.Print();
}

void UI::CreateComputer(CreateCallback create) {
  auto screen = ScreenInteractive::TerminalOutput();

  std::string name;
  std::string n_of_users;
  int is_active = 1;
  int type_selected = 0;
  std::vector<std::string> types = {"WORKSTATION", "GAMING", "LAPTOP"};
  std::vector<std::string> active_options = {"Inactive", "Active"};
  std::string error_msg;
  int step = 0;

  auto input_name = Input(&name, "Computer name");
  auto input_users = Input(&n_of_users, "Number of users");
  auto toggle_active = Menu(&active_options, &is_active);
  auto menu_type = Menu(&types, &type_selected);

  auto form = Container::Tab(
      {
          input_name,
          input_users,
          toggle_active,
          menu_type,
      },
      &step);

  auto component = Renderer(form, [&] {
    switch (step) {
    case 0:
      return vbox({
          text("Create Computer") | bold | hcenter,
          separator(),
          text("Step 1/4 - Name") | dim,
          hbox(text("Name: "), input_name->Render()),
          text(error_msg) | color(Color::Red),
          text("Press Enter to continue, Escape to cancel") | dim | hcenter,
      });
    case 1:
      return vbox({
          text("Create Computer") | bold | hcenter,
          separator(),
          text("Step 2/4 - Number of users") | dim,
          hbox(text("Users: "), input_users->Render()),
          text(error_msg) | color(Color::Red),
          text("Press Enter to continue, Escape to go back") | dim | hcenter,
      });
    case 2:
      return vbox({
          text("Create Computer") | bold | hcenter,
          separator(),
          text("Step 3/4 - Active status") | dim,
          toggle_active->Render(),
          text("Press Enter to continue, Escape to go back") | dim | hcenter,
      });
    case 3:
      return vbox({
          text("Create Computer") | bold | hcenter,
          separator(),
          text("Step 4/4 - Type") | dim,
          menu_type->Render(),
          text(error_msg) | color(Color::Red),
          text("Press Enter to confirm, Escape to go back") | dim | hcenter,
      });
    default:
      return text("");
    }
  });

  auto loop = CatchEvent(component, [&](Event event) {
    if (event == Event::Escape) {
      if (step == 0) {
        screen.ExitLoopClosure()();
        return true;
      }
      error_msg = "";
      step--;
      return true;
    }
    if (event == Event::Return) {
      error_msg = "";
      switch (step) {
      case 0:
        if (name.empty()) {
          error_msg = "Name cannot be empty!";
          return true;
        }
        step++;
        return true;
      case 1:
        try {
          std::stoi(n_of_users);
        } catch (...) {
          error_msg = "Must be a valid number!";
          return true;
        }
        step++;
        return true;
      case 2:
        step++;
        return true;
      case 3:
        {
          std::string error;
          bool success = create(error, name, n_of_users, is_active, types[type_selected]);
          if (success) {
            screen.ExitLoopClosure()();
          } else {
            error_msg = error;
          }
          return true;
        }
      }
    }
    return false;
  });

  system("cls");
  screen.Loop(loop);
}

void UI::DeleteComputer(SelectCallback select, DeleteCallback delete_) {
  auto screen = ScreenInteractive::TerminalOutput();

  std::string id_input;
  std::string error_msg;
  std::string confirm_msg;
  int step = 0;

  auto input_id = Input(&id_input, "Computer ID");

  auto component = Renderer(input_id, [&] {
    switch (step) {
    case 0:
      return vbox({
          text("Delete Computer") | bold | hcenter,
          separator(),
          hbox(text("Enter ID: "), input_id->Render()),
          text(error_msg) | color(Color::Red),
          text("Press Enter to continue, Escape to cancel") | dim | hcenter,
      });
    case 1:
      return vbox({
          text("Delete Computer") | bold | hcenter,
          separator(),
          text(confirm_msg) | color(Color::Yellow),
          text("Press Enter to confirm, Escape to go back") | dim | hcenter,
      });
    default:
      return text("");
    }
  });

  auto loop = CatchEvent(component, [&](Event event) {
    if (event == Event::Escape) {
      if (step == 0) {
        screen.ExitLoopClosure()();
        return true;
      }
      step--;
      error_msg = "";
      confirm_msg = "";
      return true;
    }
    if (event == Event::Return) {
      error_msg = "";
      switch (step) {
      case 0:
        {
          try {
            std::stoi(id_input);
          } catch (...) {
            error_msg = "Must be a valid ID!";
            return true;
          }
          std::string name;
          bool exists = select(error_msg, name, id_input);
          if (!error_msg.empty()) return true;
          if (!exists) {
            error_msg = "No computer found with that ID!";
            return true;
          }
          confirm_msg = "Delete \"" + name + "\" (ID " + id_input + ")?";
          step++;
          return true;
        }
      case 1:
        {
          bool success = delete_(error_msg, id_input);
          if (success) {
            screen.ExitLoopClosure()();
          } else {
            error_msg = "Failed to delete: " + error_msg;
          }
          return true;
        }
      }
    }
    return false;
  });

  system("cls");
  screen.Loop(loop);
}

void UI::UpdateComputer(UpdateCallback update, pqxx::connection &connection) {
  auto screen = ScreenInteractive::TerminalOutput();

  std::string id_input;
  std::string name;
  std::string n_of_users;
  int is_active = 1;
  int type_selected = 0;
  std::vector<std::string> types = {"WORKSTATION", "GAMING", "LAPTOP"};
  std::vector<std::string> active_options = {"Inactive", "Active"};
  std::string error_msg;
  int step = 0;

  auto input_id = Input(&id_input, "Computer ID");
  auto input_name = Input(&name, "Computer name");
  auto input_users = Input(&n_of_users, "Number of users");
  auto menu_active = Menu(&active_options, &is_active);
  auto menu_type = Menu(&types, &type_selected);

  auto form = Container::Tab(
      {
          input_id,
          input_name,
          input_users,
          menu_active,
          menu_type,
      },
      &step);

  auto component = Renderer(form, [&] {
    switch (step) {
    case 0:
      return vbox({
          text("Update Computer") | bold | hcenter,
          separator(),
          hbox(text("Enter ID: "), input_id->Render()),
          text(error_msg) | color(Color::Red),
          text("Press Enter to continue, Escape to cancel") | dim | hcenter,
      });
    case 1:
      return vbox({
          text("Update Computer") | bold | hcenter,
          separator(),
          text("Step 1/4 - Name") | dim,
          hbox(text("Name: "), input_name->Render()),
          text(error_msg) | color(Color::Red),
          text("Press Enter to continue, Escape to go back") | dim | hcenter,
      });
    case 2:
      return vbox({
          text("Update Computer") | bold | hcenter,
          separator(),
          text("Step 2/4 - Number of users") | dim,
          hbox(text("Users: "), input_users->Render()),
          text(error_msg) | color(Color::Red),
          text("Press Enter to continue, Escape to go back") | dim | hcenter,
      });
    case 3:
      return vbox({
          text("Update Computer") | bold | hcenter,
          separator(),
          text("Step 3/4 - Active status") | dim,
          menu_active->Render(),
          text("Press Enter to continue, Escape to go back") | dim | hcenter,
      });
    case 4:
      return vbox({
          text("Update Computer") | bold | hcenter,
          separator(),
          text("Step 4/4 - Type") | dim,
          menu_type->Render(),
          text(error_msg) | color(Color::Red),
          text("Press Enter to confirm, Escape to go back") | dim | hcenter,
      });
    default:
      return text("");
    }
  });

  auto loop = CatchEvent(component, [&](Event event) {
    if (event == Event::Escape) {
      if (step == 0) {
        screen.ExitLoopClosure()();
        return true;
      }
      error_msg = "";
      step--;
      return true;
    }
    if (event == Event::Return) {
      error_msg = "";
      switch (step) {
      case 0:
        {
          try {
            std::stoi(id_input);
          } catch (...) {
            error_msg = "Must be a valid ID!";
            return true;
          }
          try {
            pqxx::work t(connection);
            auto res = t.exec("SELECT (computer).name, (computer).n_of_users, "
                              "(computer).is_active, (computer).type "
                              "FROM computers WHERE id = " +
                              id_input);
            if (res.empty()) {
              error_msg = "No computer found with that ID!";
              return true;
            }
            name = res[0]["name"].as<std::string>();
            n_of_users = res[0]["n_of_users"].as<std::string>();
            is_active = res[0]["is_active"].as<bool>() ? 1 : 0;
            std::string type = res[0]["type"].as<std::string>();
            auto it = std::find(types.begin(), types.end(), type);
            if (it != types.end()) type_selected = std::distance(types.begin(), it);
            step++;
          } catch (const std::exception &e) {
            error_msg = std::string("Error: ") + e.what();
          }
          return true;
        }
      case 1:
        if (name.empty()) {
          error_msg = "Name cannot be empty!";
          return true;
        }
        step++;
        return true;
      case 2:
        try {
          std::stoi(n_of_users);
        } catch (...) {
          error_msg = "Must be a valid number!";
          return true;
        }
        step++;
        return true;
      case 3:
        step++;
        return true;
      case 4:
        {
          bool success =
              update(error_msg, id_input, name, n_of_users, is_active, types[type_selected]);
          if (success) {
            screen.ExitLoopClosure()();
          } else {
            error_msg = error_msg.empty() ? "Failed to update computer" : error_msg;
          }
          return true;
        }
      }
    }
    return false;
  });

  system("cls");
  screen.Loop(loop);
}

} // namespace CRUD
