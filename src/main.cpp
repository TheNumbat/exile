
#include "lib/basic.h"
#include "lib/alloc.h"
#include "engine.h"

i32 main(i32 argc, char **argv) {

	Engine eng;
	eng.init();
	eng.loop();
	eng.destroy();

	mem_validate();

	return 0;
}
