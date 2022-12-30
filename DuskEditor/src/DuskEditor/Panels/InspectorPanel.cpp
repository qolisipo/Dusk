#include "InspectorPanel.h"

#include "DuskEditor/Assets/AssetDatabaseEditor.h"

#include "Core/Application/Application.h"
#include "Core/Assets/AssetDatabase.h"
#include "Core/Assets/AssetHandler.h"
#include "Platform/OpenGL/Shader.h"
#include "Core/Assets/Assets/Mesh.h"
#include "Core/Assets/Assets/Material.h"
#include "Platform/OpenGL/Texture.h"
#include "Core/Scripting/LuaScript.h"
#include "Core/ECS/Entity.h"

#include "Utils/ImGuiUtils.h"
#include "Utils/Rendering/PrimitiveMesh.h"

#include "IconsForkAwesome.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

namespace DuskEngine
{
	template<typename T>
	bool DrawComponentNode(const char* name, std::vector<Entity>& selectedEntities, std::vector<T*>& components);

	InspectorPanel::InspectorPanel(AssetHandler* assetHandler, AssetDatabaseEditor* db)
		:m_AssetHandler(assetHandler), m_EditorDB(db)
	{
		m_DB = &Application::Get().GetAssetDatabase();
	}

	void InspectorPanel::OnImGuiRender()
	{
		ImGui::Begin(ICON_FK_INFO "  Inspector");
		CheckFocus();

		if(m_SelectedEntities->size() > 0)
		{
			if (m_SelectedEntities->size() > 1)
			{
				ImGui::Text("Inspecting multiple entities.");
				ImGui::Text("Only shared components can be edited.");
				ImGui::Separator();
			}
			else
			{
				if (ImGui::Button("Add Component"))
					ImGui::OpenPopup("AddComponent");

				if (ImGui::BeginPopup("AddComponent")) // Only add components that do not exist
				{
					if (ImGui::MenuItem("Camera"))
					{
						if (!(*m_SelectedEntities)[0].HasComponent<Camera>())
						{
							auto& camera = (*m_SelectedEntities)[0].AddComponent<Camera>();
							camera.projectionMatrix = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.01f, 100.0f);
						}
						ImGui::CloseCurrentPopup();
					}

					if (ImGui::MenuItem("Light"))
					{
						if (!(*m_SelectedEntities)[0].HasComponent<Light>())
						{
							auto& light = (*m_SelectedEntities)[0].AddComponent<Light>();
							light.type = LightType::Point;
						}
						ImGui::CloseCurrentPopup();
					}

					if(ImGui::MenuItem("Mesh Renderer"))
					{
						if(!(*m_SelectedEntities)[0].HasComponent<MeshRenderer>())
						{
							auto& mesh = (*m_SelectedEntities)[0].AddComponent<MeshRenderer>();
							mesh.meshHandle = 1;
							mesh.materialHandle = 0;
						}

						ImGui::CloseCurrentPopup();
					}

					if (ImGui::BeginMenu("Scripts"))
					{

						for (auto& scriptAsset : m_EditorDB->m_ScriptsDatabase)
						{
							// If entity has a script component, remove it and 
							if(ImGui::MenuItem(scriptAsset->Name.c_str()))
							{
								//auto& script = (*m_SelectedEntities)[0].AddComponent<Script>();
								//script.luaScriptHandle = scriptAsset->GetUUID();
								//m_AssetHandler->AddToLuaScriptPool((*m_SelectedEntities)[0].GetComponent<Meta>().entityHandle, script.luaScriptHandle);
							}
						}
						ImGui::EndMenu();
					}

					ImGui::EndPopup();
				}
			}

			MetaInspector();
			TransformInspector();
			CameraInspector();
			LightInspector();

			//DrawComponent<Meta>(ICON_FK_CUBE "  Meta", *m_SelectedEntities, MetaInspector);
			//DrawComponent<Transform>(ICON_FK_ARROWS_ALT "  Transform", *m_SelectedEntities, TransformInspector);
			//DrawComponent<Camera>(ICON_FK_VIDEO_CAMERA "  Camera", *m_SelectedEntities, CameraInspector);
			//DrawComponent<Light>(ICON_FK_LIGHTBULB_O "  Light", *m_SelectedEntities, LightInspector);
			//DrawComponent<MeshRenderer, std::vector<MeshRenderer*>& ,InspectorPanel, &InspectorPanel::MaterialInspector>(ICON_FK_PAINT_BRUSH "  Mesh Renderer", *m_SelectedEntities, this);

			/*std::string script = ICON_FK_PENCIL_SQUARE_O "  Lua Script - ";
			script.append(m_AssetHandler->LuaScriptPool((*m_SelectedEntities)[0].GetComponent<Meta>().entityHandle)->GetName());*/
			/*DrawComponent<Script, std::vector<Script*>&, InspectorPanel, &InspectorPanel::ScriptInspector>
				(ICON_FK_PENCIL_SQUARE_O "  Lua Script", *m_SelectedEntities, this);*/
		}
		ImGui::End();
	}

	void InspectorPanel::MetaInspector()
	{
		std::vector<Meta*> metas;

		if (DrawComponentNode<Meta>(ICON_FK_CUBE "  Meta", *m_SelectedEntities, metas))
		{
			if (metas.size() == 1)
			{
				// TODO: Only change name on enter
				char buffer[64];
				sprintf(buffer, metas[0]->name.c_str());
				if (ImGui::InputText("Name", buffer, IM_ARRAYSIZE(buffer)))
				{
					metas[0]->name = buffer;
				}

				ImGui::Checkbox("Enabled", &metas[0]->enabled);
			}
			else
			{
				ImGui::Text("Name change not available on multiple entites");

				// TODO: If all enabled states are the same, show enabled/disabled. Else, show checkbox with a line like in unity
				// Checking the box in that state turns all either disabled or enabled
				bool enabled = metas[0]->enabled;
				if (ImGui::Checkbox("Enabled", &enabled))
					for (auto& m : metas)
						m->enabled = enabled;
			}
			ImGui::TreePop();
		}
	}

	void InspectorPanel::TransformInspector()
	{
		std::vector<Transform*> transforms;
		
		if (DrawComponentNode<Transform>(ICON_FK_ARROWS_ALT "  Transform", *m_SelectedEntities, transforms))
		{
			if (transforms.size() == 1)
			{
				// TODO: better transform component, rotation as quaternion
				ImGui::DragFloat3("Position", &(transforms[0]->position[0]), .01f);

				glm::vec3 rotation = glm::degrees(transforms[0]->rotation);
				if (ImGui::DragFloat3("Rotation", &rotation[0], 0.1f))
					transforms[0]->rotation = glm::radians(rotation);

				ImGui::DragFloat3("Scale", &(transforms[0]->scale[0]), .01f);
			}
			else
			{
				// TODO: Common numbers are displayed normally with their value, others either as 0.0f or something else
				glm::vec3 translation(0.0f);
				ImGui::DragFloat3("Position", &translation[0], .01f);

				glm::vec3 rotation(0.0f);
				ImGui::DragFloat3("Rotation", &rotation[0], .01f);

				glm::vec3 scale(0.0f);
				ImGui::DragFloat3("Scale", &scale[0], .01f);

				for (auto& t : transforms)
				{
					t->position += translation;
					t->rotation += rotation;
					t->scale += scale;
				}
			}
			ImGui::TreePop();
		}
	}

	void InspectorPanel::CameraInspector()
	{
		std::vector<Camera*> cameras;

		if (DrawComponentNode<Camera>(ICON_FK_VIDEO_CAMERA "  Camera", *m_SelectedEntities, cameras))
		{
			if (cameras.size() == 1)
			{
				// TODO: Add projection type and other properties
				ImGui::Checkbox("Primary Camera", &(cameras[0]->main));
			}
			else
			{
				// TODO: Multiple camera edit
				ImGui::Text("Camera inspection not available on multiple entites");
			}
			ImGui::TreePop();
		}
	}
	
	void InspectorPanel::LightInspector()
	{	
		const char* types[] = { "Directional", "Point" };
		const char* comboLabel = types[0];
		static int itemCurrentIdx = 0;

		std::vector<Light*> lights;
		if (DrawComponentNode<Light>(ICON_FK_LIGHTBULB_O "  Light", *m_SelectedEntities, lights))
		{
			if (lights.size() == 1)
			{
				switch (lights[0]->type)
				{
				case LightType::Point:
					comboLabel = "Point";
					itemCurrentIdx = 1;
					break;
				case LightType::Directional:
					comboLabel = "Directional";
					itemCurrentIdx = 0;
					break;
				}

				if (ImGui::BeginCombo("Type", comboLabel))
				{
					for (uint32_t i = 0; i < IM_ARRAYSIZE(types); i++)
					{
						const bool isSelected = (itemCurrentIdx == i);
						if (ImGui::Selectable(types[i], isSelected))
						{
							itemCurrentIdx = i;
							if (i == 0)
								lights[0]->type = LightType::Directional;
							else if (i == 1)
								lights[0]->type = LightType::Point;
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				ImGui::ColorEdit3("Light Color", &(lights[0]->color[0]));
			}
			else
			{
				// TODO: fix default selected type, text shows directional (index 0) actuall index can be smth else
				if (ImGui::BeginCombo("Type", comboLabel))
				{
					for (uint32_t i = 0; i < IM_ARRAYSIZE(types); i++)
					{
						const bool is_selected = (itemCurrentIdx == i);
						if (ImGui::Selectable(types[i], is_selected))
						{
							itemCurrentIdx = i;

							LightType type;

							if (i == 0)
								type = LightType::Directional;
							else if (i == 1)
								type = LightType::Point;

							for (auto& l : lights)
								l->type = type;
						}

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				glm::vec3 color = lights[0]->color;
				if (ImGui::ColorEdit3("Light Color", &color[0]))
					for (auto& l : lights)
						l->color = color;
			}
			ImGui::TreePop();
		}
	}

	void MaterialInspector(std::vector<MeshRenderer*>& meshes)
	{
		if (meshes.size() == 1)
		{
			// This could and should probably be done once at startup, and refreshed once a new shader is added/deleted
			/*std::vector<std::string> modelList {"Primitive: Quad", "Primitive: Cube"};
			int modelIndex = 0;
			uuids::uuid modelID = m_AssetHandler->GetAsset(meshes[0]->meshHandle).UUID;

			for (unsigned int i = 0; i < m_EditorDB->m_ModelDatabase.size(); i++)
			{
				modelList.push_back(m_EditorDB->m_ModelDatabase[i]->Name);
				if (m_EditorDB->m_ModelDatabase[i]->UUID == modelID)
					modelIndex = i + 2;
			}

			if(!modelIndex)
			{
				switch (m_AssetHandler->GetAsset(meshes[0]->meshHandle).GetType())
				{
				case MeshType::Quad:
					modelIndex = 0;
					break;
				case MeshType::Cube:
					modelIndex = 1;
					break;
				default:
					break;
				}
			}*/

			//const char* model_label = modelList[modelIndex].c_str();
			//if (ImGui::BeginCombo("Mesh", model_label))
			//{
			//	for (int n = 0; n < modelList.size(); n++)
			//	{
			//		const bool is_selected = (modelIndex == n);
			//		if (ImGui::Selectable(modelList[n].c_str(), is_selected))
			//		{
			//			if (n != modelIndex)
			//			{
			//				modelIndex = n;
			//				if(modelIndex <= 1)
			//				{
			//					switch (modelIndex)
			//					{
			//					case 0:
			//						meshes[0]->meshHandle = 0;
			//						break;
			//					case 1:
			//						meshes[0]->meshHandle = 1;
			//						break;
			//					default:
			//						break;
			//					}
			//				}
			//				else
			//				{	
			//					meshes[0]->meshHandle = 
			//						m_AssetHandler->AddToAssetPool<Mesh>(m_DB->GetUUID(m_EditorDB->m_ModelDatabase[modelIndex - 2]->Path));
			//				}
			//			}
			//		}

			//		if (is_selected)
			//			ImGui::SetItemDefaultFocus();
			//	}
			//	ImGui::EndCombo();
			//}

			//// This could and should probably be done once at startup, and refreshed once a new shader is added/deleted
			//std::vector<std::string> materialList; // Later add default materials
			//int materialIndex = 0;
			//uuids::uuid materialID = m_AssetHandler->GetAsset<Material>(meshes[0]->materialHandle).UUID;
			//	;

			//for (unsigned int i = 0; i < m_EditorDB->m_MaterialDatabase.size(); i++)
			//{
			//	materialList.push_back(m_EditorDB->m_MaterialDatabase[i]->Name);
			//	if (m_EditorDB->m_MaterialDatabase[i]->UUID == materialID)
			//		materialIndex = i;
			//}

			//
			//const char* material_label = materialList[materialIndex].c_str();
			//if (ImGui::BeginCombo("Material", material_label))
			//{
			//	for (int n = 0; n < materialList.size(); n++)
			//	{
			//		const bool is_selected = (materialIndex == n);
			//		if (ImGui::Selectable(materialList[n].c_str(), is_selected))
			//		{
			//			if (n != materialIndex)
			//			{
			//				materialIndex = n;
			//				auto id = m_DB->GetUUID(m_EditorDB->m_MaterialDatabase[materialIndex]->Path);
			//				meshes[0]->materialHandle = m_AssetHandler->AddToAssetPool<Material>(id);
			//			}
			//		}

			//		if (is_selected)
			//			ImGui::SetItemDefaultFocus();
			//	}
			//	ImGui::EndCombo();
			//}

			//ImGui::Separator();
			//ImGui::Separator();
			//ImGui::Separator();

			//ImGui::Text(m_AssetHandler->GetAsset<Material>(meshes[0]->materialHandle).Name.c_str());

			//// This could and should probably be done once at startup, and refreshed once a new shader is added/deleted
			//std::vector<std::string> shaderList;
			//int shaderIndex = 0;
			//// bad
			//uuids::uuid shaderID = m_AssetHandler->GetAsset<Shader>(
			//	m_AssetHandler->GetAsset<Material>(meshes[0]->materialHandle).GetShaderHandle()).UUID;

			//for (unsigned int i = 0; i < m_EditorDB->m_ShaderDatabase.size(); i++)
			//{
			//	shaderList.push_back(m_EditorDB->m_ShaderDatabase[i]->Name);
			//	if (m_EditorDB->m_ShaderDatabase[i]->UUID == shaderID)
			//		shaderIndex = i;
			//}

			//const char* shader_label = shaderList[shaderIndex].c_str();
			//if (ImGui::BeginCombo("Shader", shader_label))
			//{
			//	for (int n = 0; n < shaderList.size(); n++)
			//	{
			//		const bool is_selected = (shaderIndex == n);
			//		if (ImGui::Selectable(shaderList[n].c_str(), is_selected))
			//		{
			//			if (n != shaderIndex)
			//			{
			//				shaderIndex = n;
			//				
			//				auto id = m_DB->GetUUID(m_EditorDB->m_ShaderDatabase[shaderIndex]->Path);
			//				auto shaderHandle = m_AssetHandler->AddToAssetPool<Shader>(id);
			//				m_AssetHandler->GetAsset<Material>(meshes[0]->materialHandle).SetShader(shaderHandle);
			//				
			//				m_AssetHandler->GetAsset<Material>(meshes[0]->materialHandle).SerializeText(
			//					m_AssetHandler->GetAsset<Material>(meshes[0]->materialHandle).Path.string());
			//			}
			//		}

			//		if (is_selected)
			//			ImGui::SetItemDefaultFocus();
			//	}
			//	ImGui::EndCombo();
			//}

			//// actually this logic is now kinda sus but might be relevant again in the future
			//
			////// if in editing mode ->					propagate
			////// if accessing from asset browser ->		propagate
			////// if playing and accessing from scene ->	do not propagate

			//auto& material = m_AssetHandler->GetAsset<Material>(meshes[0]->materialHandle);

			//for (auto& uniform : m_AssetHandler->GetAsset<Material>(meshes[0]->materialHandle).m_Uniforms)
			//{
			//	if (uniform.Type == UniformType::Vec3)
			//	{
			//		if (ImGui::ColorEdit3(uniform.Name.c_str(), &uniform.Data.vec3[0]))
			//			material.SerializeText(material.Path.string());
			//	}

			//	if (uniform.Type == UniformType::Texture)
			//	{
			//		// this is broken but whatever, will be replaced in the future
			//		ImGui::Text(uniform.Name.c_str());
			//		if (ImGui::ImageButton((ImTextureID)(size_t)m_AssetHandler->GetAsset<Texture>(uniform.Data.dataHandle).ResourceID,
			//			ImVec2{40, 40}, ImVec2{0, 1}, ImVec2{1, 0}))
			//		{
			//			TRACE("To be implemented");
			//		}

			//		if (ImGui::BeginDragDropTarget())
			//		{
			//			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE"))
			//			{
			//				const wchar_t* data = (const wchar_t*)payload->Data;
			//				std::wstring ws(data);

			//				auto& texture = m_AssetHandler->GetAsset<Texture>(m_AssetHandler->AddToAssetPool<Texture>(m_DB->GetUUID(ws)));
			//				material.SetTexture(uniform.Name, texture);
			//				material.SerializeText(material.Path.string());
			//			}
			//			ImGui::EndDragDropTarget();
			//		}
			//	}
			//}
		}
		else
		{
			// TODO only same material instance can be edited.
		}
	}

	void InspectorPanel::ScriptInspector(std::vector<Script*>& scripts)
	{
		//m_AssetHandler->
		if (scripts.size() == 1)
		{	
			//ImGui::Text(m_AssetHandler->LuaScriptPool()->GetName().c_str();
			
			//for (unsigned int i = 0; i < scripts[0]->scripts.size(); i++)
			//{
			//	ImGui::Text(scripts[0]->scripts[i]->GetName().c_str());
			//	ImGui::SameLine();
			//	if (ImGui::Button(("Remove script " + std::to_string(i)).c_str()))
			//	{
			//		scripts[0]->scripts.erase(scripts[0]->scripts.begin() + i--);
			//	}
			//}

			//if(scripts[0]->scripts.size() == 0)
			//{
			//	// Remove component, but cant be done with current setup. inspector needs rework anyway so w/e
			//}
		}
	}

	template<typename T>
	bool DrawComponentNode(const char* name, std::vector<Entity>& selectedEntities, std::vector<T*>& components)
	{
		for (auto& entity : selectedEntities)
		{
			if (!entity.HasComponent<T>())
				return false; // All selected entities need to have component, else skip component entirely

			components.push_back(&entity.GetComponent<T>());
		}

		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name);
		ImGui::PopStyleVar();

		auto& style = ImGui::GetStyle();
		float lineHeight = ImGui::GetFontSize() + style.FramePadding.y;

		ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
		if (ImGui::Button(ICON_FK_COG, ImVec2{ lineHeight, lineHeight }))
		{
			ImGui::OpenPopup("ComponentSettings");
		}

		if (ImGui::BeginPopup("ComponentSettings"))
		{
			if (ImGui::MenuItem("Remove component"))
			{
				for (auto& ent : selectedEntities)
					ent.RemoveComponent<T>();

				ImGui::EndPopup();
				if (open) ImGui::TreePop();
				return false;
			}

			ImGui::EndPopup();
		}

		return open;
	}
}