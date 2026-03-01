@echo off
set "VCPKG=D:/vcpkg/installed/x64-mingw-dynamic"

echo 检查库文件是否存在...
if not exist "%VCPKG%\lib\libopencv_core.a" echo 错误：libopencv_core.a 不存在！
if not exist "%VCPKG%\lib\libopencv_imgproc.a" echo 错误：libopencv_imgproc.a 不存在！
if not exist "%VCPKG%\lib\libopencv_highgui.a" echo 错误：libopencv_highgui.a 不存在！
if not exist "%VCPKG%\lib\libopencv_imgcodecs.a" echo 错误：libopencv_imgcodecs.a 不存在！

echo.
echo 正在编译...

g++ -std=c++17 -O2 -o rasterizer.exe ^
    main.cpp rasterizer.cpp Triangle.cpp ^
    -I"%VCPKG%/include" ^
    -I"%VCPKG%/include/eigen3" ^
    -L"%VCPKG%/lib" ^
    -lopencv_core ^
    -lopencv_imgproc ^
    -lopencv_highgui ^
    -lopencv_imgcodecs

if %ERRORLEVEL% == 0 (
    echo 编译成功！运行...
    rasterizer.exe
) else (
    echo 编译失败
)

pause