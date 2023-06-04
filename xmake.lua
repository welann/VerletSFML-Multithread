add_rules("mode.release","mode.debug")

set_languages("c++17")

add_requires("sfml")
   
add_includedirs("src")

target("verletsmfl")
    before_build( function (target)
        local target_path=path.join("$(projectdir)",string.format("%s.exe", target:name()) )
        os.rm(target_path) 
    end)
    after_build(function (target)
        local source_path = path.join(target:targetdir(), string.format("%s.exe", target:name()))
        local target_path = path.join("$(projectdir)" )
         os.cp(source_path, target_path)
    end)
    set_kind("binary")
    add_files("src/**.cpp")
    add_packages("sfml")
