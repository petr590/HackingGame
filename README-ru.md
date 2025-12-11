# Hack Game
Игра, копирующая мини-игру взлома из NieR:Automata. Использует C++ и OpenGL

## Сборка
Проект использует Cmake для сборки, установите его при необходимости. Также используются библиотеки GLEW, GLFW, GLM, SOIL и nlohmann/json.

Установка необходимых библиотек для linux:
```
sudo apt-get install libglew-dev libglfw3-dev libglm-dev libsoil-dev nlohmann-json3-dev
```

Для Windows скачайте все библиотеки сами

## Сборка
```
mkdir release/
cmake -DCMAKE_BUILD_TYPE=Release -B release/ .
make -j<количество потоков> -C release/
```

После опции `-j` укажите количество потоков процессора, чтобы компиляция шла быстрее

## Запуск
```
./release/main
```
