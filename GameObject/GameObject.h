#pragma once

// An abstract base class representing GameObjects
class GameObject
{
public:

    // Add new GameObject types to this enumeration
    enum Type
    {
        OBJ_NONE = -1,
        OBJ_PLAYER = 0,
        OBJ_SAUCER,
        OBJ_LASER,
        OBJ_ALL = 999
    };

    // Creates a new GameObject instance and adds it to the update and draw lists
    GameObject( Point2f pos );
    // Creates a new GameObject instance and adds it to the update and draw lists
    GameObject( Matrix2D& mat );
    // Removes the instance from the update and draw lists
    virtual ~GameObject();

    // Virtual Update and Draw functions to be overridden by derived classes
    virtual void Update( GameState& state ) = 0;
    virtual void Draw( GameState& gState ) const = 0;

    // Sets the screen position of the GameObject
    void SetPosition( Point2f pos ) { m_worldMat.row[2] = pos; }
    // Gets the screen position of the GameObject
    Point2f GetPosition() const { return m_worldMat.row[2].As2D(); };
    // Sets the velocity of the GameObject in pixels per frame
    void SetVelocity( Vector2f vel ) { m_velocity = vel; }
    // Gets the velocity of the GameObject in pixels per frame
    Vector2f GetVelocity() const { return m_velocity; };
    // Sets the drawing order of the object (higher values are further away)
    void SetDrawOrder( int drawOrder ) { m_drawOrder = drawOrder; }
    // Gets the drawing order of the object (higher values are further away)
    int GetDrawOrder() const { return m_drawOrder; };
    // Sets the update order of the object (lower values are updated first)
    void SetUpdateOrder( int updateOrder ) { m_updateOrder = updateOrder; }
    // Gets the update order of the object (lower values are updated first)
    int GetUpdateOrder() const { return m_updateOrder; };

    // Uses the update list to count how many objects of each type there are
    static int GetObjectCount( Type eType );
    // Adds all the GameObjects of a specified type to a vector provided
    static int GetObjectList( GameObject::Type eType, std::vector< GameObject* >& vList );

    // Sorts the update list and uses it to update all GameObjects
    static void UpdateAll( GameState& state );
    // Sorts the draw list and uses it to draw all GameObjects
    static void DrawAll( GameState& state );
    // Uses the draw list to destroy all GameObjects
    static void DestroyAll();

protected:

    // Internal functions to define the way in which the Draw and Update lists are sorted
    static bool DrawOrder( const GameObject* a, const GameObject* b ) { return a->m_drawOrder > b->m_drawOrder; }
    static bool UpdateOrder( const GameObject* a, const GameObject* b ) { return a->m_updateOrder > b->m_updateOrder; }

    // Add new object types to the enumeration above
    Type m_type{ OBJ_NONE };
    // Inactive objects are destroyed at the end of the next update
    bool m_active{ true };
    // Velocity in pixels per frame
    Vector2f m_velocity{ 0, 0 };
    // Id of the attached sprite for drawing
    int m_spriteId{ -1 };
    // Higher values are further away
    int m_drawOrder{ 0 };
    // Lower values are updated first
    int m_updateOrder{ 0 };
    // Every GameObject has a world transformation matrix
    Matrix2D m_worldMat;

    // Static class members are shared between all instances of the class
    static std::vector< GameObject* > s_vUpdateList;
    static std::vector< GameObject* > s_vDrawList;

};

