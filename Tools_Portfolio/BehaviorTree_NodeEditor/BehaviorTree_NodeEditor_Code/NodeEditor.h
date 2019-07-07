//Matthew Rosen
#pragma once

#include "Core/Singleton.h"
#include "GameObject/GameObjectID.h"
#include "BehaviorTreeEditorNode.h"
#include "BehaviorTree/Nodes/Composite.h"
#include "BehaviorTree/Nodes/Decorator.h"
#include "BehaviorTree/Nodes/Leaf.h"
#include <vector>

class Space;

class BehaviorTreeEditor : public Singleton<BehaviorTreeEditor>
{
public:
	void Init(Space* currentSpace, GameObjectID id);
	void Display();
	void Clear();
	void Exit();

	void AddNode(const char* name, BehaviorTreeNodeType nodeType, const imvec& pos);
	void DeleteNode(int id);

	void ChangeNodeType(int id, BehaviorTreeNodeType newType);


private:
	friend class Singleton<BehaviorTreeEditor>;
	BehaviorTreeEditor();

	void DisplayMenuBar();
	void DisplayNodeList();
	void DisplayNodeEditor();
	void DisplayNodeView();
	void DisplayGrid(ImDrawList* list);
	void DisplayEdges(ImDrawList* list, const imvec& offset);
	void DisplayNodes(ImDrawList* list, const imvec& offset);
	void DisplayContextMenu(const imvec& offset);
	void DisplaySlots(ImDrawList* list, const imvec& offset, int nodeID);

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


	void Copy(int nodeID);
	void Paste(const imvec& pos);

	std::vector<BehaviorTreeEditorEdge*> FindEdgesWithInput(int inputIndex);
	BehaviorTreeEditorEdge* FindEdgeWithOutput(int outputIndex);

private:
	// access to current object being edited
	Space* m_currentSpace;
	GameObjectID m_currentObject;

	// node storage
	std::vector<BehaviorTreeEditorNode *> m_nodes;
	std::vector<int> m_openNodes;
	std::vector<BehaviorTreeEditorEdge> m_edges;

	BehaviorTreeEditorNode* m_clipboard;

	// editor info
	imvec m_scrolling;
	bool m_showGrid;
	int m_nodeSelected;
	bool m_openContextMenu;
	int m_nodeHoveredInList;
	int m_nodeHoveredInScene;
	int m_inputSlotHoveredInScene;
	int m_outputSlotHoveredInScene;
};
