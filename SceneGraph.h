#ifndef SCENEGRAPH_H
#define SCENEGRAPH_H

// Glm
#include <glm/glm.hpp>


namespace gl {

class Node;
class Visitor;

/**
 * @brief The SceneGraph class
 */
class SceneGraph
{

public:
    SceneGraph();
    virtual ~SceneGraph();

public:
    Node* root();
    const Node* root() const;

public:
    void getBB(glm::vec3& min, glm::vec3& max) const;

public:
    void executeVisitor(Visitor* visitor) const;

private:
    Node* m_rootNode;

};

}   // namespace gl

#endif
