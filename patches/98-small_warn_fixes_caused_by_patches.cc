diff --git a/dwm.c b/dwm.c
index 995f86d..d550c43 100644
--- a/dwm.c
+++ b/dwm.c
@@ -979,8 +979,6 @@ void
 drawbar(Monitor *m)
 {
 	int x, w, tw = 0;
-	int boxs = drw->fonts->h / 9;
-	int boxw = drw->fonts->h / 6 + 2;
 	unsigned int i, occ = 0, urg = 0;
 	Client *c;
 
