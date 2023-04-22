#pragma once

struct CustomPath {
	std::uint32_t raceId;
	std::string racePath;

	std::uint32_t actorId;
	std::string actorPath;
};

class ThreadPathMap {
public:
	ThreadPathMap() {}
	void Add(std::thread::id key, CustomPath value) {
		std::lock_guard<std::mutex> guard(_mutex);
		_map[key] = value;
	}
	const CustomPath* Get(std::thread::id key) {
		std::lock_guard<std::mutex> guard(_mutex);
		auto it = _map.find(key);
		if (it == _map.end())
			return nullptr;
		return &it->second;
	}
	void Delete(std::thread::id key) {
		std::lock_guard<std::mutex> guard(_mutex);
		_map.erase(key);
	}
	void Clear() {
		std::lock_guard<std::mutex> guard(_mutex);
		_map.clear();
	}

private:
	std::unordered_map<std::thread::id, CustomPath> _map;
	std::mutex _mutex;
};

class FullPathMap {
public:
	FullPathMap() {}
	void Add(std::string key, CustomPath value) {
		std::lock_guard<std::mutex> guard(_mutex);
		_map[key] = value;
	}
	bool Get(std::string key, CustomPath& o_path) {
		std::lock_guard<std::mutex> guard(_mutex);
		auto it = _map.find(key);
		if (it == _map.end())
			return false;
		o_path = it->second;
		return true;
	}
	void Delete(std::string key) {
		std::lock_guard<std::mutex> guard(_mutex);
		_map.erase(key);
	}
	void Clear() {
		std::lock_guard<std::mutex> guard(_mutex);
		_map.clear();
	}

private:
	CaseInsensitiveMap<CustomPath> _map;
	std::mutex _mutex;
};

class CustomModelProcessor : public BSModelDB::BSModelProcessor {
public:
	CustomModelProcessor(BSModelDB::BSModelProcessor* oldProcessor) : m_oldProcessor(oldProcessor) { }
	virtual void Process(BSModelDB::ModelData* modelData, const char* modelName, NiAVObject** root, UInt32* typeOut);
	DEFINE_STATIC_HEAP(Heap_Allocate, Heap_Free)

protected:
	BSModelDB::BSModelProcessor* m_oldProcessor;
};

void SetModelProcessor();
void Hooks_ReplaceRefModel();
void Hooks_PrepareName();

void ClearPathMap();