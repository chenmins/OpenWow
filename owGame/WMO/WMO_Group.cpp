#include "stdafx.h"

// Include
#include "WMO.h"
#include "WMO_Group_Instance.h"

// General
#include "WMO_Group.h"

// Additional 
#include "WoWChunkReader.h"
#include "DBC/DBC__Storage.h"
#include "WMO_Base_Instance.h"
#include "WMO_Doodad_Instance.h"
#include "WMO_Liquid_Instance.h"
#include "WMO_Fixes.h"

WMO_Group::WMO_Group(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const std::shared_ptr<CWMO>& WMOModel, const uint32 GroupIndex, const SWMO_GroupInfoDef& GroupProto)
	: CLoadableObject(WMOModel)
	, m_IsMOCVExists(false)
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_WMOModel(*WMOModel)
	, m_GroupIndex(GroupIndex)
{
	if (GroupProto.nameoffset != -1)
		m_GroupName = std::string(m_WMOModel.GetGroupName(GroupProto.nameoffset));
	else
		m_GroupName = m_WMOModel.GetFilename() + "_Group" + std::to_string(GroupIndex);

	m_Bounds = GroupProto.bounding_box.Convert();

	char temp[MAX_PATH];
	strcpy_s(temp, m_WMOModel.GetFilename().c_str());
	temp[m_WMOModel.GetFilename().length() - 4] = 0;

	char groupFilename[MAX_PATH];
	sprintf_s(groupFilename, "%s_%03d.wmo", temp, m_GroupIndex);

	std::unique_ptr<WoWChunkReader> chunkReader = std::make_unique<WoWChunkReader>(m_BaseManager, groupFilename);

	// Version
	if (auto buffer = chunkReader->OpenChunk("MVER"))
	{
		uint32 version;
		buffer->readBytes(&version, 4);
		_ASSERT(version == 17);
	}

	// Header
	if (auto buffer = chunkReader->OpenChunk("MOGP"))
	{
		buffer->readBytes(&m_GroupHeader, sizeof(SWMO_Group_HeaderDef));
		_ASSERT(m_GroupHeader.flags.HAS_3_MOTV == 0);

		// Real wmo group file contains only 2 chunks: MVER and MOGP.
		// Start of MOGP is header (without fourcc).
		// After header data places others chunks.
		// We reinitialize chunk reader from current position
		// chunkReader.reset() DON'T call this, because source buffer will be free.
		m_ChunkReader = std::make_unique<WoWChunkReader>(m_BaseManager, buffer->getDataFromCurrent(), buffer->getSize() - sizeof(SWMO_Group_HeaderDef));
	}
}

WMO_Group::~WMO_Group()
{
}



//
// WMO_Group
//
const uint32 WMO_Group::GetGroupIndex() const
{
	return m_GroupIndex;
}

void WMO_Group::AddPortal(const CWMO_Part_Portal& WMOPartPortal)
{
	m_Portals.push_back(WMOPartPortal);
}

const std::vector<CWMO_Part_Portal>& WMO_Group::GetPortals() const
{
	return m_Portals;
}



//
// ISceneNodeProvider
//
void WMO_Group::CreateInsances(const std::shared_ptr<CWMO_Group_Instance>& Parent) const
{
	_ASSERT(GetState() == ILoadable::ELoadableState::Loaded);

	for (const auto& batch : m_WMOBatchIndexes)
	{
		Parent->GetComponent<IModelsComponent3D>()->AddModel(batch);
	}

	// WMO Group liquid
	if (m_WMOLiqiud != nullptr)
	{
		glm::vec3 realPos = Fix_XZmY(m_WMOLiqiud->GetHeader().pos);

		auto liquidInstance = Parent->CreateSceneNode<CWMO_Liquid_Instance>();
		m_WMOLiqiud->CreateInsances(liquidInstance);

		// Transform
		liquidInstance->SetTranslate(glm::vec3(realPos.x, 0, realPos.z));

		// IColliderComponent3D
		{
			BoundingBox bbox = Parent->GetColliderComponent()->GetBounds();
			bbox.setMin(bbox.getMin() - realPos);
			bbox.setMax(bbox.getMax() - realPos);
			bbox.calculateCenter();
			liquidInstance->GetColliderComponent()->SetBounds(bbox);
		}

		Parent->AddRoomObject(liquidInstance);
		
	}

#if 1 && defined(USE_M2_MODELS)
	//std::vector<SWMO_Doodad_SetInfo> activeDoodadSets;
	//activeDoodadSets.push_back(m_WMOModel.m_DoodadsSetInfos[0]);

	//if (parentAsWMOInstance->GetDoodadSetIndex() != -1)
	//	activeDoodadSets.push_back(m_WMOModel.m_DoodadsSetInfos[parentAsWMOInstance->GetDoodadSetIndex()]);

	// WMO Group M2s
	//for (const auto& doodadSet : activeDoodadSets)
	//{
		//for (size_t i = doodadSet.start; i < doodadSet.start + doodadSet.size; i++)
		for (size_t i = 0; i < m_DoodadsPlacementIndexes.size(); i++)
		{
			auto doodadPlacementIndex = m_DoodadsPlacementIndexes[i];

			const SWMO_Doodad_PlacementInfo& placement = m_WMOModel.GetDoodadPlacement(doodadPlacementIndex);

			std::string doodadFileName = m_WMOModel.GetDoodadFileName(placement.flags.nameIndex);
			std::shared_ptr<CM2> m2 = m_BaseManager.GetManager<IWoWObjectsCreator>()->LoadM2(m_RenderDevice, doodadFileName, true);
			if (m2)
			{
				auto inst = Parent->CreateSceneNode<CWMO_Doodad_Instance>(m2, doodadPlacementIndex, placement);

				if (!m_GroupHeader.flags.DO_NOT_USE_LIGHTING_DIFFUSE && !m_GroupHeader.flags.IS_OUTDOOR)
					inst->setColor(placement.getColor());

				inst->Load();
				inst->SetState(ELoadableState::Loaded);

				//m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(inst);
				Parent->AddRoomObject(inst);
			}
		}
	//}
#endif
}

