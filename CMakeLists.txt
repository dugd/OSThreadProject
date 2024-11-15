cmake_minimum_required(VERSION 3.10)
project(ThreadProject VERSION 0.1.0 LANGUAGES C CXX)

# Додаємо шлях до заголовкових файлів
include_directories(include)

# Створення статичних бібліотек
add_library(sortlib STATIC src/sort_utils.cpp)
target_include_directories(sortlib PUBLIC include)

# Створення динамічних бібліотек
add_library(threadlib SHARED src/ThreadPool.cpp)
target_include_directories(threadlib PUBLIC include)

add_library(mapfilelib SHARED src/Mappingfile.cpp)
target_include_directories(mapfilelib PUBLIC include)

# Додаємо макрос для експорту символів на Windows
target_compile_definitions(threadlib PRIVATE THREADLIB_EXPORTS)
target_compile_definitions(mapfilelib PRIVATE MAPFILELIB_EXPORTS)

# Створення виконуваного файлу
add_executable(ThreadProject src/main.cpp)

# Підключення бібліотек
target_link_libraries(ThreadProject PRIVATE sortlib threadlib mapfilelib)
