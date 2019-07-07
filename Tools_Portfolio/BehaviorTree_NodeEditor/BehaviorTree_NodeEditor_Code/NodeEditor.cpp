//Matthew Rosen
#include "stdafx.h"
#include "NodeEditor.h"
#include "Space/Space.h"
#include "BehaviorTreeEditorNodes.h"

using Node = BehaviorTreeEditorNode;
using Edge = BehaviorTreeEditorEdge;

static const float IM_WINDOW_PADDING = 4.f;
static const imvec NODE_WINDOW_PADDING{ 8.f, 8.f };
static const imvec NODE_SLOT_SIZE{ 16.f, 16.f };
static const float NODE_SLOT_RADIUS = 6.f;

BehaviorTreeEditor::BehaviorTreeEditor()
	: m_currentSpace(nullptr), m_currentObject(0), m_nodes(), m_openNodes(), m_edges(), m_clipboard(nullptr),
	m_scrolling{ 0, 0 }, m_showGrid(true), m_nodeSelected(-1), m_openContextMenu(false),
	m_nodeHoveredInList(-1), m_nodeHoveredInScene(-1), m_inputSlotHoveredInScene(-1), m_outputSlotHoveredInScene(-1)
{

}

void BehaviorTreeEditor::AddNode(const char * name, BehaviorTreeNodeType nodeType, const imvec & pos)
{
	BehaviorTreeEditorNode* node = nullptr;
	int nextID = 0;
	if (m_openNodes.empty())
	{
		nextID = int(m_nodes.size());
		m_nodes.push_back(nullptr);
	}
	else
	{
		nextID = m_openNodes.back();
		m_openNodes.pop_back();
		delete m_nodes[nextID];
		m_nodes[nextID] = nullptr;
	}
	switch (nodeType)
	{
	case btnt_Decorator:
		node = new BehaviorTreeEditorDecoratorNode(nextID, name, pos);
		break;
	case btnt_Leaf:
		node = new BehaviorTreeEditorLeafNode(nextID, name, pos);
		break;
	case btnt_Composite:
		node = new BehaviorTreeEditorCompositeNode(nextID, name, pos);
		break;
	}

	m_nodes[nextID] = node;
}

void BehaviorTreeEditor::DeleteNode(int id)
{
	m_nodes[id]->m_isActive = false;
	m_openNodes.push_back(id);
}

void BehaviorTreeEditor::ChangeNodeType(int id, BehaviorTreeNodeType newType)
{
	Node* node = m_nodes[id];
	std::string name = node->GetName();
	imvec pos = node->m_pos;
	DeleteNode(id);
	AddNode(name.c_str(), newType, pos);
}

void BehaviorTreeEditor::Init(Space* currentSpace, GameObjectID id)
{
	Clear();
	
	m_currentObject = id;
	m_currentSpace = currentSpace;
}

void BehaviorTreeEditor::Display()
{
	DisplayMenuBar();
	DisplayNodeList();
	DisplayNodeEditor();

	ImGui::SetCursorPos(imvec(200 + IM_WINDOW_PADDING * 4.f, IM_WINDOW_PADDING * 4.f + 26));
	DisplayNodeView();
}

void BehaviorTreeEditor::Clear()
{
	m_openNodes.clear();
	m_edges.clear();
	for (size_t i = 0; i < m_nodes.size(); ++i)
	{
		if (m_nodes[i] != nullptr)
		{
			delete m_nodes[i];
			m_nodes[i] = nullptr;
		}
	}

	m_nodes.clear();
}

void BehaviorTreeEditor::Exit()
{
	Clear();

	if (m_clipboard)
	{
		delete m_clipboard;
		m_clipboard = nullptr;
	}
}


