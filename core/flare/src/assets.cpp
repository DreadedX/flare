#include "extra/array.h"
#include "flux/read.h"
#include "flare/assets.h"

Array<flare::asset::Asset*> flare::asset::assetList;
std::unordered_map<std::string, flare::asset::Asset*> flare::asset::assetMap;
Allocator *flare::asset::asset_allocator;

void flare::asset::init(Allocator *_allocator) {

	asset_allocator = _allocator;
}

void flare::asset::reload() {

	flux::read::reload();

	for (Asset *asset : assetList) {

		// For now only reload core assets for testing
		if (asset->name.substr(0, 4).compare("core") == 0) {

			print_d("Reloading asset: %s", asset->name);
			asset->_load();
		}
	}
}

void flare::asset::close() {

	for (Asset *asset : assetList) {

		// delete asset;
		allocator::make_delete<Asset>(*asset_allocator, *asset);
		asset = nullptr;
	}
}
