# makefile for compiling the test object file inside build/ folder

CXX = c++
program_NAME := renderer
BUILDDIR := build
SRC := bg_renderer_cpp
program_CXX_SRCS := $(filter-out $(SRC)/bg_generator_c_module.cpp, $(wildcard $(SRC)/*.cpp)) $(wildcard $(SRC)/*/*.cpp)
program_OBJS := $(patsubst %.cpp, %.o, $(program_CXX_SRCS))
current_dir = $(shell pwd)

program_INCLUDE_DIRS := $(current_dir)/$(SRC)/renderer $(current_dir)/$(SRC)/open_gl_ext $(current_dir)/$(SRC)
program_LIBRARY_DIRS :=

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    $(info ************  LINUX VERSION ************)
    program_FRAMEWORKS =
    program_LIBRARIES = glut GLU GL m
endif
ifeq ($(UNAME_S),Darwin)
    $(info ************  MacOS VERSION ************)
    program_FRAMEWORKS = GLUT OpenGL Cocoa
    program_LIBRARIES =
endif

CPPFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir)) -std=c++11 -O3 -Wall -Wdeprecated-declarations
LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(program_LIBRARIES),-l$(library))
LDFLAGS += $(foreach fw,$(program_FRAMEWORKS),-framework $(fw))

all: dir $(program_NAME)

dir:
	mkdir -p $(BUILDDIR)

$(program_NAME): $(program_OBJS)
		
	$(CXX) $(CPPFLAGS) $(LDFLAGS) $(program_OBJS) -o $(BUILDDIR)/$(program_NAME)_test
	@- $(RM) $(program_OBJS)