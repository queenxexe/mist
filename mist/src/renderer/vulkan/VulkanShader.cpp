#include "VulkanShader.hpp"
#include <glslang/SPIRV/GlslangToSpv.h>
#include <spirv_cross/spirv_hlsl.hpp>
#include <filesystem>
#include "renderer/vulkan/VulkanContext.hpp"
#include "Debug.hpp"
#include "VulkanDebug.hpp"
#include "Utils.hpp"

namespace mist {
	static TBuiltInResource GetDefaultResources() {
		TBuiltInResource resources = {};
		resources.maxLights = 32;
		resources.maxClipPlanes = 6;
		resources.maxTextureUnits = 32;
		resources.maxTextureCoords = 32;
		resources.maxVertexAttribs = 64;
		resources.maxVertexUniformComponents = 4096;
		resources.maxVaryingFloats = 64;
		resources.maxVertexTextureImageUnits = 32;
		resources.maxCombinedTextureImageUnits = 80;
		resources.maxTextureImageUnits = 32;
		resources.maxFragmentUniformComponents = 4096;
		resources.maxDrawBuffers = 32;
		resources.maxVertexUniformVectors = 128;
		resources.maxVaryingVectors = 8;
		resources.maxFragmentUniformVectors = 16;
		resources.maxVertexOutputVectors = 16;
		resources.maxFragmentInputVectors = 15;
		resources.minProgramTexelOffset = -8;
		resources.maxProgramTexelOffset = 7;
		resources.maxClipDistances = 8;
		resources.maxComputeWorkGroupCountX = 65535;
		resources.maxComputeWorkGroupCountY = 65535;
		resources.maxComputeWorkGroupCountZ = 65535;
		resources.maxComputeWorkGroupSizeX = 1024;
		resources.maxComputeWorkGroupSizeY = 1024;
		resources.maxComputeWorkGroupSizeZ = 64;
		resources.maxComputeUniformComponents = 1024;
		resources.maxComputeTextureImageUnits = 16;
		resources.maxComputeImageUniforms = 8;
		resources.maxComputeAtomicCounters = 8;
		resources.maxComputeAtomicCounterBuffers = 1;
		resources.maxVaryingComponents = 60;
		resources.maxVertexOutputComponents = 64;
		resources.maxGeometryInputComponents = 64;
		resources.maxGeometryOutputComponents = 128;
		resources.maxFragmentInputComponents = 128;
		resources.maxImageUnits = 8;
		resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
		resources.maxCombinedShaderOutputResources = 8;
		resources.maxImageSamples = 0;
		resources.maxVertexImageUniforms = 0;
		resources.maxTessControlImageUniforms = 0;
		resources.maxTessEvaluationImageUniforms = 0;
		resources.maxGeometryImageUniforms = 0;
		resources.maxFragmentImageUniforms = 8;
		resources.maxCombinedImageUniforms = 8;
		resources.maxGeometryTextureImageUnits = 16;
		resources.maxGeometryOutputVertices = 256;
		resources.maxGeometryTotalOutputComponents = 1024;
		resources.maxGeometryUniformComponents = 1024;
		resources.maxGeometryVaryingComponents = 64;
		resources.maxTessControlInputComponents = 128;
		resources.maxTessControlOutputComponents = 128;
		resources.maxTessControlTextureImageUnits = 16;
		resources.maxTessControlUniformComponents = 1024;
		resources.maxTessControlTotalOutputComponents = 4096;
		resources.maxTessEvaluationInputComponents = 128;
		resources.maxTessEvaluationOutputComponents = 128;
		resources.maxTessEvaluationTextureImageUnits = 16;
		resources.maxTessEvaluationUniformComponents = 1024;
		resources.maxTessPatchComponents = 120;
		resources.maxPatchVertices = 32;
		resources.maxTessGenLevel = 64;
		resources.maxViewports = 16;
		resources.maxVertexAtomicCounters = 0;
		resources.maxTessControlAtomicCounters = 0;
		resources.maxTessEvaluationAtomicCounters = 0;
		resources.maxGeometryAtomicCounters = 0;
		resources.maxFragmentAtomicCounters = 8;
		resources.maxCombinedAtomicCounters = 8;
		resources.maxAtomicCounterBindings = 1;
		resources.maxVertexAtomicCounterBuffers = 0;
		resources.maxTessControlAtomicCounterBuffers = 0;
		resources.maxTessEvaluationAtomicCounterBuffers = 0;
		resources.maxGeometryAtomicCounterBuffers = 0;
		resources.maxFragmentAtomicCounterBuffers = 1;
		resources.maxCombinedAtomicCounterBuffers = 1;
		resources.maxAtomicCounterBufferSize = 16384;
		resources.maxTransformFeedbackBuffers = 4;
		resources.maxTransformFeedbackInterleavedComponents = 64;
		resources.maxCullDistances = 8;
		resources.maxCombinedClipAndCullDistances = 8;
		resources.maxSamples = 4;
		resources.limits.nonInductiveForLoops = 1;
		resources.limits.whileLoops = 1;
		resources.limits.doWhileLoops = 1;
		resources.limits.generalUniformIndexing = 1;
		resources.limits.generalAttributeMatrixVectorIndexing = 1;
		resources.limits.generalVaryingIndexing = 1;
		resources.limits.generalSamplerIndexing = 1;
		resources.limits.generalVariableIndexing = 1;
		resources.limits.generalConstantMatrixVectorIndexing = 1;

		return resources;
	}