void BehaviorTreeEditor::DisplayMenuBar()
{
	if(ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Add"))
		{
			if (ImGui::BeginMenu("Add Decorator Node"))
			{
				static char buffer[100] = { 0 };
				static int nodeType = 0;
				ImGui::InputText("Name", buffer, 100);

				if (ImGui::Button("Create Decorator Node"))
				{
					AddNode(buffer, btnt_Decorator, imvec{ 0, 0 });
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Add Leaf Node"))
			{
				static char buffer[100] = { 0 };
				static int nodeType = 0;
				ImGui::InputText("Name", buffer, 100);

				if (ImGui::Button("Create Leaf Node"))
				{
					AddNode(buffer, btnt_Decorator, imvec{ 0, 0 });
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Add Composite Node"))
			{
				static char buffer[100] = { 0 };
				static int nodeType = 0;
				ImGui::InputText("Name", buffer, 100);

				if (ImGui::Button("Create Composite Node"))
				{
					AddNode(buffer, btnt_Decorator, imvec{ 0, 0 });
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Compile To Component"))
			{
				CompileToComponent();
			}
			if (ImGui::MenuItem("Sync From Component"))
			{
				SyncFromComponent();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Copy"))
			{
				Copy(m_nodeSelected);
			}
			if (ImGui::MenuItem("Paste"))
			{
				Paste(imvec{ 0, 0 });
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}

void BehaviorTreeEditor::DisplayNodeList()
{
	ImGui::BeginChild("Node List", imvec(200, 300), true);
	ImGui::Text("Node List");
	ImGui::Separator();
	for (int nodeIndex = 0; nodeIndex < int(m_nodes.size()); ++nodeIndex)
	{
		Node* node = m_nodes[nodeIndex];
		if (!node || !node->m_isActive)
			continue;

		ImGui::PushID(node->m_ID);
		if (ImGui::Selectable(node->GetName(), node->m_ID == m_nodeSelected))
			m_nodeSelected = node->m_ID;
		if (ImGui::IsItemHovered())
		{
			m_nodeHoveredInList = node->m_ID;
			m_openContextMenu = ImGui::IsMouseClicked(1);
		}
		ImGui::PopID();
	}
	ImGui::EndChild();
}

void BehaviorTreeEditor::DisplayNodeEditor()
{
	ImGui::BeginChild("Node Editor View", imvec(200, 0), true);
	ImGui::Text("Node Editor");
	ImGui::Separator();

	if (m_nodeSelected >= 0 && m_nodes.size() > 0)
	{
		Node* nodeSelected = m_nodes[m_nodeSelected];
		if (nodeSelected != nullptr && nodeSelected->m_isActive)
		{
			//ImGui::PushItemWidth(200);

			nodeSelected->DisplayEditorView();

			//ImGui::PopItemWidth();
		}
		else
		{
			m_nodeSelected = -1;
		}
	}

	ImGui::EndChild();
}

void BehaviorTreeEditor::DisplayNodeView()
{
	ImGui::BeginGroup();
	if (ImGui::Button("Compile To Component"))
	{
		CompileToComponent();
	}
	ImGui::SameLine();
	if (ImGui::Button("Sync From Component"))
	{
		SyncFromComponent();
	}
	ImGui::SameLine();
	ImGui::Checkbox("Show grid", &m_showGrid);
	
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, imvec(1, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, imvec(0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, IM_COL32(60, 60, 70, 200));
	ImGui::BeginChild("scrolling_region", imvec(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
	ImGui::PushItemWidth(120.f);


	ImDrawList* drawList = ImGui::GetWindowDrawList();
	imvec offset = ImGui::GetCursorScreenPos() + m_scrolling;

	DisplayGrid(drawList);
	DisplayEdges(drawList, offset);
	DisplayNodes(drawList, offset);
	DisplayContextMenu(offset);

	if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.f))
	{
		m_scrolling = m_scrolling + ImGui::GetIO().MouseDelta;
	}

	ImGui::PopItemWidth();
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::EndGroup();
}

void BehaviorTreeEditor::DisplayGrid(ImDrawList* list)
{
	if (!m_showGrid)
	{
		return;
	}

	static const ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);
	static const float GRID_SZ = 64.f;
	imvec winPos = ImGui::GetCursorScreenPos();
	imvec canvas_sz = ImGui::GetWindowSize();
	for (float x = std::fmod(m_scrolling.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
	{
		list->AddLine(imvec(x, 0.f) + winPos, imvec(x, canvas_sz.y) + winPos, GRID_COLOR);
	}
	for (float y = std::fmod(m_scrolling.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
	{
		list->AddLine(imvec(0.f, y) + winPos, imvec(canvas_sz.x, y) + winPos, GRID_COLOR);
	}
}

void BehaviorTreeEditor::DisplayEdges(ImDrawList * list, const imvec & offset)
{
	static const ImU32 EDGE_COLOR = IM_COL32(200, 200, 100, 255);

	list->ChannelsSplit(2);
	list->ChannelsSetCurrent(0);
	for (int edgeIndex = 0; edgeIndex < int(m_edges.size()); ++edgeIndex)
	{
		Edge& edge = m_edges[edgeIndex];
		Node* nodeInp = m_nodes[edge.m_inputIndex];
		imvec p1 = offset + nodeInp->GetOutputSlotPos(edge.m_inputSlot);
		imvec p2 = offset;
		if (edge.m_outputIndex == -1 || edge.m_outputSlot == -1)
		{
			p2 = ImGui::GetMousePos();
		}
		else
		{
			Node* nodeOut = m_nodes[edge.m_outputIndex];
			p2 = p2 + nodeOut->GetInputSlotPos(edge.m_outputSlot);
			if (!nodeOut->m_isActive)
			{
				m_edges.erase(m_edges.begin() + edgeIndex);
				--edgeIndex;
				continue;
			}
		}

		if (nodeInp->m_isActive)
		{
			list->AddBezierCurve(p1, p1 + imvec(0, 50), p2 + imvec(0, -50), p2, EDGE_COLOR, 3.f);
		}
		else
		{
			m_edges.erase(m_edges.begin() + edgeIndex);
			--edgeIndex;
		}
	}
}

void BehaviorTreeEditor::DisplayNodes(ImDrawList * list, const imvec & offset)
{
	for (int nodeIndex = 0; nodeIndex < int(m_nodes.size()); ++nodeIndex)
	{
		Node* node = m_nodes[nodeIndex];
		if (!node->m_isActive)
		{
			continue;
		}

		ImGui::PushID(node->m_ID);
		imvec nodeRectMin = offset + node->m_pos;

		// display node contents
		list->ChannelsSetCurrent(1);
		bool oldAnyActive = ImGui::IsAnyItemActive();
		ImGui::SetCursorScreenPos(nodeRectMin + NODE_WINDOW_PADDING);
		ImGui::BeginGroup();

		node->DisplayNodeView();

		ImGui::EndGroup();

		// save the size of what was emitted and whether any widgets are active
		bool nodeWidgetsActive = (!oldAnyActive && ImGui::IsAnyItemActive());
		node->m_size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;

		// display node box
		list->ChannelsSetCurrent(0); // background
		ImGui::SetCursorScreenPos(nodeRectMin);
		ImGui::InvisibleButton("node", node->m_size);
		bool nodeHovered = ImGui::IsItemHovered();
		if (nodeHovered)
		{
			m_nodeHoveredInScene = node->m_ID;
			m_openContextMenu = ImGui::IsMouseClicked(1);
		}

		bool nodeMovingActive = ImGui::IsItemActive();
		if (nodeWidgetsActive || nodeMovingActive)
		{
			m_nodeSelected = node->m_ID;
		}
		if (nodeMovingActive && ImGui::IsMouseDragging(0))
		{
			node->m_pos = node->m_pos + ImGui::GetIO().MouseDelta;
		}

		// draw the node's shape
		bool isSelected = (m_nodeHoveredInList == node->m_ID || m_nodeHoveredInScene == node->m_ID || (m_nodeHoveredInList && m_nodeSelected == node->m_ID));
		node->DrawShape(list, offset, isSelected);

		// draw slots
		DisplaySlots(list, offset, nodeIndex);

		ImGui::PopID();
	}

	if (!m_edges.empty() && m_edges.back().m_outputSlot == -1 && ImGui::IsMouseReleased(0))
	{
		m_edges.pop_back();
	}
	list->ChannelsMerge();
}

void BehaviorTreeEditor::DisplayContextMenu(const imvec & offset)
{
	static int tempIn = -1;
	static int tempOut = -1;

	// open context menu
	if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseHoveringWindow() & ImGui::IsMouseClicked(1))
	{
		m_nodeSelected = m_nodeHoveredInList = m_nodeHoveredInScene = -1;
		m_openContextMenu = true;
	}
	else if (ImGui::IsAnyItemHovered() && ImGui::IsMouseHoveringWindow() && ImGui::IsMouseClicked(1))
	{
		m_openContextMenu = true;
	}
	else
	{
		m_openContextMenu = false;
	}

	if (m_openContextMenu)
	{
		ImGui::OpenPopup("context_menu");
		if (m_nodeHoveredInList != -1)
			m_nodeSelected = m_nodeHoveredInList;
		if (m_nodeHoveredInScene != -1)
			m_nodeSelected = m_nodeHoveredInScene;
		if (m_inputSlotHoveredInScene != -1 || m_outputSlotHoveredInScene != -1)
		{
			m_nodeSelected = -2;
			tempIn = m_inputSlotHoveredInScene;
			tempOut = m_outputSlotHoveredInScene;
		}
	}

	// draw context menu
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, imvec(8, 8));
	if (ImGui::BeginPopup("context_menu"))
	{
		Node* node = (m_nodeSelected >= 0) ? m_nodes[m_nodeSelected] : nullptr;
		imvec scenePos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;

		// case when hovering over a slot
		if (m_nodeSelected == -2)
		{
			if (ImGui::MenuItem("Delete Edge"))
			{
				for (size_t i = 0; i < m_edges.size(); ++i)
				{
					if (tempIn != -1 && m_edges[i].m_outputIndex == tempIn)
					{
						m_edges.erase(m_edges.begin() + i);
						--i;
					}
					else if (tempOut != -1 && m_edges[i].m_inputIndex == tempOut)
					{
						m_edges.erase(m_edges.begin() + i);
						--i;
					}
				}
				m_inputSlotHoveredInScene = m_outputSlotHoveredInScene = -1;
				ImGui::CloseCurrentPopup();
			}
		}
		// case hovering over a node
		else if (node)
		{
			ImGui::Text("Behavior Tree Node: %s", node->GetName());
			ImGui::Separator();
			
			if (ImGui::MenuItem("Delete"))
			{
				DeleteNode(node->m_ID);
			}
			if (node->m_nodeType == btnt_Composite)
			{
				if (ImGui::MenuItem("Add Output"))
				{
					++node->m_numOutputSlots;
					ImGui::CloseCurrentPopup();
				}
			}
			if (ImGui::MenuItem("Copy")) 
			{
				Copy(m_nodeSelected);
			}

			ImGui::Separator();
			node->DisplayContextMenu();
		}
		// case hovering over empty space
		else
		{
			if (ImGui::BeginMenu("Add Behavior Tree Node"))
			{
				static char buffer[100] = { 0 };
				static int nodeType = 0;
				ImGui::InputText("", buffer, 100);
				ImGui::Combo("Type", &nodeType, BEHAVIOR_TREE_NODE_TYPE_NAMES);

				if (ImGui::Button("Create Behavior Tree Node"))
				{
					AddNode(buffer, BehaviorTreeNodeType(nodeType), scenePos);
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Paste"))
			{
				Paste(scenePos);
			}
		}

		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();

}

void BehaviorTreeEditor::DisplaySlots(ImDrawList * list, const imvec & offset, int nodeID)
{
	static const ImU32 SLOT_HOVERED_COLOR = IM_COL32(0, 255, 0, 150);
	static const ImU32 SLOT_DEFAULT_COLOR = IM_COL32(75, 75, 75, 150);

	Node* node = m_nodes[nodeID];
	for (int slotIndex = 0; slotIndex < node->m_numInputSlots; ++slotIndex)
	{
		imvec pos = offset + node->GetInputSlotPos(slotIndex);
		pos.x -= 8.f;
		pos.y -= 6.f;
		ImGui::SetCursorScreenPos(pos);
		ImGui::InvisibleButton("inslot", NODE_SLOT_SIZE);
		pos.x += 8.f;
		pos.y += 6.f;

		bool itemHovered = ImGui::IsItemHovered();
		if (itemHovered)
		{
			list->AddCircleFilled(pos, NODE_SLOT_RADIUS, SLOT_HOVERED_COLOR);
			m_inputSlotHoveredInScene = node->m_ID;
		}
		else
		{
			list->AddCircleFilled(pos, NODE_SLOT_RADIUS, SLOT_DEFAULT_COLOR);
			if (!m_openContextMenu)
			{
				m_inputSlotHoveredInScene = -1;
			}
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("edge"))
			{
				if (payload->DataSize == 0 && !m_edges.empty())
				{
					if (FindEdgeWithOutput(node->m_ID) == nullptr)
					{
						Edge& edge = m_edges.back();
						edge.m_outputSlot = slotIndex;
						edge.m_outputIndex = node->m_ID;
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
	}

	for (int slotIndex = 0; slotIndex < node->m_numOutputSlots; ++slotIndex)
	{
		imvec pos = offset + node->GetOutputSlotPos(slotIndex);
		pos.x -= 8.f;
		ImGui::SetCursorScreenPos(pos);
		ImGui::InvisibleButton("outslot", NODE_SLOT_SIZE);
		pos.x += 8.f;

		bool itemHovered = ImGui::IsItemHovered();
		if (itemHovered)
		{
			list->AddCircleFilled(pos, NODE_SLOT_RADIUS, SLOT_HOVERED_COLOR);
			m_outputSlotHoveredInScene = node->m_ID;
		}
		else
		{
			list->AddCircleFilled(pos, NODE_SLOT_RADIUS, SLOT_DEFAULT_COLOR);
			if (!m_openContextMenu)
				m_outputSlotHoveredInScene = -1;
		}

		if (itemHovered && ImGui::IsMouseClicked(0))
		{
			for (size_t i = 0; i < m_edges.size(); ++i)
			{
				Edge& edge = m_edges[i];
				if (edge.m_inputIndex == node->m_ID)
				{
					if (node->m_nodeType != btnt_Composite || edge.m_inputSlot == slotIndex)
					{
						m_edges.erase(m_edges.begin() + i);
						break;
					}
				}
			}
			m_edges.push_back(Edge(node->m_ID, -1, slotIndex, -1));
		}
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("edge", nullptr, 0);
			ImGui::EndDragDropSource();
		}
	}
}

void BehaviorTreeEditor::Copy(int nodeID)
{
	if (nodeID == -1)
		return;

	Node* node = m_nodes[nodeID];
	if (m_clipboard)
	{
		delete m_clipboard;
		m_clipboard = nullptr;
	}
	imvec pos = node->m_pos + imvec(50, 50);
	switch (node->m_nodeType)
	{
		case btnt_Decorator:
		{
			m_clipboard = new BehaviorTreeEditorDecoratorNode(-1, node->GetName(), pos);
			break;
		}
		case btnt_Leaf:
		{
			m_clipboard = new BehaviorTreeEditorLeafNode(-1, node->GetName(), pos);
			break;
		}
		case btnt_Composite:
		{
			m_clipboard = new BehaviorTreeEditorCompositeNode(-1, node->GetName(), pos);
			break;
		}
	}
}

void BehaviorTreeEditor::Paste(const imvec & pos)
{
	if (!m_clipboard)
	{
		return;
	}

	imvec place = pos;
	if (pos.x == 0 && pos.y == 0)
	{
		place = m_clipboard->m_pos;
	}

	AddNode(m_clipboard->GetName(), m_clipboard->m_nodeType, place);
}

std::vector<BehaviorTreeEditorEdge*> BehaviorTreeEditor::FindEdgesWithInput(int inputIndex)
{
	std::vector<BehaviorTreeEditorEdge*> edges;

	for (Edge& edge : m_edges)
	{
		if (edge.m_inputIndex == inputIndex)
		{
			edges.push_back(&edge);
		}
	}
	return edges;
}

Edge * BehaviorTreeEditor::FindEdgeWithOutput(int outputIndex)
{
	for (Edge& edge : m_edges)
	{
		if (edge.m_outputIndex == outputIndex)
		{
			return &edge;
		}
	}
	return nullptr;
}
