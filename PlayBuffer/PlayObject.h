#ifndef PLAY_PLAYOBJECT_H
#define PLAY_PLAYOBJECT_H
//********************************************************************************************************************************
// PlayBuffer provides a managed Play::GameObject structure which can be extended below. Adding additional member variables to
// the struct will cause them to be added to ALL GameObjects regardless of their type, so you may want to consider creating your 
// own GameObject class heirarchy on the longer term.
// Note: Play::GameObject and its associated functionality is only included if you #define PLAY_USING_GAMEOBJECT_MANAGER
//********************************************************************************************************************************
#ifdef PLAY_USING_GAMEOBJECT_MANAGER

// [DOXYGEN_EXCLUDE]
//! @defgroup gameobject GameObject
//! @brief PlayBuffer has an optional GameObject system included as part of it. A GameObject is a structure (or 'struct') provided by PlayManager that represents an interactive object in a typical game. Each GameObject has a set of useful, common properties and PlayManager has a number of functions for managing them. Note that Play::GameObject and its associated functionality is only included if you add $sc\#define PLAY_USING_GAMEOBJECT_MANAGER$ec to the top of your main code file.
// [DOXYGEN_EXCLUDE]
	
//! \brief Main Namespace for PlayBuffer
namespace Play
{
	//! @brief The Gameobject struct. Holds all the data that a GameObject requires, and can be extended by the user.
	//! @snippet PlayExamples.cpp GameObject
	//! @ingroup gameobject
	struct GameObject
	{
		//! @brief GameObject constructor.
		//! @param type The type of the GameObject.
		//! @param pos The initial x/y coordinates of the GameObject.
		//! @param collisionRadius The radius of the collision circle of this GameObject.
		//! @param spriteId The sprite ID of the GameObject.
		GameObject(int type, Point2D pos, int collisionRadius, int spriteId); 

		// Default member variables: don't change these!
		//! A number representing the type of the GameObject as an int or enum. So a type of 1 might correspond to a health pickup and a type of 2 might correspond to a missile, for example. The only type value defined by PlayManager is -1, which corresponds to "no type". It is up to the user to decide how to assign other GameObject types. PlayManager will simply treat each unique value as a distinct type.
		int type{ -1 };
		//! If the object has changed type since it was created, then this holds the previous type of the object.
		int oldType{ -1 };
		//! The unique id of the sprite to be associated with the GameObject as used in certain DrawSprite commands.
		int spriteId{ -1 }; 
		//! The x/y position where the origin of the GameObject is placed. Used for calculating collisions as well as drawing the sprite.
		Point2D pos{ 0.0f, 0.0f }; 
		//! The x/y position of the GameObject from the previous frame.
		Point2D oldPos{ 0.0f, 0.0f };
		//! An x/y vector that determines how the position of the GameObject changes each time it is updated. The x-velocity will be added to the x position, and y-velocity to the y position.
		Vector2D velocity{ 0.0f, 0.0f };
		//! An x/y vector that determines how the velocity of the GameObject changes each time it is updated. The x-acceleration will be added to the x velocity, and y-acceleration to the y velocity.
		Vector2D acceleration{ 0.0f, 0.0f };
		//! The angle by which GameObject should be rotated when it is drawn. Measured in radians, clockwise from 12-o'clock.
		float rotation{ 0.0f };
		 //! A constant value which is added to the rotation of the GameObject each time it is updated.
		float rotSpeed{ 0.0f };
		//! The angle of rotation of the GameObject from the previous frame.
		float oldRot{ 0.0f };
		 //! The sprite frame to use when the GameObject is drawn.
		int frame{ 0 };
		//! The fraction of the sprite frame that the animation is currently on. animSpeed adds on to this value every frame, and when this rolls over 1, then it resets back down and increments frame.
		float framePos{ 0.0f };
		//! The amount the sprite frame is increased by each time the GameObject is updated. Numbers greater than 1 will result in frame-skipping. Numbers less than 1 will slow the animation down, but are best kept to fractions that divide equally into 1 (0.2, 0.5. 0.333, etc).
		float animSpeed{ 0.0f };
		//! The distance away from GameObject's origin to detect collisions. Measured in pixels.
		int radius{ 0 };
		//! The size to draw the sprite associated with the GameObject. 1.0f is full size, 0.5f half size, 2.0f double size, and so on.
		float scale{ 1 };
		int order{ 0 };
		//! What frame did this GameObject last get updated on? This stops GameObjects being updated multiple times per frame.
		int lastFrameUpdated{ -1 };

		// Add your own member variables here and every GameObject will have them
		// int something{ 0 };

