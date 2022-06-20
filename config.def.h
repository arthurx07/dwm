/* See LICENSE file for copyright and license details. */

/* imports */ 
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 0;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const unsigned int gappih    = 20;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 20;       /* vert inner gap between windows */
static const unsigned int gappoh    = 30;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 30;       /* vert outer gap between windows and screen edge */
static       int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 0;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int vertpad            = 0;       /* vertical padding of bar */
static const int sidepad            = 30;       /* horizontal padding of bar */
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#005577";
static char selbgcolor[]            = "#005577";
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "Gimp",    NULL,     NULL,           0,         1,          0,           0,        -1 },
	{ "firefox", NULL,     NULL,           0,    	  0,          0,          -1,        -1 },
	{ "st-256color",       NULL,     NULL,            0,          0,           1,           0,        -1 },
	{ "kitty",   NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ "Spotify", NULL,     NULL,           0,         0,          0,           0,         1 }, /* 0 -> primary monitor, 1 -> secondary monitor, -1 -> focused monitor */
       { NULL,      NULL,     "Trackma",      0,         1,          0,           0,        -1 },
	{ NULL,      NULL,     "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
        { "[@]",      spiral },
        { "[\\]",     dwindle },
        { "H[]",      deck },
        { "TTT",      bstack },
        { "===",      bstackhoriz },
        { "HHH",      grid },
        { "###",      nrowgrid },
        { "---",      horizgrid },
        { ":::",      gaplessgrid },
        { "|M|",      centeredmaster },
        { ">M>",      centeredfloatingmaster },
        { "><>",      NULL },    /* no layout function means floating behavior */
        { NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define MODKEY2 Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define STACKKEYS(MOD,ACTION) \
	{ MOD, XK_j,     ACTION##stack, {.i = INC(+1) } }, \
	{ MOD, XK_k,     ACTION##stack, {.i = INC(-1) } }, \
	{ MOD, XK_grave, ACTION##stack, {.i = PREVSEL } }, \
	{ MOD, XK_q,     ACTION##stack, {.i = 0 } }, \
	{ MOD, XK_a,     ACTION##stack, {.i = 1 } }, \
	{ MOD, XK_z,     ACTION##stack, {.i = 2 } }, \
	{ MOD, XK_x,     ACTION##stack, {.i = -1 } },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", NULL };
static const char *termcmd[]  = { "kitty", NULL };

static const char *upvol[] = { "/home/kumao/.local/bin/volume", "-i", "5", NULL };
static const char *upvol1[] = { "/home/kumao/.local/bin/volume", "-i", "1", NULL };
static const char *downvol[] = { "/home/kumao/.local/bin/volume", "-d", "5", NULL };
static const char *downvol1[] = { "/home/kumao/.local/bin/volume", "-d", "1", NULL };
static const char *mutevol[] = { "/home/kumao/.local/bin/mute", NULL };

static const char *lightup[] = { "/home/kumao/.local/bin/light", "+10%", NULL };
static const char *lightdown[] = { "/home/kumao/.local/bin/light", "10%-", NULL };

static const char *aprev[] = { "/usr/bin/playerctl", "previous", NULL };
static const char *aplay[] = { "/home/kumao/.local/bin/play-pause", NULL };
static const char *anext[] = { "/usr/bin/playerctl", "next", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	STACKKEYS(MODKEY,                          focus)
	STACKKEYS(MODKEY|ShiftMask,                push)
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_h,      setcfact,       {.f = +0.25} },
	{ MODKEY|ShiftMask,             XK_l,      setcfact,       {.f = -0.25} },
	{ MODKEY|ShiftMask,             XK_o,      setcfact,       {.f =  0.00} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY|MODKEY2,              XK_u,      incrgaps,       {.i = +1 } },
	{ MODKEY|MODKEY2|ShiftMask,    XK_u,      incrgaps,       {.i = -1 } },
	{ MODKEY|MODKEY2,              XK_i,      incrigaps,      {.i = +1 } },
	{ MODKEY|MODKEY2|ShiftMask,    XK_i,      incrigaps,      {.i = -1 } },
	{ MODKEY|MODKEY2,              XK_o,      incrogaps,      {.i = +1 } },
	{ MODKEY|MODKEY2|ShiftMask,    XK_o,      incrogaps,      {.i = -1 } },
	{ MODKEY|MODKEY2,              XK_6,      incrihgaps,     {.i = +1 } },
	{ MODKEY|MODKEY2|ShiftMask,    XK_6,      incrihgaps,     {.i = -1 } },
	{ MODKEY|MODKEY2,              XK_7,      incrivgaps,     {.i = +1 } },
	{ MODKEY|MODKEY2|ShiftMask,    XK_7,      incrivgaps,     {.i = -1 } },
	{ MODKEY|MODKEY2,              XK_8,      incrohgaps,     {.i = +1 } },
	{ MODKEY|MODKEY2|ShiftMask,    XK_8,      incrohgaps,     {.i = -1 } },
	{ MODKEY|MODKEY2,              XK_9,      incrovgaps,     {.i = +1 } },
	{ MODKEY|MODKEY2|ShiftMask,    XK_9,      incrovgaps,     {.i = -1 } },
	{ MODKEY|MODKEY2,              XK_0,      togglegaps,     {0} },
	{ MODKEY|MODKEY2|ShiftMask,    XK_0,      defaultgaps,    {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_Tab,       shiftviewclients, { .i = +1 } },
	{ MODKEY|ShiftMask,             XK_backslash, shiftviewclients, { .i = -1 } },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[13]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_u,      setlayout,      {.v = &layouts[11]} },
	{ MODKEY,                       XK_o,      setlayout,      {.v = &layouts[12]} },
	{ MODKEY,                       XK_r,      setlayout,      {.v = &layouts[4]} },
	{ MODKEY|ControlMask,		XK_comma,  cyclelayout,    {.i = -1 } },
	{ MODKEY|ControlMask,           XK_period, cyclelayout,    {.i = +1 } },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_s,      togglesticky,   {0} },
	{ MODKEY|ShiftMask,             XK_f,      togglefullscr,  {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_r,     xrdb,           {.v = NULL } },
	{ MODKEY, 			XK_KP_End,    movetoedge,       {.v = "-1 1" } },
	{ MODKEY, 			XK_KP_Down,   movetoedge,       {.v = "0 1" } },
	{ MODKEY, 			XK_KP_Next,   movetoedge,       {.v = "1 1" } },
	{ MODKEY, 			XK_KP_Left,   movetoedge,       {.v = "-1 0" } },
	{ MODKEY, 			XK_KP_Begin,  movetoedge,       {.v = "0 0" } },
	{ MODKEY, 			XK_KP_Right,  movetoedge,       {.v = "1 0" } },
	{ MODKEY, 			XK_KP_Home,   movetoedge,       {.v = "-1 -1" } },
	{ MODKEY, 			XK_KP_Up,     movetoedge,       {.v = "0 -1" } },
	{ MODKEY, 			XK_KP_Prior,  movetoedge,       {.v = "1 -1" } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_BackSpace, quit,        {0} },
	{ MODKEY|ControlMask|ShiftMask, XK_q,      quit,           {1} }, 
	{ 0,                            XF86XK_AudioLowerVolume,        spawn, {.v = downvol } },
        { MODKEY,                       XF86XK_AudioLowerVolume,        spawn, {.v = downvol1 } },
        { 0,                            XF86XK_AudioMute,               spawn, {.v = mutevol } },
        { 0,                            XF86XK_AudioRaiseVolume,        spawn, {.v = upvol } },
        { MODKEY,                       XF86XK_AudioRaiseVolume,        spawn, {.v = upvol1 } },
        { 0,                            XF86XK_MonBrightnessUp,         spawn, {.v = lightup } },
        { 0,                            XF86XK_MonBrightnessDown,       spawn, {.v = lightdown } },
        { 0,                            XF86XK_AudioPrev, spawn, {.v = aprev } },
        { 0,                            XF86XK_AudioPlay, spawn, {.v = aplay } },
        { 0,                            XF86XK_AudioNext, spawn, {.v = anext } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

