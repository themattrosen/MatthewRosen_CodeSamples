//Matthew Rosen and Brett Schiff
#pragma once

#include "BehaviorTreeEditorNode.h"
#include <vector>
#include <string>

// DO NOT CHANGE THE ORDER OF THESE VECTORS. THINGS CAN BE ADDED TO THE END, BUT NOWHERE ELSE
enum CompositeTypes
{
	ct_Executor = 0,
	ct_Selector,
	ct_Sequence
};

enum DecoratorTypes
{
	dt_RunForTime = 0,
	dt_Conditional,
	dt_ConditionalLoop,
	dt_Cooldown,
	dt_ForceFailure,
	dt_ForceSuccess,
	dt_Inverter
};

extern const std::vector<std::string> CompositeNodeTypes;
extern const std::vector<std::string> DecoratorNodeTypes;

struct BehaviorTreeEditorDecoratorNode : public BehaviorTreeEditorNode
{
	BehaviorTreeEditorDecoratorNode(int id, const char* name, const imvec &pos);
	const char* GetName() const override;
	void SetName(const char* name) override;
	void DisplayNodeView() override;
	void DisplayEditorView() override;

	void GetData(int& nodeType, float& floatAData, std::string& stringAData, std::string& miscData) override;

	const char* BASE_NAME = "Decorator Node";
	int m_type = 0;
	std::string m_name;

	// actual node information
	std::string m_luaFunctionName = "AI_Condition";
	float m_floatA = 0.f;
};

struct BehaviorTreeEditorLeafNode : public BehaviorTreeEditorNode
{
	BehaviorTreeEditorLeafNode(int id, const char* name, const imvec &pos);
	const char* GetName() const override;
	void SetName(const char* name) override;
	void DisplayNodeView() override;
	void DisplayEditorView() override;

	void GetData(int& nodeType, float& floatAData, std::string& stringAData, std::string& miscData) override;

	const char* BASE_NAME = "Leaf Node";
	std::string m_luaFunctionName = "AI_Update";
	std::string m_name;
};

struct BehaviorTreeEditorCompositeNode : public BehaviorTreeEditorNode
{
	BehaviorTreeEditorCompositeNode(int id, const char* name, const imvec &pos);
	const char* GetName() const override;
	void SetName(const char* name) override;
	void DisplayNodeView() override;
	void DisplayEditorView() override;

	void GetData(int& nodeType, float& floatAData, std::string& stringAData, std::string& miscData) override;

	const char* BASE_NAME = "Composite Node";
	int m_type = 0;

	std::string m_name;
};
