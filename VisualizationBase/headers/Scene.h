/***********************************************************************************************************************
 * Scene.h
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SCENE_H_
#define SCENE_H_

#include "visualizationbase_api.h"
#include "ModelRenderer.h"

#include <QtGui/QGraphicsScene>

namespace Visualization {

class SceneHandlerItem;
class VISUALIZATIONBASE_API Scene : public QGraphicsScene
{
	private:
		ModelRenderer* renderer_;
		SceneHandlerItem* sceneHandlerItem_;
		QList<Item*> topLevelItems;

	public:
		Scene();
		void setRenderer(ModelRenderer* renderer);

		ModelRenderer* renderer();

		void addTopLevelItem(Item* item);
		void removeTopLevelItem(Item* item);
		void updateTopLevelItems();

		virtual void customEvent(QEvent *event);

		virtual SceneHandlerItem* sceneHandlerItem();

};

inline void Scene::setRenderer(ModelRenderer* renderer) { renderer_ = renderer; }
inline ModelRenderer* Scene::renderer() { return renderer_; }
inline SceneHandlerItem* Scene::sceneHandlerItem() {return sceneHandlerItem_; }


}

#endif /* SCENE_H_ */
