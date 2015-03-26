#include <pebble.h>

Window *my_window;
GBitmap  *b_back,*b_arrow;
BitmapLayer *l_back,*l_arrow[3];
TextLayer *t_arrow[3];
static char c_arrow[3][3];
static int sx,sy;


static void updatetime() {
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  int i,angle,hour;
 
  for(i=0;i<3;i++) {
    angle=(120*i+60-2*tick_time->tm_min)%360;
    if (angle>180) {hour=(tick_time->tm_hour+23)%24;} //hour in the bottom left
    else {hour=(tick_time->tm_hour+i)%24;}
    if (!clock_is_24h_style()) {
      hour=hour%12;
      if (hour==0) hour=12;
    } 
    snprintf(c_arrow[i], sizeof(c_arrow[i]),"%i",hour);
    //center is 42,84, radius is 82. sx and sy are coords of the tip of the arrow
    sx=42+(82*cos_lookup(TRIG_MAX_ANGLE * angle / 360))/TRIG_MAX_RATIO;
    sy=84-(82*sin_lookup(TRIG_MAX_ANGLE * angle / 360))/TRIG_MAX_RATIO;
    if (sx<39) sx=39;
    layer_set_frame(bitmap_layer_get_layer(l_arrow[i]),GRect(sx-39,sy-9,39,18));
    layer_set_frame(text_layer_get_layer(t_arrow[i]),GRect(sx-38,sy-9-3,22,20));
    text_layer_set_text(t_arrow[i],c_arrow[i]);
  }           
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  // Update things here
  updatetime();
}



void handle_init(void) {
  int i;
  my_window = window_create();
  
  b_back=gbitmap_create_with_resource(RESOURCE_ID_IMG_BACK);
  b_arrow=gbitmap_create_with_resource(RESOURCE_ID_IMG_ARROW);
  // background
  l_back=bitmap_layer_create(GRect(0,0,144,168));
  bitmap_layer_set_bitmap(l_back,b_back);
  layer_add_child(window_get_root_layer(my_window), bitmap_layer_get_layer(l_back));
 
  for(i=0;i<3;i++) {
    //arrow
    l_arrow[i]=bitmap_layer_create(GRect(0,0,39,18)); 
    bitmap_layer_set_compositing_mode(l_arrow[i],GCompOpOr);
    bitmap_layer_set_bitmap(l_arrow[i],b_arrow);
    //text on the arrow
    t_arrow[i]=text_layer_create(GRect(0,0,22,20));
    text_layer_set_text_alignment(t_arrow[i],GTextAlignmentCenter);
    text_layer_set_text_color(t_arrow[i],GColorWhite);
    text_layer_set_text(t_arrow[i],c_arrow[i]);
    text_layer_set_background_color(t_arrow[i],GColorClear);
    text_layer_set_font(t_arrow[i],fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    //add to the window
    layer_add_child(window_get_root_layer(my_window), bitmap_layer_get_layer(l_arrow[i]));
    layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(t_arrow[i]));
  }
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  updatetime(); 
  window_stack_push(my_window, true);
}

void handle_deinit(void) {
  int i;
  for (i=1;i<3;i++) { 
    text_layer_destroy(t_arrow[i]);
    bitmap_layer_destroy(l_arrow[i]);
  }
  gbitmap_destroy(b_back);
  gbitmap_destroy(b_arrow);
  bitmap_layer_destroy(l_back);
  window_destroy(my_window);
  tick_timer_service_unsubscribe();
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
