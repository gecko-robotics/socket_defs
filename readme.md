<img src="docs/socket.svg" width="300px">

# Socket Definitions

Some common code that a couple of my repos use. This is a header
only library.

- Transport with `pack`/`unpack` or `encode`/`decode` methods to convert to/from
  source/`message_t`
  - JSON: super simple string only, ex: `{"hi": "23","something":"something longer"}`
  - ASCII: human readable text with separators, ex: `"one two three` --> `one,two,three`
  - RAW: similar to ASCII but no separators, it only changes `char` --> `uint8_t`
- Socket info:
  - `inetaddr_t`: takes `udp://<x.x.x.x|*|bc>:<xxxx|*>` (e.g., `udp://1.2.3.4:56789`)
    and converts into socket descriptor info
    - Doesn't support TCP right now
    - Address `*` sets it to `INADDR_ANY` which defaults to `0.0.0.0`
    - Address `bc` set it to `INADDR_BROADCAST` which is for broadcasting
    - Port `*` sets it to 0 which tells the kernel to use the first port number available
  - `unixaddr_t`: takes `unix://./my/cool/path/file.uds`
- Messages which are transmitted between sockets.
  - pack: templated function that does `pack<struct>(struct)` --> `message_t`
  - unpack: templated function that does `unpack<struct>(message_t)` --> `struct`
  - debugging to convert `message_t` --> `std::string` and support `std::ostream` with `<<`


## Usage

```cpp

```

## cmake

You can use `cmake`'s `FetchContent` to download the library and
add it to your program like this:

```cmake
include(FetchContent)

# socket_defs -----------------
FetchContent_Declare(
  socket_defs
  GIT_REPOSITORY "https://github.com/gecko-robotics/socket_defs.git"
)

FetchContent_MakeAvailable( socket_defs )

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} INTERFACE socket_defs)
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
