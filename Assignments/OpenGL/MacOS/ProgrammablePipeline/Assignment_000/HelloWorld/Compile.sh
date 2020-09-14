rm -rf Window.app/Contents/MacOS
mkdir -p Window.app/Contents/MacOS
Clang++ -o Window.app/Contents/MacOS/Window Window.m -framework Cocoa
