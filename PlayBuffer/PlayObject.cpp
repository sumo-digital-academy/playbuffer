//********************************************************************************************************************************
// File:		PlayManager.cpp
// Description:	A manager for providing simplified access to the PlayBuffer framework
// Platform:	Independent
// Notes:		The GameObject management is "opt in" as many will want to create their own GameObject class
//********************************************************************************************************************************
#include "PlayBufferPCH.h"

// Define this to opt in to the PlayManager
#ifdef PLAY_USING_GAMEOBJECT_MANAGER
namespace Play
{
	// Constructor for the GameObject struct - kept as simple as possible
	GameObject::GameObject(int type, Point2f newPos, int collisionRadius, int spriteId = 0)
		: type(type), pos(newPos), radius(collisionRadius), spriteId(spriteId)
	{
		// Member variables are assigned default values in the class header
		static int uniqueId = 0;
		m_id = uniqueId++;
	}

	// A map is used internally to store all the GameObjects and their unique ids
	static std::map<int, GameObject&> objectMap;

	// Used instead of Null return values, PlayMangager operations performed on this GameObject should fail transparently
	static GameObject noObject{ -1,{ 0, 0 }, 0, -1 };


	//**************************************************************************************************
	// GameObject functions
	//**************************************************************************************************
	int CreateGameObject(int type, Point2f newPos, int collisionRadius, const char* spriteName)
	{
		int spriteId = Play::Graphics::GetSpriteId(spriteName);
		// Deletion is handled in DestroyGameObject()
		GameObject* pObj = new GameObject(type, newPos, collisionRadius, spriteId);
		int id = pObj->GetId();
		objectMap.insert(std::map<int, GameObject&>::value_type(id, *pObj));
		return id;
	}

	GameObject& GetGameObject(int ID)
	{
		std::map<int, GameObject&>::iterator i = objectMap.find(ID);

		if (i == objectMap.end())
			return noObject;

		return i->second;
	}

	GameObject& GetGameObjectByType(int type)
	{
		int count = 0;

		for (std::pair<const int, GameObject&>& i : objectMap)
			if (i.second.type == type) { count++; }

		PLAY_ASSERT_MSG(count <= 1, "Multiple objects of type found, use CollectGameObjectIDsByType instead");

		for (std::pair<const int, GameObject&>& i : objectMap)
		{
			if (i.second.type == type)
				return i.second;
		}

		return noObject;
	}

	std::vector<int> CollectGameObjectIDsByType(int type)
	{
		std::vector<int> vec;
		for (std::pair<const int, GameObject&>& i : objectMap)
		{
			if (i.second.type == type)
				vec.push_back(i.first);
		}
		return vec; // Returning a copy of the vector
	}

	std::vector<int> CollectAllGameObjectIDs()
	{
		std::vector<int> vec;

		for (std::pair<const int, GameObject&>& i : objectMap)
			vec.push_back(i.first);

		return vec; // Returning a copy of the vector
	}

	void UpdateGameObject(GameObject& obj, bool bWrap, int wrapBorderSize, bool allowMultipleUpdatesPerFrame)
	{
		if (obj.type == -1) return; // Don't update noObject

		// We allow multiple updates if the object type has changed
		PLAY_ASSERT_MSG(obj.lastFrameUpdated != Play::frameCount || obj.type != obj.oldType || allowMultipleUpdatesPerFrame, "Trying to update the same GameObject more than once in the same frame!");
		obj.lastFrameUpdated = Play::frameCount;

		// Save the current position in case we need to go back
		obj.oldPos = obj.pos;
		obj.oldRot = obj.rotation;

		// Move the object according to a very simple physical model
		obj.velocity += obj.acceleration;
		obj.pos += obj.velocity;
		obj.rotation += obj.rotSpeed;

		// Handle the animation frame update
		obj.framePos += obj.animSpeed;
		if (obj.framePos > 1.0f)
		{
			obj.frame++;
			obj.framePos -= 1.0f;
		}

		// Wrap objects around the screen
		if (bWrap)
		{
			int dWidth = Play::Window::GetWidth();
			int dHeight = Play::Window::GetHeight();
			Vector2f origin = Play::Graphics::GetSpriteOrigin(obj.spriteId);
			Vector2f spriteSize = Play::Graphics::GetSpriteSize(obj.spriteId);

			if (obj.pos.x - origin.x + spriteSize.x - wrapBorderSize > dWidth)
				obj.pos.x = 0.0f - wrapBorderSize + origin.x;
			else if (obj.pos.x - origin.x + wrapBorderSize < 0)
				obj.pos.x = dWidth + wrapBorderSize + origin.x - spriteSize.x;

			if (obj.pos.y - origin.y + spriteSize.y - wrapBorderSize > dHeight)
				obj.pos.y = 0.0f - wrapBorderSize + origin.y;
			else if (obj.pos.y - origin.y + wrapBorderSize < 0)
				obj.pos.y = dHeight + wrapBorderSize + origin.y - spriteSize.y;
		}

	}

