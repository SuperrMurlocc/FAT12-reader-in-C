cmake_minimum_required(VERSION 3.17)
project(project1 C)

# Przyjmij standard C11
set(CMAKE_C_STANDARD 11)

# Ustaw opcje kompilatora (z raportu Dante)
add_compile_options(
        "-ggdb3"
        "-std=c11"
        "-xc"
        "-pedantic"
        "-Wall"
        "-Wextra"
        "-Werror=vla"
        "-Wno-error=parentheses"
        "-Wno-parentheses"
        "-D_GNU_SOURCE"
        "-D__TEST_BOOTSTRAP"
        "-DINSIDE_DANTE"
        "-Wno-deprecated-declarations"
)

# Ustaw opcje konsolidatora (z raportu Dante)
add_link_options(
        "-ggdb3"
)

# Utwórz target 'project1' i dodaj do niego pliki źródlowe:
add_executable(project1
        main.c # Nazwa głównego pliku, z funkcją main
        directory_reader.c
        )

