#include "./Postgre.h"
#include "../UI/UI.h"
#include <cstdlib>
#include <iostream>

namespace CRUD {

std::optional<pqxx::connection> Postgre::connection = std::nullopt;

void Postgre::Connect() {
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
}

void Postgre::Disconnect() {
  if (connection.has_value()) {
    connection->close();
    connection = std::nullopt;
    std::cout << "Disconnected from database" << std::endl;
  }
}

void Postgre::Read() {
  if (!connection.has_value()) {
    std::cerr << "Not connected to database" << std::endl;
    return;
  }
  try {
    pqxx::work transaction(*connection);
    pqxx::result result = transaction.exec(
        "SELECT id, (computer).name, (computer).n_of_users, (computer).is_active, (computer).type FROM computers");
    UI::ShowComputers(result);
  } catch (const std::exception &e) {
    std::cerr << "Read failed: " << e.what() << std::endl;
  }
  system("pause");
}

void Postgre::Create() {
  if (!connection.has_value()) {
    std::cerr << "Not connected to database" << std::endl;
    return;
  }
  UI::CreateComputer([&](std::string &errorMsg, const std::string &name,
                         const std::string &nOfUsers, const bool isActive,
                         const std::string &type) {
    try {
      pqxx::work transaction(*connection);
      transaction.exec("INSERT INTO computers (computer) VALUES (ROW(" + transaction.quote(name) +
                       ", " + nOfUsers + ", " + (isActive ? "true" : "false") + ", " +
                       transaction.quote(type) + "::computer_type))");
      transaction.commit();
      return true;
    } catch (const std::exception &e) {
      errorMsg = std::string("Create failed: ") + e.what();
      return false;
    }
  });
}

void Postgre::Delete() {
  if (!connection.has_value()) {
    std::cerr << "Not connected to database" << std::endl;
    return;
  }
  UI::DeleteComputer(
      [&](std::string &errorMsg, std::string &name, const std::string &id) {
        try {
          pqxx::work t(*connection);
          auto res = t.exec("SELECT (computer).name FROM computers WHERE id = " + id);
          if (res.empty()) return false;
          name = res[0]["name"].as<std::string>();
          return true;
        } catch (const std::exception &e) {
          errorMsg = std::string("Error: ") + e.what();
          return false;
        }
      },
      [&](std::string &errorMsg, const std::string &id) {
        try {
          pqxx::work t(*connection);
          t.exec("DELETE FROM computers WHERE id = " + id);
          t.commit();
          return true;
        } catch (const std::exception &e) {
          errorMsg = std::string("Error: ") + e.what();
          return false;
        }
      });
}

void Postgre::Update() {
  if (!connection.has_value()) {
    std::cerr << "Not connected to database" << std::endl;
    return;
  }
  UI::UpdateComputer(
      [&](std::string &errorMsg, const std::string &id, const std::string &name,
          const std::string &nOfUsers, const bool isActive, const std::string &type) {
        try {
          pqxx::work t(*connection);
          t.exec("UPDATE computers SET computer = ROW(" + t.quote(name) + ", " + nOfUsers + ", " +
                 (isActive ? "true" : "false") + ", " + t.quote(type) +
                 "::computer_type) WHERE id = " + id);
          t.commit();
          return true;
        } catch (const std::exception &e) {
          errorMsg = std::string("Error: ") + e.what();
          return false;
        }
      },
      connection.value());
}

} // namespace CRUD
