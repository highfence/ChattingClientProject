#pragma once

/* 씬에서 사용할 공용 데이터 구조체. */
struct SceneData
{
	int32 sceneNumber;
};

using MyApp = SceneManager<String, SceneData>;


