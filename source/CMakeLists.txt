set(BINARY ${CMAKE_PROJECT_NAME})

################################################################################
## file globbing ###############################################################
################################################################################

# these instructions search the directory tree when cmake is
# invoked and put all files that match the pattern in the variables 
# `sources`
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES false 
  include/*/*.h src/*/*.cpp src/main.cpp
)

set(SOURCES ${SOURCES})

################################################################################
## target definitions ##########################################################
################################################################################

add_library(${BINARY} ${SOURCES})

target_compile_options(
  ${BINARY} PUBLIC -std=c++1y 
  # Compiler warning/error flags
  -Wall -Wfloat-conversion -Wextra -Wunreachable-code -Wuninitialized 
  -pedantic-errors -Wold-style-cast -Wno-error=unused-variable
  -Wfloat-equal
  # Optimisation
  -O2
)

################################################################################
## dependencies ################################################################
################################################################################

target_link_libraries(${BINARY} PUBLIC 
  ${PrEW}
  ${spdlog} # Logging
  ROOT::Minuit2 # Minimization
)

################################################################################
## packaging ###################################################################
################################################################################

# install(TARGETS ${BINARY} RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin)
install(
  TARGETS ${BINARY}
  ARCHIVE DESTINATION lib
  COMPONENT library
)