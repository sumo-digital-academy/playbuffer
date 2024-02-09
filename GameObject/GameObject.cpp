#include "Play.h"
#include "GameObject.h"

GameObject::GameObject( GameObjectType objType, Play::Point2f position, Play::Vector2f velocity, std::string spriteName )
{
	m_type = objType;

	m_spriteID = Play::GetSpriteId( spriteName.c_str() );
	m_frame = 1;
	m_pos = position;
	m_velocity = velocity;

	m_halfWidth = Play::GetSpriteWidth( m_spriteID ) / 2;
	m_halfHeight = Play::GetSpriteHeight( m_spriteID ) / 2;
	m_radius = (float)m_halfWidth;
}

void GameObject::SetSprite( std::string spriteName, float animationSpeed )
{
	m_spriteID = Play::GetSpriteId( spriteName.c_str() );
	m_animSpeed = animationSpeed;
};

// Code adapted from Play::IsColliding
bool GameObject::IsColliding( GameObject* obj )
{
	//Don't collide with NULL objects
	if( m_type == -1 || obj->m_type == -1 )
		return false;

	int xDiff = (int)m_pos.x- (int)obj->m_pos.x;
	int yDiff = (int)m_pos.y - (int)obj->m_pos.y;
	int radii = (int)m_radius + (int)obj->m_radius;

	// Game programmers don't do square root!
	return((xDiff * xDiff) + (yDiff * yDiff) < radii * radii);
}

//  Code adapted from Play::IsLeavingDisplayArea
bool GameObject::IsLeavingDisplay( bool vertical, bool horizontal )
{
	if( m_type == -1 ) return false; 

	Play::Vector2f spriteSize = Play::Graphics::GetSpriteSize( m_spriteID );
	Play::Vector2f spriteOrigin = Play::Graphics::GetSpriteOrigin( m_spriteID );

	if( horizontal )
	{
		if( m_pos.x - spriteOrigin.x < 0 && m_velocity.x < 0 )
			return true;

		if( m_pos.x + spriteSize.width - spriteOrigin.x > Play::Window::GetWidth() && m_velocity.x > 0 )
			return true;
	}

	if( vertical )
	{
		if( m_pos.y - spriteOrigin.y < 0 && m_velocity.y < 0 )
			return true;

		if( m_pos.y + spriteSize.height - spriteOrigin.y > Play::Window::GetHeight() && m_velocity.y > 0 )
			return true;
	}

	return false;
}

//  Code adapted from Play::IsVisible
bool GameObject::IsOutsideDisplay()
{
	if( m_type == -1 ) return true;

	Play::Vector2f spriteSize = Play::Graphics::GetSpriteSize( m_spriteID );
	Play::Vector2f spriteOrigin = Play::Graphics::GetSpriteOrigin( m_spriteID );

	return !(m_pos.x + spriteSize.width - spriteOrigin.x > 0 && m_pos.x - spriteOrigin.x < Play::Window::GetWidth() &&
		m_pos.y + spriteSize.height - spriteOrigin.y > 0 && m_pos.y - spriteOrigin.y < Play::Window::GetHeight());
}

void GameObject::StandardMovementUpdate()
{
	m_rotation += m_rotSpeed;
	m_oldPos = m_pos;
	m_velocity += m_acceleration;
	m_pos += m_velocity;
}

void GameObject::Destroy( bool inOne )
{
	// The first time we destroy something we want it to flash
	// The second time we want rid of it
	if( m_type != TYPE_DESTROYED && inOne == false )
	{
		m_frame = 0;
		m_frameTimer = 0;
		m_type = TYPE_DESTROYED;
	}
	else
	{
		m_destroy = true;
	}
}

void GameObject::UpdateAnimation()
{
	m_frameTimer += m_animSpeed;

	if( m_frameTimer >= 1 )
	{
		m_frame++;
		m_frameTimer = 0;
	}
}

void GameObject::UpdateDestroyed()
{
	m_animSpeed = 0.2f;
	StandardMovementUpdate();
	UpdateAnimation();

	if( (int)m_frame % 2 )
		m_hidden = true;
	else
		m_hidden = false;

	if( IsOutsideDisplay() || m_frame >= 10 )
		Destroy();
}