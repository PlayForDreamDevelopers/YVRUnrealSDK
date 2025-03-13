// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#include "YvrPreviewLayer.h"

void SetLayerDescMember(FYvrPreviewLayer& Layer, const IStereoLayers::FLayerDesc& InLayerDesc)
{
	if (InLayerDesc.Texture != Layer.LayerDesc.Texture)
	{
		Layer.bUpdateTexture = true;
	}
	Layer.LayerDesc = InLayerDesc;
}

bool GetLayerDescMember(const FYvrPreviewLayer& Layer, IStereoLayers::FLayerDesc& OutLayerDesc)
{
	OutLayerDesc = Layer.LayerDesc;
	return true;
}

void MarkLayerTextureForUpdate(FYvrPreviewLayer& Layer)
{
	Layer.bUpdateTexture = true;
}


