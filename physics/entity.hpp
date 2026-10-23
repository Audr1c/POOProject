#pragma once
#include <string>

class Entity {
private:
  static int current_ids;
  int id;
  std::string name;

public:
  Entity(const std::string &name) : id(current_ids), name(name) {
    current_ids++;
  }
  // should not be used
  Entity(int custom_id, const std::string &name) : id(custom_id), name(name) {}
  virtual ~Entity() = default;

  int getId() const { return id; }
  const std::string &getName() const { return name; }
};

/// pour avoir de nouvlell class "clase Update : virtual Entity {};"