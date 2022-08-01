#include <cstdlib>
#include <memory>

extern "C" {
#include <X11/Xlib.h>
}

/* Mod1Mask -> Alt
 * Mod4Mask -> Super/Windows */
#define MODKEY Mod4Mask

#define GRAB_KEY(xdisplay, grab_window, key_cstring, keymask) \
	XGrabKey(xdisplay, XKeysymToKeycode(xdisplay, \
		XStringToKeysym(key_cstring)), keymask, grab_window, \
		True, GrabModeAsync, GrabModeAsync)

#define GRAB_BUTTON(xdisplay, grab_window, button_number, keymask, \
		eventmask) \
	XGrabButton(xdisplay, button_number, keymask, grab_window, \
		True, eventmask, GrabModeAsync, GrabModeAsync, None, \
		None);

#define MAX(a, b) \
	((a) > (b) ? (a) : (b))

int main() {
	std::unique_ptr<Display, void (*)(Display*)> display =
			std::unique_ptr<Display, void (*)(Display*)>(
		XOpenDisplay(None),
		[](Display* display) {
			if (display == None) {
				return;
			}

			XCloseDisplay(display);
		}
	);

	try {
		if (display == None) {
			throw;
		}
	}
	catch (...) {
		std::exit(EXIT_FAILURE);
	}

	const Window root_window = DefaultRootWindow(display.get());

	GRAB_KEY(display.get(), root_window, "X", MODKEY);
	GRAB_KEY(display.get(), root_window, "X", MODKEY | ShiftMask);

	GRAB_BUTTON(display.get(), root_window, 1, MODKEY,
		ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
	GRAB_BUTTON(display.get(), root_window, 3, MODKEY,
		ButtonPressMask | ButtonReleaseMask | PointerMotionMask);

	XEvent event;

	XButtonEvent edit_window_start;
	XWindowAttributes window_attributes;

	button_event_start.subwindow = None;

	for(;;) {
		XNextEvent(display.get(), &event);

		switch (event.type) {
		case KeyPress:
			if (event.xkey.subwindow != None && event.xkey.state & ~ShiftMask) {
				XDestroyWindow(display.get(), event.xkey.subwindow);
			}

			if (event.xkey.state & ShiftMask) {
				return EXIT_SUCCESS;
			}

			break;

		case ButtonPress:
			if (event.xbutton.subwindow != None) {
				XGetWindowAttributes(display.get(),
					event.xbutton.subwindow,
					&window_attributes);
				edit_window_start = event.xbutton;

				XRaiseWindow(display.get(),
					event.xbutton.subwindow);
				XSetInputFocus(display.get(),
					event.xbutton.subwindow,
					RevertToPointerRoot, CurrentTime);
			}

			break;

		case MotionNotify:
			if (edit_window_start.subwindow != None) {
				int x_offset = event.xbutton.x_root - edit_window_start.x_root;
				int y_offset = event.xbutton.y_root - edit_window_start.y_root;

				XMoveResizeWindow(
					display.get(),
					edit_window_start.subwindow,
					window_attributes.x + (edit_window_start.button == 1 ? x_offset : 0),
					window_attributes.y + (edit_window_start.button == 1 ? y_offset : 0),
					MAX(1, window_attributes.width + (edit_window_start.button == 3 ? x_offset : 0)),
					MAX(1, window_attributes.height + (edit_window_start.button == 3 ? y_offset : 0))
				);
			}

			break;

		case ButtonRelease:
			edit_window_start.subwindow = None;
			break;
		}

		XSync(display.get(), False);
	}

	return EXIT_SUCCESS;
}
