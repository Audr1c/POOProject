#pragma once
#include <string>

class Entity {
private:
  static int current_ids;
  int id;
  std::string name;

public:
  Entity(const std::string &name);
  // should not be used
  Entity(int custom_id, const std::string &name);
  virtual ~Entity() = default;

  int getId() const;
  const std::string &getName() const;
};

/// pour avoir de nouvlell class "clase Update : virtual Entity {};"
