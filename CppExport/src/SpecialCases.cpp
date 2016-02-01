/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2016 ETH Zurich
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

#include "SpecialCases.h"

#include "ExportHelpers.h"

#include "Export/src/tree/CompositeFragment.h"
#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/TypeAlias.h"

namespace CppExport {

void SpecialCases::handleQT_Flags(OOModel::Class* classs, Export::CompositeFragment* fragment)
{
	auto specialCaseFragment = fragment->append(new Export::CompositeFragment{classs});
	for (auto subDeclaration : *classs->subDeclarations())
		if (auto typeAlias = DCast<OOModel::TypeAlias>(subDeclaration))
			if (auto reference = DCast<OOModel::ReferenceExpression>(typeAlias->typeExpression()))
				if (reference->name() == "QFlags")
					*specialCaseFragment << "Q_DECLARE_OPERATORS_FOR_FLAGS(" << classs->name() << "::"
												<< typeAlias->name() << ")";
}

bool SpecialCases::isTestClass(OOModel::Class* classs)
{
	return classs->methods()->size() == 1 &&
			(classs->methods()->first()->name() == "test" || classs->methods()->first()->name() == "init");
}

}