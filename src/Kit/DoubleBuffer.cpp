#include "Kit/DoubleBuffer.hpp"

#include "Kit/IncOpenGL.hpp"
#include "Kit/Texture.hpp"

kit::DoubleBuffer::DoubleBuffer(glm::uvec2 resolution, kit::PixelBuffer::AttachmentList colorattachments, kit::PixelBuffer::AttachmentInfo depthattachment)
{
  this->m_resolution = resolution;
  this->m_frontBuffer = kit::PixelBuffer::create(resolution, colorattachments, depthattachment);
  this->m_backBuffer = kit::PixelBuffer::create(resolution, colorattachments, depthattachment);
}

kit::DoubleBuffer::DoubleBuffer(glm::uvec2 resolution, kit::PixelBuffer::AttachmentList colorattachments)
{
  this->m_resolution = resolution;
  this->m_frontBuffer = kit::PixelBuffer::create(resolution, colorattachments);
  this->m_backBuffer = kit::PixelBuffer::create(resolution, colorattachments);
}

kit::DoubleBuffer::DoubleBuffer(kit::Texture::Ptr front, kit::Texture::Ptr back)
{
  this->m_resolution = glm::uvec2(front->getResolution().x, front->getResolution().y);
  this->m_frontBuffer = kit::PixelBuffer::create(this->m_resolution, {PixelBuffer::AttachmentInfo(front)});
  this->m_backBuffer = kit::PixelBuffer::create(this->m_resolution, {PixelBuffer::AttachmentInfo(back)});
}

void kit::DoubleBuffer::flip()
{
  this->m_frontBuffer.swap(this->m_backBuffer);
}

void kit::DoubleBuffer::clear(std::vector<glm::vec4> colors, float depth)
{
  this->m_backBuffer->clear(colors, depth);
  this->m_backBuffer->bind();
}

void kit::DoubleBuffer::clear(std::vector<glm::vec4> colors)
{
  this->m_backBuffer->clear(colors);
  this->m_backBuffer->bind();
}

kit::DoubleBuffer::~DoubleBuffer()
{

}

kit::DoubleBuffer::Ptr kit::DoubleBuffer::create(glm::uvec2 resolution, kit::PixelBuffer::AttachmentList colorattachments, kit::PixelBuffer::AttachmentInfo depthattachment)
{
  return std::make_shared<kit::DoubleBuffer>(resolution, colorattachments, depthattachment);
}

kit::DoubleBuffer::Ptr kit::DoubleBuffer::create(glm::uvec2 resolution, kit::PixelBuffer::AttachmentList colorattachments)
{
  return std::make_shared<kit::DoubleBuffer>(resolution, colorattachments);
}

kit::DoubleBuffer::Ptr kit::DoubleBuffer::create(kit::Texture::Ptr front, kit::Texture::Ptr back)
{
  return std::make_shared<kit::DoubleBuffer>(front, back);
}

kit::PixelBuffer::Ptr kit::DoubleBuffer::getFrontBuffer(){
  return this->m_frontBuffer;
}

kit::PixelBuffer::Ptr kit::DoubleBuffer::getBackBuffer(){
  return this->m_backBuffer;
}

void kit::DoubleBuffer::blitFrom(kit::DoubleBuffer::Ptr source)
{
#ifndef KIT_SHITTY_INTEL
  glBlitNamedFramebuffer(
    source->getFrontBuffer()->getHandle(),
    this->getBackBuffer()->getHandle(),
    0, 0, source->getResolution().x, source->getResolution().y,
    0, 0, this->getResolution().x, this->getResolution().y,
    GL_COLOR_BUFFER_BIT, GL_LINEAR
  );
#else 
  source->getFrontBuffer()->bind(kit::PixelBuffer::Read);
  this->getFrontBuffer()->bind(kit::PixelBuffer::Draw);
  glBlitFramebuffer(
    0, 0, source->getResolution().x, source->getResolution().y,
    0, 0, this->getResolution().x, this->getResolution().y,
    GL_COLOR_BUFFER_BIT, GL_LINEAR
  );
#endif
}

glm::uvec2 kit::DoubleBuffer::getResolution()
{
  return this->m_resolution;
}

/*
 void glBlitNamedFramebuffer( uint32_t readFramebuffer,
  uint32_t drawFramebuffer,
  int32_t srcX0,
  int32_t srcY0,
  int32_t srcX1,
  int32_t srcY1,
  int32_t dstX0,
  int32_t dstY0,
  int32_t dstX1,
  int32_t dstY1,
  GLbitfield mask,
  GLenum filter);
 */
