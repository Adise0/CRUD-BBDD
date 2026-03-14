#pragma once

#include <pqxx/pqxx>

namespace CRUD {
class Postgre {
public:
  static std::optional<pqxx::connection> connection;

public:
  static void Connect();
  static void Disconnect();
};
} // namespace CRUD
