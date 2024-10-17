cmake_minimum_required(VERSION 3.10)
project(topd)

# Налаштування стандарту C
set(CMAKE_C_STANDARD 11)

# Вказуємо джерела та заголовки
set(SOURCES
    main.c
    linux.c
)

set(HEADERS
    linux.h
)

# Створюємо виконуваний файл
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Налаштування режиму Release і Debug
set(CMAKE_C_FLAGS_RELEASE "-O3")
set(CMAKE_C_FLAGS_DEBUG "-g")

# Встановлюємо програму в /usr/bin
install(TARGETS ${PROJECT_NAME} DESTINATION /usr/bin)

# Додаємо ціль для видалення (uninstall)
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -E remove /usr/bin/${PROJECT_NAME}
    COMMENT "Видалення програми з /usr/bin"
)
