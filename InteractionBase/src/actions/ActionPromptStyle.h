/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2012 ETH Zurich
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

/*
 * ActionPromptStyle.h
 *
 *  Created on: Sep 27, 2012
 *      Author: Dimitar Asenov
 */

#ifndef InteractionBase_ACTIONPROMPTSTYLE_H_
#define InteractionBase_ACTIONPROMPTSTYLE_H_

#include "interactionbase_api.h"

#include "../vis/TextAndDescriptionStyle.h"

namespace Interaction {

class INTERACTIONBASE_API ActionPromptStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::SequentialLayoutStyle actionsContainer_;
		Visualization::TextStyle shortcutText_;
		TextAndDescriptionStyle actionStyle_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::SequentialLayoutStyle& actionsContainer() const;
		const Visualization::TextStyle&  shortcutText() const;
		const TextAndDescriptionStyle&  actionStyle() const;
};

inline const Visualization::SequentialLayoutStyle& ActionPromptStyle::layout() const {return layout_; }
inline const Visualization::SequentialLayoutStyle& ActionPromptStyle::actionsContainer() const
	{return actionsContainer_;}
inline const Visualization::TextStyle& ActionPromptStyle::shortcutText() const {return shortcutText_; }
inline const TextAndDescriptionStyle& ActionPromptStyle::actionStyle() const {return actionStyle_; }

} /* namespace Interaction */
#endif /* InteractionBase_ACTIONPROMPTSTYLE_H_ */