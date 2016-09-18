# CMakePrj


CMakePrj可以允许用户预定义多种配置.

  - 在source.txt中配置源文件，prjconfig.txt中增加自定义配置，可以修改宏、头文件/链接路径、库文件等
  - mkdir build && cd build && cmake .. -DCONFIG=LINUX_X64_DEMO
  - make


