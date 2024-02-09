#pragma once

class GameObject;

class GameObjectManager
{
public:
	GameObjectManager() = default;
	~GameObjectManager();

	GameObject* CreateObject( GameObjectType objType, Play::Point2f pos, Play::Vector2f vel );
	void RegisterGameObject( GameObject* obj ) { m_pGameObjectList.push_back( obj ); };

	void UpdateAll();
	void DrawAll();
	void CollideAll();
	void CleanUpAll(); 

	GameObject* GetPlayer() { return m_pPlayer; }
	void SetPlayer( GameObject* pPlayer ) { m_pPlayer = pPlayer; }
	int GetAllObjectsOfType( GameObjectType objType, std::vector<GameObject*>& objList, bool clearList = true );
	void DeleteGameObjectsByType( GameObjectType type );

private:
	std::vector<GameObject*> m_pGameObjectList;
	GameObject* m_pPlayer{ nullptr };
};

GameObjectManager* GetObjectManager();
void DestroyObjectManager();
