#include "Shader.h"


unsigned int Shader::m_CompileShader(const string& source, int shaderType)
{
	unsigned int id;

	id = glCreateShader(shaderType);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, NULL);
	glCompileShader(id);

	int success;
	char infoLog[512];
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(id, 512, NULL, infoLog);
		fprintf(stderr, "%s Compile Err: %s\n", shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment", infoLog);
		return -2;
	}
	else
		fprintf(stderr, "%s shader is compiled\n", shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment");
	return id;
}

void Shader::m_CreateShader(const string& vshader, const string& fshader)
{
	m_RendererID = glCreateProgram();
	unsigned int vid = m_CompileShader(vshader, GL_VERTEX_SHADER);
	unsigned int fid = m_CompileShader(fshader, GL_FRAGMENT_SHADER);

	GLCall(glAttachShader(m_RendererID, vid));
	GLCall(glAttachShader(m_RendererID, fid));

	GLCall(glLinkProgram(m_RendererID));
	//glValidateProgram(m_RendererID);

	int success;
	char infoLog[512];

	glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(m_RendererID, 512, NULL, infoLog);
		fprintf(stderr, "Link Err: %s\n", infoLog);
		return;
	}
	glDeleteShader(vid);
	glDeleteShader(fid);
}

Shader::Shader(const string& filePath)
{
	ifstream stream(filePath);

	string line;
	ShaderType type;

	ostringstream shaderArray[2];
	while (getline(stream, line)) {
		if (line.find("#shader") != string::npos) {
			if (line.find("vertex") != string::npos) {
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != string::npos) {
				type = ShaderType::FRAGMENT;
			}
		}
		else {
			shaderArray[(int)type] << line << "\n";
		}
	}
	m_CreateShader(shaderArray[(int)ShaderType::VERTEX].str(), shaderArray[(int)ShaderType::FRAGMENT].str());
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}

void Shader::Bind()
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind()
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform4f(const string uniformName, GLfloat f1, GLfloat f2, GLfloat f3, GLfloat f4)
{
	int location;
	auto it = m_UniformCache.find(uniformName);
	if (it != m_UniformCache.end()) {
		location = it->second;
	}
	else {
		GLCall(location = glGetUniformLocation(m_RendererID, uniformName.c_str()));
		m_UniformCache.insert(pair<string, unsigned int>(uniformName, location));
	}
	GLCall(glUniform4f(location, f1, f2, f3, f4));
}

void Shader::SetUniform1i(const string uniformName, GLint i1)
{
	int location;
	auto it = m_UniformCache.find(uniformName);
	if (it != m_UniformCache.end()) {
		location = it->second;
	}
	else {
		GLCall(location = glGetUniformLocation(m_RendererID, uniformName.c_str()));
		m_UniformCache.insert(pair<string, unsigned int>(uniformName, location));
	}
	GLCall(glUniform1i(location, i1));
}

void Shader::SetUniformMat4f(const string& uniformName, const glm::mat4& proj)
{
	int location;
	auto it = m_UniformCache.find(uniformName);
	if (it != m_UniformCache.end()) {
		location = it->second;
	}
	else {
		GLCall(location = glGetUniformLocation(m_RendererID, uniformName.c_str()));
		m_UniformCache.insert(pair<string, unsigned int>(uniformName, location));
	}
	GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, &proj[0][0]));
}
