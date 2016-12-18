CXX		:= em++

CXXFLAGS	:= -O3 -std=c++1z

EMFLAGS		:= -s BINARYEN=1 -s "BINARYEN_METHOD='native-wasm'" -s "EXPORT_NAME='deus'" -s MODULARIZE=1
EMFLAGS		+= -s NO_EXIT_RUNTIME=1 -s NO_FILESYSTEM=0 -s NO_DYNAMIC_EXECUTION=1 -s DISABLE_EXCEPTION_CATCHING=0
EMFLAGS		+= -s ELIMINATE_DUPLICATE_FUNCTIONS=1 -s ASSERTIONS=1 -s USE_SDL=0 -s USE_SDL_IMAGE=0 -s USE_SDL_TTF=0
EMFLAGS		+= -s USE_GLFW=0 -s USE_WEBGL2=1 -s FULL_ES3=1 -s USE_ZLIB=0 --use-preload-plugins

INCLUDES	:= -Isrc -Ithird_party/compat/include/wasm -Ithird_party/glm/include

PROGRAM		:= deus
SOURCES		:= $(wildcard src/gl/*.cpp src/js/*.cpp src/*.cpp)
OBJECTS		:= $(SOURCES:src/%.cpp=build/wasm/%.o)
INFILES		:= $(shell find res/data -type f)

all: bin/$(PROGRAM).js res/html/index.html res/html/index.css res/html/index.js bin/favicon.ico | bin
	@echo [BIN] bin/index.html
	@python res/merge.py res/html/index.css res/html/index.js bin/$(PROGRAM).js bin/$(PROGRAM).data bin/$(PROGRAM).wasm

release: bin/$(PROGRAM).min.js res/html/index.html build/wasm/index.css build/wasm/index.js bin/favicon.ico | bin
	@echo [BIN] bin/index.html
	@python res/merge.py build/wasm/index.css build/wasm/index.js bin/$(PROGRAM).min.js bin/$(PROGRAM).data bin/$(PROGRAM).wasm

build/wasm/index.js: res/html/index.js | build/wasm
	@echo [SRC] $<
	@java -jar res/closure.jar --jscomp_off '*' --js $< --js_output_file $@

build/wasm/index.css: res/html/index.css | build/wasm
	@echo [CSS] $<
	@java -jar res/closure-stylesheets.jar -o $@ $<

bin/$(PROGRAM).min.js: bin/$(PROGRAM).js | build/wasm/cache bin
	@echo [SRC] $@
	@java -jar res/closure.jar --jscomp_off '*' --js $< --js_output_file $@

bin/$(PROGRAM).js: res/module.js $(OBJECTS) $(INFILES) makefile | bin
	@echo [SRC] $@
	@$(CXX) $(CXXFLAGS) $(EMFLAGS) --cache build/wasm/cache --preload-file res/data/@ --js-library res/module.js $(OBJECTS) -o bin/$(PROGRAM).js
	@rm -f bin/$(PROGRAM).wast
	@rm -f bin/$(PROGRAM).wast.mappedGlobals

build/wasm/%.o: src/%.cpp
	@echo [CXX] $<
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJECTS): | build/wasm build/wasm/gl build/wasm/js

bin/favicon.ico: res/main.ico | bin
	@echo [ICO] $@
	@cp $< $@

build/wasm:
	@mkdir -p $@

build/wasm/cache:
	@mkdir -p $@

build/wasm/gl:
	@mkdir -p $@

build/wasm/js:
	@mkdir -p $@

bin:
	@mkdir -p $@

clean:
	@rm -f $(OBJECTS) build/wasm/index.css build/wasm/index.js

up: | release
	@scp bin/deus.min.js bin/deus.data bin/deus.wasm bin/index.html xiphos:html/
