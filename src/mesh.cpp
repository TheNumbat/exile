
void setup_mesh_commands() { PROF
	eng->ogl.add_command((u16)mesh_cmd::_2d_col, FPTR(run_mesh_2d_col), "shaders/mesh/2d_col.v"_, "shaders/mesh/2d_col.f"_, FPTR(uniforms_mesh_2d_col), FPTR(compat_mesh_2d_col));
	eng->ogl.add_command((u16)mesh_cmd::_2d_tex, FPTR(run_mesh_2d_tex), "shaders/mesh/2d_tex.v"_, "shaders/mesh/2d_tex.f"_, FPTR(uniforms_mesh_2d_tex), FPTR(compat_mesh_2d_tex));
	eng->ogl.add_command((u16)mesh_cmd::_2d_tex_col, FPTR(run_mesh_2d_tex_col), "shaders/mesh/2d_tex_col.v"_, "shaders/mesh/2d_tex_col.f"_, FPTR(uniforms_mesh_2d_tex_col), FPTR(compat_mesh_2d_tex_col));
	eng->ogl.add_command((u16)mesh_cmd::_3d_tex, FPTR(run_mesh_3d_tex), "shaders/mesh/3d_tex.v"_, "shaders/mesh/3d_tex.f"_, FPTR(uniforms_mesh_3d_tex), FPTR(compat_mesh_3d_tex));
	eng->ogl.add_command((u16)mesh_cmd::_3d_tex_instanced, FPTR(run_mesh_3d_tex_instanced), "shaders/mesh/3d_tex_instanced.v"_, "shaders/mesh/3d_tex_instanced.f"_, FPTR(uniforms_mesh_3d_tex_instanced), FPTR(compat_mesh_3d_tex_instanced));
	eng->ogl.add_command((u16)mesh_cmd::lines, FPTR(run_mesh_lines), "shaders/lines.v"_, "shaders/lines.f"_, FPTR(uniforms_mesh_lines), FPTR(compat_mesh_lines));
	eng->ogl.add_command((u16)mesh_cmd::chunk, FPTR(run_mesh_chunk), "shaders/chunk.v"_, "shaders/chunk.f"_, FPTR(uniforms_mesh_chunk), FPTR(compat_mesh_chunk));
	eng->ogl.add_command((u16)mesh_cmd::cubemap, FPTR(run_mesh_cubemap), "shaders/cubemap.v"_, "shaders/cubemap.f"_, FPTR(uniforms_mesh_cubemap), FPTR(compat_mesh_cubemap));
}

CALLBACK void uniforms_mesh_cubemap(shader_program* prog, render_command* cmd, render_command_list* rcl) { PROF

	GLint loc = glGetUniformLocation(prog->handle, "transform");

	m4 transform = rcl->proj * rcl->view * cmd->model;

	glUniformMatrix4fv(loc, 1, gl_bool::_false, transform.a);
}

CALLBACK void uniforms_mesh_chunk(shader_program* prog, render_command* cmd, render_command_list* rcl) { PROF

	GLint loc = glGetUniformLocation(prog->handle, "transform");
	GLint szloc = glGetUniformLocation(prog->handle, "units_per_voxel");

	m4 transform = rcl->proj * rcl->view * cmd->model;

	glUniformMatrix4fv(loc, 1, gl_bool::_false, transform.a);
	glUniform1f(szloc, (f32)chunk::units_per_voxel);
}

CALLBACK void uniforms_mesh_2d_col(shader_program* prog, render_command* cmd, render_command_list* rcl) { PROF

	GLint loc = glGetUniformLocation(prog->handle, "transform");

	m4 transform = rcl->proj * rcl->view * cmd->model;

	glUniformMatrix4fv(loc, 1, gl_bool::_false, transform.a);
}

CALLBACK void uniforms_mesh_2d_tex(shader_program* prog, render_command* cmd, render_command_list* rcl) { PROF

	GLint loc = glGetUniformLocation(prog->handle, "transform");

	m4 transform = rcl->proj * rcl->view * cmd->model;

	glUniformMatrix4fv(loc, 1, gl_bool::_false, transform.a);
}

CALLBACK void uniforms_mesh_2d_tex_col(shader_program* prog, render_command* cmd, render_command_list* rcl) { PROF

	GLint loc = glGetUniformLocation(prog->handle, "transform");

	m4 transform = rcl->proj * rcl->view * cmd->model;

	glUniformMatrix4fv(loc, 1, gl_bool::_false, transform.a);
}

CALLBACK void uniforms_mesh_3d_tex(shader_program* prog, render_command* cmd, render_command_list* rcl) { PROF
	
	GLint loc = glGetUniformLocation(prog->handle, "transform");

	m4 transform = rcl->proj * rcl->view * cmd->model;

	glUniformMatrix4fv(loc, 1, gl_bool::_false, transform.a);
}

