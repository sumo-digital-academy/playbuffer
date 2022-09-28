#include "Play.h"

#include "MainGame.h"
#include "GameObject.h"

std::vector< GameObject* > GameObject::s_vUpdateList;
std::vector< GameObject* > GameObject::s_vDrawList;

GameObject::GameObject( Point2f pos )
{
	m_worldMat = MatrixIdentity();
	m_worldMat.row[2] = { pos.x, pos.y, 1.0f }; // The third [2] row of the matrix holds the position

	s_vUpdateList.push_back( this );
	s_vDrawList.push_back( this );
}

GameObject::GameObject( Matrix2D& mat )
{
	m_worldMat = mat;

	s_vUpdateList.push_back( this );
	s_vDrawList.push_back( this );
}

GameObject::~GameObject()
{
	s_vDrawList.erase( std::find( s_vDrawList.begin(), s_vDrawList.end(), this ) );
	s_vUpdateList.erase( std::find( s_vUpdateList.begin(), s_vUpdateList.end(), this ) );
}

void GameObject::UpdateAll( GameState& state )
{
	std::sort( s_vUpdateList.begin(), s_vUpdateList.end(), GameObject::UpdateOrder );

	for( int n = 0; n < s_vUpdateList.size(); n++ )
	{
		s_vUpdateList[n]->Update( state );
		   
		if( !s_vUpdateList[n]->m_active )
    		delete s_vUpdateList[n--];
	}
}

void GameObject::DrawAll( GameState& state )
{
	std::sort( s_vDrawList.begin(), s_vDrawList.end(), GameObject::DrawOrder );

	for( int n = 0; n < s_vDrawList.size(); n++ )
		s_vDrawList[n]->Draw( state );
}

void GameObject::DestroyAll()
{
	for( int n = 0; n < s_vDrawList.size(); n++ )
		delete s_vUpdateList[n--];
}

int GameObject::GetObjectCount( GameObject::Type type )
{
	int count = 0;

	for( GameObject* p : s_vUpdateList )
	{
		if( p->m_type == type || type == OBJ_ALL )
			count++;
	}

	return count;
}

int GameObject::GetObjectList( GameObject::Type type, std::vector< GameObject* >& vList )
{
	vList.clear();

	for( GameObject* p : s_vUpdateList )
	{
		if( p->m_type == type || type == OBJ_ALL )
			vList.push_back( p );
	}

	return vList.size();
}


