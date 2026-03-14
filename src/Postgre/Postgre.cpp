#include "./Postgre.h"
#include <cstdlib>
#include <iostream>

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include <ftxui/screen/screen.hpp>

namespace CRUD {

using namespace ftxui;

std::optional<pqxx::connection> Postgre::connection = std::nullopt;



void Postgre::Connect() {
  // #region Connect
  try {
    pqxx::connection conn("host=localhost "
                          "port=5432 "
                          "dbname=classroom "
                          "user=crud "
                          "password=1234");

    if (conn.is_open()) {
      std::cout << "Connected to: " << conn.dbname() << std::endl;
      connection = std::move(conn);
    }

  } catch (const std::exception &e) {
    std::cerr << "Connection failed: " << e.what() << std::endl;
  }
  // #endregion
}

void Postgre::Disconnect() {
  // #region Disconnect
  if (connection.has_value()) {
    connection->close();
    connection = std::nullopt;
    std::cout << "Disconnected from database" << std::endl;
  }
  // #endregion
}


void Postgre::Read() {
  // #region Read
  if (!connection.has_value()) {
    std::cerr << "Not connected to database" << std::endl;
    return;
  }

  try {
    pqxx::work transaction(*connection);
    pqxx::result res = transaction.exec(
        "SELECT id, (computer).name, (computer).n_of_users, (computer).is_active, (computer).type FROM computers");

    std::vector<std::vector<std::string>> table_data;
    table_data.push_back({"ID", "Name", "Users", "Active", "Type"});

    for (const auto &row : res) {
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

  } catch (const std::exception &e) {
    std::cerr << "Read failed: " << e.what() << std::endl;
  }
  system("pause");
  // #endregion
}


void Postgre::Create() {
  // #region Create
  if (!connection.has_value()) {
    std::cerr << "Not connected to database" << std::endl;
    return;
  }
  if (!connection.has_value()) {
    std::cerr << "Not connected to database" << std::endl;
    return;
  }

  auto screen = ScreenInteractive::TerminalOutput();

  std::string name;
  std::string n_of_users;
  int is_active = 1;
  int type_selected = 0;
  std::vector<std::string> types = {"WORKSTATION", "GAMING", "LAPTOP"};
  std::vector<std::string> active_options = {"Inactive", "Active"};
  std::string error_msg;

  int step = 0; // 0=name, 1=users, 2=active, 3=type, 4=confirm

  auto input_name = Input(&name, "Computer name");
  auto input_users = Input(&n_of_users, "Number of users");
  auto toggle_active = Menu(&active_options, &is_active);
  auto menu_type = Menu(&types, &type_selected);

  auto component = Renderer([&] {
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
          text("Press Enter to confirm, Escape to go back") | dim | hcenter,
      });
    default:
      return text("");
    }
  });

  // Wire up the active input component per step
  auto form = Container::Tab(
      {
          input_name,
          input_users,
          toggle_active,
          menu_type,
      },
      &step);

  auto loop = CatchEvent(form, [&](Event event) {
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
        try {
          pqxx::work transaction(*connection);
          transaction.exec("INSERT INTO computers (computer) VALUES (ROW(" +
                           transaction.quote(name) + ", " + n_of_users + ", " +
                           (is_active ? "true" : "false") + ", " +
                           transaction.quote(types[type_selected]) + "::computer_type))");
          transaction.commit();
          screen.ExitLoopClosure()();
        } catch (const std::exception &e) {
          error_msg = std::string("Create failed: ") + e.what();
        }
        return true;
      }
    }
    return false;
  });
  system("cls");
  screen.Loop(loop);
  // #endregion
}
void Postgre::Delete() {
  if (!connection.has_value()) {
    std::cerr << "Not connected to database" << std::endl;
    return;
  }

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
          try {
            pqxx::work t(*connection);
            auto res = t.exec("SELECT (computer).name FROM computers WHERE id = " + id_input);
            if (res.empty()) {
              error_msg = "No computer found with that ID!";
              return true;
            }
            confirm_msg =
                "Delete \"" + res[0]["name"].as<std::string>() + "\" (ID " + id_input + ")?";
            step++;
          } catch (const std::exception &e) {
            error_msg = std::string("Error: ") + e.what();
          }
          return true;
        }
      case 1:
        {
          try {
            pqxx::work t(*connection);
            t.exec("DELETE FROM computers WHERE id = " + id_input);
            t.commit();
            screen.ExitLoopClosure()();
          } catch (const std::exception &e) {
            error_msg = std::string("Error: ") + e.what();
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

void Postgre::Update() {
  if (!connection.has_value()) {
    std::cerr << "Not connected to database" << std::endl;
    return;
  }

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
            pqxx::work t(*connection);
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
          try {
            pqxx::work t(*connection);
            t.exec("UPDATE computers SET computer = ROW(" + t.quote(name) + ", " + n_of_users +
                   ", " + (is_active ? "true" : "false") + ", " + t.quote(types[type_selected]) +
                   "::computer_type) "
                   "WHERE id = " +
                   id_input);
            t.commit();
            screen.ExitLoopClosure()();
          } catch (const std::exception &e) {
            error_msg = std::string("Error: ") + e.what();
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
