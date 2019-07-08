/*
	------------------------------------------------------------------------------
		Licensing information can be found at the end of the file.
	------------------------------------------------------------------------------

	File:   BehaviorTreeEditorNode.h - v1.0
	Author: Matthew Rosen

	Summary:
		Prototypes of BehaviorTreeEditorNode class functions.
		The BehaviorTreeEditorNode represents the editor backend version of the 
		BehaviorTree nodes. 

	Revision history:
		1.0		(07/07/2019)	initial release
*/
#pragma once

// Includes
#include <string>
#include "ImGui/ImGui.h"

// define non-member operator overloads for ImVec2's
// makes vector math easier
inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }

// ease of use
using imvec = ImVec2;

// different node types defined by AI programmer (Brett Schiff)
// uses team code style for enum value names
enum BehaviorTreeNodeType
{
	btnt_Decorator,
	btnt_Leaf,
	btnt_Composite,

	btnt_Count
};

// externs the ImGui Format string for a combo dropdown for the different hardcoded node types
extern const char* BEHAVIOR_TREE_NODE_TYPE_NAMES;

// Defines how nodes are treated in the node editor.
struct BehaviorTreeEditorNode
{
	int m_ID;							// ID is the index into the node editor's internal array of nodes
	BehaviorTreeNodeType m_nodeType;	// Type of node
	bool m_isActive;					// Active flag == true if it is meant to be displayed in the editor.

	int m_numInputSlots;				// Number of input slots total (for this project will always be 1).
	int m_numOutputSlots;				// Number of output slots total (1 for decorators/leaves, variable for composite)

	imvec m_pos;						// position within the grid
	imvec m_size;						// size of the node used to calculate slot positions for drawing.

	// name virtual functions based on node type
	virtual const char* GetName() const = 0;
	virtual void SetName(const char* name) = 0;

	// per-NodeType details of what the node box itself will display
	virtual void DisplayNodeView() = 0;

	// per-NodeType instructions on displaying the details in the editor side-bar
	virtual void DisplayEditorView() = 0;

	// default right-click menu display for all nodes, can be overridden.
	virtual void DisplayContextMenu();

	// default shape drawing function for the editor node, can be overridden to draw a different shape than a rectangle
	virtual void DrawShape(ImDrawList* drawList, const imvec& offset, bool isSelected);

	// AI programmer implemented
	virtual void GetData(int& nodeType, float& floatAData, std::string& stringAData, std::string& miscData);
	
	// helpers to get the ImGui positions of the in/output slots.
	imvec GetInputSlotPos(int slotNum) const;
	imvec GetOutputSlotPos(int slotNum) const;

	// non-default constructor
	BehaviorTreeEditorNode(int id, imvec pos, BehaviorTreeNodeType type);
};

// defines an edge between nodes in the node editor
struct BehaviorTreeEditorEdge
{
	int m_inputIndex;	// node ID that this edge inserts into
	int m_outputIndex;	// node ID that this edge comes out from
	int m_inputSlot;	// slot index of the input slot (will always be 0 for this project)
	int m_outputSlot;	// slot index of the output slot (can be non zero)

	// non-default constructor
	BehaviorTreeEditorEdge(int inIdx, int outIdx, int inSlot, int outSlot);
	
	// right-click menu for edges
	void DisplayContextMenu();
};

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
