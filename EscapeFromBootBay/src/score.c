#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "efbb.h"
#include "score.h"
#include <app.h>

struct _Score {
     Evas_Object *layout;
     int score;
};

Score *
score_add(Evas_Object *win, int hiscore)
{
   Score *score;
   char buf[32];

   score = calloc(1, sizeof(Score));
   if (!score) return NULL;

   score->layout = elm_layout_add(win);
   char path[1024]; snprintf(path, 1024, "%s/etrophy.edj", app_get_resource_path(), GAME_THEME);
   elm_layout_file_set(score->layout, path, "score");
   evas_object_resize(score->layout, 300, 100);
   evas_object_move(score->layout, WIDTH - 300 - 20, 20);
   evas_object_layer_set(score->layout, LAYER_UI);
   evas_object_show(score->layout);

   if (hiscore < 0) hiscore = 0;
   snprintf(buf, sizeof(buf), "%d", hiscore);
   elm_object_part_text_set(score->layout, "hiscore", buf);

   return score;
}

int
score_get(Score *score)
{
   return score->score;
}

void
score_inc(Score *score, int value)
{
   char buf[32];
   score->score += value;
   snprintf(buf, sizeof(buf), "%d", score->score);
   elm_object_part_text_set(score->layout, "score", buf);
}

void
score_del(Score *score)
{
   evas_object_del(score->layout);
   free(score);
}
