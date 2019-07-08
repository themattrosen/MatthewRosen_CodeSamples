/*
	------------------------------------------------------------------------------
		Licensing information can be found at the end of the file.
	------------------------------------------------------------------------------

	File:   NodeEditor.cpp - v1.0
	Author: Matthew Rosen

	Summary:
		Implementation of BehaviorTreeEditor class functions.
		Singleton that opens a window of the node editor.

	Revision history:
		1.0		(07/07/2019)	initial release
*/

// includes
#include "stdafx.h"
#include "NodeEditor.h"
#include "Space/Space.h"
#include "BehaviorTreeEditorNodes.h"

// forward declarations (ease of use)
using Node = BehaviorTreeEditorNode;
using Edge = BehaviorTreeEditorEdge;

// local constants
static const float IM_WINDOW_PADDING = 4.f;
static const imvec NODE_WINDOW_PADDING{ 8.f, 8.f };
static const imvec NODE_SLOT_SIZE{ 16.f, 16.f };
static const float NODE_SLOT_RADIUS = 6.f;

// private default constructor
BehaviorTreeEditor::BehaviorTreeEditor()
	: m_currentSpace(nullptr), m_currentObject(0), m_nodes(), m_openNodes(), m_edges(), m_clipboard(nullptr),
	m_scrolling{ 0, 0 }, m_showGrid(true), m_nodeSelected(-1), m_openContextMenu(false),
	m_nodeHoveredInList(-1), m_nodeHoveredInScene(-1), m_inputSlotHoveredInScene(-1), m_outputSlotHoveredInScene(-1)
{

}

// public add node function
void BehaviorTreeEditor::AddNode(const char * name, BehaviorTreeNodeType nodeType, const imvec & pos)
{
	// empty node and id
	BehaviorTreeEditorNode* node = nullptr;
	int nextID = 0;

	// case of no open slots for reuse
	if (m_openNodes.empty())
	{
		// next id to create is the current size of the array
		nextID = int(m_nodes.size());
		m_nodes.push_back(nullptr);
	}

	// case there is an empty slot for reuse in node list
	else
	{
		// next id to create is the next value from openNodes
		nextID = m_openNodes.back();
		m_openNodes.pop_back();

		// delete the node currently in that slot
		delete m_nodes[nextID];
		m_nodes[nextID] = nullptr;
	}

	// allocate a new node based on the new node type
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

	// insert the new node
	m_nodes[nextID] = node;
}

// public interface to delete a node
void BehaviorTreeEditor::DeleteNode(int id)
{
	// simply set it to inactive and add its id to the open nodes list
	m_nodes[id]->m_isActive = false;
	m_openNodes.push_back(id);
}

// public interface to change the type of a node at runtime
void BehaviorTreeEditor::ChangeNodeType(int id, BehaviorTreeNodeType newType)
{
	// fetch the node to change and its information
	Node* node = m_nodes[id];
	const char* name = node->GetName();
	imvec pos = node->m_pos;

	// delete that node and re-add it
	// the index reuse will put the node in the same position
	DeleteNode(id);
	AddNode(name, newType, pos);
}

// editor initialization for a given GameObject
void BehaviorTreeEditor::Init(Space* currentSpace, GameObjectID id)
{
	// clear the current editor 
	Clear();
	
	// set the game object information
	m_currentObject = id;
	m_currentSpace = currentSpace;
}

// update function to display the editor
void BehaviorTreeEditor::Display()
{
	// draw the menu bar
	DisplayMenuBar();

	// draw the sub menu list of all active nodes
	DisplayNodeList();

	// draw the sub menu to show the editable properties of the currently selected node below the node list
	DisplayNodeEditor();

	// set the next draw location to draw the node view
	ImGui::SetCursorPos(imvec(200 + IM_WINDOW_PADDING * 4.f, IM_WINDOW_PADDING * 4.f + 26));

	// draw the node view
	DisplayNodeView();
}

