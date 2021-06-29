--- libmp3lame/id3tag.c.orig	2021-06-29 13:10:10 UTC
+++ libmp3lame/id3tag.c
@@ -749,6 +749,7 @@ isMultiFrame(uint32_t frame_id)
     case ID_ENCR:
     case ID_GRID:
     case ID_PRIV:
+    case ID_USLT:
         return 1;
     }
     return 0;
@@ -761,6 +762,7 @@ isFullTextString(int frame_id)
     switch (frame_id) {
     case ID_VSLT:
     case ID_COMMENT:
+    case ID_USLT:
         return 1;
     }
     return 0;
@@ -975,6 +977,22 @@ id3tag_set_userinfo_latin1(lame_t gfp, uint32_t id, ch
 }
 
 static int
+id3tag_set_lyrics_latin1(lame_t gfp, uint32_t id, char const *fieldvalue)
+{
+    char const separator = '=';
+    int     rc = -7;
+    int     a = local_char_pos(fieldvalue, separator);
+    if (a >= 0) {
+        char*   dup = 0;
+        local_strdup(&dup, fieldvalue);
+        dup[a] = 0;
+        rc = id3v2_add_latin1_lng(gfp, id, dup, dup+a+1);
+        free(dup);
+    }
+    return rc;
+}
+
+static int
 id3tag_set_userinfo_ucs2(lame_t gfp, uint32_t id, unsigned short const *fieldvalue)
 {
     unsigned short const separator = fromLatin1Char(fieldvalue,'=');
@@ -992,6 +1010,24 @@ id3tag_set_userinfo_ucs2(lame_t gfp, uint32_t id, unsi
     return rc;
 }
 
+static int
+id3tag_set_lyrics_ucs2(lame_t gfp, uint32_t id, unsigned short const *fieldvalue)
+{
+    unsigned short const separator = fromLatin1Char(fieldvalue,'=');
+    int     rc = -7;
+    size_t  b = local_ucs2_strlen(fieldvalue);
+    int     a = local_ucs2_pos(fieldvalue, separator);
+    if (a >= 0) {
+        unsigned short* dsc = 0, *val = 0;
+        local_ucs2_substr(&dsc, fieldvalue, 0, a);
+        local_ucs2_substr(&val, fieldvalue, a+1, b);
+        rc = id3v2_add_ucs2_lng(gfp, id, dsc, val);
+        free(dsc);
+        free(val);
+    }
+    return rc;
+}
+
 int
 id3tag_set_textinfo_utf16(lame_t gfp, char const *id, unsigned short const *text)
 {
@@ -1023,6 +1059,9 @@ id3tag_set_textinfo_utf16(lame_t gfp, char const *id, 
     if (frame_id == ID_WFED) {
         return id3v2_add_ucs2_lng(gfp, frame_id, text, 0); /* iTunes expects WFED to be a text frame */
     }
+    if (frame_id == ID_USLT) {
+        return id3tag_set_lyrics_ucs2(gfp, frame_id, text);
+    }
     if (isFrameIdMatching(frame_id, FRAME_ID('T', 0, 0, 0))
       ||isFrameIdMatching(frame_id, FRAME_ID('W', 0, 0, 0))) {
 #if 0
@@ -1072,6 +1111,9 @@ id3tag_set_textinfo_latin1(lame_t gfp, char const *id,
     if (frame_id == ID_WFED) {
         return id3v2_add_latin1_lng(gfp, frame_id, text, 0); /* iTunes expects WFED to be a text frame */
     }
+    if (frame_id == ID_USLT) {
+        return id3tag_set_lyrics_latin1(gfp, frame_id, text);
+    }
     if (isFrameIdMatching(frame_id, FRAME_ID('T', 0, 0, 0))
       ||isFrameIdMatching(frame_id, FRAME_ID('W', 0, 0, 0))) {
         return id3v2_add_latin1_lng(gfp, frame_id, 0, text);
@@ -1714,7 +1756,7 @@ lame_get_id3v2_tag(lame_t gfp, unsigned char *buffer, 
                 if (tag->v2_head != 0) {
                     FrameDataNode *node;
                     for (node = tag->v2_head; node != 0; node = node->nxt) {
-                        if (node->fid == ID_COMMENT || node->fid == ID_USER) {
+                        if (node->fid == ID_COMMENT || node->fid == ID_USER || node->fid == ID_USLT) {
                             tag_size += sizeOfCommentNode(node);
                         }
                         else if (isFrameIdMatching(node->fid, FRAME_ID('W',0,0,0))) {
@@ -1771,7 +1813,7 @@ lame_get_id3v2_tag(lame_t gfp, unsigned char *buffer, 
                 if (tag->v2_head != 0) {
                     FrameDataNode *node;
                     for (node = tag->v2_head; node != 0; node = node->nxt) {
-                        if (node->fid == ID_COMMENT || node->fid == ID_USER) {
+                        if (node->fid == ID_COMMENT || node->fid == ID_USER || node->fid == ID_USLT) {
                             p = set_frame_comment(p, node);
                         }
                         else if (isFrameIdMatching(node->fid,FRAME_ID('W',0,0,0))) {