	static EShLanguage ShaderTypeFromString(const std::string& type) {
		if (type == "vertex" || type == "vert") return EShLangVertex;
		if (type == "fragment" || type == "frag" || type == "pixel") return EShLangFragment;
		if (type == "compute" || type == "comp") return EShLangCompute;
		if (type == "geometry" || type == "geo") return EShLangGeometry;

		MIST_ASSERT(false, "Unknown shader type, defaulting to vertex");
		return EShLangVertex;
	}

	static VkShaderStageFlagBits EShLanguageToVkStageFlags(EShLanguage stage) {
		switch (stage) {
		case EShLangVertex:         return VK_SHADER_STAGE_VERTEX_BIT;
		case EShLangTessControl:    return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		case EShLangTessEvaluation: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		case EShLangGeometry:       return VK_SHADER_STAGE_GEOMETRY_BIT;
		case EShLangFragment:       return VK_SHADER_STAGE_FRAGMENT_BIT;
		case EShLangCompute:        return VK_SHADER_STAGE_COMPUTE_BIT;
		case EShLangRayGen:         return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		case EShLangIntersect:      return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
		case EShLangAnyHit:         return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
		case EShLangClosestHit:     return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		case EShLangMiss:           return VK_SHADER_STAGE_MISS_BIT_KHR;
		case EShLangCallable:       return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
		case EShLangTask:           return VK_SHADER_STAGE_TASK_BIT_EXT;
		case EShLangMesh:           return VK_SHADER_STAGE_MESH_BIT_EXT;
		default:
			MIST_ASSERT(false, "Can not convert EShLanguage enum to Vulkan shader stage");
			return VK_SHADER_STAGE_ALL;
		}
	}

	VulkanShader::VulkanShader(const std::string& path) {
		shaderName = std::filesystem::path(path).stem().string();
		std::string src = Utils::ReadFile(path);
		std::unordered_map<EShLanguage, std::string> shaderSources = PreProcess(src);

		glslang::InitializeProcess();
		for (std::pair<EShLanguage, std::string> src : shaderSources) {
			std::vector<uint32_t> spirv = ConvertGLSLToSPIRV(src.second, src.first);
			Compile(spirv, src.first);
		}
		glslang::FinalizeProcess();

		//VulkanContext& context = VulkanContext::GetContext();
		//context.pipeline.CreateGraphicsPipeline(this);

		MIST_INFO(std::string("Loaded shader and created graphics pipeline for: ") + shaderName);
	}

