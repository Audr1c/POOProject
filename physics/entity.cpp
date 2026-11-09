#include "entity.hpp"

int Entity::current_ids = 0;

Entity::Entity(const std::string &name) : id(current_ids), name(name) {
  current_ids++;
}

Entity::Entity(int custom_id, const std::string &name)
    : id(custom_id), name(name) {}

int Entity::getId() const { return id; }

const std::string &Entity::getName() const { return name; }
