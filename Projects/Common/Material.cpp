#include "Material.h"
#include "IMesh.h"
#include "Shader.h"
#include "World.h"

Material::Material()
	: m_Name("default"), m_Color(1.0f)
{
}

Material::Material(const std::string& name)
	: m_Name(name), m_Color(1.0f)
{
}

Material::~Material()
{
}

void Material::SetTexture(ITexture* texture)
{
	m_HasTexture = 1;
	switch (texture->GetTextureType()) {
	case ETextureChannels::SpecularMap:
		m_TextureSpec = texture;
		break;
	case ETextureChannels::AmbientOcclusionMap:
		m_TextureAO = texture;
		break;
	case ETextureChannels::NormalMap:
		m_TextureNorm = texture;
		break;
	case ETextureChannels::AlbedoMap:
	default:
		m_TextureDiff = texture;
	}

}

ITexture* Material::GetTexture(ETextureChannels channelMap)
{
	ITexture* retTexture = m_TextureDiff;

	switch (channelMap) {
	case ETextureChannels::SpecularMap:
		retTexture = m_TextureSpec;
		break;
	case ETextureChannels::AmbientOcclusionMap:
		retTexture = m_TextureAO;
		break;
	case ETextureChannels::NormalMap:
		retTexture = m_TextureNorm;
		break;
	case ETextureChannels::AlbedoMap:
	default:
		break;
	}

	return retTexture;
}

void Material::SetShader(IShader* shader)
{
	m_Shader = shader;
}

IShader* Material::GetShader()
{
	return m_Shader;
}

glm::vec3 Material::GetColor()
{
	return m_Color;
}

void Material::SetColor(const glm::vec3& color)
{
	m_Color = color;
}

void Material::SetName(const std::string name)
{
	m_Name = name;
}

std::string Material::GetName()
{
	return m_Name;
}

void Material::SetReflections(ReflectionType r)
{
	switch (r)
	{
	case ReflectionType::Ambient: m_HasAmbient = 1; break;
	case ReflectionType::Diffuse: m_HasDiffuse = 1; break;
	case ReflectionType::Specular: m_HasSpecular = 1; break;
	case ReflectionType::Phong: m_HasDiffuse = 1; m_HasAmbient = 1; m_HasSpecular = 1; break;
	}
}


void Material::RenderPre()
{
	if (m_Shader != nullptr)
		m_Shader->Bind();

	if (m_TextureDiff != nullptr) {
		m_TextureDiff->RenderPre();
		m_Shader->SetUniform1i("hasTexture", m_HasTexture);
		m_Shader->SetUniform1i("diffuseMap1", 0);
	}
	if (m_TextureSpec != nullptr) {
		m_TextureSpec->RenderPre();
		m_Shader->SetUniform1i("specularMap1", 1);
	}
	if (m_TextureAO != nullptr) {
		m_TextureAO->RenderPre();
		m_Shader->SetUniform1i("ambientMap1", 2);
	}
	if (m_TextureNorm != nullptr) {
		m_TextureNorm->RenderPre();
		m_Shader->SetUniform1i("normalMap1", 3);
	}
	if (m_TextureHeightMap != nullptr) {
		m_TextureHeightMap->RenderPre();
		m_Shader->SetUniform1i("heightMap1", 2);
	}
}

void Material::Render()
{
	RenderPre();
	Scene* activeScene = World::Get().GetActiveScene();
	Camera* sceneCamera = activeScene->GetSceneCamera();
	sceneCamera->UpdateMatrix(m_Shader);
	auto lights = activeScene->GetSceneLightSources();

	// Keep track of how many of each Type of textures we have

	if (m_TextureDiff != nullptr) {
		m_TextureDiff->Render(m_Shader);
	}

	m_Shader->SetUniform1i("hasAmbient", m_HasAmbient);
	m_Shader->SetUniform1i("hasDiffuse", m_HasDiffuse);
	m_Shader->SetUniform1i("hasSpecular", m_HasSpecular);
	m_Shader->SetUniform1i("hasTexture", m_HasTexture);

	m_Shader->SetUniform1i("hasDiffuseMap", m_HasDiffuseMap);
	m_Shader->SetUniform1i("hasSpecularMap", m_HasSpecularMap);

	m_Shader->SetUniform3f("material.ambient", Ambient);
	m_Shader->SetUniform3f("material.diffuse", Diffuse);
	m_Shader->SetUniform3f("material.specular", Specular);
	m_Shader->SetUniform1f("material.shininess", Shininess);
	m_Shader->SetUniform3f("material.color", m_Color);


	m_Shader->SetUniform3f("camPos", sceneCamera->Position);

	m_Shader->SetUniform1i("numPointlights", (int)lights.size());
	for (auto& l : lights)
	{
		int i = 0;
		Light* light = l.second;

		switch (light->Type)
		{
		case LightSourceType::PointLight:
			m_Shader->SetUniform3f("light.position", light->Position);
			m_Shader->SetUniform3f("light.color", light->Color);
			m_Shader->SetUniform3f("pointLights[" + std::to_string(i) + "].position", light->Position);
			m_Shader->SetUniform1f("pointLights[" + std::to_string(i) + "].constant", light->Constant);
			m_Shader->SetUniform1f("pointLights[" + std::to_string(i) + "].linear", light->Linear);
			m_Shader->SetUniform1f("pointLights[" + std::to_string(i) + "].quadratic", light->Quadratic);
			m_Shader->SetUniform3f("pointLights[" + std::to_string(i) + "].ambient", Ambient);
			m_Shader->SetUniform3f("pointLights[" + std::to_string(i) + "].diffuse", Diffuse);
			m_Shader->SetUniform3f("pointLights[" + std::to_string(i) + "].specular", Specular);
			m_Shader->SetUniform1f("material.shininess", Shininess);
			break;

		case LightSourceType::DirectionalLight:
			m_Shader->SetUniform3f("light.position", light->Position);
			m_Shader->SetUniform3f("light.color", light->Color);
			m_Shader->SetUniform3f("dirLight.direction", light->Direction);
			m_Shader->SetUniform3f("dirLight.ambient", Ambient);
			m_Shader->SetUniform3f("dirLight.diffuse", Diffuse);
			m_Shader->SetUniform3f("dirLight.specular", Specular);
			m_Shader->SetUniform1f("material.shininess", Shininess);
			break;
		case LightSourceType::SpotLight:
			break;
		}
		i++;
	}
}

void Material::RenderPost()
{
	if (m_TextureDiff != nullptr) {
		m_TextureDiff->Unbind();
	}

	if (m_TextureSpec != nullptr) {
		m_TextureSpec->Unbind();
	}

	if (m_Shader != nullptr) {
		m_Shader->Unbind();
	}
}