# Flandre

_Sorry, this page does not currently have an English version._

该项目仍在开发中

`flandre` 是一个用于开发小型 2D/3D 游戏的框架，使用 C23 编写。

项目主要使用 C 作为源代码，Lua 作为脚本代码，但是不像游戏引擎那样封装大量内容，`flandre` 只是把一些最基础的功能给到 Lua 层，所以大部分工作都在 Lua 层进行。

作为第一个正式项目，我只是出于兴趣爱好制作，如果有任何问题也希望指出。

## 依赖

该项目依赖这些库：

`SDL3` https://libsdl.org/

`Lua 5.4` https://lua.org/

`libpng` https://www.libpng.org

`uthash` https://troydhanson.github.io/uthash/

`cglm` https://github.com/recp/cglm

`glad` https://gen.glad.sh/

## 构建

```shell
xmake build
```

我还没有尝试过在其他平台构建，我用的是 `archlinux`，Xmake在其他平台的构建应该不会太困难。

## 待办

1. 添加 TTF 字体支持
2. 使用更正式一点的资源加载（从压缩包中加载资源）
3. 更多图形后端
4. 完善 `system` 模块
5. 优化性能
