//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#ifndef GFX_OGL_BUFFER_GUARD
#define GFX_OGL_BUFFER_GUARD

#include <GLES3/gl3.h>
#include "Buffer.h"

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

class Ogl_device;

//----------------------------------------------------------------------------------------------------------------------

class Ogl_buffer final : public Buffer {
public:
    Ogl_buffer(const Buffer_desc& desc, Ogl_device* device);

    ~Ogl_buffer();

    void* map() override;

    void unmap() override;

    Device* device() const override;

    inline auto& buffer() const noexcept
    { return buffer_; }

private:
    void init_buffer_(const void* data);

    void fini_buffer_();

private:
    Ogl_device* device_;
    GLuint buffer_;
    void* contents_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib

#endif // GFX_OGL_BUFFER_GUARD