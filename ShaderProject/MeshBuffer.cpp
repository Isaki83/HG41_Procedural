#include "MeshBuffer.h"

MeshBuffer::MeshBuffer(const Description& desc)
	: m_pVtxBuffer(NULL), m_vtxSize(0), m_vtxCount(0)
	, m_pIdxBuffer(NULL), m_idxSize(0), m_idxCount(0)
{
	HRESULT hr = E_FAIL;
	hr = CreateVertexBuffer(desc.pVtx, desc.vtxSize, desc.vtxCount, desc.isWrite);
	if (desc.pIdx)
	{
		hr = CreateIndexBuffer(desc.pIdx, desc.idxSize, desc.idxCount);
	}

	m_topology = desc.topology;
}
MeshBuffer::~MeshBuffer()
{
	SAFE_RELEASE(m_pIdxBuffer);
	SAFE_RELEASE(m_pVtxBuffer);
}

HRESULT MeshBuffer::CreateVertexBuffer(const void* pVtx, UINT size, UINT count, bool isWrite)
{
	//--- 作成するバッファの情報
	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.ByteWidth = size * count;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	if (isWrite)
	{
		bufDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	//--- バッファの初期値を設定
	D3D11_SUBRESOURCE_DATA subResource = {};
	subResource.pSysMem = pVtx;

	//--- 頂点バッファの作成
	HRESULT hr;
	ID3D11Device* pDevice = GetDevice();
	hr = pDevice->CreateBuffer(&bufDesc, &subResource, &m_pVtxBuffer);

	//--- そのほかの情報を設定
	if (SUCCEEDED(hr))
	{
		m_vtxSize = size;
		m_vtxCount = count;
	}
	return hr;
}

HRESULT MeshBuffer::CreateIndexBuffer(const void* pIdx, UINT size, UINT count)
{
	// インデックスサイズの確認
	switch (size)
	{
	default:
		return E_FAIL;
	case 2:
	case 4:
		break;
	}

	// バッファの情報を設定
	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.ByteWidth = size * count;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	// バッファの初期データ
	D3D11_SUBRESOURCE_DATA subResource = {};
	subResource.pSysMem = pIdx;

	// インデックスバッファ生成
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr;
	hr = pDevice->CreateBuffer(&bufDesc, &subResource, &m_pIdxBuffer);
	if (SUCCEEDED(hr))
	{
		m_idxSize = size;
		m_idxCount = count;
	}

	return hr;
}

void MeshBuffer::Draw(int count)
{
	ID3D11DeviceContext* pContext = GetContext();
	UINT stride = m_vtxSize;
	UINT offset = 0;

	pContext->IASetPrimitiveTopology(m_topology);
	pContext->IASetVertexBuffers(0, 1, &m_pVtxBuffer, &stride, &offset);

	// 描画
	if (m_idxCount > 0)
	{
		DXGI_FORMAT format;
		switch (m_idxSize)
		{
		case 4: format = DXGI_FORMAT_R32_UINT; break;
		case 2: format = DXGI_FORMAT_R16_UINT; break;
		}
		pContext->IASetIndexBuffer(m_pIdxBuffer, format, 0);
		pContext->DrawIndexed(count ? count : m_idxCount, 0, 0);
	}
	else
	{
		// 頂点バッファのみで描画
		pContext->Draw(count ? count : m_vtxCount, 0);
	}

}

HRESULT MeshBuffer::Write(void* pVtx)
{
	HRESULT hr;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pContext = GetContext();
	D3D11_MAPPED_SUBRESOURCE mapResource;

	// データコピー
	hr = pContext->Map(m_pVtxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
	if (SUCCEEDED(hr))
	{
		rsize_t size = m_vtxCount * m_vtxSize;
		memcpy_s(mapResource.pData, size, pVtx, size);
		pContext->Unmap(m_pVtxBuffer, 0);
	}
	return hr;
}