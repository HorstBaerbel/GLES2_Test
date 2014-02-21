#include "main_common.cpp"


int main(int argc, char **argv)
{
	#ifdef BCMHOST
		bcm_host_init();
	#endif

	Timing::getInstance();

	//set stuff up
	if (!setup()) {
		std::cout << "Setup failed!" << std::endl;
		return -1;
	}

	//wait for events
	bool done = false;
	while (!done) {
		//render scene
		interactive();
#ifdef USE_OPENGL_GLX
		//retrieve display hande from window
		DisplayHandle display = window->getDisplayHandle();
		//handle the events in the queue
		while (XPending(display) > 0) {
			XEvent event;
			XNextEvent(display, &event);
			switch (event.type) {
				case Expose:
					if (event.xexpose.count != 0) {
						break;
					}
					break;
				case ConfigureNotify:
					//call resizeGL only if our window-size changed
					/*if ((event.xconfigure.width != GLWin.width) || (event.xconfigure.height != GLWin.height)) {
						width = event.xconfigure.width;
						height = event.xconfigure.height;
						resizeGL(width, height);
					}*/
					break;
					//exit in case of a mouse button press
				case ButtonPress:
					done = true;
					break;
				case KeyPress:
					if (XLookupKeysym(&event.xkey, 0) == XK_Escape) {
						done = true;
					}
					/*if (XLookupKeysym(&event.xkey,0) == XK_F11) {
						destroyWindow();
						fullscreen = !fullscreen;
						createWindow();
					}*/
					break;
				case ClientMessage:
					if (strcmp(XGetAtomName(display, event.xclient.message_type), "WM_PROTOCOLS") == 0) {
						done = true;
					}
					break;
				default:
					break;
			}
		}
#endif
		//usleep(1000);
	}

	return 0;
}
