#include "Kit/Quad.hpp"

#include "Kit/IncOpenGL.hpp"
#include "Kit/Shader.hpp"
#include "Kit/Program.hpp"

unsigned int kit::Quad::m_instanceCount = 0;
uint32_t kit::Quad::m_glVertexIndices = 0;
uint32_t kit::Quad::m_glVertexArray = 0;
uint32_t kit::Quad::m_glVertexBuffer = 0;
kit::Program::Ptr kit::Quad::m_program = nullptr;
kit::Program::Ptr kit::Quad::m_ProgramTextured = nullptr;


static const std::vector<GLushort> indexData { 0, 3, 2, 0, 2, 1 };

static const std::vector<float> vertexData {
  0.0,  0.0,  0.0,  0.0,  1.0, // Bottomleft
  1.0,  0.0,  0.0,  1.0,  1.0, // Bottomright
  1.0,  1.0,  0.0,  1.0,  0.0, // Topright
  0.0,  1.0,  0.0,  0.0,  0.0  // Topleft
};

static const char * vertexSource
="#version 410 core\n\
  \n\
  layout (location = 0) in vec3 in_position;\n\
  layout (location = 1) in vec2 in_uv;\n\
  layout (location = 2) out vec2 out_uv;\n\
  \n\
  uniform vec2 uniform_position;\n\
  uniform vec2 uniform_size;\n\
  \n\
  void main()\n\
  {\n\
    vec2 finalpos = uniform_position + (in_position.xy * uniform_size);\n\
    gl_Position = vec4(finalpos, in_position.z, 1.0);\n\
    gl_Position.xy *= 2.0;\n\
    gl_Position.y = 1.0 - gl_Position.y;\n\
    gl_Position.x -= 1.0;\n\
    out_uv = in_uv;\n\
    //out_uv.y = 1.0 - out_uv.y;\n\
  }\n";

static const char * pixelSource
="#version 410 core\n\
  \n\
  uniform vec4 uniform_color;\n\
  \n\
  out vec4 out_color;\n\
  \n\
  void main()\n\
  {\n\
    out_color = uniform_color;\n\
  }\n";
  
static const char * pixelSourceTextured
="#version 410 core\n\
  \n\
  layout (location = 2) in vec2 in_uv;\n\
  \n\
  uniform vec4 uniform_color;\n\
  uniform sampler2D uniform_texture;\n\
  \n\
  out vec4 out_color;\n\
  \n\
  void main()\n\
  {\n\
    out_color = texture(uniform_texture, in_uv) * uniform_color;\n\
  }\n";

kit::Quad::Quad(glm::vec2 position, glm::vec2 size, glm::vec4  color, kit::Texture::WPtr texture)
{
  
  kit::Quad::m_instanceCount += 1;
  if(kit::Quad::m_instanceCount == 1)
  {
    this->allocateShared();
  }
  
  this->m_position = position;
  this->m_size = size;
  this->m_color = color;
  this->m_texture = texture;
  
  this->m_blending = true;
}

kit::Quad::~Quad()
{
  kit::Quad::m_instanceCount -= 1;
  if(kit::Quad::m_instanceCount < 1)
  {
    this->releaseShared();
  }
}

void kit::Quad::PrepareProgram(kit::Program::Ptr customprogram)
{
  
  if(customprogram != nullptr)
  {
    customprogram->use();
    customprogram->setUniform2f("uniform_size", this->m_size);
    customprogram->setUniform2f("uniform_position", this->m_position);
  }
  else if(!this->m_texture.lock())
  {
      if (this->m_blending)
      {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      }
      else
      {
        glDisable(GL_BLEND);
      }
      glDisable(GL_DEPTH_TEST);
      glDepthMask(GL_FALSE);

      kit::Quad::m_program->use();
      kit::Quad::m_program->setUniform4f("uniform_color", kit::srgbDec(this->m_color));
      kit::Quad::m_program->setUniform2f("uniform_size", this->m_size);
      kit::Quad::m_program->setUniform2f("uniform_position", this->m_position);
  }
  else
  {
    if (this->m_blending)
    {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
      glDisable(GL_BLEND);
    }
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    kit::Quad::m_ProgramTextured->use();
    kit::Quad::m_ProgramTextured->setUniformTexture("uniform_texture", this->m_texture);
    kit::Quad::m_ProgramTextured->setUniform4f("uniform_color", kit::srgbDec(this->m_color));
    kit::Quad::m_ProgramTextured->setUniform2f("uniform_size", this->m_size);
    kit::Quad::m_ProgramTextured->setUniform2f("uniform_position", this->m_position);
  }
}

