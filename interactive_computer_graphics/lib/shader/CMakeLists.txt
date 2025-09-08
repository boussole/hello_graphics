file(GLOB GLSL_SHADERS *.vert *.frag)

set(GLSL_H ${CMAKE_CURRENT_SOURCE_DIR}/../../include/icg/glsl.h)

file(WRITE ${GLSL_H} [=[
#pragma once

// build-in shaders
]=])

file(WRITE glsl.c [=[
// build-in shaders

#include <icg/glsl.h>

]=])

foreach(f IN LISTS GLSL_SHADERS)
	get_filename_component(name ${f} NAME)
	string(TOUPPER ${name} name)
	string(REPLACE "." "_" name ${name})
	string(CONCAT name "GLSL_SHADER_" ${name})
	file(APPEND ${GLSL_H} "extern const char *${name};\n")
	file(READ ${f} glsl)
	string(REPLACE "\n" "\\n \\\n" glsl "${glsl}")
	file(APPEND glsl.c "const char *${name} = \"${glsl}\";\n\n")
endforeach()

add_library(shader STATIC glsl.c)