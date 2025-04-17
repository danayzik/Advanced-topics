BUILD_DIR=build


CMAKE=cmake
CMAKE_BUILD=cmake --build $(BUILD_DIR)


REMOVE_BUILD_DIR=$(RM) -rf $(BUILD_DIR)

.PHONY: headless visuals

headless:
	@echo "Building without SFML (Headless mode)"
	$(REMOVE_BUILD_DIR)
	$(CMAKE) -S . -B $(BUILD_DIR) -DUSE_SFML=OFF
	$(CMAKE_BUILD)

visuals:
	@echo "Building with SFML (Visual mode)"
	$(REMOVE_BUILD_DIR)
	$(CMAKE) -S . -B $(BUILD_DIR) -DUSE_SFML=ON
	$(CMAKE_BUILD)

