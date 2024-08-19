/* See LICENSE file for copyright and license details. */

/* imports */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 0;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 20;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 20;       /* vert inner gap between windows */
static const unsigned int gappoh    = 60;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 60;       /* vert outer gap between windows and screen edge */
static const int swallowfloating    = 1;        /* 1 means swallow floating windows by default */
static       int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int vertpad            = 17;       /* vertical padding of bar */
static const int sidepad            = 60;       /* horizontal padding of bar */
static const int horizpadbar        = 0;        /* horizontal padding for statusbar */
static const int vertpadbar         = 10;        /* vertical padding for statusbar */
static const char *fonts[]          = { "Liberation Sans:style=Bold:pixelsize=15", "Symbols Nerd Font Mono:size=10", "Noto Color Emoji:size=10", "monospace:size=10" };
static const char dmenufont[]       = { "Liberation Sans:style=Bold:pixelsize=15" };
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#005577";
static char selbgcolor[]            = "#005577";
static char termcol0[] = "#000000"; /* black   */
static char termcol1[] = "#ff0000"; /* red     */
static char termcol2[] = "#33ff00"; /* green   */
static char termcol3[] = "#ff0099"; /* yellow  */
static char termcol4[] = "#0066ff"; /* blue    */
static char termcol5[] = "#cc00ff"; /* magenta */
static char termcol6[] = "#00ffff"; /* cyan    */
static char termcol7[] = "#d0d0d0"; /* white   */
static char termcol8[]  = "#808080"; /* black   */
static char termcol9[]  = "#ff0000"; /* red     */
static char termcol10[] = "#33ff00"; /* green   */
static char termcol11[] = "#ff0099"; /* yellow  */
static char termcol12[] = "#0066ff"; /* blue    */
static char termcol13[] = "#cc00ff"; /* magenta */
static char termcol14[] = "#00ffff"; /* cyan    */
static char termcol15[] = "#ffffff"; /* white   */
static char *termcolor[] = {
  termcol0,
  termcol1,
  termcol2,
  termcol3,
  termcol4,
  termcol5,
  termcol6,
  termcol7,
  termcol8,
  termcol9,
  termcol10,
  termcol11,
  termcol12,
  termcol13,
  termcol14,
  termcol15,
};
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const unsigned int ulinepad	= 8;	/* horizontal padding between the underline and tag */
static const unsigned int ulinestroke	= 1;	/* thickness / height of the underline */
static const unsigned int ulinevoffset	= 5;	/* how far above the bottom of the bar the line should appear */
static const int ulineall 		= 0;	/* 1 to show underline on all tags, 0 for just the active ones */

