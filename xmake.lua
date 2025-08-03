add_rules("mode.debug", "mode.release")

add_languages("c23")

add_requires("sdl3", "lua", "stb", "uthash", "cglm")

target("flandre")
    set_kind("binary")
    add_packages("sdl3", "lua", "stb", "uthash", "cglm")
    add_headerfiles("src/**.h")
    add_files("src/**.c")
