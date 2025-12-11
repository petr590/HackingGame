cmake_minimum_required(VERSION 3.3)
project(HackGame)

set(CMAKE_CXX_STANDARD 20)


if (CMAKE_BUILD_TYPE MATCHES "Debug")
	message("Debug build")
	add_compile_options(-g -O2)
else ()
	message("Release build")
	add_compile_options(-DNDEBUG -O3)
endif ()

add_compile_options(
	-Wall
	-Wextra
	-Wpedantic
	
    -Wcast-qual
    -Wold-style-cast
	
    -Wctor-dtor-privacy
    -Wnon-virtual-dtor
    -Woverloaded-virtual
    -Wduplicated-branches
    -Wduplicated-cond
    -Wshadow=compatible-local
    -Wzero-as-null-pointer-constant
    -Wextra-semi
    -Wsign-promo
    -Wlogical-op

    -Wno-float-equal
    -Wno-conversion
    -Wno-sign-conversion
	
	-Werror=return-type
	# -Wpadded
)

set(IMGUI_SOURCES
	imgui/imgui.cpp
	imgui/imgui_demo.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_tables.cpp
	imgui/imgui_widgets.cpp
	imgui/backends/imgui_impl_glfw.cpp
	imgui/backends/imgui_impl_opengl3.cpp
)

set_source_files_properties(${IMGUI_SOURCES} PROPERTIES COMPILE_FLAGS "-w")

add_executable(main
	${IMGUI_SOURCES}
	src/imgui_util.cpp
	src/texture.cpp
	src/util.cpp
	src/debug.cpp

	src/main/start.cpp
	src/main/init.cpp
	src/main/main.cpp
	src/main/render.cpp
	src/main/globals.cpp

	src/model/model.cpp
	src/model/models.cpp
	src/model/vao_model.cpp
	src/model/frame_model.cpp
	src/model/colored_model.cpp
	src/model/textured_model.cpp
	src/model/composite_model.cpp
	src/model/postprocessing_model.cpp

	src/level/level.cpp
	src/shader/shader.cpp
	src/shader/shader_loader.cpp
	src/shader/shader_manager.cpp

	src/entity/camera.cpp
	src/entity/simple_entity.cpp
	src/entity/player.cpp
	src/entity/enemy.cpp
	src/entity/block.cpp
	src/entity/bullet.cpp
	src/entity/minion.cpp
	src/entity/damageable.cpp
	src/entity/platform.cpp
	src/entity/walls.cpp

	src/entity/animation/animation.cpp
	src/entity/animation/billboard_animation.cpp
	src/entity/animation/flat_and_billboard_animation.cpp
	src/entity/animation/enemy_damage.cpp
	src/entity/animation/enemy_destroy.cpp
	src/entity/animation/minion_destroy.cpp
	src/entity/animation/player_damage.cpp
	src/entity/animation/player_destroy.cpp

	src/gui/gui_context.cpp
	src/gui/fading_object.cpp
	src/gui/menu.cpp
	src/gui/menu_select.cpp
	src/gui/menu_bottom_panel.cpp
	src/gui/system_message_popup.cpp
	src/gui/win_screen.cpp
)

target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/imgui)
target_link_libraries(main glfw GLEW GL SOIL dl)