CALLBACK void uniforms_mesh_lines(shader_program* prog, render_command* cmd, render_command_list* rcl) { PROF
	
	GLint loc = glGetUniformLocation(prog->handle, "transform");

	m4 transform = rcl->proj * rcl->view * cmd->model;

	glUniformMatrix4fv(loc, 1, gl_bool::_false, transform.a);
}

CALLBACK void uniforms_mesh_3d_tex_instanced(shader_program* prog, render_command* cmd, render_command_list* rcl) { PROF
	
	GLint loc = glGetUniformLocation(prog->handle, "transform");

	m4 transform = rcl->proj * rcl->view * cmd->model;

	glUniformMatrix4fv(loc, 1, gl_bool::_false, transform.a);
}

CALLBACK void update_mesh_cubemap(gpu_object* obj, void* data, bool force) { PROF

	mesh_cubemap* m = (mesh_cubemap*)data;
	if(!force && !m->dirty) return;

	glBindBuffer(gl_buf_target::array, obj->vbos[0]);
	glBufferData(gl_buf_target::array, 36 * sizeof(v3), m->vertices, gl_buf_usage::dynamic_draw);

	m->dirty = false;
}

CALLBACK void update_mesh_chunk(gpu_object* obj, void* data, bool force) { PROF

	mesh_chunk* m = (mesh_chunk*)data;
	if(!force && !m->dirty) return;

	glBindBuffer(gl_buf_target::array, obj->vbos[0]);
	glBufferData(gl_buf_target::array, m->vertices.size * sizeof(chunk_vertex), m->vertices.size ? m->vertices.memory : null, gl_buf_usage::dynamic_draw);

	glBindBuffer(gl_buf_target::element_array, obj->vbos[1]);
	glBufferData(gl_buf_target::element_array, m->elements.size * sizeof(uv3), m->elements.size ? m->elements.memory : null, gl_buf_usage::dynamic_draw);

	m->dirty = false;
}

CALLBACK void update_mesh_2d_col(gpu_object* obj, void* data, bool force) { PROF

	mesh_2d_col* m = (mesh_2d_col*)data;
	if(!force && !m->dirty) return;

	glBindBuffer(gl_buf_target::array, obj->vbos[0]);
	glBufferData(gl_buf_target::array, m->vertices.size * sizeof(v2), m->vertices.size ? m->vertices.memory : null, gl_buf_usage::dynamic_draw);

	glBindBuffer(gl_buf_target::array, obj->vbos[1]);
	glBufferData(gl_buf_target::array, m->colors.size * sizeof(colorf), m->colors.size ? m->colors.memory : null, gl_buf_usage::dynamic_draw);

	glBindBuffer(gl_buf_target::element_array, obj->vbos[2]);
	glBufferData(gl_buf_target::element_array, m->elements.size * sizeof(uv3), m->elements.size ? m->elements.memory : null, gl_buf_usage::dynamic_draw);

	m->dirty = false;
}

CALLBACK void update_mesh_2d_tex(gpu_object* obj, void* data, bool force) { PROF

	mesh_2d_tex* m = (mesh_2d_tex*)data;
	if(!force && !m->dirty) return;

	glBindBuffer(gl_buf_target::array, obj->vbos[0]);
	glBufferData(gl_buf_target::array, m->vertices.size * sizeof(v2), m->vertices.size ? m->vertices.memory : null, gl_buf_usage::dynamic_draw);

	glBindBuffer(gl_buf_target::array, obj->vbos[1]);
	glBufferData(gl_buf_target::array, m->texCoords.size * sizeof(v2), m->texCoords.size ? m->texCoords.memory : null, gl_buf_usage::dynamic_draw);

	glBindBuffer(gl_buf_target::element_array, obj->vbos[2]);
	glBufferData(gl_buf_target::element_array, m->elements.size * sizeof(uv3), m->elements.size ? m->elements.memory : null, gl_buf_usage::dynamic_draw);

	m->dirty = false;
}

CALLBACK void update_mesh_2d_tex_col(gpu_object* obj, void* data, bool force) { PROF

	mesh_2d_tex_col* m = (mesh_2d_tex_col*)data;
	if(!force && !m->dirty) return;

	glBindBuffer(gl_buf_target::array, obj->vbos[0]);
	glBufferData(gl_buf_target::array, m->vertices.size * sizeof(v2), m->vertices.size ? m->vertices.memory : null, gl_buf_usage::dynamic_draw);

	glBindBuffer(gl_buf_target::array, obj->vbos[1]);
	glBufferData(gl_buf_target::array, m->texCoords.size * sizeof(v2), m->texCoords.size ? m->texCoords.memory : null, gl_buf_usage::dynamic_draw);

	glBindBuffer(gl_buf_target::array, obj->vbos[2]);
	glBufferData(gl_buf_target::array, m->colors.size * sizeof(colorf), m->colors.size ? m->colors.memory : null, gl_buf_usage::dynamic_draw);

	glBindBuffer(gl_buf_target::element_array, obj->vbos[3]);
	glBufferData(gl_buf_target::element_array, m->elements.size * sizeof(uv3), m->elements.size ? m->elements.memory : null, gl_buf_usage::dynamic_draw);

	m->dirty = false;
}

