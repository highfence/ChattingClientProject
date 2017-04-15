
# include <Siv3D.hpp>
# include <HamFramework.hpp>
# include "Main.h"
# include "LoginScene.h"


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

	while (System::Update())
	{
		if (!sceneManager.updateAndDraw())
		{
			break;
		}
	}
}
