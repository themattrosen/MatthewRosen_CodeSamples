//Matthew Rosen
#include "stdafx.h"
#include "BehaviorTreeEditorNode.h"
#include "NodeEditor.h"
#include <string>

const char* BEHAVIOR_TREE_NODE_TYPE_NAMES = "Decorator\0Leaf\0Composite\0";

void BehaviorTreeEditorNode::DisplayContextMenu()
{
	if (ImGui::BeginMenu("Rename"))
	{
		ImGui::Text("Rename");
		ImGui::Separator();
		char buffer[100] = { 0 };
		std::strcpy(buffer, GetName());
		if (ImGui::InputText(" ", buffer, 100))
		{
			SetName(buffer);
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Change Type"))
	{
		ImGui::Text("Change Node Type");
		ImGui::Separator();
		if (ImGui::Combo("", (int *)(&m_nodeType), BEHAVIOR_TREE_NODE_TYPE_NAMES))
		{
			BehaviorTreeEditor::Instance().ChangeNodeType(m_ID, m_nodeType);
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndMenu();
	}
}

void BehaviorTreeEditorNode::DrawShape(ImDrawList * drawList, const imvec & offset, bool isSelected)
{
	static const ImU32 NODE_HOVERED_BG_COLOR = IM_COL32(75, 75, 75, 255);
	static const ImU32 NODE_DEFAULT_BG_COLOR = IM_COL32(60, 60, 60, 255);
	static const ImU32 NODE_BORDER_COLOR = IM_COL32(100, 100, 100, 255);
	imvec nodeRectMin = m_pos + offset;
	imvec nodeRectMax = nodeRectMin + m_size;

	if(isSelected)
		drawList->AddRectFilled(nodeRectMin, nodeRectMax, NODE_HOVERED_BG_COLOR, 4.f);
	else
		drawList->AddRectFilled(nodeRectMin, nodeRectMax, NODE_DEFAULT_BG_COLOR, 4.f);

	drawList->AddRect(nodeRectMin, nodeRectMax, NODE_BORDER_COLOR, 4.f);

}

void BehaviorTreeEditorNode::GetData(int & nodeType, float & floatAData, std::string & stringAData, std::string & miscData)
{
	nodeType = -1;
	floatAData = -1;
	stringAData = "";
	miscData = "";
}

imvec BehaviorTreeEditorNode::GetInputSlotPos(int slotNum) const
{
	return imvec(m_pos.x + m_size.x * (float(slotNum) + 1) / (float(m_numInputSlots) + 1), m_pos.y );
}

imvec BehaviorTreeEditorNode::GetOutputSlotPos(int slotNum) const
{
	return imvec(m_pos.x + m_size.x * (float(slotNum) + 1) / (float(m_numOutputSlots) + 1), m_pos.y + m_size.y);
}

BehaviorTreeEditorNode::BehaviorTreeEditorNode(int id, imvec pos, BehaviorTreeNodeType type)
	: m_ID(id), m_nodeType(type), m_isActive(true), m_numInputSlots(1), m_numOutputSlots(1), 
	m_pos(pos), m_size{ 0, 0 }
{
}

BehaviorTreeEditorEdge::BehaviorTreeEditorEdge(int inIdx, int outIdx, int inSlot, int outSlot)
	: m_inputIndex(inIdx), m_outputIndex(outIdx), m_inputSlot(inSlot), m_outputSlot(outSlot)
{
}

void BehaviorTreeEditorEdge::DisplayContextMenu()
{
	//nothing by default
}
