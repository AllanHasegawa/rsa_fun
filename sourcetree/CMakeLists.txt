cmake_minimum_required(VERSION 2.8.10)

project(rsafun)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
find_package(GMP REQUIRED)
include_directories(${GMP_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR})


set(CMAKE_CXX_FLAGS "-Wall -pipe -std=c++11 -pthreads")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fomit-frame-pointer -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

set(SRCS
	rfmath.cpp
	rsafun.cpp
	)

set(MAIN_SRCS
	${SRCS}
	main.cpp
)

add_executable(rsafun ${MAIN_SRCS})
target_link_libraries(rsafun ${GMPXX_LIBRARIES})


set(UNIT_TESTS_SRCS
	${SRCS}
	ut_primes.cpp
	ut_rsafun.cpp
)

add_executable(unittests ${UNIT_TESTS_SRCS})
target_link_libraries(unittests ${GMPXX_LIBRARIES})


set(GEN_SRCS
	${SRCS}
	gen.cpp
)

add_executable(rsagen ${GEN_SRCS})
target_link_libraries(rsagen ${GMPXX_LIBRARIES})

set(CRYPT_SRCS
	${SRCS}
	crypt.cpp
)

add_executable(rsacrypt ${CRYPT_SRCS})
target_link_libraries(rsacrypt ${GMPXX_LIBRARIES})

set(DECRYPT_SRCS
	${SRCS}
	decrypt.cpp
)

add_executable(rsadecrypt ${DECRYPT_SRCS})
target_link_libraries(rsadecrypt ${GMPXX_LIBRARIES})
