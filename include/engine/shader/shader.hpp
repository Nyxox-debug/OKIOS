#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class Shader {
public:
  Shader(const std::string &vertPath, const std::string &fragPath);
  ~Shader();

  void use() const;
  void setMat4(const std::string &name, const glm::mat4 &mat) const;
  void setFloat(const std::string &name, float value) const;

private:
  unsigned int program;

  int getUniformLocation(const std::string &name) const;
};
