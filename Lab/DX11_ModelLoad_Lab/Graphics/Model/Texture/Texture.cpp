// Graphics/Model/Texture.cpp
#include "Texture.h"
#include "Model/Loader/TextureLoader.h"
// Common
#include "EngineHelper.h"

/* defualt */
///////////////////////////////////////////////////////////////////

Texture::Texture() {};

Texture::~Texture() {};

bool Texture::Init(ID3D11Device* device, ID3D11DeviceContext* context, const std::string& path)
{
	return TextureLoader::CreateTextureFromFile(device, context, path, m_srv.GetAddressOf());
} // Init


void Texture::Bind(ID3D11DeviceContext* context, UINT slot)
{
	if (m_srv == nullptr)
	{
		EngineHelper::SuccessCheck(false, "Texture::Bind 실패: m_srv 가 nullptr 임");
		return;
	}

	context->PSSetShaderResources(slot, 1, m_srv.GetAddressOf());
} // Bind