uint32 to_wmo_liquid(const SWMO_Group_HeaderDef& Header, int x)
{
	DBC_LIQUIDTYPE_Type::List basic = (DBC_LIQUIDTYPE_Type::List)(x & 3);
	switch (basic)
	{
	case DBC_LIQUIDTYPE_Type::water:
		return (Header.flags.IS_NOT_WATER_BUT_OCEAN) ? 14 : 13;
	case DBC_LIQUIDTYPE_Type::ocean:
		return 14;
	case DBC_LIQUIDTYPE_Type::magma:
		return 19;
	case DBC_LIQUIDTYPE_Type::slime:
		return 20;
	}

	return UINT32_MAX;
}

bool WMO_Group::Load()
{
	// Buffer
	uint16* dataFromMOVI = nullptr;
	dataFromMOVT = nullptr;

	// CollisionTEMP
	uint32 collisionCount = 0;
	SWMO_Group_MOBNDef* collisions = nullptr;

	std::shared_ptr<IGeometry> geometry = m_RenderDevice.GetObjectsFactory().CreateGeometry();

	// Material info for triangles
	for (const auto& materialsInfo : m_ChunkReader->OpenChunkT<SWMO_Group_MaterialDef>("MOPY")) // Material info for triangles
	{
		m_MaterialsInfo.push_back(materialsInfo);
	}


	// Indices
	if (auto buffer = m_ChunkReader->OpenChunk("MOVI"))
	{
		// Buffer
		geometry->SetIndexBuffer(m_RenderDevice.GetObjectsFactory().CreateIndexBuffer((const uint16*)buffer->getData(), buffer->getSize() / sizeof(uint16)));
	}


	// Vertices chunk.
	if (auto buffer = m_ChunkReader->OpenChunk("MOVT"))
	{
		uint32 vertexesCount = buffer->getSize() / sizeof(glm::vec3);
		glm::vec3* vertexes = (glm::vec3*)buffer->getData();

		// Convert
		for (uint32 i = 0; i < vertexesCount; i++)
			vertexes[i] = Fix_XZmY(vertexes[i]);

		// Buffer
		geometry->AddVertexBuffer(BufferBinding("POSITION", 0), m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(vertexes, vertexesCount));

		std::vector<glm::vec4> colors;
		colors.resize(vertexesCount);
		std::fill(colors.begin(), colors.end(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		geometry->AddVertexBuffer(BufferBinding("COLOR", 0), m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(colors));

		dataFromMOVT = vertexes;
	}


	// Normals
	if (auto buffer = m_ChunkReader->OpenChunk("MONR"))
	{
		uint32 normalsCount = buffer->getSize() / sizeof(glm::vec3);
		glm::vec3* normals = (glm::vec3*)buffer->getDataFromCurrent();

		// Convert
		for (uint32 i = 0; i < normalsCount; i++)
			normals[i] = Fix_XZmY(normals[i]);

		// Buffer
		geometry->AddVertexBuffer(BufferBinding("NORMAL", 0), m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(normals, normalsCount));
	}


	// Texture coords
	for (const auto& buffer : m_ChunkReader->OpenChunks("MOTV"))
	{
		geometry->AddVertexBuffer(BufferBinding("TEXCOORD", 0), m_RenderDevice.GetObjectsFactory().CreateVertexBuffer((const glm::vec2*)buffer->getData(), buffer->getSize() / sizeof(glm::vec2)));
		break;
	}


	// WMO_Group_Batch
	std::vector<SWMO_Group_BatchDef> WMOBatchs;
	for (const auto& batchDef : m_ChunkReader->OpenChunkT<SWMO_Group_BatchDef>("MOBA"))
	{
		WMOBatchs.push_back(batchDef);
	}


	// Light references
	if (auto buffer = m_ChunkReader->OpenChunk("MOLR"))
	{
		_ASSERT(m_GroupHeader.flags.HAS_LIGHTS);
		uint32 lightsIndexesCount = buffer->getSize() / sizeof(uint16);
		uint16* lightsIndexes = (uint16*)buffer->getDataFromCurrent();
		for (uint32 i = 0; i < lightsIndexesCount; i++)
		{
			m_WMOLightsIndexes.push_back(lightsIndexes[i]);
		}
	}


	// Doodad references
	if (auto buffer = m_ChunkReader->OpenChunk("MODR"))
	{
		_ASSERT(m_GroupHeader.flags.HAS_DOODADS);

		uint32 doodadsIndexesCount = buffer->getSize() / sizeof(uint16);
		uint16* doodadsIndexes = (uint16*)buffer->getDataFromCurrent();
		m_DoodadsPlacementIndexes.assign(doodadsIndexes, doodadsIndexes + doodadsIndexesCount);
	}


	for (const auto& collision : m_ChunkReader->OpenChunkT<SWMO_Group_MOBNDef>("MOBN"))
	{
		std::shared_ptr<CWMO_Group_Part_BSP_Node> collisionNode = std::make_shared<CWMO_Group_Part_BSP_Node>(*this, collision);
		m_CollisionNodes.push_back(collisionNode);
	}


	if (auto buffer = m_ChunkReader->OpenChunk("MOBR"))
	{
		uint32 indexesCnt = buffer->getSize() / sizeof(uint16);
		uint16* indices = (uint16*)buffer->getDataFromCurrent();

		// Buffer
		//geometry->SetIndexBuffer(m_RenderDevice.GetObjectsFactory().CreateIndexBuffer((const uint16*)buffer->getData(), buffer->getSize() / sizeof(uint16)));

		/*collisionIndexes.reserve(indexesCnt * 3);
		for (uint32 i = 0; i < indexesCnt; i++)
		{
			collisionIndexes[i * 3 + 0] = dataFromMOVI[3 * indices[i] + 0];
			collisionIndexes[i * 3 + 1] = dataFromMOVI[3 * indices[i] + 1];
			collisionIndexes[i * 3 + 2] = dataFromMOVI[3 * indices[i] + 2];
		}*/
	}


	// Vertex colors
	for (const auto& buffer : m_ChunkReader->OpenChunks("MOCV"))
	{
		_ASSERT(m_GroupHeader.flags.HAS_VERTEX_COLORS);

		uint32 vertexColorsCount = buffer->getSize() / sizeof(CBgra);
		CBgra* vertexColors = (CBgra*)buffer->getDataFromCurrent();

#if WOW_CLIENT_VERSION >= WOW_WOTLK_3_3_5
		//FixColors(vertexColors, vertexColorsCount, WMOBatchs.data());
#endif

		// Convert
		std::vector<glm::vec4> vertexColorsConverted;
		for (uint32 i = 0; i < vertexColorsCount; i++)
		{
			vertexColorsConverted.push_back(glm::vec4(
				static_cast<float>(vertexColors[i].r) / 255.0f,
				static_cast<float>(vertexColors[i].g) / 255.0f,
				static_cast<float>(vertexColors[i].b) / 255.0f,
				static_cast<float>(vertexColors[i].a) / 255.0f
			));
		}

		geometry->AddVertexBuffer(BufferBinding("COLOR", 0), m_RenderDevice.GetObjectsFactory().CreateVoidVertexBuffer(vertexColorsConverted.data(), vertexColorsConverted.size(), 0, sizeof(glm::vec4)));
		m_IsMOCVExists = vertexColorsCount > 0;
		break;
	}


	if (auto buffer = m_ChunkReader->OpenChunk("MLIQ"))
	{
		SWMO_Group_MLIQDef liquidHeader;
		buffer->read(&liquidHeader);

		//Log::Green
		//(
		//	"WMO[%s]: Liq: headerID [%d] headerFlag [%d] MatID: [%d] MatShader[%d]", 
		//	m_WMOModel.getFilename().c_str(),
		//	m_GroupHeader.liquidType,
		//	m_WMOModel.m_Header.flags.use_liquid_type_dbc_id, 
		//	liquidHeader.materialID
		//);

		uint32 liquid_type;
		if (m_WMOModel.GetHeader().flags.use_liquid_type_dbc_id != 0)
		{
			if (m_GroupHeader.liquidType < 21)
			{
				liquid_type = to_wmo_liquid(m_GroupHeader, m_GroupHeader.liquidType - 1);
			}
			else
			{
				liquid_type = m_GroupHeader.liquidType;
			}
		}
		else
		{
			if (m_GroupHeader.liquidType < 20)
			{
				liquid_type = to_wmo_liquid(m_GroupHeader, m_GroupHeader.liquidType);
			}
			else
			{
				liquid_type = m_GroupHeader.liquidType + 1;
			}
		}

		m_WMOLiqiud = std::make_shared<CWMO_Liquid>(m_RenderDevice, m_WMOModel, *this, buffer, liquidHeader);
	}

	for (const auto& WMOGroupBatchProto : WMOBatchs)
	{
		std::shared_ptr<WMO_Group_Part_Batch> batch = std::make_shared<WMO_Group_Part_Batch>(m_RenderDevice, m_WMOModel, WMOGroupBatchProto);

		batch->AddConnection(m_WMOModel.GetMaterial(WMOGroupBatchProto.material_id), geometry, SGeometryDrawArgs(WMOGroupBatchProto.indexStart, WMOGroupBatchProto.indexCount));

		m_WMOBatchIndexes.push_back(batch);
	}
	// TODO: enable me std::sort(m_WMOBatchIndexes.begin(), m_WMOBatchIndexes.end(), WMO_Group_Part_BatchCompare());
	WMOBatchs.clear();

	m_ChunkReader.reset();

	return true;
}

void WMO_Group::FixColors(CBgra* mocv, uint32 mocv_count, const SWMO_Group_BatchDef* moba)
{
	uint32 begin_second_fixup = 0;
	if (m_GroupHeader.transBatchCount)
	{
		begin_second_fixup = moba[(uint16)m_GroupHeader.transBatchCount].vertexStart + 1;
	}

	if (m_WMOModel.GetHeader().flags.lighten_interiors)
	{
		for (uint32 i = begin_second_fixup; i < mocv_count; ++i)
		{
			mocv[i].a = m_GroupHeader.flags.IS_OUTDOOR ? 255 : 0;
		}
	}
	else
	{
		uint8 r = 0, g = 0, b = 0;

		if (m_WMOModel.GetHeader().flags.skip_base_color == 0)
		{
			r = m_WMOModel.GetHeader().ambColor.r;
			g = m_WMOModel.GetHeader().ambColor.g;
			b = m_WMOModel.GetHeader().ambColor.b;
		}

		for (uint32 mocv_index = 0; mocv_index < begin_second_fixup; ++mocv_index)
		{
			mocv[mocv_index].b -= b;
			mocv[mocv_index].g -= g;
			mocv[mocv_index].r -= r;

			float v38 = (float)mocv[mocv_index].a / 255.0f;

			float v11 = (float)mocv[mocv_index].b - v38 * (float)mocv[mocv_index].b;
			_ASSERT(v11 > -0.5f);
			_ASSERT(v11 < 255.5f);
			mocv[mocv_index].b = v11 / 2;

			float v13 = (float)mocv[mocv_index].g - v38 * (float)mocv[mocv_index].g;
			_ASSERT(v13 > -0.5f);
			_ASSERT(v13 < 255.5f);
			mocv[mocv_index].g = v13 / 2;

			float v14 = (float)mocv[mocv_index].r - v38 * (float)mocv[mocv_index].r;
			_ASSERT(v14 > -0.5f);
			_ASSERT(v14 < 255.5f);
			mocv[mocv_index].r = v14 / 2;
		}

		for (uint32 i = begin_second_fixup; i < mocv_count; ++i)
		{
			int32 v19 = (mocv[i].b * mocv[i].a) / 64 + mocv[i].b - b;
			mocv[i].b = std::min(255, std::max(v19 / 2, 0));

			int32 v30 = (mocv[i].g * mocv[i].a) / 64 + mocv[i].g - g;
			mocv[i].g = std::min(255, std::max(v30 / 2, 0));

			int32 v33 = (mocv[i].a * mocv[i].r) / 64 + mocv[i].r - r;
			mocv[i].r = std::min(255, std::max(v33 / 2, 0));

			mocv[i].a = m_GroupHeader.flags.IS_OUTDOOR ? 0xFF : 0x00;
		}
	}
}
