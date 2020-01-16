//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include "ogl_lib.h"
#include "Ogl_buffer.h"
#include "Ogl_device.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

Ogl_buffer::Ogl_buffer(const Buffer_desc& desc, Ogl_device* device) :
    Buffer {desc},
    device_ {device},
    buffer_ {0},
    contents_ {nullptr}
{
    init_buffer_(desc.data);
}

//----------------------------------------------------------------------------------------------------------------------

Ogl_buffer::~Ogl_buffer()
{
    fini_buffer_();
}

//----------------------------------------------------------------------------------------------------------------------

void* Ogl_buffer::map()
{
    if (!contents_)
        contents_ = glMapBufferRange(GL_COPY_WRITE_BUFFER, 0, size_, GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);

    return contents_;
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_buffer::unmap()
{
    glUnmapBuffer(GL_COPY_WRITE_BUFFER);
    contents_ = nullptr;
}

//----------------------------------------------------------------------------------------------------------------------

Device* Ogl_buffer::device() const
{
    return device_;
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_buffer::init_buffer_(const void* data)
{
    glGenBuffers(1, &buffer_);
    glBindBuffer(GL_COPY_WRITE_BUFFER, buffer_);
    glBufferData(GL_COPY_WRITE_BUFFER, size_, data, to_GLDataUsage(heap_type_));

    if (Heap_type::local != heap_type_) {
        contents_ = glMapBufferRange(GL_COPY_WRITE_BUFFER, 0, size_, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
        unmap();
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Ogl_buffer::fini_buffer_()
{
    glDeleteBuffers(1, &buffer_);
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib