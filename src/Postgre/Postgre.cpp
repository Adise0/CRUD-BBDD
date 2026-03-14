#include "./Postgre.h"
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

} // namespace CRUD
