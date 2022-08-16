#pragma once

struct WeaponPosition {
	const char* name;
	double eulerHeading;
	double eulerAttitude;
	double eulerBank;
	double posX;
	double posY;
	double posZ;
	std::string potentialParentNode;
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