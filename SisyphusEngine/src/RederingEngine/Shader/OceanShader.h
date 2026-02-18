#pragma once
#include "ShaderBuffers.h"
// Framework
#include "Shader.h"


class OceanShader : public Shader {
public:
	OceanShader();
	OceanShader(const OceanShader& other) = delete;
	virtual ~OceanShader() override = default;

	virtual bool Init(ID3D11Device*, HWND,
		const std::wstring&, const std::wstring&) override;
	virtual void SetShaders(ID3D11DeviceContext*) override;

public:
	void SetConstantBuffers(ID3D11DeviceContext*, ID3D11Buffer*);
	bool UpdateOceanBuffer(ID3D11DeviceContext* context, const OceanBuffer& data);

private:
	bool InitBuffers(ID3D11Device*);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_oceanBuffer;
}; // OceanShader
