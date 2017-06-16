
gui_manager make_gui(allocator* alloc, opengl* ogl, asset* font) {

	gui_manager ret;

	ret.alloc = alloc;
	ret.font = font;
	ret.font_point = font->font.point;

	ret.ogl.context = ogl_add_draw_context(ogl, &ogl_mesh_2d_attribs);
	ret.ogl.texture = ogl_add_texture_from_font(ogl, font);
	ret.ogl.shader 	= ogl_add_program(ogl, string_literal("shaders/gui.v"), string_literal("shaders/gui.f"), &ogl_uniforms_gui);
	ret.windows = make_vector<_gui_window>(16, alloc);

	ret.mesh = make_mesh_2d(alloc, 32);

	return ret;
}

guiid getid(gui_manager* gui) {
	return gui->last_id++;
}

void destroy_gui(gui_manager* gui) {

	FORVEC(gui->windows,
		destroy_stack(&it->offsets);
	)

	destroy_vector(&gui->windows);
	destroy_mesh(&gui->mesh);

	gui->ogl.context = 0;
	gui->ogl.texture = 0;
	gui->ogl.shader  = 0;
}

void gui_begin_frame(gui_manager* gui, gui_input_state input) {

	gui->currentwin = 0;
	gui->hot = 1;

	gui->input = input;
}

v2 gui_render_widget_text(gui_manager* gui, _gui_window* win, widget_text* text) {

	v2 pos = add(win->rect.xy, win->base);
	FORVEC(win->offsets.contents,
		pos = add(pos, *it);
	)

	return V2(0.0f, gui->style.line_padding + mesh_push_text_line(&gui->mesh, gui->font, text->text, pos, text->point, text->c));
}

v2 gui_render_widget_carrot(gui_manager* gui, _gui_window* win, widget_carrot* carrot) {

	u32 idx = gui->mesh.verticies.size;
	v2 pos = add(win->rect.xy, win->base);
	FORVEC(win->offsets.contents,
		pos = add(pos, *it);
	)

	if(carrot->active) {
		vector_push(&gui->mesh.verticies, V2(pos.x, 	pos.y));
		vector_push(&gui->mesh.verticies, V2(pos.x + 10, pos.y));
		vector_push(&gui->mesh.verticies, V2(pos.x + 5, pos.y + 10));
	} else {
		vector_push(&gui->mesh.verticies, V2(pos.x + 5 , pos.y));
		vector_push(&gui->mesh.verticies, V2(pos.x, 	 pos.y + 10));
		vector_push(&gui->mesh.verticies, V2(pos.x + 10, pos.y + 10));
	}

	colorf cf = color_to_f(carrot->c);
	FOR(3) vector_push(&gui->mesh.colors, cf);

	FOR(3) vector_push(&gui->mesh.texCoords, V3(0.0f, 0.0f, 0.0f));

	vector_push(&gui->mesh.elements, V3(idx, idx + 1, idx + 2));

	return V2(0.0f, gui->style.line_padding + 10.0f);
}

void gui_end_frame_render(opengl* ogl, gui_manager* gui) {

	if(!gui->input.lclick && !gui->input.rclick && !gui->input.mclick && !gui->input.ldbl) {
		gui->active = 1;
	} else if(gui->active == 1) {
		gui->active = 0;
	}

	render_command_list rcl = make_command_list();
	render_command cmd = make_render_command(render_mesh_2d, &gui->mesh);
	cmd.shader  = gui->ogl.shader;
	cmd.texture = gui->ogl.texture;
	cmd.context = gui->ogl.context;

	render_add_command(&rcl, cmd);
	rcl.proj = ortho(0, (f32)global_state->window_w, (f32)global_state->window_h, 0, -1, 1);

	ogl_send_mesh_2d(ogl, &gui->mesh, gui->ogl.context);
	ogl_render_command_list(ogl, &rcl);
	destroy_command_list(&rcl);

	clear_mesh(&gui->mesh);
}

void push_windowhead(gui_manager* gui, _gui_window* win) {
	
	u32 idx = gui->mesh.verticies.size;
	r2 r = win->rect;
	f32 pt = gui->font_point + gui->style.title_padding;

	vector_push(&gui->mesh.verticies, V2(r.x + r.w - 10.0f, r.y));
	vector_push(&gui->mesh.verticies, V2(r.x + 10.0f, r.y));
	vector_push(&gui->mesh.verticies, V2(r.x, r.y + pt));
	vector_push(&gui->mesh.verticies, V2(r.x + r.w, r.y + pt));

	FOR(4) vector_push(&gui->mesh.texCoords, V3f(0,0,0));

	colorf topf = color_to_f(V4b(gui->style.win_top, 255));
	FOR(4) vector_push(&gui->mesh.colors, topf);

	vector_push(&gui->mesh.elements, V3u(idx + 2, idx, idx + 1));
	vector_push(&gui->mesh.elements, V3u(idx + 3, idx, idx + 2));
}

void push_windowbody(gui_manager* gui, _gui_window* win, f32 opacity) {

	u32 idx = gui->mesh.verticies.size;
	r2 r = win->rect;
	f32 pt = gui->font_point + gui->style.title_padding;

	vector_push(&gui->mesh.verticies, V2(r.x, r.y + pt));
	vector_push(&gui->mesh.verticies, V2(r.x, r.y + r.h));
	vector_push(&gui->mesh.verticies, V2(r.x + r.w - 10.0f, r.y + r.h));
	vector_push(&gui->mesh.verticies, V2(r.x + r.w, r.y + r.h - 10.0f));
	vector_push(&gui->mesh.verticies, V2(r.x + r.w, r.y + pt));

	FOR(5) vector_push(&gui->mesh.texCoords, V3f(0,0,0));

	colorf cf = color_to_f(V4b(gui->style.win_back, opacity * 255.0f));

	FOR(5) vector_push(&gui->mesh.colors, cf);

	vector_push(&gui->mesh.elements, V3u(idx, idx + 1, idx + 2));
	vector_push(&gui->mesh.elements, V3u(idx, idx + 2, idx + 3));
	vector_push(&gui->mesh.elements, V3u(idx, idx + 3, idx + 4));
}

