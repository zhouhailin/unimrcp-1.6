cmake_minimum_required (VERSION 2.8)
project (xfyunsynth)

# Set source files
set (XFYUN_SYNTH_SOURCES
	src/xfyun_synth_engine.c
)
source_group ("src" FILES ${XFYUN_SYNTH_SOURCES})

# Plug-in declaration
add_library (${PROJECT_NAME} MODULE ${XFYUN_SYNTH_SOURCES}
	$<TARGET_OBJECTS:mrcpengine>
	$<TARGET_OBJECTS:mrcp>
	$<TARGET_OBJECTS:mpf>
	$<TARGET_OBJECTS:aprtoolkit>
)
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "plugins")

# Input libraries
target_link_libraries(${PROJECT_NAME}
	${APU_LIBRARIES}
	${APR_LIBRARIES}
)
# Input system libraries
if (WIN32)
	target_link_libraries(${PROJECT_NAME} ws2_32 winmm)
elseif (UNIX)
	target_link_libraries(${PROJECT_NAME} m)
endif ()

# Preprocessor definitions
add_definitions (
	${MRCP_DEFINES}
	${MPF_DEFINES}
	${APR_TOOLKIT_DEFINES}
	${APR_DEFINES}
	${APU_DEFINES}
)

# Include directories
include_directories (
	${PROJECT_SOURCE_DIR}/include
	${MRCP_ENGINE_INCLUDE_DIRS}
	${MRCP_INCLUDE_DIRS}
	${MPF_INCLUDE_DIRS}
	${APR_TOOLKIT_INCLUDE_DIRS}
	${APR_INCLUDE_DIRS}
	${APU_INCLUDE_DIRS}
)

# Installation directives
install (TARGETS ${PROJECT_NAME} LIBRARY DESTINATION plugin)
if (MSVC)
	install (FILES ${PROJECT_BINARY_DIR}/Debug/${PROJECT_NAME}.pdb DESTINATION plugin CONFIGURATIONS Debug)
	install (FILES ${PROJECT_BINARY_DIR}/RelWithDebInfo/${PROJECT_NAME}.pdb DESTINATION plugin CONFIGURATIONS RelWithDebInfo)
endif (MSVC)