// clears the nodes and edges list, handling allocated memory
void BehaviorTreeEditor::Clear()
{
	// clear openNodes and edges (non-dynamically allocated)
	m_openNodes.clear();
	m_edges.clear();

	// delete each allocated node in the nodes list
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

// called on editor shutdown
void BehaviorTreeEditor::Exit()
{
	// clear the lists
	Clear();

	// delete the clipboard if available
	if (m_clipboard)
	{
		delete m_clipboard;
		m_clipboard = nullptr;
	}
}

// draws the menu bar
void BehaviorTreeEditor::DisplayMenuBar()
{
	if(ImGui::BeginMenuBar())
	{
		// ADD submenu
		if (ImGui::BeginMenu("Add"))
		{
			// can add different types of nodes via the menu
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

		// FILE submenu
		if (ImGui::BeginMenu("File"))
		{
			// component related saving and loading
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

		// EDIT submenu
		if (ImGui::BeginMenu("Edit"))
		{
			// copy/paste can be done from here
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

// draw sub-menu to show list of active nodes
void BehaviorTreeEditor::DisplayNodeList()
{
	// create child section of a specified size with window border
	ImGui::BeginChild("Node List", imvec(200, 300), true);
	ImGui::Text("Node List");
	ImGui::Separator();

	// handle each node in the list.
	for (int nodeIndex = 0; nodeIndex < int(m_nodes.size()); ++nodeIndex)
	{
		Node* node = m_nodes[nodeIndex];

		// skip over inactive nodes
		if (!node || !node->m_isActive)
			continue;

		ImGui::PushID(node->m_ID);

		// if item is clicked, select it
		if (ImGui::Selectable(node->GetName(), node->m_ID == m_nodeSelected))
			m_nodeSelected = node->m_ID;

		// case item is hovered over
		if (ImGui::IsItemHovered())
		{
			m_nodeHoveredInList = node->m_ID;
			m_openContextMenu = ImGui::IsMouseClicked(1);
		}
		ImGui::PopID();
	}
	ImGui::EndChild();
}

// draw sub-menu to show editable view of selected node
void BehaviorTreeEditor::DisplayNodeEditor()
{
	// create child section of the specific size with window border
	ImGui::BeginChild("Node Editor View", imvec(200, 0), true);
	ImGui::Text("Node Editor");
	ImGui::Separator();

	// only display something if there is a node selected
	if (m_nodeSelected >= 0 && m_nodes.size() > 0)
	{
		Node* nodeSelected = m_nodes[m_nodeSelected];

		// case selected node is valid
		if (nodeSelected != nullptr && nodeSelected->m_isActive)
		{
			// call its display function
			nodeSelected->DisplayEditorView();
		}

		// case selected is invalid (means its been deleted while selected)
		else
		{
			// unselect the node
			m_nodeSelected = -1;
		}
	}

	ImGui::EndChild();
}

// draw the node view of the editor
void BehaviorTreeEditor::DisplayNodeView()
{
	// begin group of the window for top section
	ImGui::BeginGroup();

	// buttons to save to/load from the game object the editor is attached to.
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

	// grid toggle
	ImGui::Checkbox("Show grid", &m_showGrid);
	
	// window styles
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, imvec(1, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, imvec(0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, IM_COL32(60, 60, 70, 200));

	// create child section of the window for the actual view
	ImGui::BeginChild("scrolling_region", imvec(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
	ImGui::PushItemWidth(120.f);

	// for drawing primitives need the drawlist
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	imvec offset = ImGui::GetCursorScreenPos() + m_scrolling;

	// draw the grid
	DisplayGrid(drawList);

	// draw edges between nodes
	DisplayEdges(drawList, offset);

	// draw nodes
	DisplayNodes(drawList, offset);

	// draw the right-click menu depending on what's being right-clicked
	DisplayContextMenu(offset);

	// update the middle-click scrolling value
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

// draw the grid on the node view
void BehaviorTreeEditor::DisplayGrid(ImDrawList* list)
{
	// only draw if user wants it to
	if (!m_showGrid)
	{
		return;
	}

	// local constants
	static const ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);
	static const float GRID_SZ = 64.f;

	imvec winPos = ImGui::GetCursorScreenPos();
	imvec canvas_sz = ImGui::GetWindowSize();

	// draw the grid lines to the drawlist
	for (float x = std::fmod(m_scrolling.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
	{
		list->AddLine(imvec(x, 0.f) + winPos, imvec(x, canvas_sz.y) + winPos, GRID_COLOR);
	}
	for (float y = std::fmod(m_scrolling.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
	{
		list->AddLine(imvec(0.f, y) + winPos, imvec(canvas_sz.x, y) + winPos, GRID_COLOR);
	}
}

// draw edges between nodes
void BehaviorTreeEditor::DisplayEdges(ImDrawList * list, const imvec & offset)
{
	// local constants
	static const ImU32 EDGE_COLOR = IM_COL32(200, 200, 100, 255);

	// split the drawlist
	list->ChannelsSplit(2);
	list->ChannelsSetCurrent(0);

	// process each edge in the list
	for (int edgeIndex = 0; edgeIndex < int(m_edges.size()); ++edgeIndex)
	{
		// get the next each and its input node
		Edge& edge = m_edges[edgeIndex];
		Node* nodeInp = m_nodes[edge.m_inputIndex];

		// get beginning position and end position
		imvec p1 = offset + nodeInp->GetOutputSlotPos(edge.m_inputSlot);
		imvec p2 = offset;

		// case the edge doesn't have an output slot:
		// means the user is dragging from an output and hasn't connected the edge yet
		if (edge.m_outputIndex == -1 || edge.m_outputSlot == -1)
		{
			// end position is current mouse position
			p2 = ImGui::GetMousePos();
		}

		// case the edge has two ends
		else
		{
			// set end position
			Node* nodeOut = m_nodes[edge.m_outputIndex];
			p2 = p2 + nodeOut->GetInputSlotPos(edge.m_outputSlot);

			// handle case output has been deleted
			if (!nodeOut->m_isActive)
			{
				// delete the edge from the array
				m_edges.erase(m_edges.begin() + edgeIndex);
				--edgeIndex;
				continue;
			}
		}

		// case input node is active
		if (nodeInp->m_isActive)
		{
			// draw the bezier curve from input to output
			list->AddBezierCurve(p1, p1 + imvec(0, 50), p2 + imvec(0, -50), p2, EDGE_COLOR, 3.f);
		}

		// handle case input node has been deleted
		else
		{
			// delete the edge
			m_edges.erase(m_edges.begin() + edgeIndex);
			--edgeIndex;
		}
	}
}

// draw all nodes in the view
void BehaviorTreeEditor::DisplayNodes(ImDrawList * list, const imvec & offset)
{
	// loop through each node
	for (int nodeIndex = 0; nodeIndex < int(m_nodes.size()); ++nodeIndex)
	{
		// skip the node if it has been deleted
		Node* node = m_nodes[nodeIndex];
		if !node || !node->m_isActive)
		{
			continue;
		}

		ImGui::PushID(node->m_ID);

		// calculate bottom left corner of the node
		imvec nodeRectMin = offset + node->m_pos;

		// display node contents
		list->ChannelsSetCurrent(1); // foreground
		bool oldAnyActive = ImGui::IsAnyItemActive();
		ImGui::SetCursorScreenPos(nodeRectMin + NODE_WINDOW_PADDING);

		// create group for the node
		ImGui::BeginGroup();

		// polymorphically call the node's display function
		node->DisplayNodeView();

		ImGui::EndGroup();

		// save the size of what was emitted and whether any widgets are active
		bool nodeWidgetsActive = (!oldAnyActive && ImGui::IsAnyItemActive());
		node->m_size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;

		// display node box
		list->ChannelsSetCurrent(0); // background
		ImGui::SetCursorScreenPos(nodeRectMin);

		// create an invisible button to handle selection
		ImGui::InvisibleButton("node", node->m_size);

		// handle right-click and hovered coloring
		bool nodeHovered = ImGui::IsItemHovered();
		if (nodeHovered)
		{
			m_nodeHoveredInScene = node->m_ID;
			m_openContextMenu = ImGui::IsMouseClicked(1);
		}

		// handle selection
		bool nodeMovingActive = ImGui::IsItemActive();
		if (nodeWidgetsActive || nodeMovingActive)
		{
			m_nodeSelected = node->m_ID;
		}

		// handle moving the node around
		if (nodeMovingActive && ImGui::IsMouseDragging(0))
		{
			node->m_pos = node->m_pos + ImGui::GetIO().MouseDelta;
		}

		// polymorphically draw the node's shape
		bool isSelected = (m_nodeHoveredInList == node->m_ID || m_nodeHoveredInScene == node->m_ID || (m_nodeHoveredInList && m_nodeSelected == node->m_ID));
		node->DrawShape(list, offset, isSelected);

		// draw slots on the node
		DisplaySlots(list, offset, nodeIndex);

		ImGui::PopID();
	}

	// handle if the mouse is released while it was in the middle of creating a new edge
	if (!m_edges.empty() && m_edges.back().m_outputSlot == -1 && ImGui::IsMouseReleased(0))
	{
		m_edges.pop_back();
	}

	// unsplit the drawlist
	list->ChannelsMerge();
}

// draws the right click menu depending on where was clicked
void BehaviorTreeEditor::DisplayContextMenu(const imvec & offset)
{
	static int tempIn = -1;
	static int tempOut = -1;

	// private enum to dictate what is currently being hovered over
	enum HoveredType
	{
		ht_EdgeSelected = -2,
		ht_NothingSelected = -1,
		ht_NodeSelected = 0
	};

	// case open context menu
	if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseHoveringWindow() && ImGui::IsMouseClicked(1))
	{
		m_nodeSelected = m_nodeHoveredInList = m_nodeHoveredInScene = ht_NothingSelected;
		m_openContextMenu = true;
	}

	// case shouldn't open the context menu
	else
	{
		m_openContextMenu = false;
	}

	// handle opening context menu
	if (m_openContextMenu)
	{
		ImGui::OpenPopup("context_menu");

		// handle logic for exactly what the mouse is hovered over
		// -2 for hovering over a slot, -1 for blank space, ID for a node
		if (m_nodeHoveredInList != ht_NothingSelected)
			m_nodeSelected = m_nodeHoveredInList;
		if (m_nodeHoveredInScene != ht_NothingSelected)
			m_nodeSelected = m_nodeHoveredInScene;
		if (m_inputSlotHoveredInScene != ht_NothingSelected || m_outputSlotHoveredInScene != ht_NothingSelected)
		{
			m_nodeSelected = ht_EdgeSelected;
			tempIn = m_inputSlotHoveredInScene;
			tempOut = m_outputSlotHoveredInScene;
		}
	}

	// draw context menu
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, imvec(8, 8));
	if (ImGui::BeginPopup("context_menu"))
	{
		// get node and menu popup location
		Node* node = (m_nodeSelected >= 0) ? m_nodes[m_nodeSelected] : nullptr;
		imvec scenePos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;

		// case when hovering over a slot
		if (m_nodeSelected == ht_EdgeSelected)
		{
			// context menu just has edge deletion
			if (ImGui::MenuItem("Delete Edge"))
			{
				for (size_t i = 0; i < m_edges.size(); ++i)
				{
					if (tempIn != ht_NothingSelected && m_edges[i].m_outputIndex == tempIn)
					{
						m_edges.erase(m_edges.begin() + i);
						--i;
					}
					else if (tempOut != ht_NothingSelected && m_edges[i].m_inputIndex == tempOut)
					{
						m_edges.erase(m_edges.begin() + i);
						--i;
					}
				}
				m_inputSlotHoveredInScene = m_outputSlotHoveredInScene = ht_NothingSelected;
				ImGui::CloseCurrentPopup();
			}
		}

		// case hovering over a node
		else if (node != ht_NothingSelected)
		{
			// display default context menu
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

			// polymorphically display the node's context menu
			node->DisplayContextMenu();
		}

		// case hovering over empty space
		else
		{
			// can either create a new node in empty space
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

			// or paste a new node in empty space
			if (ImGui::MenuItem("Paste"))
			{
				Paste(scenePos);
			}
		}

		ImGui::EndPopup();
	}

	ImGui::PopStyleVar();
}

// draws the in/output slots over nodes
void BehaviorTreeEditor::DisplaySlots(ImDrawList * list, const imvec & offset, int nodeID)
{
	// local constant colors
	static const ImU32 SLOT_HOVERED_COLOR = IM_COL32(0, 255, 0, 150);
	static const ImU32 SLOT_DEFAULT_COLOR = IM_COL32(75, 75, 75, 150);

	// retrieve the current node
	Node* node = m_nodes[nodeID];

	// process each input slot (only 1)
	for (int slotIndex = 0; slotIndex < node->m_numInputSlots; ++slotIndex)
	{
		// get the slot position
		imvec pos = offset + node->GetInputSlotPos(slotIndex);

		// adjust for the button
		pos.x -= 8.f;
		pos.y -= 6.f;

		// add invisible button where slot is
		ImGui::SetCursorScreenPos(pos);
		ImGui::InvisibleButton("inslot", NODE_SLOT_SIZE);

		// adjust back for drawing
		pos.x += 8.f;
		pos.y += 6.f;

		// draw circle based on mouse hovering
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

		// handle drag/drop (this is a drop target) for creating new edges
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("edge"))
			{
				if (payload->DataSize == 0 && !m_edges.empty())
				{
					if (FindEdgeWithOutput(node->m_ID) == nullptr)
					{
						// set the output slot index in the edge that's being dragged
						Edge& edge = m_edges.back();
						edge.m_outputSlot = slotIndex;
						edge.m_outputIndex = node->m_ID;
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
	}

	// process each output slot
	for (int slotIndex = 0; slotIndex < node->m_numOutputSlots; ++slotIndex)
	{
		// get slot position
		imvec pos = offset + node->GetOutputSlotPos(slotIndex);

		// adjust position for button
		pos.x -= 8.f;

		// add invisible button where slot is
		ImGui::SetCursorScreenPos(pos);
		ImGui::InvisibleButton("outslot", NODE_SLOT_SIZE);

		// adjust position back for drawing
		pos.x += 8.f;

		// draw circle based on mouse hovering
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

		// handle drag/drop (this is a drag source) for creating new edges
		if (itemHovered && ImGui::IsMouseClicked(0))
		{
			// if applicable, find and delete the edge currently attached to this output slot
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

			// add an edge with invalid outputs.
			m_edges.push_back(Edge(node->m_ID, -1, slotIndex, -1));
		}

		// signal ImGui if drag/drop has been initiated
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("edge", nullptr, 0);
			ImGui::EndDragDropSource();
		}
	}
}

// copies a given node to the clipboard
void BehaviorTreeEditor::Copy(int nodeID)
{
	// check input validity
	if (nodeID == -1)
		return;

	// retrieve the node
	Node* node = m_nodes[nodeID];

	// delete current clipboard
	if (m_clipboard)
	{
		delete m_clipboard;
		m_clipboard = nullptr;
	}

	// calculate new node position
	imvec pos = node->m_pos + imvec(50, 50);

	// allocate a new node to the clipboard based on type with an invalid id
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

// pastes a new node from the clipboard
void BehaviorTreeEditor::Paste(const imvec & pos)
{
	// handles an empty clipboard
	if (!m_clipboard)
	{
		return;
	}

	// case pos is invalid (0, 0)
	imvec place = pos;
	if (pos.x == 0 && pos.y == 0)
	{
		// use clipboard's position
		place = m_clipboard->m_pos;
	}

	// add node to the view
	AddNode(m_clipboard->GetName(), m_clipboard->m_nodeType, place);
}

// helper find edges with input
std::vector<BehaviorTreeEditorEdge*> BehaviorTreeEditor::FindEdgesWithInput(int inputIndex)
{
	// vector to return
	// returning by value, this function is only used on compile, which is a costly operation anyway
	std::vector<BehaviorTreeEditorEdge*> edges;

	// search edge list
	for (Edge& edge : m_edges)
	{
		if (edge.m_inputIndex == inputIndex)
		{
			edges.push_back(&edge);
		}
	}

	// return vector by value
	return edges;
}

Edge * BehaviorTreeEditor::FindEdgeWithOutput(int outputIndex)
{
	// search edge list
	for (Edge& edge : m_edges)
	{
		if (edge.m_outputIndex == outputIndex)
		{
			return &edge;
		}
	}
	
	return nullptr;
}

/*
	------------------------------------------------------------------------------
	This software is available under 2 licenses - you may choose the one you like.
	------------------------------------------------------------------------------
	ALTERNATIVE A - zlib license
	Copyright (c) 2019 Matthew Rosen
	This software is provided 'as-is', without any express or implied warranty.
	In no event will the authors be held liable for any damages arising from
	the use of this software.
	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:
	  1. The origin of this software must not be misrepresented; you must not
		 claim that you wrote the original software. If you use this software
		 in a product, an acknowledgment in the product documentation would be
		 appreciated but is not required.
	  2. Altered source versions must be plainly marked as such, and must not
		 be misrepresented as being the original software.
	  3. This notice may not be removed or altered from any source distribution.
	------------------------------------------------------------------------------
	ALTERNATIVE B - Public Domain (www.unlicense.org)
	This is free and unencumbered software released into the public domain.
	Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
	software, either in source code form or as a compiled binary, for any purpose,
	commercial or non-commercial, and by any means.
	In jurisdictions that recognize copyright laws, the author or authors of this
	software dedicate any and all copyright interest in the software to the public
	domain. We make this dedication for the benefit of the public at large and to
	the detriment of our heirs and successors. We intend this dedication to be an
	overt act of relinquishment in perpetuity of all present and future rights to
	this software under copyright law.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
	ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
	WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
	------------------------------------------------------------------------------
*/