guiid get_guiid(_gui_window* win, string text) {

	string str = make_cat_string(win->title, text);
	guiid ret = hash_string(str);
	free_string(str);
	return ret;
}

bool gui_window(gui_manager* gui, string title, r2 rect, f32 opacity) {

	guiid id = hash_string(title);

	bool found = false;
	FORVEC(gui->windows,
		if(it->title == title) {
			gui->currentwin = __i;
			found = true;
		}
	)

	_gui_window* current = vector_get(&gui->windows, gui->currentwin);
	
	if(!found) {
		_gui_window win;
		gui->currentwin = gui->windows.size;
		vector_push(&gui->windows, win);
		current = vector_back(&gui->windows);

		current->rect = rect;
		current->title = title;
		current->offsets = make_stack<v2>(8, gui->alloc);
	}
	current->base = V2f(5, 0);

	rect = current->rect;

	push_windowhead(gui, current);

	gui_push_offset(gui, V2(rect.w - 25.0f, (gui->font_point + gui->style.title_padding) / 2.0f - 10.0f));
	gui_carrot(gui, string_literal("closecarrot"), &current->shown, V4b(gui->style.win_close,255));;
	gui_pop_offset(gui);
	current->base = sub(current->base, current->last);

	gui_push_offset(gui, V2(10.0f, 0.0f));
	gui_text_line(gui, title, gui->font_point, V4b(255, 255, 255, 255));
	gui_pop_offset(gui);

	current->base = add(current->base, V2(0.0f, 5.0f));

	if(current->shown) {
		push_windowbody(gui, current, opacity);
	}

	r2 moverect = R2(rect.x, rect.y, rect.w, gui->font_point + gui->style.title_padding);
	if(inside(moverect, (f32)gui->input.mousex, (f32)gui->input.mousey)) {
		gui->hot = id;
		if(gui->active == 1 && gui->input.lclick) {
			gui->active = id;
			current->clickoffset = V2(gui->input.mousex - rect.x, gui->input.mousey - rect.y);
		} 
		if(gui->active == 1 && gui->input.ldbl) {
			gui->active = id;
			current->shown = !current->shown;
		} 
	}

	if(gui->active == id) {
		current->rect.x = gui->input.mousex - current->clickoffset.x;
		current->rect.y = gui->input.mousey - current->clickoffset.y;
	}

	return current->shown;
}

void gui_push_offset(gui_manager* gui, v2 offset) {

	_gui_window* current = vector_get(&gui->windows,gui->currentwin);

	stack_push(&current->offsets, offset);
}

void gui_pop_offset(gui_manager* gui) {

	_gui_window* current = vector_get(&gui->windows,gui->currentwin);

	stack_pop(&current->offsets);
}

bool gui_carrot(gui_manager* gui, string text, bool* toggle, color c) {

	_gui_window* current = vector_get(&gui->windows, gui->currentwin);

	guiid id = get_guiid(current, text);

	widget_carrot car;
	car.c = c;

	gui_push_offset(gui, V2(0.0f, 5.0f));
	v2 pos = add(current->rect.xy, current->base);
	FORVEC(current->offsets.contents,
		pos = add(pos, *it);
	)

	r2 rect = R2(pos, V2(10.0f, 10.0f));
	if(inside(rect, (f32)gui->input.mousex, (f32)gui->input.mousey)) {
		gui->hot = id;
		if(gui->active == 1 && (gui->input.lclick || gui->input.ldbl)) {
			if(toggle) {
				*toggle = !*toggle;
			}
			gui->active = id;
		}
	}

	car.active = *toggle;

	current->last = gui_render_widget_carrot(gui, current, &car);
	current->base = add(current->base, current->last);

	// gui_push_offset(gui, V2(15.0f, 0.0f));
	// gui_text_line(gui, text, 0.0f, c);
	// gui_pop_offset(gui);

	return *toggle;
}

void gui_text_line_f(gui_manager* gui, string fmt, ...) {

	va_list args;
	va_start(args, fmt);
	gui_text_line_fexv(gui, fmt, 0.0f, V4b(255, 255, 255, 255), args);
	va_end(args);
}

void gui_text_line_fexv(gui_manager* gui, string fmt, f32 point, color c, va_list args) {

	string final = make_vstringf(fmt, args);
	gui_text_line(gui, final, point, c);

	free_string(final);
}

void gui_text_line_fex(gui_manager* gui, string fmt, f32 point, color c, ...) {
	
	va_list args;
	va_start(args, fmt);
	gui_text_line_fexv(gui, fmt, point, c, args);
	va_end(args);
}

void gui_text_line(gui_manager* gui, string str, f32 point, color c) {

	if (point == 0.0f) {
		point = gui->font_point;
	}

	_gui_window* current = vector_get(&gui->windows,gui->currentwin);

	widget_text t;
	t.text = str;
	t.point = point;
	t.c = c;

	current->last = gui_render_widget_text(gui, current, &t);
	current->base = add(current->base, current->last);
}
