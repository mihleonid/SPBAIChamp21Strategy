#include "../Core.h"

void Core::destroyLogic(int priority, GameWrapper& game_wrapper) {
	std::cout << "DESTROY!" << std::endl;
	for (int planet_id = 0; planet_id < game_wrapper.getPlanets().size(); planet_id++) {
		auto building = game_wrapper.getBuilding(planet_id);
		int player_id = game_wrapper.getMyPlayerIdBySpecialty(Specialty::PRODUCTION);
		if (building.has_value() && player_id != -1)
			addTask(new DestroyTask(planet_id, 10, Specialty::PRODUCTION), priority, game_wrapper);
	}
};
