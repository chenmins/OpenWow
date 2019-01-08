#pragma once

class IMesh;
class SceneNode;
class RenderEventArgs;
class IVisitor;

class Scene : public Object
{
public:
	virtual std::shared_ptr<SceneNode> GetRootNode() const = 0;

	virtual std::shared_ptr<SceneNode> CreateSceneNode(std::shared_ptr<SceneNode> parent = nullptr) = 0;

	virtual void ImportSceneNode(std::shared_ptr<SceneNode> parent, std::shared_ptr<IMesh> _mesh) = 0;

	virtual void Accept(IVisitor& visitor) = 0;
};