void kit::Quad::renderGeometry()
{
  
  glBindVertexArray(kit::Quad::m_glVertexArray);
  glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
}

void kit::Quad::renderAltGeometry()
{
  
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void kit::Quad::render(kit::Program::Ptr customprogram)
{
  this->PrepareProgram(customprogram);
  kit::Quad::renderGeometry();
}


kit::Quad::Ptr kit::Quad::create(glm::vec2 position, glm::vec2 size, glm::vec4 color, kit::Texture::WPtr texture)
{
  //return kit::Quad::Ptr(new kit::Quad(position, size, color, texture));
  return std::make_shared<kit::Quad>(position, size, color, texture);
}

glm::vec2 const & kit::Quad::getPosition()
{
  return this->m_position;
}

glm::vec2 const & kit::Quad::getSize()
{
  return this->m_size;
}

void kit::Quad::setPosition(glm::vec2 position)
{
  this->m_position = position;
}

void kit::Quad::setSize(glm::vec2 size)
{
  this->m_size = size;
}

void kit::Quad::setColor(glm::vec4 color)
{
  this->m_color = (color);
}

void kit::Quad::setTexture(kit::Texture::WPtr texture)
{
  this->m_texture = texture;
}

void kit::Quad::allocateShared()
{
  
  // Generate and bind our Vertex Array Object
  glGenVertexArrays(1, &kit::Quad::m_glVertexArray);
  glBindVertexArray(kit::Quad::m_glVertexArray);

  // Generate our Vertex Index Buffer Object
  glGenBuffers(1, &kit::Quad::m_glVertexIndices);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kit::Quad::m_glVertexIndices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(GLushort), &indexData[0], GL_STATIC_DRAW);

  // Generate our Vertex Buffer Object
  glGenBuffers(1, &kit::Quad::m_glVertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, kit::Quad::m_glVertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), &vertexData[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*) (sizeof(float) * 3));
  
  // allocate our programs!
  auto vertexShader = kit::Shader::create(Shader::Type::Vertex);
  vertexShader->sourceFromString(vertexSource);
  vertexShader->compile();

  auto pixelShader = kit::Shader::create(Shader::Type::Fragment);
  pixelShader->sourceFromString(pixelSource);
  pixelShader->compile();
  
  auto pixelShaderTextured = kit::Shader::create(Shader::Type::Fragment);
  pixelShaderTextured->sourceFromString(pixelSourceTextured);
  pixelShaderTextured->compile();

  kit::Quad::m_program = kit::Program::create();
  kit::Quad::m_program->attachShader(vertexShader);
  kit::Quad::m_program->attachShader(pixelShader);
  kit::Quad::m_program->link();
  kit::Quad::m_program->detachShader(vertexShader);
  kit::Quad::m_program->detachShader(pixelShader);
  
  kit::Quad::m_ProgramTextured = kit::Program::create();
  kit::Quad::m_ProgramTextured->attachShader(vertexShader);
  kit::Quad::m_ProgramTextured->attachShader(pixelShaderTextured);
  kit::Quad::m_ProgramTextured->link();
  kit::Quad::m_ProgramTextured->detachShader(vertexShader);
  kit::Quad::m_ProgramTextured->detachShader(pixelShaderTextured);
}

void kit::Quad::releaseShared()
{
  
  kit::Quad::m_ProgramTextured.reset();
  kit::Quad::m_program.reset();
  
  glDeleteBuffers(1, &kit::Quad::m_glVertexIndices);
  glDeleteBuffers(1, &kit::Quad::m_glVertexBuffer);
  glDeleteVertexArrays(1, &kit::Quad::m_glVertexArray);
}

const float & kit::Quad::getDepth()
{
  return this->m_depth;
}

void kit::Quad::setDepth(float d)
{
  this->m_depth = d;
}

void kit::Quad::setBlending(bool b)
{
  this->m_blending = b;
}