		//! @brief Allows you to get the unique ID of a GameObject if you only have a reference or a copy of it.
		//! @return This GameObject's unique ID.
		int GetId() { return m_id; }

	private:
		// The GameObject's id should never be changed manually so we make it private!
		int m_id{ -1 };

		// Preventing assignment and copying reduces the potential for bugs
		GameObject& operator=(const GameObject&) = delete;
		GameObject(const GameObject&) = delete;
	};

	// GameObject functions
	//**************************************************************************************************

	//! @brief Creates a new GameObject and adds it to the managed list.
	//! @snippet PlayExamples.cpp CreateGameObject
	//! @ingroup gameobject
	//! @param type The type of the GameObject.
	//! @param pos The initial x/y coordinates of the GameObject.
	//! @param collisionRadius The radius of the collision circle of this GameObject.
	//! @param spriteName The name of the sprite to use for the GameObject.
	//! @return Returns the new object's unique id.
	int CreateGameObject(int type, Point2D pos, int collisionRadius, const char* spriteName);
	//! @brief Retrieves a GameObject from the ID passed to this function.
	//! @snippet PlayExamples.cpp GetGameObject
	//! @ingroup gameobject
	//! @param id The ID of the GameObject you wish to retrieve.
	//! @return The game object associated with that ID. An object with a type of -1 is returned if no object can be found.
	GameObject& GetGameObject(int id);
	//! @brief Retrieves the first GameObject matching the type that you pass through as a parameter.
	//! @snippet PlayExamples.cpp GetGameObjectByType
	//! @ingroup gameobject
	//! @param type The type of the GameObject you wish to retrieve.
	//! @return The first GameObject that matches the given type. An object with a type of -1 is returned if no object can be found.
	GameObject& GetGameObjectByType(int type);
	//! @brief Collects the IDs of all of the GameObjects with the matching type
	//! @snippet PlayExamples.cpp CollectGameObjectIDsByType
	//! @ingroup gameobject
	//! @param type The type of the GameObjects you wish to retrieve.
	//! @return A vector containing IDs of GameObjects of that type. The vector will be empty if no GameObjects match the type.
	std::vector<int> CollectGameObjectIDsByType(int type);
	//! @brief Collects the IDs of all of the GameObjects
	//! @snippet PlayExamples.cpp CollectAllGameObjectIDs
	//! @ingroup gameobject
	//! @return A vector containing the IDs of all of the GameObjects that the manager contains. The vector will be empty if there are no GameObjects.
	std::vector<int> CollectAllGameObjectIDs();
	//! @brief Performs a typical update of the object's position and animation. Changes its velocity by its acceleration, its position by its velocity, its rotation by its rotation speed, and its animation frame by its animation speed.
	//! @snippet PlayExamples.cpp UpdateGameObject
	//! @ingroup gameobject
	//! @note Can only be called once per object per frame unless allowMultipleUpdatesPerFrame is set to true.
	//! @param object The GameObject you wish to update.
	//! @param bWrap Should the object wrap around the edge of the screen to the other side? Defaults to no.
	//! @param wrapBorderSize If the object is wrapping, then how far off the edge of the screen should the object get before it wraps? Defaults to 0 pixels.
	//! @param allowMultipleUpdatesPerFrame If set to true, then this allows for the object to be updated again if it already has this frame.
	void UpdateGameObject(GameObject& object, bool bWrap = false, int wrapBorderSize = 0, bool allowMultipleUpdatesPerFrame = false);
	//! @brief Deletes the GameObject with the corresponding Id.
	//! @snippet PlayExamples.cpp DestroyGameObject
	//! @ingroup gameobject
	//! @param id The unique id of the GameObject you wish to delete.
	void DestroyGameObject(int id);
	//! @brief Deletes all GameObjects with the corresponding type.
	//! @snippet PlayExamples.cpp DestroyGameObjectsByType
	//! @ingroup gameobject
	//! @param type The type of the GameObjects you wish to delete.
	void DestroyGameObjectsByType(int type);
	//! @brief Deletes all GameObjects.
	//! @snippet PlayExamples.cpp DestroyAllGameObjects
	//! @ingroup gameobject
	void DestroyAllGameObjects();

