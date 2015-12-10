/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2015 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
 **    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 **      following disclaimer in the documentation and/or other materials provided with the distribution.
 **    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
 **      derived from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 ** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 ** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 ** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 ** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 **********************************************************************************************************************/

#include "StandardMetaDefinitions.h"
#include "../ClangHelpers.h"
#include "MacroDefinitions.h"
#include "MacroExpansion.h"
#include "MacroExpansions.h"
#include "MacroArgumentInfo.h"
#include "MacroArgumentLocation.h"
#include "NodeToCloneMap.h"
#include "NodeHelpers.h"

#include "OOModel/src/allOOModelNodes.h"

namespace CppImport {

StandardMetaDefinitions::StandardMetaDefinitions(const ClangHelpers& clang, const MacroDefinitions& definitionManager,
																 MacroExpansions& macroExpansions)
	: clang_(clang), definitionManager_(definitionManager), macroExpansions_(macroExpansions) {}

OOModel::MetaDefinition* StandardMetaDefinitions::createMetaDef(const clang::MacroDirective* md)
{
	if (metaDefinition(md)) return nullptr;

	auto metaDef = new OOModel::MetaDefinition(definitionManager_.definitionName(md));
	standardMetaDefinitions_.insert(definitionManager_.signature(md), metaDef);

	// add formal arguments based on the expansion definition
	for (auto argName : clang_.argumentNames(md))
		metaDef->arguments()->append(new OOModel::FormalMetaArgument(argName));

	return metaDef;
}

OOModel::MetaDefinition* StandardMetaDefinitions::metaDefinition(const clang::MacroDirective* md)
{
	QString h = definitionManager_.signature(md);

	auto it = standardMetaDefinitions_.find(h);

	return it != standardMetaDefinitions_.end() ? *it : nullptr;
}

void StandardMetaDefinitions::createMetaDefinitionBody(OOModel::MetaDefinition* metaDef, QVector<Model::Node*> nodes,
																		 MacroExpansion* expansion, NodeToCloneMap& mapping,
																		 QVector<MacroArgumentInfo>& arguments)
{
	if (nodes.size() > 0)
	{
		// create a new context with type equal to the first node's context
		auto actualContext = NodeHelpers::actualContext(mapping.original(nodes.first()));
		metaDef->setContext(NodeHelpers::createContext(actualContext));

		// clone and add nodes to the metaDef
		for (auto n : nodes)
		{
			NodeToCloneMap childMapping;
			auto cloned = NodeHelpers::cloneWithMapping(mapping.original(n), childMapping);

			insertChildMetaCalls(expansion, childMapping);
			if (removeUnownedNodes(cloned, expansion, childMapping)) continue;
			insertArgumentSplices(mapping, childMapping, arguments);

			// handle predefined meta definition: SET_OVERRIDE_FLAG
			if (metaDef->arguments()->size() == 1)
				if (metaDef->arguments()->first()->name() == "OVERRIDE")
					if (auto ooMethod = DCast<OOModel::Method>(cloned))
						if (ooMethod->modifiers()->isSet(OOModel::Modifier::Virtual))
						{
							auto predefinedMetaCall = new OOModel::MetaCallExpression("SET_OVERRIDE_FLAG");
							predefinedMetaCall->arguments()->append(new OOModel::ReferenceExpression("OVERRIDE"));
							ooMethod->metaCalls()->append(predefinedMetaCall);
						}

			NodeHelpers::addNodeToDeclaration(cloned, metaDef->context());
		}
	}

	// add all child expansion meta calls that are not yet added anywhere else as declaration meta calls
	for (auto childExpansion : expansion->children())
		if (!childExpansion->metaCall()->parent())
			metaDef->context()->metaCalls()->append(childExpansion->metaCall());
}

void StandardMetaDefinitions::insertChildMetaCalls(MacroExpansion* expansion, NodeToCloneMap& childMapping)
{
	for (auto childExpansion : expansion->children())
	{
		// do not handle xMacro children here
		if (childExpansion->xMacroParent()) continue;

		// retrieve the node that the child meta call should replace
		if (auto replacementNode = childExpansion->replacementNode())
			// replacementNode is an original node therefore we need to get to the cloned domain first
			// clonedReplacementNode represents the cloned version of replacementNode
			if (auto clonedReplacementNode = childMapping.clone(replacementNode))
			{
				if (DCast<OOModel::VariableDeclaration>(clonedReplacementNode))
				{
					if (clonedReplacementNode->parent()->parent())
						clonedReplacementNode->parent()->parent()
								->replaceChild(clonedReplacementNode->parent(), childExpansion->metaCall());
					else
						qDebug() << "not inserted metacall" << clonedReplacementNode->typeName();
				}
				else if (!DCast<OOModel::Declaration>(clonedReplacementNode))
				{
					if (clonedReplacementNode->parent())
						clonedReplacementNode->parent()->replaceChild(clonedReplacementNode, childExpansion->metaCall());
					else
						qDebug() << "not inserted metacall" << clonedReplacementNode->typeName();
				}
			}
	}
}

void StandardMetaDefinitions::childrenUnownedByExpansion(Model::Node* node, MacroExpansion* expansion,
																			NodeToCloneMap& mapping, QVector<Model::Node*>& result)
{
	Q_ASSERT(expansion);

	// do not remove child meta calls
	if (DCast<OOModel::MetaCallExpression>(node)) return;

	if (auto original = mapping.original(node))
	{
		if (macroExpansions_.expansions(original).contains(expansion) ||
			 original->typeName() == "TypedListOfExpression")
		{
			for (auto child : node->children())
				childrenUnownedByExpansion(child, expansion, mapping, result);

			return;
		}


		result.append(node);
	}
}

bool StandardMetaDefinitions::removeUnownedNodes(Model::Node* cloned, MacroExpansion* expansion,
																 NodeToCloneMap& mapping)
{
	QVector<Model::Node*> unownedNodes;
	childrenUnownedByExpansion(cloned, expansion, mapping, unownedNodes);

	// if the unowned nodes contain the node itself then the node should not even be added to the meta definition
	if (unownedNodes.contains(cloned)) return true;

	NodeHelpers::removeNodesFromParent(NodeHelpers::topLevelNodes(unownedNodes));

	return false;
}

void StandardMetaDefinitions::insertArgumentSplices(NodeToCloneMap& mapping, NodeToCloneMap& childMapping,
																	 QVector<MacroArgumentInfo>& arguments)
{
	for (auto argument : arguments)
	{
		// map the argument node to the corresponding node in childMapping
		auto original = mapping.original(argument.node_);

		if (auto child = childMapping.clone(original))
		{
			// the first entry of the spelling history is where the splice for this argument should be
			auto spliceLoc = argument.history_.first();

			// the splice name is equal to the formal argument name where the argument is coming from
			auto argName = clang_.argumentNames(spliceLoc.expansion_->definition()).at(spliceLoc.argumentNumber_);
			auto newNode = new OOModel::ReferenceExpression(argName);

			// insert the splice into the tree
			if (child->parent()) child->parent()->replaceChild(child, newNode);
			childMapping.replaceClone(child, newNode);
		}
	}
}

void StandardMetaDefinitions::replaceWithReference(Model::Node* current, const QString& replacement,
																	NodeToCloneMap& mapping) const
{
	auto newValue = NodeHelpers::createNameExpressionFromString(replacement);
	current->parent()->replaceChild(current, newValue);
	mapping.replaceClone(current, newValue);
}

void StandardMetaDefinitions::replaceWithStringificationConcatenation(OOModel::StringLiteral* current,
																							 const QString& replacement,
																							 NodeToCloneMap& mapping) const
{
	if (!(replacement.startsWith('"') || replacement.startsWith('#')))
		return;

	//TODO: we assume that no regular string starts with #
	QStringList parts;
	bool foundStringification = false;
	bool inQuote = false;
	bool inName = false;
	bool escaped = false;
	for (auto ch : replacement)
	{
		Q_ASSERT(! (inQuote && inName));
		if (inQuote)
		{
			parts.last().append(ch);

			if (escaped) escaped = false;
			else if (ch == '\\') escaped = true;
			else if (ch == '"') inQuote = false;
		}
		else // In a name or just before/after a quote name
		{
			if (ch == '#')
			{
				foundStringification = true;
				inName = true;
				parts.append("#");
			}
			else if (ch == '"')
			{
				inName = false;
				inQuote = true;
				parts.append("\"");
			}
			else if (ch == '_' || ch.isLetterOrNumber())
			{
				parts.last().append(ch);
				Q_ASSERT(inName);
			}
			else Q_ASSERT(ch == ' ');// do nothing
		}
	}

	if (foundStringification)
	{
		auto newNode = constructStringConcatenation(parts);
		current->parent()->replaceChild(current, newNode);
		mapping.replaceClone(current, newNode);
	}
}

OOModel::Expression* StandardMetaDefinitions::constructStringConcatenation(QStringList strings) const
{
	Q_ASSERT(!strings.isEmpty());

	auto leftString = strings.takeFirst();
	OOModel::Expression* leftNode = nullptr;
	if (leftString.startsWith('#')) leftNode = new OOModel::ReferenceExpression(leftString);
	else
	{
		Q_ASSERT(leftString.startsWith('"') && leftString.endsWith('"') && leftString.size() >= 2);
		leftNode = new OOModel::StringLiteral(leftString.mid(1, leftString.length()-2));
	}

	if (strings.isEmpty()) return leftNode;
	else
		// TODO: We should have a dedicated operator for this. Equivalent to "asf" "something else" in C/C++
		return new OOModel::BinaryOperation(OOModel::BinaryOperation::PLUS, leftNode,
														constructStringConcatenation(strings));
}

}
