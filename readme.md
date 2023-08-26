<img src="docs/socket.svg" width="300px">

# Socket Definitions

Some common code that a couple of my repos use. This is a header
only library.

## Usage

## cmake

You can use `cmake`'s `FetchContent` to download the library and
add it to your program like this:

```cmake
include(FetchContent)

# socket -----------------
FetchContent_Declare(
  socket
  GIT_REPOSITORY "https://github.com/gecko-robotics/socket.git"
  GIT_TAG "origin/main"
)

FetchContent_MakeAvailable( socket )

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} INTERFACE socket)
```

# MIT License

**Copyright (c) 2003 Kevin J. Walchko**

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