	void DestroyGameObject(int ID)
	{
		if (objectMap.find(ID) == objectMap.end())
		{
			PLAY_ASSERT_MSG(false, "Unable to find object with given ID");
		}
		else
		{
			GameObject* go = &objectMap.find(ID)->second;
			delete go;
			objectMap.erase(ID);
		}
	}

	void DestroyAllGameObjects(void)
	{
		for (std::pair<const int, GameObject&>& p : objectMap)
			delete& p.second;
		objectMap.clear();
	}

	void DestroyGameObjectsByType(int objType)
	{
		std::vector<int> typeVec = CollectGameObjectIDsByType(objType);
		for (size_t i = 0; i < typeVec.size(); i++)
			DestroyGameObject(typeVec[i]);
	}

	bool IsColliding(GameObject& object1, GameObject& object2)
	{
		//Don't collide with noObject
		if (object1.type == -1 || object2.type == -1)
			return false;

		int xDiff = int(object1.pos.x) - int(object2.pos.x);
		int yDiff = int(object1.pos.y) - int(object2.pos.y);
		int radii = object2.radius + object1.radius;

		// Game progammers don't do square root!
		return((xDiff * xDiff) + (yDiff * yDiff) < radii * radii);
	}

	bool IsVisible(GameObject& obj)
	{
		if (obj.type == -1) return false; // Not for noObject

		int spriteID = obj.spriteId;
		Vector2f spriteSize = Play::Graphics::GetSpriteSize(obj.spriteId);
		Vector2f spriteOrigin = Play::Graphics::GetSpriteOrigin(spriteID);

		Point2f pos = TRANSFORM_SPACE( obj.pos );

		return(pos.x + spriteSize.width - spriteOrigin.x > 0 && pos.x - spriteOrigin.x < Window::GetWidth() &&
			pos.y + spriteSize.height - spriteOrigin.y > 0 && pos.y - spriteOrigin.y < Window::GetHeight());
	}

	bool IsLeavingDisplayArea(GameObject& obj, Direction dirn)
	{
		if (obj.type == -1) return false; // Not for noObject

		int spriteID = obj.spriteId;
		Vector2f spriteSize = Play::Graphics::GetSpriteSize(obj.spriteId);
		Vector2f spriteOrigin = Play::Graphics::GetSpriteOrigin(spriteID);

		Point2f pos = TRANSFORM_SPACE( obj.pos );

		if (dirn != Direction::VERTICAL)
		{
			if (pos.x - spriteOrigin.x < 0 && obj.velocity.x < 0)
				return true;

			if (pos.x + spriteSize.width - spriteOrigin.x > Window::GetWidth() && obj.velocity.x > 0)
				return true;
		}

		if (dirn != Direction::HORIZONTAL)
		{
			if (pos.y - spriteOrigin.y < 0 && obj.velocity.y < 0)
				return true;

			if (pos.y + spriteSize.height - spriteOrigin.y > Window::GetHeight() && obj.velocity.y > 0)
				return true;
		}

		return false;
	}

