
cmake_minimum_required(VERSION 3.19)
project(PicadoGlance)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /JMC")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror -Wno-unused-parameter -Wno-implicit-fallthrough")
endif()

# Производим поиск wxWidgets - нашего основного оконного каркаса, на котором основана программа.
set(wxWidgets_USE_STATIC ON)
set(wxWidgets_USE_UNICODE ON)
find_package(wxWidgets REQUIRED COMPONENTS mono)
#find_package(wxWidgets REQUIRED)
if (NOT wxWidgets_FOUND)
  message(FATAL_ERROR "wxWidgets NOT FOUND")
endif()

# -- Для отладочных целей выведем в лог переменные, которые должны быть установлены и стать доступными после find_package(wxWidgets).
message(STATUS "wxWidgets include dirs - " "${wxWidgets_INCLUDE_DIRS}")
message(STATUS "wxWidgets libraries - " "${wxWidgets_LIBRARIES}")
message(STATUS "wxrc path - " ${wxWidgets_wxrc_EXECUTABLE})
# --------

set(PICADO_GLANCE_MODULES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(PICADO_GLANCE_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(PICADO_GLANCE_RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resource)
file(GLOB HDR_FILES ${PICADO_GLANCE_HEADERS_DIR}/*.h)
file(GLOB SRC_FILES ${PICADO_GLANCE_MODULES_DIR}/*.cpp)
file(GLOB ALL_RESOURCE_FILES ${PICADO_GLANCE_RESOURCE_DIR}/*.*)
if (WIN32)
  file(GLOB WINDOWS_RESOURCE_FILES ${PICADO_GLANCE_RESOURCE_DIR}/*.rc)
endif()

# Перекомпиляция wxWidgets-ресурсов ресурсов (в формате xrs), если их исходники изменились.
set(XRC_FILE ${PICADO_GLANCE_RESOURCE_DIR}/PCADViewer.xrc)
set(XRS_FILE ${CMAKE_BINARY_DIR}/PCADViewer.xrs)
message(STATUS "XRC_FILE - " ${XRC_FILE})
message(STATUS "XRS_FILE - " ${XRS_FILE})
add_custom_command(
    OUTPUT ${XRS_FILE}
    COMMAND ${wxWidgets_wxrc_EXECUTABLE} -o ${XRS_FILE} ${XRC_FILE}
    DEPENDS ${ALL_RESOURCE_FILES}
    COMMENT "Генерируем XRS-файл wxWidgets-ресурсов"
)

add_custom_target(RunGenXRSResources ALL
    DEPENDS ${XRS_FILE}
    COMMENT "Запуск создания wxWidgets-ресурсов - ${XRS_FILE}"
)

# Основная цель - исполняемый файл просмотрщика.
add_executable(${PROJECT_NAME} ${SRC_FILES} ${HDR_FILES} ${WINDOWS_RESOURCE_FILES})
# Настройки созданной цели.
add_dependencies(${PROJECT_NAME} RunGenXRSResources)
target_include_directories(${PROJECT_NAME} PRIVATE ${PICADO_GLANCE_HEADERS_DIR} ${wxWidgets_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
