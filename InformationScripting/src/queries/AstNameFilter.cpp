/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#include "AstNameFilter.h"

#include "ModelBase/src/nodes/composite/CompositeNode.h"
#include "ModelBase/src/nodes/Text.h"

#include "QueryRegistry.h"

namespace InformationScripting {

AstNameFilter::AstNameFilter(Model::SymbolMatcher matcher)
	: GenericFilter {
		  [matcher](const Tuple& t) {
			auto it = t.find("ast");
			if (it != t.end())
			{
				Model::Node* astNode = it->second;
				if (auto compositeNode = DCast<Model::CompositeNode>(astNode))
				{
					if (compositeNode->hasAttribute("name"))
					{
						if (auto nameText = DCast<Model::Text>(compositeNode->get("name")))
							return matcher.matches(nameText->get());
					}
				}
			}
			return false;
		}
}
{}

void AstNameFilter::registerDefaultQueries()
{
	QueryRegistry::instance().registerQueryConstructor("filter", [](Model::Node*, QStringList args) {
		Q_ASSERT(args.size());
		if (args[0].contains("*"))
		{
			QString regexString = args[0];
			regexString.replace("*", "\\w*");
			return new AstNameFilter(Model::SymbolMatcher(new QRegExp(regexString)));
		}
		return new AstNameFilter(Model::SymbolMatcher(args[0]));
	});
}

} /* namespace InformationScripting */
