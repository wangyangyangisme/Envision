/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#pragma once

#include "../../interactionbase_api.h"

#include "Token.h"
#include "ParseResult.h"

namespace Interaction {

class OperatorDescriptorList;
class ExpressionTreeBuildInstruction;

struct INTERACTIONBASE_API ExpectedToken
{
	enum ExpectedType {ANY, TYPE, VALUE, ID, FIRST_DELIM, FOLLOWING_DELIM, END};
	// TYPE and VALUE are subtypes of ANY and are not yet used
	// FIRST_DELIM is used for the first delimiter or a multi-delimiter prefix/infix/postfix. E.g. delete [] expr
	// or new SPACE expr
	// FOLLOWING_DELIM is used for any delimiters immediately following FIRST_DELIM. Each prefix, postfix and infix
	// always starts with a FIRST_DELIM

	ExpectedToken(ExpectedType type, const QString& text) : type{type}, text{text} {}
	ExpectedToken(ExpectedType type = ANY) : type{type} {}

	ExpectedType type{ANY};
	QString text{};
};

class INTERACTIONBASE_API Parser {
	public:
		Parser(const OperatorDescriptorList* ops);
		QVector<ExpressionTreeBuildInstruction*> parse(QVector<Token> tokens);

	private:
		QVector<ExpressionTreeBuildInstruction*> parse(QVector<Token> tokens, ParseResult& parseResult);

		ParseResult parse(QVector<Token>::iterator token, ParseResult result, QList<ExpectedToken>& expected,
				bool hasLeft, QVector<ExpressionTreeBuildInstruction*>& instructions,
								ParseResult& bestParseSoFar);

		ParseResult processExpectedOperatorDelimiters(bool& processed, QList<ExpectedToken>& expected,
				QVector<Token>::iterator& token, ParseResult& result,
				QVector<ExpressionTreeBuildInstruction*>& instructions, ParseResult& bestParseSoFar);
		void processIdentifiersAndLiterals(bool& error, QList<ExpectedToken>& expected,
				QVector<Token>::iterator& token, bool& hasLeft,
				QVector<ExpressionTreeBuildInstruction*>& instructions);
		void processNewOperatorDelimiters(bool& processed, bool& error, QList<ExpectedToken>& expected,
				QVector<Token>::iterator& token, bool& hasLeft, ParseResult& result,
				QVector<ExpressionTreeBuildInstruction*>& instructions, ParseResult& bestParseSoFar,
				bool unexpectedIdentifierOrLiteral);
		void processSubExpression(bool& error, QList<ExpectedToken>& expected,
				QVector<Token>::iterator& token, bool& hasLeft, ParseResult& result,
				QVector<ExpressionTreeBuildInstruction*>& instructions);

		QVector<Token>::iterator endTokens_;
		const OperatorDescriptorList* ops_;
};

}
