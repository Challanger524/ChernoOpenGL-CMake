// Aim of this source file is to test headers for possibility to include them in more than one source file

#if __has_include("IndexBuffer.hpp")
#         include "IndexBuffer.hpp"
#endif
#if __has_include("Renderer.hpp")
#         include "Renderer.hpp"
#endif
#if __has_include("Shader.hpp")
#         include "Shader.hpp"
#endif
#if __has_include("Texture.hpp")
#         include "Texture.hpp"
#endif
#if __has_include("Shader.hpp")
#         include "Shader.hpp"
#endif
#if __has_include("Utility.hpp")
#         include "Utility.hpp"
#endif
#if __has_include("VertexArray.hpp")
#         include "VertexArray.hpp"
#endif
#if __has_include("VertexBuffer.hpp")
#         include "VertexBuffer.hpp"
#endif
#if __has_include("VertexBufferLayout.hpp")
#         include "VertexBufferLayout.hpp"
#endif

#if __has_include("tests/Test.hpp")
#         include "tests/Test.hpp"
#endif
#if __has_include("tests/Test-ClearColor.hpp")
#         include "tests/Test-ClearColor.hpp"
#endif
#if __has_include("tests/Test-Texture2D.hpp")
#         include "tests/Test-Texture2D.hpp"
#endif
#if __has_include("tests/Test-Batching.hpp")
#         include "tests/Test-Batching.hpp"
#endif
#if __has_include("tests/Test-Batching-Textures.hpp")
#         include "tests/Test-Batching-Textures.hpp"
#endif
#if __has_include("tests/Test-Batching-Textures-dynamic.hpp")
#         include "tests/Test-Batching-Textures-dynamic.hpp"
#endif
