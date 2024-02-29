emcc -o index.html main.c entity.c player.c perlin.c map.c overworld.c lab.c -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -Os -I. -I ../raylib/src -I ../raylib/src/external -L. -L ../raylib/src -s USE_GLFW=3 -s ASYNCIFY -s TOTAL_MEMORY=67108864 -s FORCE_FILESYSTEM=1 --shell-file ../raylib/src/shell.html ../raylib/src/libraylib.a -DPLATFORM_WEB -s 'EXPORTED_FUNCTIONS=["_free","_malloc","_main"]' -s EXPORTED_RUNTIME_METHODS=ccall --preload-file fov100.fs --preload-file urizen.png --shell-file mmmshell.html
