#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include <ftxui/screen/screen.hpp>
#include <functional>
#include <pqxx/pqxx>


namespace CRUD {

using CreateCallback =
    std::function<bool(std::string &errorMsg, const std::string &name, const std::string &nOfUsers,
                       const bool isActive, const std::string &type)>;
using SelectCallback =
    std::function<bool(std::string &errorMsg, std::string &name, const std::string &id)>;

using DeleteCallback = std::function<bool(std::string &errorMsg, const std::string &id)>;

using UpdateCallback =
    std::function<bool(std::string &errorMsg, const std::string &id, const std::string &name,
                       const std::string &nOfUsers, const bool isActive, const std::string &type)>;

class UI {
public:
  static void ShowComputers(const pqxx::result &result);
  static void CreateComputer(CreateCallback create);
  static void DeleteComputer(SelectCallback select, DeleteCallback delete_);
  static void UpdateComputer(UpdateCallback update, pqxx::connection &connection);
}; // namespace CRUD

} // namespace CRUD
