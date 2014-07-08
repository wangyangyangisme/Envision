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

#include "../filepersistence_api.h"
#include "ModelBase/src/persistence/PersistentStore.h"

namespace FilePersistence {

class GenericNode;
class GenericNodeAllocator;

class FILEPERSISTENCE_API Parser {
	public:

		static void parseData(GenericNode* node, char* data, int start, int lineEnd);

		static void save(QTextStream& stream, GenericNode* node, int tabLevel = 0);
		static GenericNode* load(const QString& filename, bool lazy, GenericNodeAllocator* allocator);

	private:

		static int countTabs(char* data, int lineStart, int lineEnd);
		static QString rawStringToQString(char* data, int startAt, int endInclusive);
		static QString escape(const QString& line);

		static Model::NodeIdType toId(char* data, int start, int endInclusive, bool& ok);
		static uchar hexDigitToChar(char d, bool& ok);

		static bool nextNonEmptyLine(char* data, int dataSize, int& lineStart, int& lineEnd);
		static int indexOf(const char c, char* data, int start, int endInclusive);
		static bool nextHeaderPart(char* data, int& start, int&endInclusive, int lineEnd);
};

} /* namespace FilePersistence */
