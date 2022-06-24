# ChernoOpenGL-CMake

`git clone --recurse --shallow-submodules https://github.com/Challanger524/ChernoOpenGL-CMake.git`

### About

**CMake** based implementation of TheCherno's [OpenGL series](https://www.youtube.com/playlist?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2) with minimal code deviancy according to videos.

### Features

- **cmake** - cross-platform build system
	- cross-platform dependencies handling
	- tested on:
		- Windows 10 (Visual Studio 2022)
- **perfect commits** - reached through git-history rewriting
	- pros: clear diffs between commits without typos
	- cons: unstable commit hashes and timestamps
	- commands: `rebase -i <commit>~`; `rebase -i --root`
- **git-submodules** - each dependency present as a submodule
	- much clear dependency-introduction commits
	- up-to-date source of dependencies' repositories
	- no need for additional downloads during setup stage

### Differences

- **header-only** due to my laziness :) - easier to read/navigate :+1:
- **error-logging** - both `GLCall()` macro and `glDebugMessageCallback`
	- debugging stuff moved from `Renderer.hpp` to new `Utility.hpp` file
	- awesome breakpoints on calls with errors with `GLCall()`
	- detailed gpu-vendor error description with `glDebugMessageCallback`
		- may be implemented in OpenGL versions prior to 4.3 (by gpu-vendor)
		- worked on OpenGL 4.0 (Intel CPU) for me
- **imgui docking** branch - allows moving gui-window outside
- **adopted** last 4 (*Batch Rendering* related) commits
	- Cherno explained *Batch Rendering* with different source code repository
- some **stl::type_traits** related bragging :sunglasses:

-----

Special thanks to **speauty**'s [ChernoOpenGL](https://github.com/speauty/ChernoOpenGL) implementation for working and mostly accurate code which greatly saved my time.

Misc:
- It took a bit more than a month to finish this project.
- This project helped me to cure: git- and cmake- anxiety ;)
- Cherno is the only and the best C++ teacher, see also his [C++ playlist](https://www.youtube.com/playlist?list=PLlrATfBNZ98dudnM48yfGUldqGD0S4FFb).
- Console output example:
	```cmd
	Info: GLEW version: 2.2.0
	Info: GL   version: 3.3.0 NVIDIA 516.59
	Info: GPU  vendor : NVIDIA Corporation
	Info: Renderer    : NVIDIA GeForce GTX 1060 3GB/PCIe/SSE2
	Info: ImGui version: 1.89 WIP +viewport +docking

	Trace: Registering test: Clear Color
	Trace: Registering test: Texture 2D
	Trace: Registering test: Batching
	Trace: Registering test: Batching Textures
	Trace: Registering test: Batching Textures (dynamic)
	```
