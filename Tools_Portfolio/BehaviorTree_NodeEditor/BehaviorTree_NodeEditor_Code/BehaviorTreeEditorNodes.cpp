// Matthew Rosen and Brett Schiff
#include "stdafx.h"
#include "BehaviorTreeEditorNodes.h"
#include "NodeEditor.h"

// DO NOT CHANGE THE ORDER OF THESE VECTORS. THINGS CAN BE ADDED TO THE END, BUT NOWHERE ELSE
const std::vector<std::string> CompositeNodeTypes = { "Executor", "Selector", "Sequence" };
const std::vector<std::string> DecoratorNodeTypes = { "RunForTime", "Conditional", "ConditionalLoop", "Cooldown", "ForceFailure", "ForceSuccess", "Inverter" };

char const* ImGuiCompositeNodeTypes = "Executor\0Selector\0Sequence";
char const* ImGuiDecoratorNodeTypes = "RunForTime\0Conditional\0ConditionalLoop\0Cooldown\0ForceFailure\0ForceSuccess\0Inverter";

BehaviorTreeEditorDecoratorNode::BehaviorTreeEditorDecoratorNode(int id, const char* name, const imvec & pos)
	: BehaviorTreeEditorNode(id, pos, btnt_Decorator), m_name(name)
{
}

const char * BehaviorTreeEditorDecoratorNode::GetName() const
{
	return m_name.c_str();
}

void BehaviorTreeEditorDecoratorNode::SetName(const char * name)
{
	m_name = name;
}

void BehaviorTreeEditorDecoratorNode::DisplayNodeView()
{
	ImGui::Text("%s", m_name.c_str());
	ImGui::Text("%s Decorator Node", DecoratorNodeTypes[m_type].c_str());

	// show runtime or cooldown if runtime/cooldown node
	if (m_type == dt_RunForTime)
	{
		ImGui::Text("RunTime: %fs", m_floatA);
	}
	else if (m_type == dt_Cooldown)
	{
		ImGui::Text("Cooldown: %fs", m_floatA);
	}

	// show function used for condition checking if conditional node
	else if (m_type == dt_Conditional || m_type == dt_ConditionalLoop)
	{
		ImGui::Text("Lua Condition Function: %s", m_luaFunctionName.c_str());
	}
}

void BehaviorTreeEditorDecoratorNode::DisplayEditorView()
{
	if (ImGui::Combo("Type", (int *)(&m_nodeType), BEHAVIOR_TREE_NODE_TYPE_NAMES))
	{
		BehaviorTreeEditor::Instance().ChangeNodeType(m_ID, m_nodeType);
		ImGui::CloseCurrentPopup();
	}

	if (ImGui::Combo("DecoratorType", &m_type, ImGuiDecoratorNodeTypes))
	{
		ImGui::CloseCurrentPopup();
	}

	char buffer[100] = { 0 };
	std::strcpy(buffer, m_name.c_str());
	if (ImGui::InputText("Name", buffer, 100))
	{
		m_name = buffer;
	}

	// run for time and cooldown
	if (m_type == dt_RunForTime)
	{
		ImGui::DragFloat("Time to Run", &m_floatA, .1f, 0.f);
	}
	else if (m_type == dt_Cooldown)
	{
		ImGui::DragFloat("Cooldown", &m_floatA, .1f, 0.f);
	}

	// conditional and conditional loop
	else if (m_type == dt_Conditional || m_type == dt_ConditionalLoop)
	{
		ImGui::Text("Lua Function to Check for Condition");
		char funcNameBuffer[100] = { 0 };
		std::strcpy(funcNameBuffer, m_luaFunctionName.c_str());
		if (ImGui::InputText("Func Name", funcNameBuffer, 99))
		{
			m_luaFunctionName = funcNameBuffer;
		}
	}
}

void BehaviorTreeEditorDecoratorNode::GetData(int & nodeType, float & floatAData, std::string & stringAData, std::string & miscData)
{
	nodeType = m_type;
	floatAData = m_floatA;
	stringAData = m_luaFunctionName;
	miscData = m_name;
}

BehaviorTreeEditorLeafNode::BehaviorTreeEditorLeafNode(int id, const char* name, const imvec & pos)
	: BehaviorTreeEditorNode(id, pos, btnt_Leaf), m_name(name)
{
	m_numOutputSlots = 0;
}

const char * BehaviorTreeEditorLeafNode::GetName() const
{
	return m_name.c_str();
}

void BehaviorTreeEditorLeafNode::SetName(const char * name)
{
	m_name = name;
}

void BehaviorTreeEditorLeafNode::DisplayNodeView()
{
	ImGui::Text("%s", m_name.c_str());
	ImGui::Text("Leaf Node");
	ImGui::Text("Lua Function To Run: %s", m_luaFunctionName.c_str());
}

void BehaviorTreeEditorLeafNode::DisplayEditorView()
{
	if (ImGui::Combo("Type", (int *)(&m_nodeType), BEHAVIOR_TREE_NODE_TYPE_NAMES))
	{
		BehaviorTreeEditor::Instance().ChangeNodeType(m_ID, m_nodeType);
		ImGui::CloseCurrentPopup();
	}

	char buffer[100] = { 0 };
	std::strcpy(buffer, m_name.c_str());
	if (ImGui::InputText("Name", buffer, 100))
	{
		m_name = buffer;
	}

	ImGui::Text("Lua Function to Run");
	char funcNameBuffer[100] = { 0 };
	std::strcpy(funcNameBuffer, m_luaFunctionName.c_str());
	if (ImGui::InputText("Func Name", funcNameBuffer, 99))
	{
		m_luaFunctionName = funcNameBuffer;
	}
}

void BehaviorTreeEditorLeafNode::GetData(int & nodeType, float & floatAData, std::string & stringAData, std::string & miscData)
{
	nodeType = -1;
	floatAData = -1.f;
	stringAData = m_luaFunctionName;
	miscData = m_name;
}

BehaviorTreeEditorCompositeNode::BehaviorTreeEditorCompositeNode(int id, const char* name, const imvec & pos)
	: BehaviorTreeEditorNode(id, pos, btnt_Composite), m_name(name)
{
}

const char * BehaviorTreeEditorCompositeNode::GetName() const
{
	return m_name.c_str();
}

void BehaviorTreeEditorCompositeNode::SetName(const char * name)
{
	m_name = name;
}

void BehaviorTreeEditorCompositeNode::DisplayNodeView()
{
	ImGui::Text("%s", m_name.c_str());
	ImGui::Text("%s Composite Node", CompositeNodeTypes[m_type].c_str());
}

void BehaviorTreeEditorCompositeNode::DisplayEditorView()
{
	if (ImGui::Combo("Type", (int *)(&m_nodeType), BEHAVIOR_TREE_NODE_TYPE_NAMES))
	{
		BehaviorTreeEditor::Instance().ChangeNodeType(m_ID, m_nodeType);
		ImGui::CloseCurrentPopup();
	}

	if (ImGui::Combo("DecoratorType", &m_type, ImGuiCompositeNodeTypes))
	{
		ImGui::CloseCurrentPopup();
	}

	char buffer[100] = { 0 };
	std::strcpy(buffer, m_name.c_str());
	if (ImGui::InputText("Name", buffer, 100))
	{
		m_name = buffer;
	}

	ImGui::InputInt("Outputs", &m_numOutputSlots, 1, 1);
}

void BehaviorTreeEditorCompositeNode::GetData(int & nodeType, float & floatAData, std::string & stringAData, std::string & miscData)
{
	nodeType = m_type;
	floatAData = -1.f;
	stringAData = "";
	miscData = m_name;
}