/* Lockfile */
static char lockfile[] = "/tmp/dwm.lock";

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	/* Tags rules */ /* 0 -> primary monitor, 1 -> secondary monitor, -1 -> focused monitor */
	{ "st-256color", NULL, NULL,           0,         0,          1,           0,        -1 },
	/* Floating rules */
	{ "Wpg",     NULL,     NULL,           0,         1,          0,           0,        -1 },
	{ "Pavucontrol", NULL, NULL,           0,         1,          0,           0,        -1 },
	{ "qpwgraph", "qpwgraph", NULL,        0,         1,          0,           0,        -1 },
	{ NULL,      "qalculate-gtk", NULL,    0,         1,          0,           0,        -1 },
	{ NULL,      NULL,     "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "H[]",      deck },
	{ "TTT",      bstack },
	{ "###",      nrowgrid },
	{ "[@]",      spiral },
	{ "[\\]",     dwindle },
	{ "===",      bstackhoriz },
	{ "HHH",      grid },
	{ "---",      horizgrid },
	{ ":::",      gaplessgrid },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-i", "-h", "20", NULL }; /* -i for case insensitive */
static const char *termcmd[]  = { "st", NULL };
/* volume */
static const char *upvol[] = { "/home/tur/.local/bin/kvolume", "-i", "5", NULL };
static const char *upvol1[] = { "/home/tur/.local/bin/kvolume", "-i", "1", NULL };
static const char *downvol[] = { "/home/tur/.local/bin/kvolume", "-d", "5", NULL };
static const char *downvol1[] = { "/home/tur/.local/bin/kvolume", "-d", "1", NULL };
static const char *mutevol[] = { "/home/tur/.local/bin/kmute", NULL };
/* backlight */
static const char *lightup[] = { "/home/tur/.local/bin/klight", "+10%", NULL };
static const char *lightdown[] = { "/home/tur/.local/bin/klight", "10%-", NULL };
static const char *lightupsec[] = { "/home/tur/.local/bin/klight", "+10%", "secondary", NULL };
static const char *lightdownsec[] = { "/home/tur/.local/bin/klight", "10%-", "secondary", NULL };
/* player */
static const char *aprev[] = { "/usr/bin/playerctl", "previous", NULL };
static const char *aplay[] = { "/home/tur/.local/bin/play-pause", NULL };
static const char *anext[] = { "/usr/bin/playerctl", "next", NULL };

#include "movestack.c"
static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_j,      movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      movestack,      {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_h,      setcfact,       {.f = +0.25} },
	{ MODKEY|ShiftMask,             XK_l,      setcfact,       {.f = -0.25} },
	{ MODKEY|ShiftMask,             XK_o,      setcfact,       {.f =  0.00} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY|ControlMask,           XK_u,      incrgaps,       {.i = +1 } },
	{ MODKEY|ControlMask|ShiftMask, XK_u,      incrgaps,       {.i = -1 } },
	{ MODKEY|ControlMask,           XK_i,      incrigaps,      {.i = +1 } },
	{ MODKEY|ControlMask|ShiftMask, XK_i,      incrigaps,      {.i = -1 } },
	{ MODKEY|ControlMask,           XK_o,      incrogaps,      {.i = +1 } },
	{ MODKEY|ControlMask|ShiftMask, XK_o,      incrogaps,      {.i = -1 } },
	{ MODKEY|ControlMask,           XK_6,      incrihgaps,     {.i = +1 } },
	{ MODKEY|ControlMask|ShiftMask, XK_6,      incrihgaps,     {.i = -1 } },
	{ MODKEY|ControlMask,           XK_7,      incrivgaps,     {.i = +1 } },
	{ MODKEY|ControlMask|ShiftMask, XK_7,      incrivgaps,     {.i = -1 } },
	{ MODKEY|ControlMask,           XK_8,      incrohgaps,     {.i = +1 } },
	{ MODKEY|ControlMask|ShiftMask, XK_8,      incrohgaps,     {.i = -1 } },
	{ MODKEY|ControlMask,           XK_9,      incrovgaps,     {.i = +1 } },
	{ MODKEY|ControlMask|ShiftMask, XK_9,      incrovgaps,     {.i = -1 } },
	{ MODKEY|ControlMask,           XK_0,      togglegaps,     {0} },
	{ MODKEY|ControlMask|ShiftMask, XK_0,      defaultgaps,    {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_Tab,       shiftviewclients, { .i = +1 } },
	{ MODKEY|ShiftMask,             XK_backslash, shiftviewclients, { .i = -1 } },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[13]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_a,      focusmaster,    {0} },
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
	{ MODKEY,                       XK_r,     xrdb,           {.v = NULL } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
	{ MODKEY|ControlMask|ShiftMask, XK_q,      quit,           {1} },
	{ 0,                            XF86XK_AudioRaiseVolume, spawn, {.v = upvol } },
	{ MODKEY,                       XF86XK_AudioRaiseVolume, spawn, {.v = upvol1 } },
	{ 0,                            XF86XK_AudioMute,        spawn, {.v = mutevol } },
	{ 0,                            XF86XK_AudioLowerVolume, spawn, {.v = downvol } },
	{ MODKEY,                       XF86XK_AudioLowerVolume, spawn, {.v = downvol1 } },
	{ 0,                            XF86XK_MonBrightnessUp,  spawn, {.v = lightup } },
	{ 0,                            XF86XK_MonBrightnessDown, spawn, {.v = lightdown } },
	{ ShiftMask,                    XF86XK_MonBrightnessUp,  spawn, {.v = lightupsec } },
	{ ShiftMask,                    XF86XK_MonBrightnessDown, spawn, {.v = lightdownsec } },
	{ 0,                            XF86XK_AudioPrev, spawn, {.v = aprev } },
	{ 0,                            XF86XK_AudioPlay, spawn, {.v = aplay } },
	{ 0,                            XF86XK_AudioNext, spawn, {.v = anext } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

