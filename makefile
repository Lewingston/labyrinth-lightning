COM = g++

C_FLAGS = -std=c++2a -Wall -Wextra -MMD
C_FLAGS += -I../SFML-2.5.1/include -DSFML_STATIC

# name of the executable
EXECUTABLE = lightning.exe

# directory names
SRCDIR = src
OBJDIR = obj

OBJSUBDIRS = $(OBJDIR)

# lis of cpp files and according o files
CPPFILES = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/*/*.cpp)
OBJFILES = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CPPFILES))

# flags to link executable against sfml library
SFML_LINKER_FLAGS = -L../SFML-2.5.1/lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s
SFML_LINKER_FLAGS += -lwinmm -lopengl32 -lgdi32 -lfreetype

# linker flags for static gclibgcc linking
GCCLIB_LINKER_FLAGS = -static -static-libgcc -static-libstdc++

# all rule: create executable
all: $(EXECUTABLE)
	@echo BUILD SUCCESSFULL

# clean removes the executable and all o files
clean:
	rm -rf $(EXECUTABLE)
	rm -rf $(OBJDIR)

test: all
	$(EXECUTABLE)

# Link all object files together to an executabel
$(EXECUTABLE) : $(OBJFILES)
	@echo Link executable
	@$(COM) $^ -o $(EXECUTABLE) $(SFML_LINKER_FLAGS) $(GCCLIB_LINKER_FLAGS)

# Rule to compile cpp files to object files
$(OBJDIR)/%.o : $(SRCDIR)/%.cpp | $(OBJSUBDIRS)
	@echo Compile $<
	@$(COM) $(C_FLAGS) -c $< -o $@

# create subdirectories in obj directory
$(OBJSUBDIRS) :
	mkdir -p $@

# include dependency (.d) files
-include $(OBJFILES:.o=.d)
