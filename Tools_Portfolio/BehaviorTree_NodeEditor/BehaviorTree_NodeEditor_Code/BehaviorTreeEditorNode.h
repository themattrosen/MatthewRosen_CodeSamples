//Matthew Rosen
#pragma once

#include <string>

#include "ImGui/ImGui.h"

inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }

using imvec = ImVec2;

enum BehaviorTreeNodeType
{
	btnt_Decorator,
	btnt_Leaf,
	btnt_Composite,

	btnt_Count
};

extern const char* BEHAVIOR_TREE_NODE_TYPE_NAMES;

struct BehaviorTreeEditorNode
{
	int m_ID;
	BehaviorTreeNodeType m_nodeType;
	bool m_isActive;

	int m_numInputSlots;
	int m_numOutputSlots;

	imvec m_pos;
	imvec m_size;

	virtual const char* GetName() const = 0;
	virtual void SetName(const char* name) = 0;
	virtual void DisplayNodeView() = 0;
	virtual void DisplayEditorView() = 0;
	virtual void DisplayContextMenu();
	virtual void DrawShape(ImDrawList* drawList, const imvec& offset, bool isSelected);
	virtual void GetData(int& nodeType, float& floatAData, std::string& stringAData, std::string& miscData);
	
	imvec GetInputSlotPos(int slotNum) const;
	imvec GetOutputSlotPos(int slotNum) const;

	BehaviorTreeEditorNode(int id, imvec pos, BehaviorTreeNodeType type);
};

struct BehaviorTreeEditorEdge
{
	int m_inputIndex;
	int m_outputIndex;
	int m_inputSlot;
	int m_outputSlot;

	BehaviorTreeEditorEdge(int inIdx, int outIdx, int inSlot, int outSlot);

	void DisplayContextMenu();
};
