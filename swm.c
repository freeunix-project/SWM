#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

/* --- Configuration Section (In-line for simplicity) --- */
#define MODKEY      Mod4Mask
#define BORDER_PX   2
#define GAPS        12
#define BAR_HT      26
#define MASTER_SIZE 0.55

#define COL_BG      "#2E3440"
#define COL_BORDER  "#4C566A"
#define COL_ACCENT  "#88C0D0"

/* --- Data Structures --- */
typedef struct Client Client;
struct Client {
    Window win;
    Client *next;
};

/* --- Global Variables --- */
static Display *dpy;
static Window root, barwin;
static int sw, sh, screen;
static Client *clients = NULL;
static Client *sel = NULL;
static float mfact = MASTER_SIZE;
static GC gc;

/* --- Core Functions --- */

unsigned long getcolor(const char *colstr) {
    Colormap cm = DefaultColormap(dpy, screen);
    XColor col;
    XAllocNamedColor(dpy, cm, colstr, &col, &col);
    return col.pixel;
}

void focus(Client *c) {
    if (!c) return;
    // 이전 선택창 테두리 초기화
    if (sel) XSetWindowBorder(dpy, sel->win, getcolor(COL_BORDER));
    
    sel = c;
    XSetInputFocus(dpy, c->win, RevertToPointerRoot, CurrentTime);
    XSetWindowBorder(dpy, c->win, getcolor(COL_ACCENT));
    XRaiseWindow(dpy, c->win);
}

void detach(Client *c) {
    Client **tc;
    for (tc = &clients; *tc && *tc != c; tc = &(*tc)->next);
    *tc = c->next;
}

void attach(Client *c) {
    c->next = clients;
    clients = c;
}

void arrange(void) {
    int n = 0, i = 0;
    Client *c;
    for (c = clients; c; c = c->next) n++;

    int th = sh - BAR_HT;
    int ty = BAR_HT;
    int mw = (n > 1) ? sw * mfact : sw;

    for (c = clients; c; c = c->next, i++) {
        int x, y, w, h;
        if (i == 0) { // Master
            x = GAPS;
            y = ty + GAPS;
            w = mw - (2 * GAPS) - (2 * BORDER_PX);
            h = th - (2 * GAPS) - (2 * BORDER_PX);
        } else { // Stack
            x = mw + GAPS;
            h = (th / (n - 1)) - (2 * GAPS) - (2 * BORDER_PX);
            w = (sw - mw) - (2 * GAPS) - (2 * BORDER_PX);
            y = ty + ((i - 1) * (th / (n - 1))) + GAPS;
        }
        XMoveResizeWindow(dpy, c->win, x, y, w, h);
    }
}

void manage(Window w) {
    Client *c = malloc(sizeof(Client));
    c->win = w;
    attach(c);
    
    XSelectInput(dpy, w, EnterWindowMask | FocusChangeMask | PropertyChangeMask);
    XSetWindowBorderWidth(dpy, w, BORDER_PX);
    XMapWindow(dpy, w);
    focus(c);
    arrange();
}

void unmanage(Window w) {
    Client *c, **tc;
    for (tc = &clients; *tc && (*tc)->win != w; tc = &(*tc)->next);
    if (!*tc) return;
    c = *tc;
    *tc = c->next;
    free(c);
    focus(clients);
    arrange();
}

void drawbar(void) {
    XSetForeground(dpy, gc, getcolor(COL_BG));
    XFillRectangle(dpy, barwin, gc, 0, 0, sw, BAR_HT);
    XSetForeground(dpy, gc, getcolor("#D8DEE9"));
    XDrawString(dpy, barwin, gc, 20, 18, "SIGMA WINDOW MANAGER", 20);
}

void setup(void) {
    screen = DefaultScreen(dpy);
    root = DefaultRootWindow(dpy);
    sw = DisplayWidth(dpy, screen);
    sh = DisplayHeight(dpy, screen);

    // 상단 바 생성
    XSetWindowAttributes wa = { .override_redirect = True, .background_pixel = getcolor(COL_BG) };
    barwin = XCreateWindow(dpy, root, 0, 0, sw, BAR_HT, 0, DefaultDepth(dpy, screen), 
                           CopyFromParent, DefaultVisual(dpy, screen), CWOverrideRedirect | CWBackPixel, &wa);
    gc = XCreateGC(dpy, barwin, 0, 0);
    XMapRaised(dpy, barwin);

    XSelectInput(dpy, root, SubstructureRedirectMask | SubstructureNotifyMask);
    
    // 키 바인딩 (Alt+Shift+Q: 종료, Alt+Shift+Return: 터미널)
    XGrabKey(dpy, XKeysymToKeycode(dpy, XK_q), MODKEY|ShiftMask, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(dpy, XKeysymToKeycode(dpy, XK_Return), MODKEY|ShiftMask, root, True, GrabModeAsync, GrabModeAsync);
}

void run(void) {
    XEvent ev;
    while (!XNextEvent(dpy, &ev)) {
        if (ev.type == MapRequest) manage(ev.xmaprequest.window);
        if (ev.type == UnmapNotify) unmanage(ev.xunmap.window);
        if (ev.type == KeyPress) {
            KeySym keysym = XLookupKeysym(&ev.xkey, 0);
            if (keysym == XK_q && (ev.xkey.state & (MODKEY|ShiftMask))) return; // Quit
            if (keysym == XK_Return && (ev.xkey.state & (MODKEY|ShiftMask))) {
                if (fork() == 0) {
                    execlp("st", "st", NULL); // st 터미널 실행
                    exit(0);
                }
            }
        }
        if (ev.type == Expose) drawbar();
    }
}

int main(void) {
    if (!(dpy = XOpenDisplay(NULL))) return 1;
    setup();
    drawbar();
    run();
    XCloseDisplay(dpy);
    return 0;
}
