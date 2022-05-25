#pragma once
#include "PanelBase.h"

#include "Core/ECS/Entity.h"

#include "Utils/Memory/Memory.h"

#include "glm/glm.hpp"

#include <vector>

#ifdef DUSK_WINDOWS
namespace ImGuizmo
{
	enum OPERATION;
}
#endif

namespace DuskEngine
{
	class FrameBuffer;
	class EditorCamera;
	class KeyPressedEvent;
	
	class SceneViewportPanel : public Panel
	{
	public:
		SceneViewportPanel(Ref<FrameBuffer>& fb, EditorCamera* camera);
		~SceneViewportPanel() = default;

		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

		void SelectedEntities(std::vector<Entity>& entities) { m_SelectedEntities = &entities; }
		bool Test(KeyPressedEvent& e);
	private:
		std::vector<Entity>* m_SelectedEntities = nullptr;

		Ref<FrameBuffer> m_FB;
		glm::vec2 m_ViewportSize;
		EditorCamera* m_Camera;

		void MoveEditorCamera();
		bool m_IsLeftMousePressed = false;
		bool m_HasFirstMousePosition = false;
		glm::vec2 m_LastMousePosition;
		#ifdef DUSK_WINDOWS
		ImGuizmo::OPERATION m_ImGuizmoOperation;
		#endif
	};
}