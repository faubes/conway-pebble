#include <pebble.h>

Window *window;
Layer *layer;

static void my_layer_draw();
static void window_load();
static void window_unload();
static void randomize_board();
static void born(int x, int y);
static void dies(int x, int y);
static void up_click_handler(ClickRecognizerRef recognizer, void *context);
static void click_config_provider(void *context);

unsigned char density[48][48];
bool board[48][48];
bool alldead=false;

int abs(int v) 
{
  return v * ( (v<0) * (-1) + (v>0));
  // simpler: v * ((v>0) - (v<0))   thanks Jens
}

void handle_init(void) {
	// Create a window and text layer
	window = window_create();	
	// Push the window
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  window_set_click_config_provider(window, click_config_provider);
	window_stack_push(window, true);
	randomize_board();
}

static void randomize_board() {
  for (int i=0; i<48; i++) {
    for (int j=0; j<48; j++) {
      board[i][j]=false;
      density[i][j]=0;
      if ((rand() % 10) < 2) born(i,j);
    }
  }
}


static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  
  // Create Layer
  layer = layer_create(GRect(0, 12, 144, 144));
  layer_add_child(window_layer, layer);

  // Set the update_proc
  layer_set_update_proc(layer, my_layer_draw);  
  
}

static void window_unload(Window *window) {
  layer_destroy(layer); 
  
}

static void dies(int x, int y) {
  int l = abs((x-1) % 48);
  int r = (x+1) % 48;
  int t = (y+1) % 48;
  int b = abs((y-1) % 48);
  density[l][t]--;
  density[l][y]--;
  density[l][b]--;
  density[x][t]--;
  density[x][b]--;
  density[r][t]--;
  density[r][y]--;
  density[r][b]--;
  board[x][y] = false;
}

static void born(int x, int y) {
  int l = abs((x-1) % 48);
  int r = (x+1) % 48;
  int t = (y+1) % 48;
  int b = abs((y-1) % 48);
  density[l][t]++;
  density[l][y]++;
  density[l][b]++;
  density[x][t]++;
  density[x][b]++;
  density[r][t]++;
  density[r][y]++;
  density[r][b]++;
  board[x][y] = true;
}

static void parse() {
  bool lifefound = false;
  for (int x=0; x<48; x++) {
    for (int y=0; y<48; y++) {
      if (board[x][y]) lifefound = true;
      if (density[x][y] < 2 || density[x][y] >3 ) {
        dies(x,y);
      }
      if (!board[x][y] && density[x][y] == 3) {
        born(x,y);
      }
    }
  }
  if (!lifefound) alldead=true;
}

static void my_layer_draw(Layer *layer, GContext *ctx) {
  graphics_context_set_stroke_color(ctx, GColorWhite);
  for (int i=0; i<48; i++) {
    for (int j=0; j<48; j++) {
      if (board[i][j]) {
        graphics_context_set_fill_color(ctx, GColorBlack);
      }
      else {
        graphics_context_set_fill_color(ctx, GColorWhite);
      }
    graphics_fill_rect(ctx, GRect(3*i, 3*j, 3, 3), 3, 0);
    }
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Up clicked.");
  layer_mark_dirty(layer);
  parse();
}

static void click_config_provider(void *context) {
  // Register the ClickHandlers
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
}

void handle_deinit(void) {
	// Destroy the text layer
	layer_destroy(layer);
	
	// Destroy the window
	window_destroy(window);
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}
