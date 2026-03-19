#include "engine/shader/shader.hpp"
#include "engine/shader/shader_utils.hpp"
#include <glad/glad.h>

Shader::Shader(const std::string &vertPath, const std::string &fragPath) {
  std::string vertexSourceStr = loadShaderFile(vertPath);
  std::string fragmentSourceStr = loadShaderFile(fragPath);

  if (vertexSourceStr.empty() || fragmentSourceStr.empty()) {
    std::cerr << "Failed to load shader files." << std::endl;
  }

  const char *vertexSource = vertexSourceStr.c_str();
  const char *fragmentSource = fragmentSourceStr.c_str();

  unsigned int vertShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertShader, 1, &vertexSource, NULL);
  glCompileShader(vertShader);

  unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragShader, 1, &fragmentSource, NULL);
  glCompileShader(fragShader);

  checkShaderCompile(vertShader, "Vertex");
  checkShaderCompile(fragShader, "Fragment");

  program = glCreateProgram();
  glAttachShader(program, vertShader);
  glAttachShader(program, fragShader);
  glLinkProgram(program);
  checkProgramLink(program);

  glDeleteShader(vertShader);
  glDeleteShader(fragShader);
}

int Shader::getUniformLocation(const std::string &name) const {
    return glGetUniformLocation(program, name.c_str());
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(getUniformLocation(name), value);
}

Shader::~Shader() { glDeleteProgram(program); }

void Shader::use() const { glUseProgram(program); }
