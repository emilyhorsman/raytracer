OBJECT_DEPS=main.o Scene.o Objects.o Vector.o Camera.o Material.o Utility.o PointLight.o Stats.o Renderer.o ImageFile.o
TEST_OBJECT_DEPS=tests/tests.o Scene.o Objects.o Vector.o Camera.o Material.o Utility.o PointLight.o Stats.o Renderer.o ImageFile.o

# Linux (default)
LDFLAGS=-lGL -lGLU -lglut
CFLAGS=-Wall -Werror -std=c++11 -DGL_SILENCE_DEPRECATION
CC=g++
CXX=g++
CPPFLAGS=-DGL_SILENCE_DEPRECATION -Wall -std=c++11 -Wno-deprecated
EXEEXT=
RM=rm
DOXYGEN=doxygen

# Windows (cygwin)
ifeq "$(OS)" "Windows_NT"
	EXEEXT=.exe #on windows applications must have .exe extension
	RM=del #rm command for windows powershell
    LDFLAGS=-lfreeglut -lglu32 -lopengl32
else
	# OS X
	OS := $(shell uname)
	ifeq ($(OS), Darwin)
		LDFLAGS=-framework Carbon -framework OpenGL -framework GLUT -DGL_SILENCE_DEPRECATION -Wno-deprecated
	endif
endif

PROGRAM_NAME=Ray

run: $(PROGRAM_NAME)
	./$(PROGRAM_NAME)$(EXEEXT)

$(PROGRAM_NAME): $(OBJECT_DEPS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	$(RM) *.o $(PROGRAM_NAME)$(EXEEXT)
	$(RM) -rf html latex

docs:
	$(DOXYGEN) Doxyfile


build_tests: $(TEST_OBJECT_DEPS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o test $^

test: build_tests
	./test