	bool IsAnimationComplete(GameObject& obj)
	{
		if (obj.type == -1) return false; // Not for noObject
		return obj.frame >= Play::Graphics::GetSpriteFrames(obj.spriteId) - 1;
	}

	void SetGameObjectDirection(GameObject& obj, int speed, float angle)
	{
		if (obj.type == -1) return; // Not for noObject

		obj.rotation = angle;

		obj.velocity.x = speed * cos(obj.rotation);
		obj.velocity.y = speed * sin(obj.rotation);
	}

	void PointGameObject(GameObject& obj, int targetX, int targetY)
	{
		if (obj.type == -1) return; // Not for noObject

		float xdiff = targetX - obj.pos.x;
		float ydiff = targetY - obj.pos.y;

		obj.rotation = atan2( ydiff, xdiff );
	}

	void SetSprite(GameObject& obj, const char* spriteName, float animSpeed)
	{
		int newSprite = Play::Graphics::GetSpriteId(spriteName);
		// Only reset the animation back to the start when it is new
		if (newSprite != obj.spriteId)
			obj.frame = 0;
		obj.spriteId = newSprite;
		obj.animSpeed = animSpeed;
	}

	void SetSprite( GameObject& obj, int newSprite, float animSpeed )
	{
		// Only reset the animation back to the start when it is new
		if( newSprite != obj.spriteId )
			obj.frame = 0;
		obj.spriteId = newSprite;
		obj.animSpeed = animSpeed;
	}


	void DrawObject(GameObject& obj)
	{
		if (obj.type == -1) return; // Don't draw noObject
		Play::Graphics::Draw(obj.spriteId, TRANSFORM_SPACE( obj.pos ), obj.frame);
	}

	void DrawObjectTransparent(GameObject& obj, float opacity)
	{
		if (obj.type == -1) return; // Don't draw noObject
		Play::Graphics::DrawTransparent(obj.spriteId, TRANSFORM_SPACE( obj.pos ), obj.frame, { opacity, 1.0f, 1.0f, 1.0f });
	}

	void DrawObjectRotated(GameObject& obj, float opacity)
	{
		if (obj.type == -1) return; // Don't draw noObject
		Play::Graphics::DrawRotated(obj.spriteId, TRANSFORM_SPACE( obj.pos ), obj.frame, obj.rotation, obj.scale, { opacity, 1.0f, 1.0f, 1.0f });
	}

	void DrawGameObjectsDebug()
	{
		for( std::pair<const int, GameObject&>& i : objectMap )
		{
			GameObject& obj = i.second;
			int id = obj.spriteId;
			Play::Vector2D size = Play::Graphics::GetSpriteSize( obj.spriteId );
			Play::Vector2D origin = Play::Graphics::GetSpriteOrigin( id );

			// Corners of sprite drawing area
			Play::Point2D p0 = obj.pos - origin;
			Play::Point2D p2 = { obj.pos.x + size.width - origin.x, obj.pos.y + size.height - origin.y };
			Play::Point2D p1 = { p2.x, p0.y };
			Play::Point2D p3 = { p0.x, p2.y };

			DrawLine( p0, p1, Play::cRed );
			DrawLine( p1, p2, Play::cRed );
			DrawLine( p2, p3, Play::cRed );
			DrawLine( p3, p0, Play::cRed );

			DrawCircle( obj.pos, obj.radius, Play::cBlue );

			Play::DrawLine( { obj.pos.x - 20,  obj.pos.y - 20 }, { obj.pos.x + 20, obj.pos.y + 20 }, Play::cWhite );
			Play::DrawLine( { obj.pos.x + 20, obj.pos.y - 20 }, { obj.pos.x - 20, obj.pos.y + 20 }, Play::cWhite );

			std::string s = Play::Graphics::GetSpriteName( obj.spriteId ) + " f[" + std::to_string( obj.frame % Play::Graphics::GetSpriteFrames( obj.spriteId ) ) + "]";
			Play::DrawDebugText( { (p0.x + p1.x) / 2.0f, p0.y - 20 }, s.c_str() );
		}
	}
}
#endif