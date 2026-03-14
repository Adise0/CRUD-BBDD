#pragma once

#include <pqxx/pqxx>

namespace CRUD {
class Postgre {
public:
  static std::optional<pqxx::connection> connection;

public:
  static void Connect();
  static void Disconnect();

  static void Create();
  static void Read();
  static void Update();
  static void Delete();
};
} // namespace CRUD
