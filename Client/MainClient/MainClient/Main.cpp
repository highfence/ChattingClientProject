# include "stdafx.h"
# include "Main.h"
# include "LoginScene.h"
# include "LobbyScene.h"
# include "RoomList.h"
# include "Room.h"


void Main()
{
	/* Set Siv3D window Title */
	Window::SetTitle(L"Chatting Client Program");

	/* Make Scene Manager */
	MyApp sceneManager(SceneManagerOption::ShowSceneName);

	/* Scene Manager Setting */
	sceneManager.setFadeColor(Palette::White);

	/* Add Scenes into Manager */
	sceneManager.add<Login>(L"Login");
	sceneManager.add<Lobby>(L"Lobby");
	sceneManager.add<RoomList>(L"RoomList");
	sceneManager.add<Room>(L"Room");

	// packet queue
	// net service
	// broadcaster

	while (System::Update())
	{
		if (!sceneManager.updateAndDraw())
		{
			break;
		}
	}
}
