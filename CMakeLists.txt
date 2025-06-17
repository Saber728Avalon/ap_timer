project ( TestApTimer 
	LANGUAGES C CXX)
cmake_minimum_required ( VERSION 2.8 )

#不是debug,也不是release。强制转为debug
if (NOT CMAKE_BUILD_TYPE MATCHES "Debug" AND NOT CMAKE_BUILD_TYPE MATCHES "Debug")
	set(CMAKE_BUILD_TYPE "CMAKE_BUILD_TYPE Debug")
endif()



IF(CMAKE_COMPILER_IS_GNUCXX)
	add_compile_options(-std=c++11)
	add_compile_options(-fPIC)
	add_compile_options(-fPIE)
	message(STATUS "option:-std=c++11")
ENDIF()

IF(MSVC)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)

	IF (BUUILD_MT_CODE)
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
		set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
		
		#有可能原来存在，因此需要在做一次替换
	ELSE()
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
		set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
	ENDIF()
ENDIF()

MESSAGE(STATUS "value=${CMAKE_CXX_FLAGS_DEBUG}") 

include_directories(./include)

add_executable(TestApTimer test/test.cpp)
target_link_libraries(TestApTimer pthread)