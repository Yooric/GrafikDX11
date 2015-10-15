#include "TextureClass.h"



TextureClass::TextureClass()
{
	m_targaData = 0;
	m_texture = 0;
	m_textureView = 0;
}

TextureClass::TextureClass(const TextureClass &)
{
}


TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* p_device, ID3D11DeviceContext* p_deviceContext, char* p_filename)
{
	bool t_result;
	int t_height, t_width;
	D3D11_TEXTURE2D_DESC t_textureDesc;
	HRESULT t_hResult;
	unsigned int t_rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC t_srvDesc;
	t_result = LoadTarga(p_filename, t_height, t_width);
	if (!t_result)
	{
		return false;
	}

	t_textureDesc.Height = t_height;
	t_textureDesc.Width = t_width;
	t_textureDesc.MipLevels = 0;
	t_textureDesc.ArraySize = 1;
	t_textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	t_textureDesc.SampleDesc.Count = 1;
	t_textureDesc.SampleDesc.Quality = 0;
	t_textureDesc.Usage = D3D11_USAGE_DEFAULT;
	t_textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	t_textureDesc.CPUAccessFlags = 0;
	t_textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;


	t_hResult = p_device->CreateTexture2D(&t_textureDesc, NULL, &m_texture);
	if (FAILED(t_hResult))
	{
		return false;
	}


	t_rowPitch = (t_width * 4)*sizeof(unsigned char);
	p_deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, t_rowPitch, 0);

	t_srvDesc.Format = t_textureDesc.Format;
	t_srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	t_srvDesc.Texture2D.MostDetailedMip = 0;
	t_srvDesc.Texture2D.MipLevels = -1;
	t_hResult = p_device->CreateShaderResourceView(m_texture, &t_srvDesc, &m_textureView);
	if (FAILED(t_hResult))
	{
		return false;
	}

	p_deviceContext->GenerateMips(m_textureView);
	delete[] m_targaData;
	m_targaData = 0;


	return true;
}

void TextureClass::Shutdown()
{
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}
	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = 0;
	}
}

ID3D11ShaderResourceView * TextureClass::GetTexture()
{
	return m_textureView;
}


bool TextureClass::LoadTarga(char* p_filename, int& p_height, int& p_width)
{
	int t_error, t_bpp, t_imageSize, t_index, k;
	FILE* t_filePtr;
	unsigned int t_count;
	TargaHeader t_targaFileHeader;
	unsigned char* t_targaImage;

	t_error = fopen_s(&t_filePtr, p_filename, "rb");
	if (t_error != 0)
	{
		return false;
	}
	t_count = (unsigned int)fread(&t_targaFileHeader, sizeof(TargaHeader), 1, t_filePtr);
	if (t_count != 1)
	{
		return false;
	}
	p_height = (int)t_targaFileHeader.height;
	p_width = (int)t_targaFileHeader.width;
	t_bpp = (int)t_targaFileHeader.bpp;

	//kolla om det e 32 bit kan bara kra på 24
	if (t_bpp != 32)
	{
		return false;
	}

	t_imageSize = p_height * p_width * 4;

	t_targaImage = new unsigned char[t_imageSize];
	if (!t_targaImage)
	{
		return false;
	}

	t_count = (unsigned int)fread(t_targaImage, 1, t_imageSize, t_filePtr);
	if (t_count != t_imageSize)
	{
		return false;
	}
	t_error = fclose(t_filePtr);
	if (t_error != 0)
	{
		return false;
	}

	m_targaData = new unsigned char[t_imageSize];
	if (!m_targaData)
	{
		return false;
	}
	t_index = 0;
	k = (p_width*p_height * 4) - (p_width * 4);
	//kopiera targa image data till targa destinatoin i rätt ordning eftersom den e storad uppOner
	for (size_t i = 0; i < p_height; i++)
	{
		for (size_t j = 0; j < p_width; j++)
		{
			m_targaData[t_index + 0] = t_targaImage[k + 2];//röd
			m_targaData[t_index + 1] = t_targaImage[k + 1];//green
			m_targaData[t_index + 2] = t_targaImage[k + 0];//blå
			m_targaData[t_index + 3] = t_targaImage[k + 3];//alpha

			k += 4;
			t_index += 4;

		}
		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down. säger dem
		k -= (p_width * 8);
	}

	delete[] t_targaImage;
	t_targaImage = 0;
	return true;
	}