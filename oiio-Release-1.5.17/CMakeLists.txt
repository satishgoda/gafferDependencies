project (OpenImageIO)

# Release version of the library
set (OIIO_VERSION_MAJOR 1)
set (OIIO_VERSION_MINOR 5)
set (OIIO_VERSION_PATCH 17)
set (OIIO_VERSION_RELEASE_TYPE "")   # "dev", "betaX", "RCY", ""

cmake_minimum_required (VERSION 2.6)
if (NOT CMAKE_VERSION VERSION_LESS 2.8.4)
    cmake_policy (SET CMP0017 NEW)
endif ()
message (STATUS "Project source dir = ${PROJECT_SOURCE_DIR}")
message (STATUS "Project build dir = ${CMAKE_BINARY_DIR}")

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Release")
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set (DEBUGMODE ON)
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    # cmake bug workaround -- on some platforms, cmake doesn't set
    # NDEBUG for RelWithDebInfo mode
    add_definitions ("-DNDEBUG")
endif ()

option (CMAKE_USE_FOLDERS "Use the FOLDER target property to organize targets into folders." ON)
mark_as_advanced (CMAKE_USE_FOLDERS)
if (CMAKE_USE_FOLDERS)
    set_property (GLOBAL PROPERTY USE_FOLDERS ON)
endif ()

# Figure out which compiler we're using
if (CMAKE_COMPILER_IS_GNUCC)
    execute_process (COMMAND ${CMAKE_C_COMPILER} -dumpversion
                     OUTPUT_VARIABLE GCC_VERSION
                     OUTPUT_STRIP_TRAILING_WHITESPACE)
    message (STATUS "Using gcc ${GCC_VERSION} as the compiler")
endif ()
message (STATUS "CMAKE_CXX_COMPILER is ${CMAKE_CXX_COMPILER}")
message (STATUS "CMAKE_CXX_COMPILER_ID is ${CMAKE_CXX_COMPILER_ID}")
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set (CMAKE_COMPILER_IS_CLANG 1)
endif ()
if (CMAKE_CXX_COMPILER MATCHES "[Cc]lang")
    # Second try: for earlier versions of CMake, the CMAKE_CXX_COMPILER_ID
    # appears to be unreliable and may say "GNU" despite using clang, so
    # directly match against the compiler name.
    set (CMAKE_COMPILER_IS_CLANG 1)
endif ()
if (CMAKE_COMPILER_IS_CLANG)
    if (NOT CLANG_VERSION_STRING)
        EXECUTE_PROCESS( COMMAND ${CMAKE_CXX_COMPILER} --version OUTPUT_VARIABLE clang_full_version_string )
        string (REGEX REPLACE ".*clang version ([0-9]+\\.[0-9]+).*" "\\1" CLANG_VERSION_STRING ${clang_full_version_string})
    endif ()
    message (STATUS "The compiler is Clang version ${CLANG_VERSION_STRING}")
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    set (CMAKE_COMPILER_IS_INTEL 1)
    message (STATUS "Using Intel as the compiler")
endif ()

## turn on more detailed warnings and consider warnings as errors
set (STOP_ON_WARNING ON CACHE BOOL "Stop building if there are any compiler warnings")
if (NOT MSVC)
    add_definitions ("-Wall")
    if (STOP_ON_WARNING)
        add_definitions ("-Werror")
    endif ()
endif ()

