#include <string>
class Shader {
public:
  Shader(const std::string &vertPath, const std::string &fragPath);
  ~Shader();

  void use() const;

private:
  unsigned int program;
};
