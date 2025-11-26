CXX = C:/msys64/mingw64/bin/g++.exe
CXXFLAGS = -std=c++17 -Wall -IC:/msys64/mingw64/include
LIBS = -LC:/msys64/mingw64/lib -lglfw3 -lglew32 -lopengl32 -lgdi32

SRCDIR = src
SOURCES = $(SRCDIR)/main.cpp \
          $(SRCDIR)/render/shader.cpp \
          $(SRCDIR)/render/camera.cpp \
          $(SRCDIR)/render/texture.cpp \
          $(SRCDIR)/meshes/mesh.cpp \
          $(SRCDIR)/algorithms/algorithms.cpp \
          $(SRCDIR)/scene/city_scene.cpp \
          $(SRCDIR)/stb_impl.cpp

TARGET = bin/city_designer

all: $(TARGET)

$(TARGET): $(SOURCES)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) $(LIBS)

clean:
	rm -rf bin

.PHONY: all clean