if (CMAKE_COMPILER_IS_CLANG OR CMAKE_COMPILER_IS_GNUCC)
    # CMake doesn't automatically know what do do with
    # include_directories(SYSTEM...) when using clang or gcc.
    set (CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
    # Ensure this macro is set for stdint.h
    add_definitions ("-D__STDC_LIMIT_MACROS")
    # this allows native instructions to be used for sqrtf instead of a function call
    add_definitions ("-fno-math-errno")
endif ()

if (CMAKE_COMPILER_IS_CLANG)
    # Disable some warnings for Clang, for some things that are too awkward
    # to change just for the sake of having no warnings.
    add_definitions ("-Wno-unused-function")
    add_definitions ("-Wno-overloaded-virtual")
    add_definitions ("-Wno-unneeded-internal-declaration")
    add_definitions ("-Wno-unused-private-field")
    # disable warning about unused command line arguments
    add_definitions ("-Qunused-arguments")
    # Don't warn if we ask it not to warn about warnings it doesn't know
    add_definitions ("-Wunknown-warning-option")
    if (CLANG_VERSION_STRING VERSION_GREATER 3.5)
        add_definitions ("-Wno-unused-local-typedefs")
    endif ()
endif ()

if (CMAKE_COMPILER_IS_GNUCC AND (NOT CMAKE_COMPILER_IS_CLANG) AND (NOT ${GCC_VERSION} VERSION_LESS 4.8))
    # suppress a warning that Boost::Python hits in g++ 4.8
    add_definitions ("-Wno-error=unused-local-typedefs")
    add_definitions ("-Wno-unused-local-typedefs")
endif ()

if (CMAKE_COMPILER_IS_CLANG OR CMAKE_COMPILER_IS_GNUCC)
    if (HIDE_SYMBOLS AND NOT DEBUGMODE)
        # Turn default symbol visibility to hidden
        set (VISIBILITY_COMMAND "-fvisibility=hidden -fvisibility-inlines-hidden")
        add_definitions (${VISIBILITY_COMMAND})
        if (CMAKE_SYSTEM_NAME MATCHES "Linux|kFreeBSD" OR CMAKE_SYSTEM_NAME STREQUAL "GNU")
            # Linux/FreeBSD/Hurd: also hide all the symbols of dependent
            # libraries to prevent clashes if an app using OIIO is linked
            # against other verions of our dependencies.
            set (VISIBILITY_MAP_COMMAND "-Wl,--version-script=${PROJECT_SOURCE_DIR}/src/libOpenImageIO/libOpenImageIO.map")
        endif ()
    endif ()
endif ()


set (VERBOSE OFF CACHE BOOL "Print lots of messages while compiling")
set (EMBEDPLUGINS ON CACHE BOOL "Embed format plugins in libOpenImageIO")
set (OIIO_BUILD_TOOLS ON CACHE BOOL "Build the command-line tools")
set (OIIO_BUILD_TESTS ON CACHE BOOL "Build the unit tests")
set (BUILDSTATIC OFF CACHE BOOL "Build static library instead of shared")
set (LINKSTATIC OFF CACHE BOOL "Link with static external libraries when possible")
set (HIDE_SYMBOLS OFF CACHE BOOL "Hide symbols not in the public API")
set (USE_OPENGL ON CACHE BOOL "Include OpenGL support")
set (USE_QT ON CACHE BOOL "Include Qt support")
set (FORCE_OPENGL_1 OFF CACHE BOOL "Force iv to use OpenGL's fixed pipeline")
set (USE_PYTHON ON CACHE BOOL "Build the Python bindings")
set (USE_PYTHON3 OFF CACHE BOOL "Build the Python3 bindings")
set (USE_FIELD3D ON CACHE BOOL "Use Field3D if found")
set (USE_FFMPEG ON CACHE BOOL "Use FFmpeg if found")
set (USE_OPENJPEG ON CACHE BOOL "Use OpenJpeg if found")
set (USE_OCIO ON CACHE BOOL "Use OpenColorIO for color management if found")
set (USE_OPENCV ON CACHE BOOL "Use OpenCV if found")
set (USE_OPENSSL OFF CACHE BOOL "Use OpenSSL if found (for faster SHA-1)")
set (USE_FREETYPE ON CACHE BOOL "Use Freetype if found")
set (USE_GIF ON CACHE BOOL "Use GIF if found")
set (USE_LIBRAW ON CACHE BOOL "Use LibRaw if found")
set (LIBRAW_PATH "" CACHE STRING "Custom LibRaw path")
set (NOTHREADS OFF CACHE BOOL "Compile with no threads or locking")
set (OIIO_THREAD_ALLOW_DCLP ON CACHE BOOL "OIIO threads may use DCLP for speed")
set (PYTHON_VERSION 2.6)
set (PYTHON3_VERSION 3.2)
set (USE_NUKE ON CACHE BOOL "Build Nuke plugins, if Nuke is found")
set (Nuke_ROOT "" CACHE STRING "Where to find Nuke installation")
set (NUKE_VERSION 7.0)
set (USE_EXTERNAL_PUGIXML OFF CACHE BOOL
     "Use an externally built shared library version of the pugixml library")
set (PUGIXML_HOME "" CACHE STRING "Hint about where to find external PugiXML library")

set (SOVERSION ${OIIO_VERSION_MAJOR}.${OIIO_VERSION_MINOR}
     CACHE STRING "Set the SO version in the SO name of the output library")

set (PYLIB_INCLUDE_SONAME OFF CACHE BOOL "If ON, soname/soversion will be set for Python module library")
set (PYLIB_LIB_PREFIX OFF CACHE BOOL "If ON, prefix the Python module with 'lib'")
set (BUILD_OIIOUTIL_ONLY OFF CACHE BOOL "If ON, will build *only* libOpenImageIO_Util")
set (USE_CPP11 OFF CACHE BOOL "Compile in C++11 mode")
set (USE_LIBCPLUSPLUS OFF CACHE BOOL "Compile with clang libc++")
set (EXTRA_CPP_ARGS "" CACHE STRING "Extra C++ command line definitions")
set (USE_SIMD "" CACHE STRING "Use SIMD directives (0, sse2, sse3, ssse3, sse4.1, sse4.2)")

if (BUILDSTATIC AND ${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    # On Linux, the lack of -fPIC when building static libraries seems
    # incompatible with the dynamic library needed for the Python bindings.
    set (USE_PYTHON OFF)
    set (USE_PYTHON3 OFF)
endif()

if (NOTHREADS)
    message (STATUS "NO THREADS!")
    add_definitions ("-DNOTHREADS=1")
else ()
    if (VERBOSE)
        message (STATUS "Building with thread support")
        if (NOT OIIO_THREAD_ALLOW_DCLP)
            add_definitions ("-DOIIO_THREAD_ALLOW_DCLP=0")
        endif ()
    endif ()
endif ()

if (EXTRA_CPP_ARGS)
    message (STATUS "Extra C++ args: ${EXTRA_CPP_ARGS}")
    add_definitions ("${EXTRA_CPP_ARGS}")
endif()

if (USE_CPP11)
    message (STATUS "Building for C++11")
    add_definitions ("-std=c++11")
    if (CMAKE_COMPILER_IS_CLANG)
        # C++11 doesn't like 'register' keyword, which is in Qt headers
        add_definitions ("-Wno-deprecated-register")
    endif ()
endif ()

if (USE_LIBCPLUSPLUS AND CMAKE_COMPILER_IS_CLANG)
    message (STATUS "Using libc++")
    add_definitions ("-stdlib=libc++")
endif ()

if (NOT USE_SIMD STREQUAL "")
    message (STATUS "Compiling with SIMD level ${USE_SIMD}")
    if (USE_SIMD STREQUAL "0")
        add_definitions ("-DOIIO_NO_SSE=1")
    else ()
        if (MSVC OR CMAKE_COMPILER_IS_INTEL)
            add_definitions ("/arch:${USE_SIMD}")
        else ()
            add_definitions ("-m${USE_SIMD}")
        endif ()
    endif ()
endif ()

set (CMAKE_MODULE_PATH
     "${PROJECT_SOURCE_DIR}/src/cmake/modules"
     "${PROJECT_SOURCE_DIR}/src/cmake")

# Set the default namespace
if (NOT OIIO_NAMESPACE)
    set (OIIO_NAMESPACE OpenImageIO CACHE STRING
         "Specify the master OpenImageIO C++ namespace: Options include OpenImageIO OpenImageIO_<YOURFACILITY> etc."
         FORCE)
endif ()

message(STATUS "Setting Namespace to: ${OIIO_NAMESPACE}")


include (util_macros)
include (oiio_macros)
include (platform)
include (externalpackages)


include_directories(
    "${CMAKE_SOURCE_DIR}/src/include"
    "${CMAKE_BINARY_DIR}/include/OpenImageIO"
)


###########################################################################
# Paths for install tree customization.  Note that relative paths are relative
# to CMAKE_INSTALL_PREFIX.
set (DEFAULT_BIN_INSTALL_DIR   "bin")
set (DEFAULT_LIB_INSTALL_DIR   "lib")
set (DEFAULT_INCLUDE_INSTALL_DIR "include/OpenImageIO")
if (UNIX AND NOT SELF_CONTAINED_INSTALL_TREE)
    # Try to be well-behaved and install into reasonable places according to
    # the "standard" unix directory heirarchy
    # TODO: Figure out how to get the correct python directory
    set (DEFAULT_PYLIB_INSTALL_DIR "lib/python/site-packages")
    set (DEFAULT_PYLIB3_INSTALL_DIR "lib/python3/site-packages")
    set (DEFAULT_DOC_INSTALL_DIR "share/doc/OpenImageIO")
    set (DEFAULT_MAN_INSTALL_DIR "share/man/man1")
else ()
    # Here is the "self-contained install tree" case: the expectation here is
    # that everything OIIO related will go into its own directory, not into
    # some standard system heirarchy.
    set (DEFAULT_PYLIB_INSTALL_DIR "python")
    set (DEFAULT_PYLIB3_INSTALL_DIR "python3")
    set (DEFAULT_DOC_INSTALL_DIR "doc")
    set (DEFAULT_MAN_INSTALL_DIR "doc/man")
endif ()
if (EXEC_INSTALL_PREFIX)
    # Tack on an extra prefix to support multi-arch builds.
    set (DEFAULT_BIN_INSTALL_DIR   "${EXEC_INSTALL_PREFIX}/${DEFAULT_BIN_INSTALL_DIR}")
    set (DEFAULT_LIB_INSTALL_DIR   "${EXEC_INSTALL_PREFIX}/${DEFAULT_LIB_INSTALL_DIR}")
    set (DEFAULT_PYLIB_INSTALL_DIR "${EXEC_INSTALL_PREFIX}/${DEFAULT_PYLIB_INSTALL_DIR}")
    set (DEFAULT_PYLIB3_INSTALL_DIR "${EXEC_INSTALL_PREFIX}/${DEFAULT_PYLIB3_INSTALL_DIR}")
endif ()
# Set up cmake cache variables corresponding to the defaults deduced above, so
# that the user can override them as desired:
set (BIN_INSTALL_DIR ${DEFAULT_BIN_INSTALL_DIR} CACHE STRING
     "Install location for binaries (relative to CMAKE_INSTALL_PREFIX or absolute)")
set (LIB_INSTALL_DIR ${DEFAULT_LIB_INSTALL_DIR} CACHE STRING
     "Install location for libraries (relative to CMAKE_INSTALL_PREFIX or absolute)")
set (PYLIB_INSTALL_DIR ${DEFAULT_PYLIB_INSTALL_DIR} CACHE STRING
     "Install location for python libraries (relative to CMAKE_INSTALL_PREFIX or absolute)")
set (PYLIB3_INSTALL_DIR ${DEFAULT_PYLIB3_INSTALL_DIR} CACHE STRING
     "Install location for python3 libraries (relative to CMAKE_INSTALL_PREFIX or absolute)")
set (INCLUDE_INSTALL_DIR ${DEFAULT_INCLUDE_INSTALL_DIR} CACHE STRING
     "Install location of header files (relative to CMAKE_INSTALL_PREFIX or absolute)")
set (DOC_INSTALL_DIR ${DEFAULT_DOC_INSTALL_DIR} CACHE STRING
     "Install location for documentation (relative to CMAKE_INSTALL_PREFIX or absolute)")
if (UNIX)
    set (MAN_INSTALL_DIR ${DEFAULT_MAN_INSTALL_DIR} CACHE STRING
         "Install location for manual pages (relative to CMAKE_INSTALL_PREFIX or absolute)")
endif()
set (PLUGIN_SEARCH_PATH "" CACHE STRING "Default plugin search path")

set (INSTALL_DOCS ON CACHE BOOL "Install documentation")


###########################################################################
# Rpath handling.
if (CMAKE_SKIP_RPATH)
    # We need to disallow the user from truly setting CMAKE_SKIP_RPATH, since
    # we want to run the generated executables from the build tree in order to
    # generate the manual page documentation.  However, we make sure the
    # install rpath is unset so that the install tree is still free of rpaths
    # for linux packaging purposes.
    set (CMAKE_SKIP_RPATH FALSE)
    unset (CMAKE_INSTALL_RPATH)
else ()
    set (CMAKE_INSTALL_RPATH "${LIB_INSTALL_DIR}")
    if (NOT IS_ABSOLUTE ${CMAKE_INSTALL_RPATH})
        set (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}")
    endif ()
    set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif ()


if (MSVC)
    add_definitions (-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions (-D_CRT_SECURE_NO_WARNINGS)
    add_definitions (-D_CRT_NONSTDC_NO_WARNINGS)
    add_definitions (-D_SCL_SECURE_NO_WARNINGS)
    add_definitions (-DJAS_WIN_MSVC_BUILD)
    if(NOT LINKSTATIC)
        add_definitions (-DBOOST_ALL_DYN_LINK)
        add_definitions (-DOPENEXR_DLL)
    endif ()
    if (BUILDSTATIC)
         add_definitions(-DOIIO_STATIC_BUILD=1)
    endif ()
endif (MSVC)


# We want CTest for testing
# N.B. This needs to be added before any of the subdirectories, or
# their add_test commands will not register.
include (CTest)


# Tell CMake to process the sub-directories
add_subdirectory (src/libutil)

if (NOT BUILD_OIIOUTIL_ONLY)
    add_subdirectory (src/libOpenImageIO)
endif ()

if (OIIO_BUILD_TOOLS AND NOT BUILD_OIIOUTIL_ONLY)
  add_subdirectory (src/iconvert)
  add_subdirectory (src/idiff)
  add_subdirectory (src/igrep)
  add_subdirectory (src/iinfo)
  add_subdirectory (src/maketx)
  add_subdirectory (src/oiiotool)
  add_subdirectory (src/testtex)
  add_subdirectory (src/iv)
endif ()

# Add IO plugin directories
if (NOT EMBEDPLUGINS)
    add_subdirectory (src/bmp.imageio)
    add_subdirectory (src/cineon.imageio)
    add_subdirectory (src/dds.imageio)
    add_subdirectory (src/dpx.imageio)
    add_subdirectory (src/ffmpeg.imageio)
    add_subdirectory (src/field3d.imageio)
    add_subdirectory (src/fits.imageio)
    add_subdirectory (src/gif.imageio)
    add_subdirectory (src/hdr.imageio)
    add_subdirectory (src/ico.imageio)
    add_subdirectory (src/iff.imageio)
    add_subdirectory (src/jpeg.imageio)
    add_subdirectory (src/jpeg2000.imageio)
    add_subdirectory (src/openexr.imageio)
    add_subdirectory (src/png.imageio)
    add_subdirectory (src/pnm.imageio)
    add_subdirectory (src/psd.imageio)
    add_subdirectory (src/ptex.imageio)
    add_subdirectory (src/raw.imageio)
    add_subdirectory (src/rla.imageio)
    add_subdirectory (src/sgi.imageio)
    add_subdirectory (src/socket.imageio)
    add_subdirectory (src/softimage.imageio)
    add_subdirectory (src/targa.imageio)
    add_subdirectory (src/tiff.imageio)
    add_subdirectory (src/webp.imageio)
    add_subdirectory (src/zfile.imageio)
endif ()

if (USE_PYTHON AND oiio_boost_PYTHON_FOUND AND NOT BUILD_OIIOUTIL_ONLY)
    add_subdirectory (src/python)
endif ()
if (USE_PYTHON3 AND NOT BUILD_OIIOUTIL_ONLY)
    #build the python3 module in a different binary directory since it will
    #have the same name as the python2 module (e.g. OpenImageIO.so)
    add_subdirectory (src/python src/python3)
endif ()

add_subdirectory (src/include)
add_subdirectory (src/doc)
add_subdirectory (src/nuke)


#########################################################################
# Testing
#
# Just call oiio_add_tests(testname...) for each test.  Additional
# optional arguments include:
#     FOUNDVAR   specifies the name of a CMAKE variable; if not defined,
#                    the test will not be added for 'make test' (helpful
#                    for excluding tests for libraries not found).
#     IMAGEDIR   specifies a directory for test images, one level higher
#                    than where the oiio top level source lives -- a
#                    message will be printed if not found.
#     URL        URL where the test images can be found, will be
#                    incorporated into the error message if the test
#                    image directory is not found.
#     LABEL      If set to "broken", will designate the test as one
#                    that is known to be broken, so will only be run
#                    for "make testall", but not "make test".
#

# Make a copy of the testsuite into the build area
if (DEFINED CMAKE_VERSION AND NOT CMAKE_VERSION VERSION_LESS 2.8)
    file (COPY "${PROJECT_SOURCE_DIR}/testsuite"
          DESTINATION "${CMAKE_BINARY_DIR}")
endif()

# List all the individual testsuite tests here:
oiio_add_tests (gpsread misnamed-file nonwhole-tiles
                oiiotool oiiotool-composite oiiotool-deep oiiotool-fixnan
                oiiotool-readerror oiiotool-text
                perchannel dither
                dpx ico iff png psd rla sgi
                python-typedesc python-imagespec python-roi python-deep
                python-imageinput python-imageoutput
                python-imagebuf python-imagebufalgo
                maketx
                texture-blurtube texture-crop texture-cropover
                texture-derivs texture-fill texture-filtersize
                texture-interp-bicubic
                texture-interp-bilinear
                texture-interp-closest
                texture-gray texture-grid
                texture-mip-nomip texture-mip-onelevel texture-mip-trilinear
                texture-missing texture-overscan
                texture-pointsample texture-width0blur texture-res
                texture-skinny texture-fat texture-wrapfill
                texture-icwrite
               )


# List testsuites which need special external reference images from the web
# here:
oiio_add_tests (bmp
    IMAGEDIR bmpsuite
    URL http://entropymine.com/jason/bmpsuite/bmpsuite.zip)

oiio_add_tests (tiff-suite tiff-depths
    IMAGEDIR libtiffpic
    URL http://www.remotesensing.org/libtiff/images.html)

oiio_add_tests (tiff-misc)

oiio_add_tests (openexr-suite openexr-multires openexr-chroma openexr-v2
    IMAGEDIR openexr-images
    URL http://www.openexr.com/downloads.html)

oiio_add_tests (gif
    FOUNDVAR GIF_FOUND
    IMAGEDIR oiio-images
    URL "Recent checkout of oiio-images")

oiio_add_tests (jpeg2000
    FOUNDVAR OPENJPEG_FOUND
    IMAGEDIR j2kp4files_v1_5
    URL http://www.itu.int/net/ITU-T/sigdb/speimage/ImageForm-s.aspx?val=10100803)

oiio_add_tests (pnm
    IMAGEDIR oiio-images/pnm
    URL "Recent checkout of oiio-images")

oiio_add_tests (targa-tgautils
    IMAGEDIR TGAUTILS
    URL http://tgautils.inequation.org/)

oiio_add_tests (fits
    IMAGEDIR fits-images
    URL http://www.cv.nrao.edu/fits/data/tests/)

oiio_add_tests (webp
    FOUNDVAR WEBP_FOUND
    IMAGEDIR webp-images
    URL http://code.google.com/speed/webp/gallery.html)

oiio_add_tests (field3d texture-field3d
    FOUNDVAR FIELD3D_FOUND)

if (SPI_TESTS)
  oiio_add_tests (spi
    FOUNDVAR SPI_TESTS
    IMAGEDIR spi-oiio-tests
    URL "noplace -- it's SPI specific tests")
endif ()



#########################################################################
# Packaging
set (CPACK_PACKAGE_VERSION_MAJOR ${OIIO_VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${OIIO_VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${OIIO_VERSION_PATCH})
# "Vendor" is only used in copyright notices, so we use the same thing that
# the rest of the copyright notices say.
set (CPACK_PACKAGE_VENDOR "Larry Gritz et al.")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "OpenImageIO is an open source library for reading and writing image file formats, a nice format-agnostic image viewer, and other image-related classes and utilities.")
set (CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/src/doc/Description.txt")
set (CPACK_PACKAGE_FILE_NAME OpenImageIO-${OIIO_VERSION_MAJOR}.${OIIO_VERSION_MINOR}.${OIIO_VERSION_PATCH}-${platform})
#SET (CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_SOURCE_DIR}")
file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/cpack")
file (COPY "${PROJECT_SOURCE_DIR}/LICENSE" DESTINATION "${CMAKE_BINARY_DIR}/cpack")
file (RENAME "${CMAKE_BINARY_DIR}/cpack/LICENSE" "${CMAKE_BINARY_DIR}/cpack/License.txt")
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_BINARY_DIR}/cpack/License.txt")
file (COPY "${PROJECT_SOURCE_DIR}/README.rst" DESTINATION "${CMAKE_BINARY_DIR}/cpack")
file (RENAME "${CMAKE_BINARY_DIR}/cpack/README.rst" "${CMAKE_BINARY_DIR}/cpack/Readme.rst")
set (CPACK_RESOURCE_FILE_README "${CMAKE_BINARY_DIR}/cpack/Readme.rst")
set (CPACK_RESOURCE_FILE_WELCOME "${PROJECT_SOURCE_DIR}/src/doc/Welcome.txt")
#SET (CPACK_STRIP_FILES Do we need this?)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set (CPACK_GENERATOR "TGZ;STGZ;RPM;DEB")
    set (CPACK_SOURCE_GENERATOR "TGZ")
endif ()
if (APPLE)
    set (CPACK_GENERATOR "TGZ;STGZ;PackageMaker")
    set (CPACK_SOURCE_GENERATOR "TGZ")
endif ()
if (WIN32)
    set (CPACK_GENERATOR "NSIS")
    set(CPACK_PACKAGE_EXECUTABLES "iv" "iv - Image Viewer")
#    set(CPACK_CREATE_DESCTOP_LINKS "iv" "iv - Image Viewer")
    set(CPACK_NSIS_MODIFY_PATH ON)
    include (InstallRequiredSystemLibraries)
endif ()
set (CPACK_SOURCE_PACKAGE_FILE_NAME OpenImageIO-${OIIO_VERSION_MAJOR}.${OIIO_VERSION_MINOR}.${OIIO_VERSION_PATCH}-source)
#set (CPACK_SOURCE_STRIP_FILES Do we need this?)
set (CPACK_SOURCE_IGNORE_FILES ".*~")
set (CPACK_COMPONENT_UNSPECIFIED_HIDDEN TRUE)
set (CPACK_COMPONENT_UNSPECIFIED_REQUIRED TRUE)
set (CPACK_COMPONENTS_ALL user developer documentation Unspecified)
set (CPACK_COMPONENT_USER_DISPLAY_NAME "Applications")
set (CPACK_COMPONENT_DEVELOPER_DISPLAY_NAME "Developer files")
set (CPACK_COMPONENT_DOCUMENTATION_DISPLAY_NAME "Documentation")
set (CPACK_COMPONENT_USER_DESCRIPTION
     "Applications: iv, iinfo, iconvert, idiff, igrep, maketx and libraries")
set (CPACK_COMPONENT_DEVELOPER_DESCRIPTION "Include files")
set (CPACK_COMPONENT_DOCUMENTATION_DESCRIPTION "OpenImageIO documentation")
set (CPACK_COMPONENT_DEVELOPER_DEPENDS user)
include (CPack)

# TODO: equivalents of the old:
#  * make doxygen
#  * BOOST_DYNAMIC

# Do TIFF, JPEG, PNG actually look in external?
