/*
	------------------------------------------------------------------------------
		Licensing information can be found at the end of the file.
	------------------------------------------------------------------------------

	File:   NodeEditor.h - v1.0
	Author: Matthew Rosen

	Summary:
		Prototypes of BehaviorTreeEditor class functions.
		Singleton that opens a window of the node editor.

	Revision history:
		1.0		(07/07/2019)	initial release
*/
#pragma once

// includes
#include "Core/Singleton.h"
#include "GameObject/GameObjectID.h"
#include "BehaviorTreeEditorNode.h"
#include "BehaviorTree/Nodes/Composite.h"
#include "BehaviorTree/Nodes/Decorator.h"
#include "BehaviorTree/Nodes/Leaf.h"
#include <vector>

// forward declarations
class Space;

// Singleton that opens a window of the node editor.
class BehaviorTreeEditor : public Singleton<BehaviorTreeEditor>
{
public:

	// must be called before start using
	// initialized to a specific GameObject using its current space and GameObjectID.
	void Init(Space* currentSpace, GameObjectID id);

	// called each frame to display all nodes and subwindows
	void Display();

	// deletes all nodes
	void Clear();

	// called at engine shutdown or after usage is finished
	// frees all allocated memory and clears arrays
	void Exit();

	// public interface to add a new node to the grid
	void AddNode(const char* name, BehaviorTreeNodeType nodeType, const imvec& pos);

	// public interface to delete a node with a given ID
	void DeleteNode(int id);

	// public interface to change the type of a given node
	void ChangeNodeType(int id, BehaviorTreeNodeType newType);

private:
	// used to enforce Singleton pattern
	friend class Singleton<BehaviorTreeEditor>;
	BehaviorTreeEditor();

	// helper functions

	// called in Display to render the window's menu bar and associated sub-menus
	void DisplayMenuBar();

	// called in Display to render side sub-window that shows a list of all current nodes
	void DisplayNodeList();

	// called in Display to render side sub-window that shows the editable properties of the current node selected
	void DisplayNodeEditor();

	// called in Display to render the sub-window that shows the main node editor view
	void DisplayNodeView();

	// called in DisplayNodeView to render the background grid of lines
	void DisplayGrid(ImDrawList* list);

	// called in DisplayNodeView to draw all edges between nodes
	void DisplayEdges(ImDrawList* list, const imvec& offset);

	// called in DisplayNodeView to draw all individual nodes
	void DisplayNodes(ImDrawList* list, const imvec& offset);

	// called from DisplayNodeView if the right mouse button is clicked
	void DisplayContextMenu(const imvec& offset);

	// called from DisplayNodeView to draw all the in/output slots on top of their corresponding nodes
	// also handles the drag/drop regions and invisible buttons
	void DisplaySlots(ImDrawList* list, const imvec& offset, int nodeID);

	// BEGIN implementation by AI programmer Brett Schiff
	void CompileToComponent();
	void SyncFromComponent();
	bool ValidateGraph();

	// helper functions for the above
	int FindHeadOfTree();
	void ProcessNode(size_t nodeIndex);
	bvt_CompositeNode* CompileNode_Recursive_Composite(AIComponent& AiComponent, BehaviorTreeEditorNode* compositeNode, size_t nodeIndex);
	bvt_DecoratorNode* CompileNode_Recursive_Decorator(AIComponent& AiComponent, BehaviorTreeEditorNode* decoratorNode, size_t nodeIndex);
	bvt_LeafNode*      CompileNode_Recursive_Leaf(AIComponent& AiComponent, BehaviorTreeEditorNode*      leafNode, size_t nodeIndex);
	int SyncNode_Recursive_Composite(bvt_CompositeNode* compositeNode, int itNumX, int itNumY);
	int SyncNode_Recursive_Decorator(bvt_DecoratorNode* decoratorNode, int itNumX, int itNumY);
	int      SyncNode_Recursive_Leaf(bvt_LeafNode*           leafNode, int itNumX, int itNumY);
	// END implementation by AI programmer Brett Schiff

	// copy/paste functionality for nodes
	void Copy(int nodeID);
	void Paste(const imvec& pos);

	// helper to find edges between specific nodes
	std::vector<BehaviorTreeEditorEdge*> FindEdgesWithInput(int inputIndex);
	BehaviorTreeEditorEdge* FindEdgeWithOutput(int outputIndex);

private:
	// access to current object being edited
	Space* m_currentSpace;
	GameObjectID m_currentObject;

	// node storage
	std::vector<BehaviorTreeEditorNode *> m_nodes;	// list of all node slots. includes inactive nodes that can be reused.
													// nodes stored by dynamic allocation for polymorphism to work.
	std::vector<int> m_openNodes;					// list of open slots available for reuse in the nodes list.
	std::vector<BehaviorTreeEditorEdge> m_edges;	// list of edges currently being used. stored contiguously, not reused. 

	BehaviorTreeEditorNode* m_clipboard;	// clipboard of copied node to be pasted

	// editor info
	imvec m_scrolling;				// vec2 scrolling increment this frame
	bool m_showGrid;				// user can set if they want to display the grid behind the nodes
	int m_nodeSelected;				// id of the node selected (index into m_nodes). doesn't support multiselect.
	bool m_openContextMenu;			// flag if the right-click menu is open
	int m_nodeHoveredInList;		// id of the node hovered in the list sub-menu. -1 if not hovered.
	int m_nodeHoveredInScene;		// id of the node hovered in the node view. -1 if not hovered.
	int m_inputSlotHoveredInScene;	// input slot number hovered over in node view. -1 if not hovered. also uses m_nodeHoveredInScene
	int m_outputSlotHoveredInScene;	// output slot number hovered over in node view. -1 if not hovered. also uses m_nodeHoveredInScene
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
