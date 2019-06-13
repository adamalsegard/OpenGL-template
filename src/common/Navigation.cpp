#include <common/Navigation.hpp>

void KeyTranslator::init(GLFWwindow *window) {
     horizontal = 0.0;
     zoom = -5.0;
     lastTime = glfwGetTime();
};

void KeyTranslator::poll(GLFWwindow *window) {

	double currentTime, elapsedTime;

	currentTime = glfwGetTime();
	elapsedTime = currentTime - lastTime;
	lastTime = currentTime;

	if(glfwGetKey(window, GLFW_KEY_RIGHT)) {
		horizontal += elapsedTime * 2.5f; //Move right
	}

	if(glfwGetKey(window, GLFW_KEY_LEFT)) {
		horizontal -= elapsedTime * 2.5f; //Move left

	}

	if(glfwGetKey(window, GLFW_KEY_UP)) {
		zoom += elapsedTime * 2.5f; // Zoom in
	}

	if(glfwGetKey(window, GLFW_KEY_DOWN)) {
		zoom -= elapsedTime * 2.5f; // Zoom out
	}
}


void MouseRotator::init(GLFWwindow *window) {
    yaw = -90.0f;
    pitch = 0.0f;
    glfwGetCursorPos(window, &lastX, &lastY);
	lastLeft = GL_FALSE;
	lastRight = GL_FALSE;
}

void MouseRotator::poll(GLFWwindow *window) {

  double currentX;
  double currentY;
  int currentLeft;
  int currentRight;
  int windowWidth;
  int windowHeight;

  // Find out where the mouse pointer is, and which buttons are pressed
  glfwGetCursorPos(window, &currentX, &currentY);
  currentLeft = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
  currentRight = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT); //TODO: Not used yet
  glfwGetWindowSize( window, &windowWidth, &windowHeight );

  if(currentLeft && lastLeft) { // If a left button drag is in progress
    double moveX = currentX - lastX;
    double moveY = lastY - currentY;
    
  	yaw += moveX * SENSITIVITY;

  	pitch += moveY * SENSITIVITY;
	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;
  }
  
  lastLeft = currentLeft;
  lastRight = currentRight;
  lastX = currentX;
  lastY = currentY;
}
