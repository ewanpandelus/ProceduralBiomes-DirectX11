#include "pch.h"
#include "DepthTexture.h"

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> DepthTexture::GenerateDepthTexture(ID3D11Device* device)
{
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthBuffer;
    {
        D3D11_TEXTURE2D_DESC depthTexDesc;
        ZeroMemory(&depthTexDesc, sizeof(depthTexDesc));
        depthTexDesc.Width = 1920;
        depthTexDesc.Height = 1080;
        depthTexDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
        depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        depthTexDesc.MipLevels = 1;
        depthTexDesc.ArraySize = 1;
        depthTexDesc.SampleDesc.Count = 1;
        depthTexDesc.SampleDesc.Quality = 0;
        depthTexDesc.CPUAccessFlags = 0;
        depthTexDesc.MiscFlags = 0;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> depthTexture;
        HRESULT hresult = device->CreateTexture2D(&depthTexDesc, nullptr, &depthTexture);

        D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc;
        ZeroMemory(&DSVDesc, sizeof(DSVDesc));
        DSVDesc.Format = DXGI_FORMAT_D32_FLOAT;
        DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        DSVDesc.Texture2D.MipSlice = 0;
        ID3D11DepthStencilView* mDepthStencilView;

        hresult = device->CreateDepthStencilView(depthTexture.Get(), &DSVDesc, &mDepthStencilView);

        D3D11_SHADER_RESOURCE_VIEW_DESC gbDepthTexDesc;
        ZeroMemory(&gbDepthTexDesc, sizeof(gbDepthTexDesc));
        gbDepthTexDesc.Format = DXGI_FORMAT_R32_FLOAT;
        gbDepthTexDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        gbDepthTexDesc.Texture2D.MipLevels = 1;
        gbDepthTexDesc.Texture2D.MostDetailedMip = 0;

        device->CreateShaderResourceView(depthTexture.Get(), &gbDepthTexDesc, &depthBuffer);
    }

    return depthBuffer;
}