	VulkanShader::VulkanShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) : shaderName(name) {
		std::unordered_map<EShLanguage, std::string> shaderSources;
		shaderSources[EShLangVertex] = vertexSrc;
		shaderSources[EShLangFragment] = fragmentSrc;

		glslang::InitializeProcess();
		for (std::pair<EShLanguage, std::string> src : shaderSources) {
			std::vector<uint32_t> spirv = ConvertGLSLToSPIRV(src.second, src.first);
			Compile(spirv, src.first);
		}
		glslang::FinalizeProcess();

		//VulkanContext& context = VulkanContext::GetContext();
		//context.pipeline.CreateGraphicsPipeline(this);

		MIST_INFO(std::string("Loaded shader and created graphics pipeline for: ") + name);
	}

	VulkanShader::~VulkanShader() {
		Clear();
	}

	void VulkanShader::Clear() {
		VulkanContext& context = VulkanContext::GetContext();
		for (std::pair<std::string, InputShaderResource> pair : shaderInputs) {
			vkDestroyShaderModule(context.GetDevice(), pair.second.shaderModule, context.GetAllocationCallbacks());
		}

		for (std::pair<std::string, UBOShaderResource> pair : shaderUbos) {
			vkDestroyShaderModule(context.GetDevice(), pair.second.shaderModule, context.GetAllocationCallbacks());
		}
		
		for (std::pair<std::string, SampledImageShaderResources> pair : shaderSampledImages) {
			vkDestroyShaderModule(context.GetDevice(), pair.second.shaderModule, context.GetAllocationCallbacks());
		}
	}

	std::unordered_map<EShLanguage, std::string> VulkanShader::PreProcess(const std::string& src) {
		std::unordered_map<EShLanguage, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = src.find(typeToken, 0);
		while (pos != std::string::npos) {
			size_t eol = src.find_first_of("\r\n", pos);
			MIST_ASSERT(eol != std::string::npos, "Syntax Error.");

			size_t begin = pos + typeTokenLength + 1;
			std::string type = src.substr(begin, eol - begin);

			size_t nextLinePos = src.find_first_not_of("\r\n", eol);
			pos = src.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] = src.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? src.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	std::vector<uint32_t> VulkanShader::ConvertGLSLToSPIRV(const std::string& src, EShLanguage stage) {
		const char* shaderStrings[1];
		shaderStrings[0] = src.c_str();

		glslang::TShader shader(stage);
		shader.setEnvInput(glslang::EShSourceGlsl, stage, glslang::EShClientVulkan, 130);
		shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_3);
		shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_3);

		shader.setStrings(shaderStrings, 1);

		TBuiltInResource resources = GetDefaultResources();
		EShMessages messages = EShMsgDefault;

		if (!shader.parse(&resources, 100, false, messages)) {
			MIST_ASSERT(false, std::string("Failed to parse GLSL: ") + shader.getInfoLog());
			return {};
		}

		glslang::TProgram program;
		program.addShader(&shader);

		if (!program.link(messages)) {
			MIST_ASSERT(false, std::string("Failed to parse GLSL: ") + shader.getInfoLog());
			return {};
		}

		std::vector<uint32_t> spirv;
		glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);

		return spirv;
	}

	uint32_t VulkanShader::CalculateSize(const spirv_cross::Compiler& compiler, const spirv_cross::SPIRType& type) {
		uint32_t size = 0;

		switch (type.basetype) {
		case spirv_cross::SPIRType::Boolean:
			size = sizeof(bool);
			break;
		case spirv_cross::SPIRType::Char:
		case spirv_cross::SPIRType::SByte:
		case spirv_cross::SPIRType::UByte:
			size = sizeof(char);
			break;
		case spirv_cross::SPIRType::UShort:
		case spirv_cross::SPIRType::Short:
			size = sizeof(short);
			break;
		case spirv_cross::SPIRType::UInt:
		case spirv_cross::SPIRType::Int:
			size = sizeof(int);
			break;
		case spirv_cross::SPIRType::UInt64:
		case spirv_cross::SPIRType::Int64:
			size = sizeof(int64_t);
			break;
		case spirv_cross::SPIRType::AtomicCounter:
			size = sizeof(uint32_t);
			break;
		case spirv_cross::SPIRType::Half:
			size = sizeof(uint16_t);
		case spirv_cross::SPIRType::Float:
			size = sizeof(float);
			break;
		case spirv_cross::SPIRType::Double:
			size = sizeof(double);
			break;
		case spirv_cross::SPIRType::Struct:
			for (const auto& member : type.member_types) {
				const spirv_cross::SPIRType& memberType = compiler.get_type(member);
				size += CalculateSize(compiler, memberType);
			}
			break;
		default:
			MIST_ERROR("Unsupported type");
		}

		if (type.vecsize > 1)
			size *= type.vecsize;
		if (type.columns > 1)
			size *= type.columns;

		return size;
	}

	VkFormat VulkanShader::GetDescriptionFormat(const spirv_cross::Compiler& compiler, const spirv_cross::SPIRType type) {
		if (type.basetype == spirv_cross::SPIRType::Float) {
			if (type.vecsize == 1) return VK_FORMAT_R32_SFLOAT;
			if (type.vecsize == 2) return VK_FORMAT_R32G32_SFLOAT;
			if (type.vecsize == 3) return VK_FORMAT_R32G32B32_SFLOAT;
			if (type.vecsize == 4) return VK_FORMAT_R32G32B32A32_SFLOAT;
		}

		if (type.basetype == spirv_cross::SPIRType::Int) {
			if (type.vecsize == 1) return VK_FORMAT_R32_SINT;
			if (type.vecsize == 2) return VK_FORMAT_R32G32_SINT;
			if (type.vecsize == 3) return VK_FORMAT_R32G32B32_SINT;
			if (type.vecsize == 4) return VK_FORMAT_R32G32B32A32_SINT;
		}

		if (type.basetype == spirv_cross::SPIRType::UInt) {
			if (type.vecsize == 1) return VK_FORMAT_R32_UINT;
			if (type.vecsize == 2) return VK_FORMAT_R32G32_UINT;
			if (type.vecsize == 3) return VK_FORMAT_R32G32B32_UINT;
			if (type.vecsize == 4) return VK_FORMAT_R32G32B32A32_UINT;
		}

		if (type.basetype == spirv_cross::SPIRType::Boolean) return VK_FORMAT_R32_SINT;

		if (type.basetype == spirv_cross::SPIRType::Struct) {
			if (type.member_types.empty()) return VK_FORMAT_UNDEFINED;
			return GetDescriptionFormat(compiler, compiler.get_type(type.member_types[0]));
		}

		MIST_WARN("Couldnt get description format");
		return VK_FORMAT_R32_SFLOAT;
	}

	VkShaderModule VulkanShader::CreateShaderModule(const std::vector<uint32_t>& spirv) {
		VkShaderModuleCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		info.codeSize = spirv.size() * sizeof(uint32_t);
		info.pCode = spirv.data();

		VulkanContext& context = VulkanContext::GetContext();
		VkShaderModule module;
		CheckVkResult(vkCreateShaderModule(context.GetDevice(), &info, context.GetAllocationCallbacks(), &module));
		return module;
	}

	void VulkanShader::Compile(const std::vector<uint32_t>& spirv, EShLanguage stage) {
		spirv_cross::CompilerGLSL compiler(spirv);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		uint32_t inputStride = 0;
		for (const spirv_cross::Resource& res : resources.stage_inputs) {
			inputStride += CalculateSize(compiler, compiler.get_type(res.type_id));
		}

		for (const spirv_cross::Resource& inputs : resources.stage_inputs) {
			InputShaderResource res;
			res.binding = compiler.get_decoration(inputs.id, spv::DecorationBinding);
			res.location = compiler.get_decoration(inputs.id, spv::DecorationLocation);
			res.format = GetDescriptionFormat(compiler, compiler.get_type(inputs.type_id));
			res.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			res.flags = EShLanguageToVkStageFlags(stage);
			res.shaderModule = CreateShaderModule(spirv);
			
			uint32_t offset = 0;
			for (const spirv_cross::Resource& i : resources.stage_inputs) {
				if (compiler.get_decoration(i.id, spv::DecorationLocation) < res.location)
					offset += CalculateSize(compiler, compiler.get_type(i.type_id));
			}

			res.offset = offset;
			res.stride = inputStride;

			shaderInputs[inputs.name] = res;
		}

		for (const spirv_cross::Resource& ubo : resources.uniform_buffers) {
			UBOShaderResource res;
			res.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			res.binding = compiler.get_decoration(ubo.id, spv::DecorationBinding);
			res.offset = compiler.get_decoration(ubo.id, spv::DecorationOffset);
			res.count = 1;
			res.flags = EShLanguageToVkStageFlags(stage);
			res.shaderModule = CreateShaderModule(spirv);

			uint32_t size = 0;
			const spirv_cross::SPIRType& type = compiler.get_type(ubo.base_type_id);
			for (uint32_t i = 0; i < type.member_types.size(); ++i) {
				const spirv_cross::SPIRType& memberType = compiler.get_type(type.member_types[i]);
				uint32_t memberSize = CalculateSize(compiler, memberType);
				uint32_t offset = (uint32_t)compiler.get_member_decoration(ubo.base_type_id, i, spv::DecorationOffset);
				size = std::max(size, offset + memberSize);
			}
			res.size = size;
			
			shaderUbos[ubo.name] = res;
		}

		for (const spirv_cross::Resource& pushConstant : resources.push_constant_buffers) {
			for (spirv_cross::BufferRange& bufferRange : compiler.get_active_buffer_ranges(pushConstant.id)) {
				std::string name = compiler.get_member_name(pushConstant.base_type_id, bufferRange.index);
				if (shaderPushConstants.contains(name)) {
					shaderPushConstants[name].flags |=  EShLanguageToVkStageFlags(stage);
					continue;
				}

				PushConstantResource res;
				res.offset = bufferRange.offset;
				res.size = bufferRange.range;
				res.flags = EShLanguageToVkStageFlags(stage);
				shaderPushConstants[name] = res;
			}
		}
		
		for (const spirv_cross::Resource& sampled : resources.sampled_images) {
			SampledImageShaderResources res;
			res.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			res.binding = compiler.get_decoration(sampled.id, spv::DecorationBinding);
			res.count = 1;
			res.flags = EShLanguageToVkStageFlags(stage);
			res.shaderModule = CreateShaderModule(spirv);
		
			shaderSampledImages[sampled.name] = res;
		}
	}

	void VulkanShader::Bind(const uint8_t renderDataId) const {
		VulkanContext& context = VulkanContext::GetContext();
		Ref<VulkanRenderData> data = context.GetRenderData(renderDataId);
		if (!data->pipeline.HasPipeline(shaderName))
			data->CreateGraphicsPipeline(this);

		vkCmdBindPipeline(
			context.GetCurrentFrameCommandBuffer(),
			VK_PIPELINE_BIND_POINT_GRAPHICS, 	// TODO: make a way to detect correct bind point, will probably just have to hold a reference if cant defer
			data->pipeline.GetGraphicsPipeline(shaderName)
		);
	}

	void VulkanShader::Unbind(const uint8_t renderDataId) const {}

	void VulkanShader::SetUniformData(const uint8_t renderDataId, const std::string& name, const int size, const void* data) {
		VulkanContext& context = VulkanContext::GetContext();
		PushConstantResource& res = shaderPushConstants[name];
		Ref<VulkanRenderData> renderData = context.GetRenderData(renderDataId);
		
#if DEBUG
		MIST_ASSERT(shaderPushConstants.contains(name), std::string("Invalid push constants name passed: " + name));

		if (res.size != size)
			MIST_WARN(std::string("Expected size of: %d, you passed %d", res.size, size));
#endif

		vkCmdPushConstants(
			context.GetCurrentFrameCommandBuffer(), 
			renderData->pipeline.GetGraphicsPipelineLayout(shaderName),
			res.flags,
			res.offset,
			res.size,
			data
		);
	}
}