CALLBACK void update_mesh_3d_tex(gpu_object* obj, void* data, bool force) { PROF

	mesh_3d_tex* m = (mesh_3d_tex*)data;
	if(!force && !m->dirty) return;

	glBindBuffer(gl_buf_target::array, obj->vbos[0]);
	glBufferData(gl_buf_target::array, m->vertices.size * sizeof(v3), m->vertices.size ? m->vertices.memory : null, gl_buf_usage::dynamic_draw);

	glBindBuffer(gl_buf_target::array, obj->vbos[1]);
	glBufferData(gl_buf_target::array, m->texCoords.size * sizeof(v2), m->texCoords.size ? m->texCoords.memory : null, gl_buf_usage::dynamic_draw);

	glBindBuffer(gl_buf_target::element_array, obj->vbos[2]);
	glBufferData(gl_buf_target::element_array, m->elements.size * sizeof(uv3), m->elements.size ? m->elements.memory : null, gl_buf_usage::dynamic_draw);

	m->dirty = false;
}

CALLBACK void update_mesh_lines(gpu_object* obj, void* data, bool force) { PROF

	mesh_lines* m = (mesh_lines*)data;
	if(!force && !m->dirty) return;

	glBindBuffer(gl_buf_target::array, obj->vbos[0]);
	glBufferData(gl_buf_target::array, m->vertices.size * sizeof(v3), m->vertices.size ? m->vertices.memory : null, gl_buf_usage::dynamic_draw);

	glBindBuffer(gl_buf_target::array, obj->vbos[1]);
	glBufferData(gl_buf_target::array, m->colors.size * sizeof(colorf), m->colors.size ? m->colors.memory : null, gl_buf_usage::dynamic_draw);

	m->dirty = false;
}

CALLBACK void update_mesh_3d_tex_instanced(gpu_object* obj, void* d, bool force) { PROF

	mesh_3d_tex_instance_data* data = (mesh_3d_tex_instance_data*)d;
	mesh_3d_tex* m = data->parent;
	
	gpu_object* par_obj = eng->ogl.get_object(m->gpu);

	par_obj->update(par_obj, m, force);

	if(force || data->dirty) {

		glBindBuffer(gl_buf_target::array, obj->vbos[0]);
		glBufferData(gl_buf_target::array, data->data.size * sizeof(v3), data->data.size ? data->data.memory : null, gl_buf_usage::dynamic_draw);
	}

	data->dirty = false;
}

CALLBACK void run_mesh_cubemap(render_command* cmd, gpu_object* gpu) { PROF

	glDrawArrays(gl_draw_mode::triangles, 0, 36);
}

CALLBACK void run_mesh_chunk(render_command* cmd, gpu_object* gpu) { PROF

	mesh_chunk* m = (mesh_chunk*)gpu->data;

	u32 num_tris = ((cmd->num_tris ? cmd->num_tris : m->elements.size) - cmd->start_tri) * 3;
	glDrawElementsBaseVertex(gl_draw_mode::triangles, num_tris, gl_index_type::unsigned_int, (void*)(u64)(0), cmd->offset);
}

CALLBACK void run_mesh_2d_col(render_command* cmd, gpu_object* gpu) { PROF

	mesh_2d_col* m = (mesh_2d_col*)gpu->data;

	u32 num_tris = ((cmd->num_tris ? cmd->num_tris : m->elements.size) - cmd->start_tri) * 3;
	glDrawElementsBaseVertex(gl_draw_mode::triangles, num_tris, gl_index_type::unsigned_int, (void*)(u64)(0), cmd->offset);
}

CALLBACK void run_mesh_2d_tex(render_command* cmd, gpu_object* gpu) { PROF

	mesh_2d_tex* m = (mesh_2d_tex*)gpu->data;

	u32 num_tris = ((cmd->num_tris ? cmd->num_tris : m->elements.size) - cmd->start_tri) * 3;
	glDrawElementsBaseVertex(gl_draw_mode::triangles, num_tris, gl_index_type::unsigned_int, (void*)(u64)(0), cmd->offset);
}

