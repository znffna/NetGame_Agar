#include "GameManager.h"


GameManager::GameManager() : main_scene { }, game_scene { }, current_scene { &main_scene }, mouse_position { 0, 0 } {

}

GameManager::~GameManager() {

}


void GameManager::show(const HDC& hdc) {
	current_scene->show(hdc);
}

void GameManager::syncSize(const HWND& hWnd) {
	current_scene->syncSize(hWnd);
	syncMousePosition(hWnd);
}


void GameManager::keyboardInput(const HWND& hWnd, int keycode) {
	switch(current_scene->getID()) {
	case Main:
		if(!main_scene.keyboardInput(keycode)) {
			if(keycode == VK_ESCAPE) {
				quit(hWnd);
			}
		}
		break;
	case Game:
		switch(keycode) {
		case VK_ESCAPE:
			game_scene.togglePauseState();
			releaseCursor();
			break;
		case L'1':
			game_scene.setCameraMode(GameScene::CameraMode::Dynamic);
			break;
		case L'2':
			game_scene.setCameraMode(GameScene::CameraMode::Fixed);
			break;
		case L'p': case L'P':
			game_scene.show_score = !game_scene.show_score;
			break;
		}
		break;
	}
}


void GameManager::mouseMove(const HWND& hWnd, const POINT& point) {
	current_scene->mouseMove(point);
}

void GameManager::clickScene(const HWND& hWnd, const POINT& point, const Direction& dir) {
	switch(dir) {
	case Left:
		switch(buttonClicked(point)) {
		case StartButton:
			if(main_scene.isValid()) {
				gameStart(hWnd);
			}
			break;
		case ResumeGame:
			game_scene.resume();
			lockCursor(hWnd);
			break;
		case RestartGame:
			gameReStart(hWnd);
			break;
		case QuitGame:
			quit(hWnd);
			break;
		}
		break;
	case Right:
		switch(current_scene->getID()) {
        case Main:
            main_scene.clickR(point);
            break;
		case Game:
			game_scene.clickR(point);
			break;
		}
		break;
	}
}

void GameManager::update(const HWND& hWnd) {
	switch(current_scene->getID()) {
	case Game:
		game_scene.update(mouse_position);
		break;
	case Main:

		break;
	}
}

void GameManager::quit(const HWND& hWnd) {
	switch(current_scene->getID()) {
	case Game:
		current_scene = &main_scene;
		game_scene.disconnect();
		releaseCursor();
		break;
	case Main:
		PostQuitMessage(0);
		break;
	}
}


void GameManager::setTimers(const HWND& hWnd) {
	SetTimer(hWnd, Display, fps(60), NULL);
	SetTimer(hWnd, UpdateGame, fps(60), NULL);
}

void GameManager::timer(const HWND& hWnd, int id) {
	switch(id) {
	case Display:
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case UpdateGame:
		update(hWnd);
		break;
	}
}

void GameManager::interrupt() {
	releaseCursor();
	switch(current_scene->getID()) {
	case Game:
		game_scene.pause();
		break;
	case Main:

		break;
	}
}


void GameManager::gameStart(const HWND& hWnd) {
	fixCursor(hWnd);

	game_scene.setUp();

    game_scene.nickname = main_scene.getName();
	game_scene.connect(main_scene.getAddress());
    
	current_scene = &game_scene;
}

void GameManager::gameReStart(const HWND& hWnd) {
	fixCursor(hWnd);

	game_scene.restart();
}

void GameManager::lockCursor(const HWND& hWnd) {
	RECT rect;
	GetClientRect(hWnd, &rect);
	POINT lt = { rect.left, rect.top };
	ClientToScreen(hWnd, &lt);
	rect.left += lt.x;
	rect.top += lt.y;
	rect.right += lt.x;
	rect.bottom += lt.y;
	ClipCursor(&rect);
}

void GameManager::fixCursor(const HWND& hWnd) {
	RECT rect;
	GetClientRect(hWnd, &rect);
	POINT lt = { rect.left, rect.top };
	ClientToScreen(hWnd, &lt);
	rect.left += lt.x;
	rect.top += lt.y;
	rect.right += lt.x;
	rect.bottom += lt.y;
	ClipCursor(&rect);
	SetCursorPos((rect.left + rect.right)/2, (rect.top + rect.bottom)/2);
}

void GameManager::releaseCursor() {
	ClipCursor(NULL);
}

ButtonID GameManager::buttonClicked(const POINT& point) {
	switch(current_scene->getID()) {
	case SceneID::Main:
		return main_scene.clickL(point);
	case SceneID::Game:
		return game_scene.clickL(point);
	default:
		return None;
	}
}

void GameManager::syncMousePosition(const HWND& hWnd) {
	GetCursorPos(&mouse_position);
	ScreenToClient(hWnd, &mouse_position);
}