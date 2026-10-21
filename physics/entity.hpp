#include <string>

class Entity {
private:
  int id;
  std::string name;

public:
  Entity(int id, const std::string &name) : id(id), name(name) {}
  virtual ~Entity() = default;

  int getId() const { return id; }
  const std::string &getName() const { return name; }
};

/// pour avoir de nouvlell class "clase Update : virtual Entity {};"