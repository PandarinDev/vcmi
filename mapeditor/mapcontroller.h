#ifndef MAPCONTROLLER_H
#define MAPCONTROLLER_H

#include "maphandler.h"
#include "mapview.h"
#include "../lib/mapping/CMap.h"
#include "../lib/Terrain.h"

class MainWindow;
class MapController
{
public:
	MapController(MainWindow *);
	MapController(const MapController &) = delete;
	MapController(const MapController &&) = delete;
	~MapController();
	
	void setMap(std::unique_ptr<CMap>);
	
	CMap * map();
	MapHandler * mapHandler();
	MapScene * scene(int level);
	
	void resetMapHandler();
	
	void sceneForceUpdate();
	void sceneForceUpdate(int level);
	
	void commitTerrainChange(int level, const Terrain & terrain);
	void commitObjectErase(const CGObjectInstance* obj);
	void commitObjectErase(int level);
	void commitObstacleFill(int level);
	void commitChangeWithoutRedraw();
	void commitObjectShiftOrCreate(int level);
	void commitObjectCreate(int level);
	void commitObjectChange(int level);
	
	bool discardObject(int level) const;
	void createObject(int level, CGObjectInstance * obj) const;

	void undo();
	void redo();
	
private:
	std::unique_ptr<CMap> _map;
	std::unique_ptr<MapHandler> _mapHandler;
	MainWindow * main;
	mutable std::array<std::unique_ptr<MapScene>, 2> _scenes;
};

#endif // MAPCONTROLLER_H