	//! @brief Checks whether the two GameObjects are within each other's collision radii.
	//! @snippet PlayExamples.cpp IsColliding
	//! @ingroup gameobject
	//! @param obj1 The first GameObject we want to check has collided.
	//! @param obj2 The second GameObject we want to check has collided.
	//! @return Returns true if the GameObjects are overlapping, false otherwise.
	bool IsColliding(GameObject& obj1, GameObject& obj2);
	//! @brief Checks whether any part of the GameObject is visible within the DisplayBuffer
	//! @snippet PlayExamples.cpp IsVisible
	//! @ingroup gameobject
	//! @param obj The GameObject that we want to check for visibility.
	bool IsVisible(GameObject& obj);
	//! @brief Checks whether the GameObject is overlapping the edge of the screen and moving outwards.
	//! @snippet PlayExamples.cpp IsLeavingDisplayArea
	//! @ingroup gameobject
	//! @param obj The GameObject that we want to check for overlapping.
	//! @param dirn Which side of the screen are we checking? Defaults to all sides.
	//! @return Returns true if the object is overlapping, false otherwise.
	bool IsLeavingDisplayArea(GameObject& obj, Direction dirn = Direction::ALL);
	//! @brief Checks whether the animation has completed playing.
	//! @snippet PlayExamples.cpp IsAnimationComplete
	//! @ingroup gameobject
	//! @param obj The GameObject that we want to check.
	//! @return Returns true if the animation has finished, false if it hasn't.
	bool IsAnimationComplete(GameObject& obj);
	//! @brief Sets the velocity and angle of the GameObject.
	//! @snippet PlayExamples.cpp SetGameObjectDirection
	//! @ingroup gameobject
	//! @param obj The GameObject you wish to change the velocity and direction of.
	//! @param speed The number of pixels you want the GameObject to move in a straight line each time it is updated.
	//! @param rotation The angle you want the GameObject to move in.
	void SetGameObjectDirection(GameObject& obj, int speed, float rotation);
	//! @brief Sets the rotation of the object to point towards the target coordinate.
	//! @snippet PlayExamples.cpp PointGameObject
	//! @ingroup gameobject
	//! @param obj The GameObject you wish to change the direction of.
	//! @param targetX The x coordinate of the point you wish the object to point at.
	//! @param targetY The y coordinate of the point you wish the object to point at.
	void PointGameObject(GameObject& obj,int targetX, int targetY);
	//! @brief Changes the GameObject's current spite and resets its animation frame to the start.
	//! @snippet PlayExamples.cpp SetSprite
	//! @ingroup gameobject
	//! @param obj The GameObject you wish to set the sprite for.
	//! @param spriteName The name of the sprite you wish to set for the GameObject.
	//! @param animSpeed The number of frames to increase the animation by each time the GameObject is updated.
	void SetSprite(GameObject& obj, const char* spriteName, float animSpeed );
	//! @brief Changes the GameObject's current spite and resets its animation frame to the start.
	//! @snippet PlayExamples.cpp SetSprite
	//! @ingroup gameobject
	//! @param obj The GameObject you wish to set the sprite for.
	//! @param spriteIndex The index of the sprite you wish to set for the GameObject.
	//! @param animSpeed The number of frames to increase the animation by each time the GameObject is updated.
	void SetSprite( GameObject& obj, int spriteIndex, float animSpeed );
	//! @brief Draws the GameObject's sprite without rotation or transparency. This is the fastest way to draw a GameObject, and so should be the preferred method when rotation and alpha are not required.
	//! @snippet PlayExamples.cpp DrawObject
	//! @ingroup gameobject
	//! @param obj The GameObject you wish to draw.
	void DrawObject(GameObject& obj);
	//! @brief Draws the GameObject's sprite with transparency. This is slower than DrawObject and so should only be used if you require transparency.
	//! @snippet PlayExamples.cpp DrawObjectTransparent
 	//! @ingroup gameobject
	//! @param obj The GameObject you wish to draw.
	//! @param opacity How transparent the object should be. 0.0f is fully transparent and 1.0f is fully opaque.
	void DrawObjectTransparent(GameObject& obj, float opacity);
	//! @brief Draws the object's sprite with rotation, scale, and transparency. This is slower than DrawObject or DrawObjectTransparent and so should only be used if you require rotation or scale. Rotation and scale are taken from the properties of the GameObject.
	//! @snippet PlayExamples.cpp DrawObjectRotated
	//! @ingroup gameobject
	//! @param obj The GameObject you wish to draw.
	//! @param opacity How transparent the object should be. 0.0f is fully transparent and 1.0f is fully opaque.
	void DrawObjectRotated(GameObject& obj, float opacity = 1.0f);
	//! @brief Draws debug info for all of the GameObjects that exist.
	//! @snippet PlayExamples.cpp DrawGameObjectsDebug
	//! @ingroup gameobject
	//! @details This includes the object's ID, sprite name and current animation frame. It will also draw the sprite's boundaries and the collision radius of the object.
	void DrawGameObjectsDebug();
}
#endif
#endif // PLAY_PLAYOBJECT_H