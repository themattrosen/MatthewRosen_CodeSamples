/*
	------------------------------------------------------------------------------
		Licensing information can be found at the end of the file.
	------------------------------------------------------------------------------

	File:   BehaviorTreeEditorNode.cpp - v1.0
	Author: Matthew Rosen

	Summary:
		Implementation of BehaviorTreeEditorNode class functions.
		The BehaviorTreeEditorNode represents the editor backend version of the 
		BehaviorTree nodes. 

	Revision history:
		1.0		(07/07/2019)	initial release
*/

// includes
#include "stdafx.h"
#include "BehaviorTreeEditorNode.h"
#include "NodeEditor.h"
#include <string>

// ImGui Format string for a combo dropdown for the different hardcoded node types
const char* BEHAVIOR_TREE_NODE_TYPE_NAMES = "Decorator\0Leaf\0Composite\0";

// Displays ImGui right-click menu for editor nodes.
// Allows two basic functions: renaming a node and changing its type.
void BehaviorTreeEditorNode::DisplayContextMenu()
{
	// renaming context menu
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

	// change node type context menu
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

// Displays the proper shape of an editor node.
// @param drawList: draw channel to draw shapes to.
// @param offset: current offset based on if the user is scrolling or not. required to draw properly.
// @param isSelected: true if the user has this node selected.
// Function is designed to be extensible if different node types need to be drawn as different shapes.
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

// internal function
void BehaviorTreeEditorNode::GetData(int & nodeType, float & floatAData, std::string & stringAData, std::string & miscData)
{
	nodeType = -1;
	floatAData = -1;
	stringAData = "";
	miscData = "";
}

// Calculates the draw position of an input slot on the node
// Designed to be extensible if there are multiple input slots on a node
imvec BehaviorTreeEditorNode::GetInputSlotPos(int slotNum) const
{
	return imvec(m_pos.x + m_size.x * (float(slotNum) + 1) / (float(m_numInputSlots) + 1), m_pos.y );
}

// Calculates the draw position of an output slot on the node
// Designed to be extensible if there are multiple output slots on a node
imvec BehaviorTreeEditorNode::GetOutputSlotPos(int slotNum) const
{
	return imvec(m_pos.x + m_size.x * (float(slotNum) + 1) / (float(m_numOutputSlots) + 1), m_pos.y + m_size.y);
}

// non-default constructor for editor node.
BehaviorTreeEditorNode::BehaviorTreeEditorNode(int id, imvec pos, BehaviorTreeNodeType type)
	: m_ID(id), m_nodeType(type), m_isActive(true), m_numInputSlots(1), m_numOutputSlots(1), 
	m_pos(pos), m_size{ 0, 0 }
{
}

// non-default constructor for an editor edge between nodes.
BehaviorTreeEditorEdge::BehaviorTreeEditorEdge(int inIdx, int outIdx, int inSlot, int outSlot)
	: m_inputIndex(inIdx), m_outputIndex(outIdx), m_inputSlot(inSlot), m_outputSlot(outSlot)
{
}

// right click menu for editor edges.
void BehaviorTreeEditorEdge::DisplayContextMenu()
{
	//nothing by default
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
