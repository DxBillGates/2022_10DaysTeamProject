#pragma once
#include "IShader.h"

#include <d3d12.h>

namespace GE
{
	enum class GraphicsPipelineBlendMode
	{
		BLENDMODE_ADD,
		BLENDMODE_SUB,
		BLENDMODE_INV,
		BLENDMODE_ALPHA
	};

	enum class GraphicsPipelinePrimitiveTopolotyType
	{
		UNDIFINED,
		POINT,
		LINE,
		TRIANGLE,
		PATCH,
	};

	enum class GraphicsPipelineComparison
	{
		COMPARISON_FUNC_NEVER = 1,
		COMPARISON_FUNC_LESS,
		COMPARISON_FUNC_EQUAL,
		COMPARISON_FUNC_LESS_EQUAL,
		COMPARISON_FUNC_GREATER,
		COMPARISON_FUNC_NOT_EQUAL,
		COMPARISON_FUNC_GREATER_EQUAL,
		COMPARISON_FUNC_ALWAYS,
	};

	enum class GraphicsPipelineCullingMode
	{
		CULL_MODE_NONE = 1,
		CULL_MODE_FRONT = 2,
		CULL_MODE_BACK = 3
	};

	struct GraphicsPipelineInfo
	{
		GraphicsPipelineBlendMode blendMode = GraphicsPipelineBlendMode::BLENDMODE_ALPHA;
		GraphicsPipelinePrimitiveTopolotyType topologyType = GraphicsPipelinePrimitiveTopolotyType::TRIANGLE;
		bool isUseDepthClip = true;
		bool isWriteDepthStencil = true;
		int renderTargetCount = 1; 
		bool isUseAlphaToCoverage = false;
		GraphicsPipelineComparison depthComparison = GraphicsPipelineComparison::COMPARISON_FUNC_LESS;
		GraphicsPipelineCullingMode cullMode = GraphicsPipelineCullingMode::CULL_MODE_BACK;
	};

	enum class GraphicsPipelineInputLayout
	{
		POSITION,
		UV,
		NORMAL,
		COLOR,
	};

	struct ShaderStages
	{
		IShader* vShader;
		IShader* gShader;
		IShader* dShader;
		IShader* hShader;
		IShader* pShader;
	};
}
