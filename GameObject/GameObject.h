#pragma once

// The GameObject type is the only representation of type which is visible to code externally
enum GameObjectType
{
	TYPE_NULL = -1,
	TYPE_AGENT8,
	TYPE_FAN,
	TYPE_TOOL,
	TYPE_COIN,
	TYPE_STAR,
	TYPE_LASER,
	TYPE_DESTROYED,
};

class GameObject
{
public:
	// Constructors and (virtual) destructor
	GameObject() = delete; // Delete the default constructor as we don't want unmanaged GameObjects
	GameObject( GameObjectType objType, Play::Point2f position, Play::Vector2f velocity, std::string spriteName );
	virtual ~GameObject() {}; 
	
	// Providing no implementation of the virtual Update function makes this an abstract base class
	virtual void Update() = 0;
	// These virtuals have implementations and so are optional overrides
	virtual void OnCollision( GameObject* ) {};
	virtual void UpdateDestroyed();

	// Various tests on game objects
	bool IsColliding( GameObject* obj );
	bool IsDestroyed() { return m_destroy; }
	bool IsLeavingDisplay( bool vertical = true, bool horizontal = true );
	bool IsOutsideDisplay();
	bool IsHidden() { return m_hidden; }

	// Standard updates and destruction flagging
	void StandardMovementUpdate();
	void UpdateAnimation();
	void Destroy( bool inOne = false );

	// Setters
	void SetVelocity( Play::Vector2f velocity ) { m_velocity = velocity; }
	void SetAcceleration( Play::Vector2f acceleration ) { m_acceleration = acceleration; }
	void SetPosition( Play::Point2f pos ) { m_pos = pos; }
	void SetSprite( std::string spriteName, float animationSpeed );
	void SetAnimationSpeed( float animationSpeed ) { m_animSpeed = animationSpeed; }
	void SetRotation( float rotation ) { m_rotation = rotation; }
	void SetRotationSpeed( float rotationSpeed ) { m_rotSpeed = rotationSpeed; }
	void SetFrame( float frame ) { m_frame = frame; }
	void SetVisible( bool render ) { m_hidden = render; }

	// Getters
	Play::Vector2f GetVelocity() { return m_velocity; }
	Play::Vector2f GetAcceleration() { return m_acceleration; }
	Play::Vector2f GetPosition() { return m_pos; }
	float GetRotation() { return m_rotation; }
	float GetFrame() { return m_frame; };
	int GetSpriteID() { return m_spriteID; }
	GameObjectType GetObjectType() { return m_type; }

protected:

	// Mostly just adapted from Play::GameObject
	GameObjectType m_type{ GameObjectType::TYPE_NULL };

	Play::Point2f m_pos{ 0, 0 };
	Play::Point2f m_oldPos{ 0, 0 };
	Play::Vector2f m_velocity{ 0, 0 };
	Play::Vector2f m_acceleration{ 0, 0 };

	float m_frame{ -1 };
	float m_frameTimer{ 0 };
	float m_rotation{ 0 };
	float m_rotSpeed{ 0 };
	float m_animSpeed{ 1 }; 
	float m_radius{ -1 };

	int m_spriteID{ -1 };
	int m_halfWidth{ -1 };
	int m_halfHeight{ -1 };

	bool m_destroy{ false };
	bool m_hidden{ false };
};