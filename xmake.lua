add_rules("mode.debug", "mode.release")

add_languages("c23")

add_requires("sdl3", "lua", "uthash", "cglm", "libpng")

target("flandre")
    set_kind("binary")
    add_packages("sdl3", "lua", "uthash", "cglm", "libpng")
    add_headerfiles("src/**.h")
    add_files("src/**.c")
