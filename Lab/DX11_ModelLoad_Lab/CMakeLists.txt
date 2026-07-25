cmake_minimum_required(VERSION 3.21)
project(SisyphusEngine) # 프로젝트 이름 변경

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Assimp 로컬 경로 설정
set(ASSIMP_ROOT "C:/dev/External/assimp-6.0.2-build")
set(ASSIMP_LIB_NAME "assimp-vc143-mtd")
set(ZLIB_LIB_NAME   "zlibstaticd")
set(ASSIMP_BIN_DIR  "${ASSIMP_ROOT}/bin/Debug")
set(ASSIMP_LIB_DIR  "${ASSIMP_ROOT}/lib/Debug")
set(ZLIB_LIB_DIR    "${ASSIMP_ROOT}/contrib/zlib/Debug")

# 2. 소스 파일 수집
file(GLOB_RECURSE SOURCES 
    "src/*.cpp" 
    "src/*.h"
    "src/*.hlsl"
)

# WIN32 옵션을 넣어 콘솔창 없는 윈도우 애플리케이션으로 생성
add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

# 3. 헤더 포함 경로
target_include_directories(${PROJECT_NAME} PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/System"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/World"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Application"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Framework"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Common"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/UserInterface"
    "${ASSIMP_ROOT}/include"
    "C:/dev"
)

# 링커에게 라이브러리 탐색 경로 알려주기
target_link_directories(${PROJECT_NAME} PRIVATE "${ASSIMP_LIB_DIR}" "${ZLIB_LIB_DIR}")

# 4. vcpkg 라이브러리 (ImGui)
find_package(imgui CONFIG REQUIRED)
find_package(directxtk CONFIG REQUIRED)

# 5. 라이브러리 연결
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    d3d11
    dxgi
    d3dcompiler
    dinput8
    dxguid
    imgui::imgui
    Microsoft::DirectXTK
    ${ASSIMP_LIB_NAME}
    ${ZLIB_LIB_NAME}
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}")

# --- 6. 리소스 및 셰이더 복사 로직 ---

file(GLOB_RECURSE HLSL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hlsl/*.hlsl")

foreach(SHADER_PATH ${HLSL_SOURCES})
    file(RELATIVE_PATH REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/hlsl" ${SHADER_PATH})
    set(DEST_PATH "${CMAKE_BINARY_DIR}/hlsl/${REL_PATH}")
    get_filename_component(DEST_DIR ${DEST_PATH} DIRECTORY)
    
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${DEST_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SHADER_PATH}" "${DEST_PATH}"
        COMMENT "Copying shader: ${REL_PATH}"
    )
endforeach()

# 에셋 폴더도 build/assets로 복사
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/assets")