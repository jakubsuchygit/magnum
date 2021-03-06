/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "Version.h"

#include <Utility/Debug.h>

namespace Magnum {

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug operator<<(Debug debug, Version value) {
    switch(value) {
        #define _c(value, string) case Version::value: return debug << string;
        _c(None, "None")
        #ifndef MAGNUM_TARGET_GLES
        _c(GL210, "OpenGL 2.1")
        _c(GL300, "OpenGL 3.0")
        _c(GL310, "OpenGL 3.1")
        _c(GL320, "OpenGL 3.2")
        _c(GL330, "OpenGL 3.3")
        _c(GL400, "OpenGL 4.0")
        _c(GL410, "OpenGL 4.1")
        _c(GL420, "OpenGL 4.2")
        _c(GL430, "OpenGL 4.3")
        _c(GL440, "OpenGL 4.4")
        #else
        _c(GLES200, "OpenGL ES 2.0")
        _c(GLES300, "OpenGL ES 3.0")
        #endif
        #undef _c
    }

    return debug << "Invalid";
}
#endif

}
