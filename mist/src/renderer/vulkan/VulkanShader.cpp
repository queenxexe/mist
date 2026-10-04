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

		MIST_WARN("Unknown shader type: {}, defaulting to vertex", type);
		return EShLangVertex;
	}

	static CullMode CullModeFromString(const std::string& mode) {
		if (mode == "back") return CullMode::CULL_BACK;
		if (mode == "front") return CullMode::CULL_FRONT;
		if (mode == "off") return CullMode::CULL_OFF;
		MIST_WARN("Unknown cull mode: {}, defaulting to back", mode);
		return CullMode::CULL_BACK;
	}

	static bool BoolFromString(const std::string& s) {
		if (s == "true") return true;
		if (s == "false") return false;
		MIST_WARN("Unknown bool: {}, defaulting to false", s);
		return false;
	}

	static VkShaderStageFlagBits EShLanguageToVkStageFlags(const EShLanguage& stage) {
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

	static std::string GetStringFromEshLang(const EShLanguage& stage) {
		switch (stage) {
		case EShLangVertex:         return "VERTEX STAGE";
		case EShLangTessControl:    return "TESSELATION CONTROL";
		case EShLangTessEvaluation: return "TESSELATION EVAL";
		case EShLangGeometry:       return "GEOMETRY STAGE";
		case EShLangFragment:       return "FRAGMENT STAGE";
		case EShLangCompute:        return "COMPUTE STAGE";
		case EShLangRayGen:         return "RAY GEN";
		case EShLangIntersect:      return "STAGE INTERSECT";
		case EShLangAnyHit:         return "ANY HIT";
		case EShLangClosestHit:     return "CLOSEST HIT";
		case EShLangMiss:           return "MISS";
		case EShLangCallable:       return "CALLABLE";
		case EShLangTask:           return "TASK";
		case EShLangMesh:           return "MESH";
		default:
			MIST_ASSERT(false, "Can not convert EShLanguage enum to String");
			return "INVALID STAGE";
		}
	}

	static std::string FindTokenResult(const std::string& src, const char* token) {
		size_t len = strlen(token);
		size_t pos = src.find(token, 0);
		if (pos != std::string::npos) {
			size_t eol = src.find_first_of("\r\n", pos);
			size_t begin = pos + len + 1;
			return src.substr(begin, eol - begin);
		}
		return "";
	}

	static void AddIncludes(const std::string& pathRelativeTo, std::string& src) {
		// Make sure all includes are after any #version within a shader if there is one
		size_t insertPos = 0;
		size_t versionLinePos = src.find("#version", 0);
		if (versionLinePos != std::string::npos) {
			insertPos = src.find_first_of("\r\n", versionLinePos) + 1;
		}

		const char* typeToken = "#include";
		size_t len = strlen(typeToken);
		size_t pos = src.find(typeToken, 0);
		while (pos != std::string::npos) {
			size_t eol = src.find_first_of("\r\n", pos);
			size_t begin = pos + len + 1;
			std::string result = src.substr(begin, eol - begin);
			
			// Remove the include directive otherwise compiler will complain about it
			// as it will think it will require extensions to handle the includes itself
			src.erase(pos, len + result.length() + 1);

			std::string includePath;
			if (result.find("engine") != std::string::npos) {
				std::string fileName = Utils::GetFileNameWithoutExtension(result);
				includePath = Utils::GetEngineShaderPath(fileName);
			} else {
				includePath = Utils::GetParentPath(pathRelativeTo) + "/" + result;
			}
			
			std::string includeSrc = Utils::ReadFile(includePath);
			AddIncludes(includePath, includeSrc);
			src.insert(insertPos, includeSrc);
			
			size_t nextLinePos = src.find_first_not_of("\r\n", eol);
			pos = src.find(typeToken, nextLinePos);
		}
	}

	static int GetInfoLogLinePos(const std::string& infoLog) {
		// Since TShader doesnt expose the line number I have to fetch out of log
		// example error:
		// ERROR: 0:214: '' :  syntax error, unexpected IDENTIFIER

		size_t firstColon = infoLog.find(":");
		size_t secondColon = infoLog.find(":", firstColon + 1);
		size_t thirdColon = infoLog.find(":", secondColon + 1);

		std::string numberString = infoLog.substr(secondColon + 1, thirdColon - secondColon - 1);
		return std::stoi(numberString);
	}

	static std::string GetLine(const std::string& src, const int lineNumber) {
		std::istringstream stream(src);
		std::string line;
		for (int i = 1; i <= lineNumber; ++i) {
			if (!std::getline(stream, line))
				return "";
		}

		return line;
	}

	VulkanShader::VulkanShader(const std::string& path) : Shader() {
		shaderName = std::filesystem::path(path).stem().string();
		std::string src = Utils::ReadFile(path);
		PreprocessInfo preprocess = PreProcess(path, src);

		cullMode = preprocess.cullMode;
		depthTestingEnabled = preprocess.depthTestingEnabled;

		glslang::InitializeProcess();
		for (std::pair<EShLanguage, std::string> src : preprocess.shaderSources) {
			std::vector<uint32_t> spirv = ConvertGLSLToSPIRV(src.second, src.first);
			Compile(spirv, src.first);
		}
		CreateDescriptorSetLayouts();
		glslang::FinalizeProcess();

		MIST_INFO("Loaded shader and created graphics pipeline for: {}", shaderName);
	}

	VulkanShader::~VulkanShader() {
		Cleanup();
	}

	void VulkanShader::Cleanup() {
		VulkanContext& context = VulkanContext::GetContext();
		for (VkDescriptorSetLayout& layout : descriptorSetLayouts) {
			if (layout != VK_NULL_HANDLE) {
				vkDestroyDescriptorSetLayout(context.GetDevice(), layout, context.GetAllocationCallbacks());
				layout = VK_NULL_HANDLE;
			}
		}

		for (auto& stage : shaderStages) {
			if (stage.module != VK_NULL_HANDLE) {
				vkDestroyShaderModule(context.GetDevice(), stage.module, context.GetAllocationCallbacks());
				stage.module = VK_NULL_HANDLE;
			}
		}
	}

	PreprocessInfo VulkanShader::PreProcess(const std::string& pathRelativeTo, const std::string& src) {
		PreprocessInfo info{};
		info.cullMode = CullMode::CULL_BACK;
		info.depthTestingEnabled = true;

		std::string cullMode = FindTokenResult(src, "#cullmode");
		if (cullMode != "")
			info.cullMode = CullModeFromString(cullMode);

		std::string depthtest = FindTokenResult(src, "#depthtest");
		if (depthtest != "")
			info.depthTestingEnabled = BoolFromString(depthtest);

		{
			const char* typeToken = "#type";
			size_t tokenLength = strlen(typeToken);
			size_t pos = src.find(typeToken, 0);
			while (pos != std::string::npos) {
				size_t eol = src.find_first_of("\r\n", pos);
				MIST_ASSERT(eol != std::string::npos, "Syntax Error.");

				size_t begin = pos + tokenLength + 1;
				std::string result = src.substr(begin, eol - begin);

				size_t nextLinePos = src.find_first_not_of("\r\n", eol);
				pos = src.find(typeToken, nextLinePos);
				info.shaderSources[ShaderTypeFromString(result)] = src.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? src.size() - 1 : nextLinePos));
			}
		}

		for (auto& [stage, stageSrc] : info.shaderSources)
			AddIncludes(pathRelativeTo, stageSrc);

		return info;
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
			std::string infoLog = shader.getInfoLog();
			int linePos = GetInfoLogLinePos(infoLog);
			MIST_ERROR("Failed to parse GLSL in {}:\n{}\n{}", GetStringFromEshLang(shader.getStage()), GetLine(src, linePos), infoLog);
			return {};
		}

		glslang::TProgram program;
		program.addShader(&shader);

		if (!program.link(messages)) {
			MIST_ERROR("Failed to parse GLSL in {}:\n{}", GetStringFromEshLang(shader.getStage()), shader.getInfoLog());
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
			break;
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
		
		VulkanShaderStage shaderStage{};
		shaderStage.language = stage;
		shaderStage.stage = EShLanguageToVkStageFlags(stage);
		shaderStage.module = CreateShaderModule(spirv);
		shaderStages.push_back(shaderStage);

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
			ShaderDescriptorResource res;
			res.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			res.binding = compiler.get_decoration(ubo.id, spv::DecorationBinding);
			res.offset = compiler.get_decoration(ubo.id, spv::DecorationOffset);
			res.set = compiler.get_decoration(ubo.id, spv::DecorationDescriptorSet);
			res.count = 1;
			res.flags = EShLanguageToVkStageFlags(stage);

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
			CreateDescriptorSetLayoutBinding(res.set, res.binding, res.type, res.count, res.flags);
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
			ShaderDescriptorResource res;
			res.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			res.set = compiler.get_decoration(sampled.id, spv::DecorationDescriptorSet);
			res.binding = compiler.get_decoration(sampled.id, spv::DecorationBinding);
			res.count = 1;
			res.flags = EShLanguageToVkStageFlags(stage);
		
			shaderTextures[sampled.name] = res;
			CreateDescriptorSetLayoutBinding(res.set, res.binding, res.type, res.count, res.flags);
		}

		for (const spirv_cross::Resource& images : resources.separate_images) {
			ShaderDescriptorResource res;
			res.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			res.set = compiler.get_decoration(images.id, spv::DecorationDescriptorSet);
			res.binding = compiler.get_decoration(images.id, spv::DecorationBinding);
			res.count = 1;
			res.flags = EShLanguageToVkStageFlags(stage);
		
			shaderDescriptors[images.name] = res;
			CreateDescriptorSetLayoutBinding(res.set, res.binding, res.type, res.count, res.flags);
		}

		for (const spirv_cross::Resource& samplers : resources.separate_samplers) {
			ShaderDescriptorResource res;
			res.type = VK_DESCRIPTOR_TYPE_SAMPLER;
			res.binding = compiler.get_decoration(samplers.id, spv::DecorationBinding);
			res.offset = 0;
			res.set = compiler.get_decoration(samplers.id, spv::DecorationDescriptorSet);
			res.count = 1;
			res.flags = EShLanguageToVkStageFlags(stage);
		
			shaderDescriptors[samplers.name] = res;
			CreateDescriptorSetLayoutBinding(res.set, res.binding, res.type, res.count, res.flags);
		} 
	}

	void VulkanShader::CreateDescriptorSetLayoutBinding(uint32_t set, uint32_t binding, VkDescriptorType type, uint32_t count, VkShaderStageFlags stageFlags) {
		if (descriptorSetLayoutBindings.size() <= set)
			descriptorSetLayoutBindings.resize(set + 1);

		auto& bindings = descriptorSetLayoutBindings[set];
		auto it = std::find_if(bindings.begin(), bindings.end(), [binding](const VkDescriptorSetLayoutBinding& existing) {
			return existing.binding == binding;
		});

		if (it != bindings.end()) {
			MIST_ASSERT(it->descriptorType == type, "Descriptor type mismatch between shader stages");
			it->stageFlags |= stageFlags;
			return;
		}

		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding;
		layoutBinding.descriptorType = type;
		layoutBinding.descriptorCount = count;
		layoutBinding.stageFlags = stageFlags;
		layoutBinding.pImmutableSamplers = nullptr;
		bindings.push_back(layoutBinding);
	}

	void VulkanShader::CreateDescriptorSetLayouts() {
		VulkanContext& context = VulkanContext::GetContext();
		descriptorSetLayouts.resize(descriptorSetLayoutBindings.size());

		for (uint32_t set = 0; set < descriptorSetLayoutBindings.size(); ++set) {
			const auto& bindings = descriptorSetLayoutBindings[set];

			VkDescriptorSetLayoutCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			info.bindingCount = static_cast<uint32_t>(bindings.size());
			info.pBindings = bindings.data();
			CheckVkResult(vkCreateDescriptorSetLayout(context.GetDevice(), &info, context.GetAllocationCallbacks(), &descriptorSetLayouts[set]));
		}
	}

	void VulkanShader::Bind(const uint8_t renderDataId) const {
		VulkanContext& context = VulkanContext::GetContext();
		std::shared_ptr<VulkanRenderData> data = context.GetRenderData(renderDataId);
		if (!data->pipeline.HasPipeline(shaderName))
			data->pipeline.CreateGraphicsPipeline(*this, data->renderPass, data->colorAttachmentCount);

		vkCmdBindPipeline(
			context.GetCurrentFrameCommandBuffer(),
			VK_PIPELINE_BIND_POINT_GRAPHICS, 	// TODO: make a way to detect correct bind point, will probably just have to hold a reference if cant defer
			data->pipeline.GetGraphicsPipeline(shaderName)
		);
	}

	void VulkanShader::Unbind(const uint8_t renderDataId) const {}

	void VulkanShader::SetPushConstant(const uint8_t renderDataId, const std::string& name, const int size, const void* value) {
		VulkanContext& context = VulkanContext::GetContext();
		std::shared_ptr<VulkanRenderData> renderData = context.GetRenderData(renderDataId);
		
#if DEBUG
		MIST_ASSERT(shaderPushConstants.contains(name), std::string("Invalid push constants name passed: " + name));
#endif

		PushConstantResource& res = shaderPushConstants[name];
#if DEBUG
		if (res.size != size)
			MIST_WARN("[" + name + "] Expected size of: " + std::to_string(res.size) + ", you passed " + std::to_string(size));
#endif

		vkCmdPushConstants(
			context.GetCurrentFrameCommandBuffer(), 
			renderData->pipeline.GetGraphicsPipelineLayout(shaderName),
			res.flags,
			res.offset,
			res.size,
			value
		);
	}
}