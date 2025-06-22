# Flandre

_Sorry, this page does not currently have an English version._

`flandre` 是一个用于开发小型 2D/3D 游戏的框架，使用 C23 编写（是的你没看错，C23）。现在，它还只是个初学者的玩具。

创建这个项目的主要目的是为了完成我的一款小游戏，不过现在还在构思中不宜公开。但是我很乐意将创建该游戏的框架分享出来，这也算是我开始参与这个社区的一个小小的里程碑。

项目主要使用 C 作为源代码，Lua 作为脚本代码，但是不像游戏引擎那样封装大量内容，`flandre` 只是把一些最基础的功能给到 Lua 层，所以大部分工作都在 Lua 层进行。我知道这会有性能问题，不过这只是由于这样很“有趣”。

## 特性

所有的接口都存放在 `flandre` 表中，整个程序将在根目录寻找 `main.lua` ，这样做仅仅是为了方便调试。

下面一段不那么严谨的代码展示了 `flandre` 的部分功能

```lua
local vs_str = io.open("shader.vert", "r"):read("a")
local fs_str = io.open("shader.frag", "r"):read("a")

-- flandre 不会提供默认的着色器，所以需要手动编写着色器
local pl = flandre.graphics.pipeline {
    shader = {
        vertex = vs_str, -- 直接写字符串也可以哦
        fragment = fs_str
    }
}

-- flandre 使用了简单的对象管理来让整个游戏运行
local obj = flandre.entity.new()
obj.itr = function()
    print("updating per frame!")
end
obj.drw = function()
    print("drawing per frame!")
end

-- 我故意把这里写复杂了
local texture = flandre.graphics.texture(flandre.decoder.png(io.open("foo.png", "rb"):read("a")))
```

这样的写法有一部分受到 [LÖVE](https://love2d.org/) 引擎的启发，不过 _看起来_ 更简洁了

还有一点，关于图形后端，目前可用的只有 OpenGL，之后可能会增加 Vulkan 后端（取决于我能不能接受得了 Vulkan 的复杂性）

## 依赖

该项目依赖这些库：

`SDL3` https://libsdl.org/

`Lua 5.4` https://lua.org/

`stb_*` https://github.com/nothings/stb

`uthash` https://troydhanson.github.io/uthash/

`cglm` https://github.com/recp/cglm

`glad` https://gen.glad.sh/

`log.c` https://github.com/rxi/log.c

`microtar`（暂时还没有用到）https://github.com/rxi/microtar

## 构建

```shell
xmake build
```

我还没有尝试过在其他平台构建，我当前使用的是 `archlinux`，Xmake在其他平台的构建应该不会太困难。