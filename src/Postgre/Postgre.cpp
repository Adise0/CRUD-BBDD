#include "./Postgre.h"
#include <iostream>

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
} // namespace CRUD
