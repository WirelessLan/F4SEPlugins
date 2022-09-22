#pragma once

struct CustomPath {
	std::string racePath;
	std::string actorpath;
};

class ThreadPathMap {
public:
	ThreadPathMap() {}
	void Add(std::thread::id key, CustomPath value);
	const CustomPath* Get(std::thread::id key);
	void Delete(std::thread::id key);

private:
	std::unordered_map<std::thread::id, CustomPath> _map;
	std::mutex _mutex;
};

void Hooks_ActorChangeMeshes();
void Hooks_SetModelPath();
void Hooks_GetNiObject();