CALLBACK void run_mesh_2d_tex_col(render_command* cmd, gpu_object* gpu) { PROF

	mesh_2d_tex_col* m = (mesh_2d_tex_col*)gpu->data;

	u32 num_tris = ((cmd->num_tris ? cmd->num_tris : m->elements.size) - cmd->start_tri) * 3;
	glDrawElementsBaseVertex(gl_draw_mode::triangles, num_tris, gl_index_type::unsigned_int, (void*)(u64)(0), cmd->offset);
}

CALLBACK void run_mesh_3d_tex(render_command* cmd, gpu_object* gpu) { PROF

	mesh_3d_tex* m = (mesh_3d_tex*)gpu->data;

	u32 num_tris = ((cmd->num_tris ? cmd->num_tris : m->elements.size) - cmd->start_tri) * 3;
	glDrawElementsBaseVertex(gl_draw_mode::triangles, num_tris, gl_index_type::unsigned_int, (void*)(u64)(0), cmd->offset);
}

CALLBACK void run_mesh_lines(render_command* cmd, gpu_object* gpu) { PROF

	mesh_lines* m = (mesh_lines*)gpu->data;

	glDrawArrays(gl_draw_mode::lines, 0, m->vertices.size);
}

CALLBACK void run_mesh_3d_tex_instanced(render_command* cmd, gpu_object* gpu) { PROF

	mesh_3d_tex_instance_data* data = (mesh_3d_tex_instance_data*)gpu->data;
	mesh_3d_tex* m = data->parent;

	u32 num_tris = ((cmd->num_tris ? cmd->num_tris : m->elements.size) - cmd->start_tri) * 3;

	glDrawElementsInstancedBaseVertex(gl_draw_mode::triangles, num_tris, gl_index_type::unsigned_int, (void*)(u64)(0), data->instances, cmd->offset);
}

CALLBACK bool compat_mesh_cubemap(ogl_info* info) { PROF
	return info->check_version(3, 2);
}

CALLBACK bool compat_mesh_chunk(ogl_info* info) { PROF
	return info->check_version(3, 2);
}

CALLBACK bool compat_mesh_2d_col(ogl_info* info) { PROF
	return info->check_version(3, 2);
}

CALLBACK bool compat_mesh_2d_tex(ogl_info* info) { PROF
	return info->check_version(3, 2);
}

CALLBACK bool compat_mesh_2d_tex_col(ogl_info* info) { PROF
	return info->check_version(3, 2);
}

CALLBACK bool compat_mesh_3d_tex(ogl_info* info) { PROF
	return info->check_version(3, 2);
}

CALLBACK bool compat_mesh_lines(ogl_info* info) { PROF
	return info->check_version(3, 2);
}

CALLBACK bool compat_mesh_3d_tex_instanced(ogl_info* info) { PROF
	return info->check_version(3, 3);
}

CALLBACK void setup_mesh_cubemap(gpu_object* obj) { PROF

	glBindBuffer(gl_buf_target::array, obj->vbos[0]);

	glVertexAttribPointer(0, 3, gl_vert_attrib_type::_float, gl_bool::_false, sizeof(v3), (void*)0);
	glEnableVertexAttribArray(0);
}

