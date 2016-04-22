SET(CMAKE_SYSTEM_NAME Linux)
# specify the cross compiler
	SET(CMAKE_C_COMPILER arm-none-linux-gnueabi-gcc)
	SET(CMAKE_CXX_COMPILER arm-none-linuxgnueabi-g++)
	SET(EXTRA_C_FLAGS "-mcpu=cortex-a8 -mfpu=neon")
# where is the target environment
	SET(CMAKE_FIND_ROOT_PATH /home/openCV/arm-env)
# search for programs in the build host directories
	SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
	SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
	SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)