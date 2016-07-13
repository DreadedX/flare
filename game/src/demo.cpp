#include "demo.h"

glm::vec3 cubePositions[] = {
	glm::vec3( 0.0f,  0.0f,  0.0f), 
	glm::vec3( 2.0f,  5.0f, -15.0f), 
	glm::vec3(-1.5f, -2.2f, -2.5f),  
	glm::vec3(-3.8f, -2.0f, -12.3f),  
	glm::vec3( 2.4f, -0.4f, -3.5f),  
	glm::vec3(-1.7f,  3.0f, -7.5f),  
	glm::vec3( 1.3f, -2.0f, -2.5f),  
	glm::vec3( 1.5f,  2.0f, -2.5f), 
	glm::vec3( 1.5f,  0.2f, -1.5f), 
	glm::vec3(-1.3f,  1.0f, -1.5f)  
};

struct JoyDebugControl : fuse::Component {

	JoyDebugControl(fuse::Entity *parent) {
		super(parent);
	}

	void _update() override {

		int countButtons;
		const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_2, &countButtons);

		static bool reloaded = false;

		if (buttons[3] == 1 && !reloaded) {

			print::d("Reloading assets");
			flare::asset::reload();
		}

		if (buttons[3] == 0 && reloaded) {

			reloaded = false;
		}
	}
};

// TODO: Controllers cause some issues on wine, they are detected, but are null
int main() {

	flare::init();

	print::m("Welcome to Flare Engine");

	// NOTE: This is code to test joystick input
	int present = glfwJoystickPresent(GLFW_JOYSTICK_2);
	print::d("Present: %i", present);

	if (present == 1) {
		const char *name = glfwGetJoystickName(GLFW_JOYSTICK_2);

		print::d("Controller name: %s", name);

	}
	// END

	fuse::Entity *player = flare::getState()->mainState->manager->createEntity("Player");
	player->addComponent<Camera>();
	// Make sure that the controller is plugged in before adding the control
	if (present == 1) {

		player->addComponent<JoyCamera>();
		player->addComponent<JoyDebugControl>();
	}

#if 0
	for (int i = 0; i < 10; ++i) {

		fuse::Entity *cube = fuse::createEntity("Container");
		cube->addComponent<flare::component::Position>(cubePositions[i]);
		cube->addComponent<flare::component::Rotation>(glm::radians(20.0f * i), glm::vec3(0.5f, 1.0f, 0.0f));
		cube->addComponent<flare::component::Object>("demo/cube");
		cube->addComponent<Spin>();
	}
#elif 1
	fuse::Entity *house = flare::getState()->mainState->manager->createEntity("Sponza");
	house->addComponent<flare::component::Position>(glm::vec3(0.0f, 0.0f, 0.0f));
	house->addComponent<flare::component::Scale>(glm::vec3(0.1f, 0.1f, 0.1f));
	house->addComponent<flare::component::Object>("demo/sponza");
#elif 0
	fuse::Entity *rungholt = fuse::createEntity("Rungholt");
	rungholt->addComponent<flare::component::Position>(glm::vec3(0.0f, 0.0f, 0.0f));
	rungholt->addComponent<flare::component::Object>("demo/rungholt");

	flare::State::Render::Camera *camera = &flare::getState()->render.camera;
	camera->position = glm::vec3(377, 115, -246);

	camera->rotation.y = -16.0f;
	camera->rotation.x = 157.0f;
#endif

	// Example of post proccessing
	// PostFX *scanline = new PostFX("demo/scanline");
	// scanline->init();
	// flare::getState()->render.renderPasses.add(scanline);

	flare::PauseState *pauseState = new flare::PauseState;
	pauseState->previousState = flare::getState()->mainState;
	pauseState->manager = new fuse::Manager;

	// Two pass gausian blurrender
	// PostFX *gausian = new PostFX("demo/gausian", flare::getState()->mainState);
	// gausian->init();
	// flare::getState()->mainState->renderPasses.add(gausian);
	// PostFX *gausian2 = new PostFX("demo/gausian2", flare::getState()->mainState);
	// gausian2->init();
	// flare::getState()->mainState->renderPasses.add(gausian2);
	
	Gausian *gausian = new Gausian;
	gausian->init();
	pauseState->renderPasses.add(gausian);

	while (flare::isRunning()) {

		// Shadow test
		// flare::getState()->render.light.direction.x = 100 * cos(glfwGetTime());
		// flare::getState()->render.light.direction.z = 100 * sin(glfwGetTime());
		//////

		flare::update();

			flare::Timer timer = flare::Timer("demo code", glfwGetTime());
		static bool blur = false;
		static bool paused = false;
		if (flare::input::keyCheck(GLFW_KEY_P) && player != nullptr) {

			if (!paused) {

				flare::getState()->mainState = pauseState;
				blur = true;
			} else {

				blur = false;
			}

			print::d("Toggling pause");
			paused = !paused;
			flare::input::keySet(GLFW_KEY_P, false);
		}

		if (flare::input::keyCheck(GLFW_KEY_X) && player != nullptr) {

			print::d("Toggling pause effect");
			blur = !blur;
			flare::input::keySet(GLFW_KEY_X, false);
		}

		if (blur) {

			gausian->test = fmin(gausian->test + 7*flare::render::getDeltaTime(), 1.0f);
		}
		if (!blur) {

			gausian->test = fmax(gausian->test - 7*flare::render::getDeltaTime(), 0.0f);
			// This is really not efficient at all
			if (gausian->test == 0.0f) {

				flare::getState()->mainState = pauseState->previousState;
			}
		}
			timer.time = glfwGetTime() - timer.time;
			flare::getState()->timers.push_back(timer);
	}

	flare::terminate();
}
