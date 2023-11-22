#pragma once

#include "IRenderer.h"
#include "IApplication.h"

#include <GL/gl.h>
#if defined (_WINDOWS)
#include "./GL/glext.h"
#endif
#include "./GL/myGL.h" // Declare newer OpenGL functions


class OpenGLRenderer : public IRenderer
{
public:
	OpenGLRenderer();
	~OpenGLRenderer();

	/**
	 * Create method from IRenderer
	 * @return true if succesful, or false to cancel app startup
	 */
	bool Create() override;

	/**
	 * Flip method from IRenderer.
	 * Transfer graphics to screen
	 */
	void Flip() override;

	/**
	 * Clear the color, depth and stencil buffers
	 */
	void Clear(float r, float g, float b, float a, float depth = 1.0f, int32_t stencil = 0) override;

	/**
	 * Set viewport area of the screen
	 * @param area, x, y, width, height of the viewport area
	 */
	void SetViewport(const glm::ivec4& area) override;

	/**
	 * Set the texture active into rendering
	 * @param program
	 * @param texture handle
	 * @param slot index of the texture slot
	 * @param uniformName name of the uniform in shader
	 * @return true if uniform is found and set
	 */
	bool SetTexture(GLuint program, GLuint texture, int32_t slot, const std::string_view& uniformName) override;

	/**
	 * SetUniformXXX helpers to set uniforms into shader program
	 * @param program program to set the uniform into
	 * @param name uniform name in shader code
	 * @param value(s) to set to the uniform if it is found from the shader.
	 * @return true if uniform is found and set.
	 */
	static inline bool SetUniformFloat(GLuint program, const char* name, float v)
	{
		const GLint location = glGetUniformLocation(program, name);
		if (location != -1)
		{
			glUniform1f(location, v);
		}
		return location != -1;
	}

	static inline bool SetUniformVec3(GLuint program, const char* name, const glm::vec3& v)
	{
		const GLint location = glGetUniformLocation(program, name);
		if (location != -1)
		{
			glUniform3fv(location, 1, &v.x);
		}
		return location != -1;
	}

	static inline bool SetUniformVec4(GLuint program, const char* name, const glm::vec4& v)
	{
		const GLint location = glGetUniformLocation(program, name);
		if (location != -1)
		{
			glUniform4fv(location, 1, &v.x);
		}
		return location != -1;
	}

	static inline bool SetUniformMatrix3(GLuint program, const char* name, const glm::mat3& m)
	{
		const GLint location = glGetUniformLocation(program, name);
		if (location != -1)
		{
			glUniformMatrix3fv(location, 1, GL_FALSE, &m[0][0]);
		}
		return location != -1;
	}

	static inline bool SetUniformMatrix4(GLuint program, const char* name, const glm::mat4& m)
	{
		const GLint location = glGetUniformLocation(program, name);
		if (location != -1)
		{
			glUniformMatrix4fv(location, 1, GL_FALSE, &m[0][0]);
		}
		return location != -1;
	}

	/**
	 * Create OpenGL texture handle from image file
	 * @param filename file to load
	 * @return OpenGL texture handle, or 0 if failed
	 */
	GLuint CreateTexture(const std::string_view& filename);

	/**
	 * Create OpenGL vertex shader from text
	 * @param vertexShader shader source code
	 * @return OpenGL shader handle, or 0 if failed
	 */
	GLuint CreateVertexShader(const char* vertexShader);

	/**
	 * Create OpenGL vertex shader from text file
	 * @param filename text file to load
	 * @return OpenGL shader handle, or 0 if failed
	 */
	GLuint CreateVertexShaderFromFile(const std::string_view& filename);

	/**
	 * Create OpenGL fragment shader from text
	 * @param fragmentShader shader source code
	 * @return OpenGL shader handle, or 0 if failed
	 */
	GLuint CreateFragmentShader(const char* fragmentShader);

	/**
	 * Create OpenGL fragment shader from text file
	 * @param filename text file to load
	 * @return OpenGL shader handle, or 0 if failed
	 */
	GLuint CreateFragmentShaderFromFile(const std::string_view& filename);

	/**
	 * Link OpenGL program from vertex and fragment shader
	 * @param vertexShader
	 * @param fragmentShader
	 * @return OpenGL program handle, or 0 if failed
	 */
	GLuint CreateProgram(GLuint vertexShader, GLuint fragmentShader);

	/**
	 * Print detailed information of shader errors
	 * @param shader
	 */
	void PrintShaderError(GLuint shader);

	/**
	 * Print detailed information of shader program errors
	 * @param program
	 */
	void PrintProgramError(GLuint program);

	/**
	 * Static helper to get OpenGL function pointers from OpenGL dll after
	 * context has been created
	 */
	static bool InitFunctions();

private:
	/**
	* Set default states of OpenGL
	*/
	bool SetDefaultSettings();

#if defined (_WINDOWS)
	HDC				m_Context; // Handle to the device context (screen)
	HGLRC			m_hRC; // Handle to OpenGL resource context
#endif

#if defined (_LINUX)
	void* m_Context;
#endif
};
