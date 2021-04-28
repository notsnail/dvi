raylib_path = E:/lib64/raylib/raylib
raylib_flags = `pkg-config --libs --cflags raylib` -DPLATFORM_DESKTOP -Iexternal -framework OpenGL -framework Cocoa -framework IOKit

cc = gcc

src_path = ./src
bin_path = ./bin
lib_path = ./lib/osx
resource_path = ./res
bin = hal

src = $(src_path)/*.c $(src_path)/gfx/*.c $(src_path)/app/*.c $(src_path)/util/*.c $(src_path)/vendor/*.c
c_flags = $(raylib_flags) -w -s -O2 
ld_flags = -L"$(lib_path)" -lxml2

build:
	@$(cc) -o $(bin_path)/$(bin) $(src) $(c_flags) $(ld_flags)
	@cp -a $(resource_path)/. $(bin_path)/
	@echo "successful build."
clean:
	@rm -r -f $(bin_path)
	@mkdir bin
	@echo "cleaned bin."
run:
	@cd $(bin_path); \
	./$(bin)
	@echo "process finished."