#ifndef EVENTHANDLER
#define EVENTHANDLER
#include <map>
#include <functional>
using std::map;
class KeyEventHandler {
private:
	typedef std::function<void()> func;
	map<int, func> function;
public:
	void registerKeyboardFunction(int key, func f) {
		function[key]=f;
	}
	void operator ()(int key) {
		if(function[key]!=NULL) function[key]();
	}
};
class MouseEventHandler {
private:
	typedef std::function<void()> func;
	typedef std::function<void(double,double,double,double)> cursorFunc;
	func onClick, onRelease;
	cursorFunc onDrag;
	double originalX,originalY;
	double X,Y;
	bool click,isOriginExist;
public:
	MouseEventHandler() {
		click = false;
		isOriginExist = false;
	}
	void registerClick(func f) {
		onClick=f;
	}
	void registerDrag(cursorFunc f) {
		onDrag=f;
	}
	void registerRelease(func f) {
		onRelease=f;
	}
	bool isClicked() { return click; }
	void operator ()(int status) {
		if(status==GLFW_PRESS) {
			if(!click) {
				printf("GLFW_PRESS\n");
				onClick();
				click=true;
			}
		} else {
			printf("GLFW_RELEASE\n");
			click = false;
			isOriginExist = false;
			onRelease();
		}
	}
	void operator ()(double x,double y) {
		if(!isOriginExist) {
			originalX = x;
			originalY = y;
			isOriginExist=true;
		}
		X=x;
		Y=y;
		onDrag(originalX,originalY,X,Y);
	}
};
#endif