CALLBACK void setup_mesh_chunk(gpu_object* obj) { PROF

	glBindBuffer(gl_buf_target::array, obj->vbos[0]);

	glVertexAttribIPointer(0, 2, gl_vert_attrib_type::unsigned_int, sizeof(chunk_vertex), (void*)0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(gl_buf_target::element_array, obj->vbos[1]);
}

CALLBACK void setup_mesh_2d_col(gpu_object* obj) { PROF

	glBindVertexArray(obj->vao);

	glBindBuffer(gl_buf_target::array, obj->vbos[0]);
	glVertexAttribPointer(0, 2, gl_vert_attrib_type::_float, gl_bool::_false, sizeof(v2), (void*)0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(gl_buf_target::array, obj->vbos[1]);
	glVertexAttribPointer(1, 4, gl_vert_attrib_type::_float, gl_bool::_false, sizeof(v4), (void*)0);
	glEnableVertexAttribArray(1);
	
	glBindBuffer(gl_buf_target::element_array, obj->vbos[2]);

	glBindVertexArray(0);
}

CALLBACK void setup_mesh_2d_tex(gpu_object* obj) { PROF

	glBindVertexArray(obj->vao);

	glBindBuffer(gl_buf_target::array, obj->vbos[0]);
	glVertexAttribPointer(0, 2, gl_vert_attrib_type::_float, gl_bool::_false, sizeof(v2), (void*)0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(gl_buf_target::array, obj->vbos[1]);
	glVertexAttribPointer(1, 2, gl_vert_attrib_type::_float, gl_bool::_false, sizeof(v2), (void*)0);
	glEnableVertexAttribArray(1);
	
	glBindBuffer(gl_buf_target::element_array, obj->vbos[2]);

	glBindVertexArray(0);
}

CALLBACK void setup_mesh_2d_tex_col(gpu_object* obj) { PROF

	glBindVertexArray(obj->vao);

	glBindBuffer(gl_buf_target::array, obj->vbos[0]);
	glVertexAttribPointer(0, 2, gl_vert_attrib_type::_float, gl_bool::_false, sizeof(v2), (void*)0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(gl_buf_target::array, obj->vbos[1]);
	glVertexAttribPointer(1, 2, gl_vert_attrib_type::_float, gl_bool::_false, sizeof(v2), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(gl_buf_target::array, obj->vbos[2]);
	glVertexAttribPointer(2, 4, gl_vert_attrib_type::_float, gl_bool::_false, sizeof(v4), (void*)0);
	glEnableVertexAttribArray(2);

	glBindBuffer(gl_buf_target::element_array, obj->vbos[3]);

	glBindVertexArray(0);
}

CALLBACK void setup_mesh_3d_tex(gpu_object* obj) { PROF

	glBindVertexArray(obj->vao);

	glBindBuffer(gl_buf_target::array, obj->vbos[0]);
	glVertexAttribPointer(0, 3, gl_vert_attrib_type::_float, gl_bool::_false, sizeof(v3), (void*)0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(gl_buf_target::array, obj->vbos[1]);
	glVertexAttribPointer(1, 2, gl_vert_attrib_type::_float, gl_bool::_false, sizeof(v2), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(gl_buf_target::element_array, obj->vbos[2]);

	glBindVertexArray(0);
}

CALLBACK void setup_mesh_3d_tex_instanced(gpu_object* obj) { PROF

	glBindVertexArray(obj->vao);

	glBindBuffer(gl_buf_target::array, obj->vbos[0]);
	glVertexAttribPointer(2, 3, gl_vert_attrib_type::_float, gl_bool::_false, sizeof(v3), (void*)0);
	glVertexAttribDivisor(2, 1);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

CALLBACK void setup_mesh_lines(gpu_object* obj) { PROF

	glBindVertexArray(obj->vao);

	glBindBuffer(gl_buf_target::array, obj->vbos[0]);
	glVertexAttribPointer(0, 3, gl_vert_attrib_type::_float, gl_bool::_false, sizeof(v3), (void*)0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(gl_buf_target::array, obj->vbos[1]);
	glVertexAttribPointer(1, 4, gl_vert_attrib_type::_float, gl_bool::_false, sizeof(colorf), (void*)0);
	glEnableVertexAttribArray(1);
	
	glBindVertexArray(0);
}

void mesh_cubemap::init() {

	dirty = true;
	gpu = eng->ogl.add_object(FPTR(setup_mesh_cubemap), FPTR(update_mesh_cubemap), this);
}

void mesh_cubemap::destroy() {

	eng->ogl.destroy_object(gpu);
}

void mesh_lines::init(allocator* alloc) { PROF

	if(!alloc) alloc = CURRENT_ALLOC();

	vertices = vector<v3>::make(32, alloc);
	colors = vector<colorf>::make(32, alloc);
	gpu = eng->ogl.add_object(FPTR(setup_mesh_lines), FPTR(update_mesh_lines), this);
}

void mesh_lines::push(v3 p1, v3 p2, colorf c1, colorf c2) { PROF

	vertices.push(p1);
	vertices.push(p2);
	colors.push(c1);
	colors.push(c2);

	dirty = true;
}

void mesh_lines::destroy() { PROF

	vertices.destroy();
	colors.destroy();

	eng->ogl.destroy_object(gpu);
	gpu = -1;
}

void mesh_3d_tex_instance_data::init(mesh_3d_tex* par, u32 i, allocator* alloc) { PROF

	if(alloc == null) {
		alloc = CURRENT_ALLOC();
	}

	parent = par;
	data = vector<v3>::make(i, alloc);
	gpu = eng->ogl.add_object(FPTR(setup_mesh_3d_tex_instanced), FPTR(update_mesh_3d_tex_instanced), this);
}

void mesh_3d_tex_instance_data::destroy() { PROF

	data.destroy();

	eng->ogl.destroy_object(gpu);
	gpu = -1;
}

void mesh_3d_tex_instance_data::clear() { PROF

	data.clear();
	instances = 0;
	dirty = true;
}

bool mesh_3d_tex_instance_data::empty() { PROF

	return !data.size;
}

void mesh_2d_col::init(allocator* alloc) { PROF

	if(alloc == null) {
		alloc = CURRENT_ALLOC();
	}

	vertices = vector<v2>::make(1024, alloc);
	colors 	  =	vector<colorf>::make(1024, alloc);
	elements  = vector<uv3>::make(1024, alloc); 
	gpu = eng->ogl.add_object(FPTR(setup_mesh_2d_col), FPTR(update_mesh_2d_col), this);
}

void mesh_2d_col::destroy() { PROF

	vertices.destroy();
	colors.destroy();
	elements.destroy();

	eng->ogl.destroy_object(gpu);
	gpu = -1;
}

void mesh_2d_col::clear() { PROF
	vertices.clear();
	colors.clear();
	elements.clear();
	dirty = true;
}

bool mesh_2d_col::empty() { PROF

	return !vertices.size;
}

void mesh_2d_col::push_tri(v2 p1, v2 p2, v2 p3, color c) { PROF

	u32 idx = vertices.size;
	
	vertices.push(p1);
	vertices.push(p2);
	vertices.push(p3);

	DO(3) colors.push(c.to_f());

	elements.push(uv3(idx, idx + 1, idx + 2));

	dirty = true;
}

void mesh_2d_col::push_cutrect(r2 r, f32 round, color c) { PROF

	u32 idx = vertices.size;

	vertices.push(v2(r.x, r.y + round));
	vertices.push(v2(r.x, r.y + r.h - round));
	vertices.push(v2(r.x + round, r.y + r.h));
	vertices.push(v2(r.x + r.w - round, r.y + r.h));
	vertices.push(v2(r.x + r.w, r.y + r.h - round));
	vertices.push(v2(r.x + r.w, r.y + round));
	vertices.push(v2(r.x + r.w - round, r.y));
	vertices.push(v2(r.x + round, r.y));

	colorf cf = c.to_f();
	DO(8) colors.push(cf);

	elements.push(uv3(idx, idx + 1, idx + 2));
	elements.push(uv3(idx, idx + 2, idx + 7));
	elements.push(uv3(idx + 7, idx + 2, idx + 3));
	elements.push(uv3(idx + 7, idx + 6, idx + 3));
	elements.push(uv3(idx + 3, idx + 4, idx + 5));
	elements.push(uv3(idx + 3, idx + 5, idx + 6));

	dirty = true;
}

void mesh_2d_col::push_rect(r2 r, color c) { PROF

	u32 idx = vertices.size;

	vertices.push(v2(r.x, r.y + r.h));	// BLC
	vertices.push(r.xy);				// TLC
	vertices.push(r.xy + r.wh);		 	// BRC
	vertices.push(v2(r.x + r.w, r.y));	// TRC

	DO(4) colors.push(c.to_f());

	elements.push(uv3(idx, idx + 1, idx + 2));
	elements.push(uv3(idx + 1, idx + 2, idx + 3));

	dirty = true;
}


void mesh_2d_tex::init(allocator* alloc) { PROF

	if(alloc == null) {
		alloc = CURRENT_ALLOC();
	}

	vertices = vector<v2>::make(1024, alloc);
	texCoords =	vector<v2>::make(1024, alloc);
	elements  = vector<uv3>::make(1024, alloc); 
	gpu = eng->ogl.add_object(FPTR(setup_mesh_2d_tex), FPTR(update_mesh_2d_tex), this);
}

void mesh_2d_tex::destroy() { PROF

	vertices.destroy();
	texCoords.destroy();
	elements.destroy();

	eng->ogl.destroy_object(gpu);
	gpu = -1;
}

void mesh_2d_tex::clear() { PROF
	vertices.clear();
	texCoords.clear();
	elements.clear();

	dirty = true;
}

bool mesh_2d_tex::empty() { PROF

	return !vertices.size;
}

void mesh_2d_tex_col::init(allocator* alloc) { PROF

	if(alloc == null) {
		alloc = CURRENT_ALLOC();
	}

	vertices  = vector<v2>::make(1024, alloc);
	texCoords =	vector<v2>::make(1024, alloc);
	colors 	  = vector<colorf>::make(1024, alloc);
	elements  = vector<uv3>::make(1024, alloc); 
	gpu = eng->ogl.add_object(FPTR(setup_mesh_2d_tex_col), FPTR(update_mesh_2d_tex_col), this);
}

void mesh_2d_tex_col::destroy() { PROF

	vertices.destroy();
	texCoords.destroy();
	colors.destroy();
	elements.destroy();

	eng->ogl.destroy_object(gpu);
	gpu = -1;
}

void mesh_2d_tex_col::clear() { PROF
	vertices.clear();
	texCoords.clear();
	elements.clear();
	colors.clear();
}

bool mesh_2d_tex_col::empty() { PROF

	return !vertices.size;
}

void mesh_3d_tex::init(allocator* alloc) { PROF

	if(alloc == null) {
		alloc = CURRENT_ALLOC();
	}

	vertices = vector<v3>::make(1024, alloc);
	texCoords = vector<v2>::make(1024, alloc);
	elements  = vector<uv3>::make(1024, alloc);
	gpu = eng->ogl.add_object(FPTR(setup_mesh_3d_tex), FPTR(update_mesh_3d_tex), this);
}

void mesh_3d_tex::destroy() { PROF

	vertices.destroy();
	texCoords.destroy();
	elements.destroy();

	eng->ogl.destroy_object(gpu);
	gpu = -1;
}

void mesh_3d_tex::clear() { PROF
	vertices.clear();
	texCoords.clear();
	elements.clear();
}

bool mesh_3d_tex::empty() { PROF

	return !vertices.size;
}

f32 mesh_2d_tex_col::push_text_line(asset* font, string text_utf8, v2 pos, f32 point, color c) { PROF

	colorf cf = c.to_f();
	f32 x = pos.x;
	f32 y = pos.y;
	f32 scale = point / font->raster_font.point;
	if(point == 0.0f) {
		scale = 1.0f;
	}
	y += scale * font->raster_font.linedist;

	u32 index = 0;
	while(u32 codepoint = text_utf8.get_next_codepoint(&index)) {

		u32 idx = vertices.size;
		glyph_data glyph = font->raster_font.get_glyph(codepoint);

		f32 w = (f32)font->raster_font.width;
		f32 h = (f32)font->raster_font.height;
		v2 tlc (glyph.x1/w, 1.0f - glyph.y1/h);
		v2 brc (glyph.x2/w, 1.0f - glyph.y2/h);
		v2 trc (glyph.x2/w, 1.0f - glyph.y1/h);
		v2 blc (glyph.x1/w, 1.0f - glyph.y2/h);

		vertices.push(v2(x + scale*glyph.xoff1, y + scale*glyph.yoff2)); 	// BLC
 		vertices.push(v2(x + scale*glyph.xoff1, y + scale*glyph.yoff1));	// TLC
 		vertices.push(v2(x + scale*glyph.xoff2, y + scale*glyph.yoff2));	// BRC
 		vertices.push(v2(x + scale*glyph.xoff2, y + scale*glyph.yoff1));	// TRC

		texCoords.push(blc);
		texCoords.push(tlc);
		texCoords.push(brc);
		texCoords.push(trc);

		DO(4) colors.push(cf);

		elements.push(uv3(idx, idx + 1, idx + 2));
		elements.push(uv3(idx + 1, idx + 2, idx + 3));

		x += scale * glyph.advance;
	}

	dirty = true;
	return scale * font->raster_font.linedist;
}

void mesh_3d_tex::push_cube(v3 pos, f32 len) {

	u32 idx = vertices.size;

	f32 len2 = len / 2.0f;
	vertices.push(pos + v3( len2,  len2,  len2));
	vertices.push(pos + v3(-len2,  len2,  len2));
	vertices.push(pos + v3( len2, -len2,  len2));
	vertices.push(pos + v3( len2,  len2, -len2));
	vertices.push(pos + v3(-len2, -len2,  len2));
	vertices.push(pos + v3( len2, -len2, -len2));
	vertices.push(pos + v3(-len2,  len2, -len2));
	vertices.push(pos + v3(-len2, -len2, -len2));

	texCoords.push(v2(0.0f, 0.0f));
	texCoords.push(v2(1.0f, 0.0f));
	texCoords.push(v2(0.0f, 1.0f));
	texCoords.push(v2(0.0f, 0.0f));
	texCoords.push(v2(1.0f, 0.0f));
	texCoords.push(v2(0.0f, 1.0f));
	texCoords.push(v2(1.0f, 0.0f));
	texCoords.push(v2(1.0f, 1.0f));	

	elements.push(uv3(idx + 0, idx + 3, idx + 6));
	elements.push(uv3(idx + 0, idx + 3, idx + 5));
	elements.push(uv3(idx + 0, idx + 1, idx + 6));
	elements.push(uv3(idx + 1, idx + 4, idx + 7));
	elements.push(uv3(idx + 1, idx + 6, idx + 7));
	elements.push(uv3(idx + 4, idx + 2, idx + 5));
	elements.push(uv3(idx + 4, idx + 7, idx + 5));
	elements.push(uv3(idx + 7, idx + 5, idx + 3));
	elements.push(uv3(idx + 7, idx + 6, idx + 3));
	elements.push(uv3(idx + 0, idx + 2, idx + 4));
	elements.push(uv3(idx + 0, idx + 1, idx + 4));

	dirty = true;
}

chunk_vertex chunk_vertex::from_vec(v3 v, v3 uv, bv4 ao) { PROF

	LOG_DEBUG_ASSERT(v.x >= 0 && v.x < 256);
	LOG_DEBUG_ASSERT(v.y >= 0 && v.y < 4096);
	LOG_DEBUG_ASSERT(v.z >= 0 && v.z < 256);
	LOG_DEBUG_ASSERT(uv.x >= 0 && uv.x < 256);
	LOG_DEBUG_ASSERT(uv.y >= 0 && uv.y < 256);
	LOG_DEBUG_ASSERT(uv.z >= 0 && uv.z < 1024);

	chunk_vertex ret;
	ret.x = (u8)v.x;
	ret.z = (u8)v.z;
	ret.y_ao |= (u16)v.y << 4;
	ret.y_ao |= ao.x << 2;
	ret.y_ao |= ao.y;

	ret.u = (u8)uv.x;
	ret.v = (u8)uv.y;
	ret.ao_t = (u16)uv.z;
	ret.ao_t |= (u16)ao.z << 14;
	ret.ao_t |= (u16)ao.w << 12;

	return ret;
}

mesh_chunk mesh_chunk::make_cpu(u32 verts, allocator* alloc) { PROF

	if(alloc == null) {
		alloc = CURRENT_ALLOC();
	}

	mesh_chunk ret;

	ret.vertices = vector<chunk_vertex>::make(verts, alloc);
	ret.elements = vector<uv3>::make(verts, alloc);

	return ret;
}

void mesh_chunk::init_gpu() { PROF

	gpu = eng->ogl.add_object(FPTR(setup_mesh_chunk), FPTR(update_mesh_chunk), this);
}

void mesh_chunk::swap_mesh(mesh_chunk other) { PROF

	vertices.destroy();
	elements.destroy();

	vertices = other.vertices;
	elements = other.elements;

	dirty = true;
}

void mesh_chunk::destroy() { PROF

	vertices.destroy();
	elements.destroy();

	eng->ogl.destroy_object(gpu);
	gpu = -1;
}

void mesh_chunk::free_cpu() { PROF

	vertices.resize(0);
	elements.resize(0);
}

void mesh_chunk::clear() { PROF

	vertices.clear();
	elements.clear();

	dirty = true;
}

void mesh_chunk::quad(v3 p1, v3 p2, v3 p3, v3 p4, v3 uv_ext, bv4 ao) { PROF

	u32 idx = vertices.size;

	vertices.push(chunk_vertex::from_vec(p1 * (f32)chunk::units_per_voxel, v3(0.0f, 0.0f, uv_ext.z), ao));
	vertices.push(chunk_vertex::from_vec(p2 * (f32)chunk::units_per_voxel, v3(uv_ext.x, 0.0f, uv_ext.z), ao));
	vertices.push(chunk_vertex::from_vec(p3 * (f32)chunk::units_per_voxel, v3(0.0f, uv_ext.y, uv_ext.z), ao));
	vertices.push(chunk_vertex::from_vec(p4 * (f32)chunk::units_per_voxel, v3(uv_ext.x, uv_ext.y, uv_ext.z), ao));

	elements.push(uv3(idx, idx + 1, idx + 2));
	elements.push(uv3(idx + 3, idx + 2, idx + 1));

	dirty = true;
}

void mesh_chunk::cube(v3 pos, f32 len) { PROF

	u32 idx = vertices.size;

	f32 len2 = len / 2.0f;
	pos += {len2, len2, len2};

	vertices.push(chunk_vertex::from_vec(pos + v3( len2,  len2,  len2), v3(0,0,0), bv4()));
	vertices.push(chunk_vertex::from_vec(pos + v3(-len2,  len2,  len2), v3(1,0,0), bv4()));
	vertices.push(chunk_vertex::from_vec(pos + v3( len2, -len2,  len2), v3(0,1,0), bv4()));
	vertices.push(chunk_vertex::from_vec(pos + v3( len2,  len2, -len2), v3(0,0,0), bv4()));
	vertices.push(chunk_vertex::from_vec(pos + v3(-len2, -len2,  len2), v3(1,0,0), bv4()));
	vertices.push(chunk_vertex::from_vec(pos + v3( len2, -len2, -len2), v3(0,1,0), bv4()));
	vertices.push(chunk_vertex::from_vec(pos + v3(-len2,  len2, -len2), v3(1,0,0), bv4()));
	vertices.push(chunk_vertex::from_vec(pos + v3(-len2, -len2, -len2), v3(1,1,0), bv4()));

	elements.push(uv3(idx + 0, idx + 3, idx + 5));
	elements.push(uv3(idx + 0, idx + 3, idx + 6));
	elements.push(uv3(idx + 0, idx + 1, idx + 6));
	elements.push(uv3(idx + 1, idx + 4, idx + 7));
	elements.push(uv3(idx + 1, idx + 6, idx + 7));
	elements.push(uv3(idx + 4, idx + 2, idx + 5));
	elements.push(uv3(idx + 4, idx + 7, idx + 5));
	elements.push(uv3(idx + 7, idx + 5, idx + 3));
	elements.push(uv3(idx + 7, idx + 6, idx + 3));
	elements.push(uv3(idx + 0, idx + 2, idx + 4));
	elements.push(uv3(idx + 0, idx + 1, idx + 4));

	dirty = true;
}