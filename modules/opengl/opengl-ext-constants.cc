/*
  opengl-ext.cc
  
  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <qore/Qore.h>

#include "qore-opengl.h"

void addOpenGLExtConstants()
{
#if GL_ARB_imaging == 1
   opengl_ns.addConstant("GL_ARB_imaging",                             &True);
#else
   opengl_ns.addConstant("GL_ARB_imaging",                             &False);
#endif
#if GL_ARB_transpose_matrix == 1
   opengl_ns.addConstant("GL_ARB_transpose_matrix",                    &True);
#else
   opengl_ns.addConstant("GL_ARB_transpose_matrix",                    &False);
#endif
#if GL_ARB_multitexture == 1
   opengl_ns.addConstant("GL_ARB_multitexture",                        &True);
#else
   opengl_ns.addConstant("GL_ARB_multitexture",                        &False);
#endif
#if GL_ARB_texture_env_add == 1
   opengl_ns.addConstant("GL_ARB_texture_env_add",                     &True);
#else
   opengl_ns.addConstant("GL_ARB_texture_env_add",                     &False);
#endif
#if GL_ARB_texture_env_combine == 1
   opengl_ns.addConstant("GL_ARB_texture_env_combine",                 &True);
#else
   opengl_ns.addConstant("GL_ARB_texture_env_combine",                 &False);
#endif
#if GL_ARB_texture_env_dot3 == 1
   opengl_ns.addConstant("GL_ARB_texture_env_dot3",                    &True);
#else
   opengl_ns.addConstant("GL_ARB_texture_env_dot3",                    &False);
#endif
#if GL_ARB_texture_env_crossbar == 1
   opengl_ns.addConstant("GL_ARB_texture_env_crossbar",                &True);
#else
   opengl_ns.addConstant("GL_ARB_texture_env_crossbar",                &False);
#endif
#if GL_ARB_texture_cube_map == 1
   opengl_ns.addConstant("GL_ARB_texture_cube_map",                    &True);
#else
   opengl_ns.addConstant("GL_ARB_texture_cube_map",                    &False);
#endif
#if GL_ARB_texture_compression == 1
   opengl_ns.addConstant("GL_ARB_texture_compression",                 &True);
#else
   opengl_ns.addConstant("GL_ARB_texture_compression",                 &False);
#endif
#if GL_ARB_multisample == 1
   opengl_ns.addConstant("GL_ARB_multisample",                         &True);
#else
   opengl_ns.addConstant("GL_ARB_multisample",                         &False);
#endif
#if GL_ARB_texture_border_clamp == 1
   opengl_ns.addConstant("GL_ARB_texture_border_clamp",                &True);
#else
   opengl_ns.addConstant("GL_ARB_texture_border_clamp",                &False);
#endif
#if GL_ARB_point_parameters == 1
   opengl_ns.addConstant("GL_ARB_point_parameters",                    &True);
#else
   opengl_ns.addConstant("GL_ARB_point_parameters",                    &False);
#endif
#if GL_ARB_vertex_program == 1
   opengl_ns.addConstant("GL_ARB_vertex_program",                      &True);
#else
   opengl_ns.addConstant("GL_ARB_vertex_program",                      &False);
#endif
#if GL_ARB_fragment_program == 1
   opengl_ns.addConstant("GL_ARB_fragment_program",                    &True);
#else
   opengl_ns.addConstant("GL_ARB_fragment_program",                    &False);
#endif
#if GL_ARB_fragment_program_shadow == 1
   opengl_ns.addConstant("GL_ARB_fragment_program_shadow",             &True);
#else
   opengl_ns.addConstant("GL_ARB_fragment_program_shadow",             &False);
#endif
#if GL_ARB_texture_mirrored_repeat == 1
   opengl_ns.addConstant("GL_ARB_texture_mirrored_repeat",             &True);
#else
   opengl_ns.addConstant("GL_ARB_texture_mirrored_repeat",             &False);
#endif
#if GL_ARB_depth_texture == 1
   opengl_ns.addConstant("GL_ARB_depth_texture",                       &True);
#else
   opengl_ns.addConstant("GL_ARB_depth_texture",                       &False);
#endif
#if GL_ARB_shadow == 1
   opengl_ns.addConstant("GL_ARB_shadow",                              &True);
#else
   opengl_ns.addConstant("GL_ARB_shadow",                              &False);
#endif
#if GL_ARB_shadow_ambient == 1
   opengl_ns.addConstant("GL_ARB_shadow_ambient",                      &True);
#else
   opengl_ns.addConstant("GL_ARB_shadow_ambient",                      &False);
#endif
#if GL_ARB_vertex_blend == 1
   opengl_ns.addConstant("GL_ARB_vertex_blend",                        &True);
#else
   opengl_ns.addConstant("GL_ARB_vertex_blend",                        &False);
#endif
#if GL_ARB_window_pos == 1
   opengl_ns.addConstant("GL_ARB_window_pos",                          &True);
#else
   opengl_ns.addConstant("GL_ARB_window_pos",                          &False);
#endif
#if GL_ARB_occlusion_query == 1
   opengl_ns.addConstant("GL_ARB_occlusion_query",                     &True);
#else
   opengl_ns.addConstant("GL_ARB_occlusion_query",                     &False);
#endif
#if GL_ARB_shader_objects == 1
   opengl_ns.addConstant("GL_ARB_shader_objects",                      &True);
#else
   opengl_ns.addConstant("GL_ARB_shader_objects",                      &False);
#endif
#if GL_ARB_vertex_shader == 1
   opengl_ns.addConstant("GL_ARB_vertex_shader",                       &True);
#else
   opengl_ns.addConstant("GL_ARB_vertex_shader",                       &False);
#endif
#if GL_ARB_fragment_shader == 1
   opengl_ns.addConstant("GL_ARB_fragment_shader",                     &True);
#else
   opengl_ns.addConstant("GL_ARB_fragment_shader",                     &False);
#endif
#if GL_ARB_shading_language_100 == 1
   opengl_ns.addConstant("GL_ARB_shading_language_100",                &True);
#else
   opengl_ns.addConstant("GL_ARB_shading_language_100",                &False);
#endif
#if GL_ARB_vertex_buffer_object == 1
   opengl_ns.addConstant("GL_ARB_vertex_buffer_object",                &True);
#else
   opengl_ns.addConstant("GL_ARB_vertex_buffer_object",                &False);
#endif
#if GL_ARB_point_sprite == 1
   opengl_ns.addConstant("GL_ARB_point_sprite",                        &True);
#else
   opengl_ns.addConstant("GL_ARB_point_sprite",                        &False);
#endif
#if GL_ARB_texture_non_power_of_two == 1
   opengl_ns.addConstant("GL_ARB_texture_non_power_of_two",            &True);
#else
   opengl_ns.addConstant("GL_ARB_texture_non_power_of_two",            &False);
#endif
#if GL_ARB_texture_rectangle == 1
   opengl_ns.addConstant("GL_ARB_texture_rectangle",                   &True);
#else
   opengl_ns.addConstant("GL_ARB_texture_rectangle",                   &False);
#endif
#if GL_ARB_draw_buffers == 1
   opengl_ns.addConstant("GL_ARB_draw_buffers",                        &True);
#else
   opengl_ns.addConstant("GL_ARB_draw_buffers",                        &False);
#endif
#if GL_ARB_pixel_buffer_object == 1
   opengl_ns.addConstant("GL_ARB_pixel_buffer_object",                 &True);
#else
   opengl_ns.addConstant("GL_ARB_pixel_buffer_object",                 &False);
#endif
#if GL_ARB_shader_texture_lod == 1
   opengl_ns.addConstant("GL_ARB_shader_texture_lod",                  &True);
#else
   opengl_ns.addConstant("GL_ARB_shader_texture_lod",                  &False);
#endif
#if GL_ARB_texture_float == 1
   opengl_ns.addConstant("GL_ARB_texture_float",                       &True);
#else
   opengl_ns.addConstant("GL_ARB_texture_float",                       &False);
#endif
#if GL_ARB_half_float_pixel == 1
   opengl_ns.addConstant("GL_ARB_half_float_pixel",                    &True);
#else
   opengl_ns.addConstant("GL_ARB_half_float_pixel",                    &False);
#endif
#if GL_EXT_clip_volume_hint == 1
   opengl_ns.addConstant("GL_EXT_clip_volume_hint",                    &True);
#else
   opengl_ns.addConstant("GL_EXT_clip_volume_hint",                    &False);
#endif
#if GL_EXT_rescale_normal == 1
   opengl_ns.addConstant("GL_EXT_rescale_normal",                      &True);
#else
   opengl_ns.addConstant("GL_EXT_rescale_normal",                      &False);
#endif
#if GL_EXT_blend_color == 1
   opengl_ns.addConstant("GL_EXT_blend_color",                         &True);
#else
   opengl_ns.addConstant("GL_EXT_blend_color",                         &False);
#endif
#if GL_EXT_blend_minmax == 1
   opengl_ns.addConstant("GL_EXT_blend_minmax",                        &True);
#else
   opengl_ns.addConstant("GL_EXT_blend_minmax",                        &False);
#endif
#if GL_EXT_blend_subtract == 1
   opengl_ns.addConstant("GL_EXT_blend_subtract",                      &True);
#else
   opengl_ns.addConstant("GL_EXT_blend_subtract",                      &False);
#endif
#if GL_EXT_compiled_vertex_array == 1
   opengl_ns.addConstant("GL_EXT_compiled_vertex_array",               &True);
#else
   opengl_ns.addConstant("GL_EXT_compiled_vertex_array",               &False);
#endif
#if GL_EXT_texture_lod_bias == 1
   opengl_ns.addConstant("GL_EXT_texture_lod_bias",                    &True);
#else
   opengl_ns.addConstant("GL_EXT_texture_lod_bias",                    &False);
#endif
#if GL_EXT_texture_env_add == 1
   opengl_ns.addConstant("GL_EXT_texture_env_add",                     &True);
#else
   opengl_ns.addConstant("GL_EXT_texture_env_add",                     &False);
#endif
#if GL_EXT_abgr == 1
   opengl_ns.addConstant("GL_EXT_abgr",                                &True);
#else
   opengl_ns.addConstant("GL_EXT_abgr",                                &False);
#endif
#if GL_EXT_bgra == 1
   opengl_ns.addConstant("GL_EXT_bgra",                                &True);
#else
   opengl_ns.addConstant("GL_EXT_bgra",                                &False);
#endif
#if GL_EXT_texture_filter_anisotropic == 1
   opengl_ns.addConstant("GL_EXT_texture_filter_anisotropic",          &True);
#else
   opengl_ns.addConstant("GL_EXT_texture_filter_anisotropic",          &False);
#endif
#if GL_EXT_paletted_texture == 1
   opengl_ns.addConstant("GL_EXT_paletted_texture",                    &True);
#else
   opengl_ns.addConstant("GL_EXT_paletted_texture",                    &False);
#endif
#if GL_EXT_secondary_color == 1
   opengl_ns.addConstant("GL_EXT_secondary_color",                     &True);
#else
   opengl_ns.addConstant("GL_EXT_secondary_color",                     &False);
#endif
#if GL_EXT_separate_specular_color == 1
   opengl_ns.addConstant("GL_EXT_separate_specular_color",             &True);
#else
   opengl_ns.addConstant("GL_EXT_separate_specular_color",             &False);
#endif
#if GL_EXT_texture_compression_s3tc == 1
   opengl_ns.addConstant("GL_EXT_texture_compression_s3tc",            &True);
#else
   opengl_ns.addConstant("GL_EXT_texture_compression_s3tc",            &False);
#endif
#if GL_EXT_texture_rectangle == 1
   opengl_ns.addConstant("GL_EXT_texture_rectangle",                   &True);
#else
   opengl_ns.addConstant("GL_EXT_texture_rectangle",                   &False);
#endif
#if GL_EXT_fog_coord == 1
   opengl_ns.addConstant("GL_EXT_fog_coord",                           &True);
#else
   opengl_ns.addConstant("GL_EXT_fog_coord",                           &False);
#endif
#if GL_EXT_draw_range_elements == 1
   opengl_ns.addConstant("GL_EXT_draw_range_elements",                 &True);
#else
   opengl_ns.addConstant("GL_EXT_draw_range_elements",                 &False);
#endif
#if GL_EXT_stencil_wrap == 1
   opengl_ns.addConstant("GL_EXT_stencil_wrap",                        &True);
#else
   opengl_ns.addConstant("GL_EXT_stencil_wrap",                        &False);
#endif
#if GL_EXT_blend_func_separate == 1
   opengl_ns.addConstant("GL_EXT_blend_func_separate",                 &True);
#else
   opengl_ns.addConstant("GL_EXT_blend_func_separate",                 &False);
#endif
#if GL_EXT_multi_draw_arrays == 1
   opengl_ns.addConstant("GL_EXT_multi_draw_arrays",                   &True);
#else
   opengl_ns.addConstant("GL_EXT_multi_draw_arrays",                   &False);
#endif
#if GL_EXT_shadow_funcs == 1
   opengl_ns.addConstant("GL_EXT_shadow_funcs",                        &True);
#else
   opengl_ns.addConstant("GL_EXT_shadow_funcs",                        &False);
#endif
#if GL_EXT_stencil_two_side == 1
   opengl_ns.addConstant("GL_EXT_stencil_two_side",                    &True);
#else
   opengl_ns.addConstant("GL_EXT_stencil_two_side",                    &False);
#endif
#if GL_EXT_depth_bounds_test == 1
   opengl_ns.addConstant("GL_EXT_depth_bounds_test",                   &True);
#else
   opengl_ns.addConstant("GL_EXT_depth_bounds_test",                   &False);
#endif
#if GL_EXT_blend_equation_separate == 1
   opengl_ns.addConstant("GL_EXT_blend_equation_separate",             &True);
#else
   opengl_ns.addConstant("GL_EXT_blend_equation_separate",             &False);
#endif
#if GL_EXT_texture_mirror_clamp == 1
   opengl_ns.addConstant("GL_EXT_texture_mirror_clamp",                &True);
#else
   opengl_ns.addConstant("GL_EXT_texture_mirror_clamp",                &False);
#endif
#if GL_EXT_texture_compression_dxt1 == 1
   opengl_ns.addConstant("GL_EXT_texture_compression_dxt1",            &True);
#else
   opengl_ns.addConstant("GL_EXT_texture_compression_dxt1",            &False);
#endif
   opengl_ns.addConstant("GL_EXT_texture_sRGB",                        new QoreBigIntNode(1));
#if GL_EXT_framebuffer_object == 1
   opengl_ns.addConstant("GL_EXT_framebuffer_object",                  &True);
#else
   opengl_ns.addConstant("GL_EXT_framebuffer_object",                  &False);
#endif
#if GL_EXT_framebuffer_blit == 1
   opengl_ns.addConstant("GL_EXT_framebuffer_blit",                    &True);
#else
   opengl_ns.addConstant("GL_EXT_framebuffer_blit",                    &False);
#endif
#if GL_EXT_framebuffer_multisample == 1
   opengl_ns.addConstant("GL_EXT_framebuffer_multisample",             &True);
#else
   opengl_ns.addConstant("GL_EXT_framebuffer_multisample",             &False);
#endif
#if GL_EXT_packed_depth_stencil == 1
   opengl_ns.addConstant("GL_EXT_packed_depth_stencil",                &True);
#else
   opengl_ns.addConstant("GL_EXT_packed_depth_stencil",                &False);
#endif
#if GL_EXT_gpu_program_parameters == 1
   opengl_ns.addConstant("GL_EXT_gpu_program_parameters",              &True);
#else
   opengl_ns.addConstant("GL_EXT_gpu_program_parameters",              &False);
#endif
#if GL_EXT_geometry_shader4 == 1
   opengl_ns.addConstant("GL_EXT_geometry_shader4",                    &True);
#else
   opengl_ns.addConstant("GL_EXT_geometry_shader4",                    &False);
#endif
#if GL_EXT_transform_feedback == 1
   opengl_ns.addConstant("GL_EXT_transform_feedback",                  &True);
#else
   opengl_ns.addConstant("GL_EXT_transform_feedback",                  &False);
#endif
#if GL_EXT_bindable_uniform == 1
   opengl_ns.addConstant("GL_EXT_bindable_uniform",                    &True);
#else
   opengl_ns.addConstant("GL_EXT_bindable_uniform",                    &False);
#endif
#if GL_EXT_texture_integer == 1
   opengl_ns.addConstant("GL_EXT_texture_integer",                     &True);
#else
   opengl_ns.addConstant("GL_EXT_texture_integer",                     &False);
#endif
#if GL_EXT_gpu_shader4 == 1
   opengl_ns.addConstant("GL_EXT_gpu_shader4",                         &True);
#else
   opengl_ns.addConstant("GL_EXT_gpu_shader4",                         &False);
#endif
#if GL_APPLE_flush_buffer_range == 1
   opengl_ns.addConstant("GL_APPLE_flush_buffer_range",                &True);
#else
   opengl_ns.addConstant("GL_APPLE_flush_buffer_range",                &False);
#endif
#if GL_APPLE_specular_vector == 1
   opengl_ns.addConstant("GL_APPLE_specular_vector",                   &True);
#else
   opengl_ns.addConstant("GL_APPLE_specular_vector",                   &False);
#endif
#if GL_APPLE_transform_hint == 1
   opengl_ns.addConstant("GL_APPLE_transform_hint",                    &True);
#else
   opengl_ns.addConstant("GL_APPLE_transform_hint",                    &False);
#endif
#if GL_APPLE_packed_pixels == 1
   opengl_ns.addConstant("GL_APPLE_packed_pixels",                     &True);
#else
   opengl_ns.addConstant("GL_APPLE_packed_pixels",                     &False);
#endif
#if GL_APPLE_client_storage == 1
   opengl_ns.addConstant("GL_APPLE_client_storage",                    &True);
#else
   opengl_ns.addConstant("GL_APPLE_client_storage",                    &False);
#endif
#if GL_APPLE_ycbcr_422 == 1
   opengl_ns.addConstant("GL_APPLE_ycbcr_422",                         &True);
#else
   opengl_ns.addConstant("GL_APPLE_ycbcr_422",                         &False);
#endif
#if GL_APPLE_texture_range == 1
   opengl_ns.addConstant("GL_APPLE_texture_range",                     &True);
#else
   opengl_ns.addConstant("GL_APPLE_texture_range",                     &False);
#endif
#if GL_APPLE_fence == 1
   opengl_ns.addConstant("GL_APPLE_fence",                             &True);
#else
   opengl_ns.addConstant("GL_APPLE_fence",                             &False);
#endif
#if GL_APPLE_vertex_array_range == 1
   opengl_ns.addConstant("GL_APPLE_vertex_array_range",                &True);
#else
   opengl_ns.addConstant("GL_APPLE_vertex_array_range",                &False);
#endif
#if GL_APPLE_vertex_array_object == 1
   opengl_ns.addConstant("GL_APPLE_vertex_array_object",               &True);
#else
   opengl_ns.addConstant("GL_APPLE_vertex_array_object",               &False);
#endif
#if GL_APPLE_element_array == 1
   opengl_ns.addConstant("GL_APPLE_element_array",                     &True);
#else
   opengl_ns.addConstant("GL_APPLE_element_array",                     &False);
#endif
#if GL_APPLE_vertex_program_evaluators == 1
   opengl_ns.addConstant("GL_APPLE_vertex_program_evaluators",         &True);
#else
   opengl_ns.addConstant("GL_APPLE_vertex_program_evaluators",         &False);
#endif
#if GL_APPLE_float_pixels == 1
   opengl_ns.addConstant("GL_APPLE_float_pixels",                      &True);
#else
   opengl_ns.addConstant("GL_APPLE_float_pixels",                      &False);
#endif
#if GL_APPLE_flush_render == 1
   opengl_ns.addConstant("GL_APPLE_flush_render",                      &True);
#else
   opengl_ns.addConstant("GL_APPLE_flush_render",                      &False);
#endif
#if GL_APPLE_pixel_buffer == 1
   opengl_ns.addConstant("GL_APPLE_pixel_buffer",                      &True);
#else
   opengl_ns.addConstant("GL_APPLE_pixel_buffer",                      &False);
#endif
#if GL_APPLE_aux_depth_stencil == 1
   opengl_ns.addConstant("GL_APPLE_aux_depth_stencil",                 &True);
#else
   opengl_ns.addConstant("GL_APPLE_aux_depth_stencil",                 &False);
#endif
#if GL_APPLE_row_bytes == 1
   opengl_ns.addConstant("GL_APPLE_row_bytes",                         &True);
#else
   opengl_ns.addConstant("GL_APPLE_row_bytes",                         &False);
#endif
#if GL_APPLE_object_purgeable == 1
   opengl_ns.addConstant("GL_APPLE_object_purgeable",                  &True);
#else
   opengl_ns.addConstant("GL_APPLE_object_purgeable",                  &False);
#endif
#if GL_ATI_point_cull_mode == 1
   opengl_ns.addConstant("GL_ATI_point_cull_mode",                     &True);
#else
   opengl_ns.addConstant("GL_ATI_point_cull_mode",                     &False);
#endif
#if GL_ATI_texture_mirror_once == 1
   opengl_ns.addConstant("GL_ATI_texture_mirror_once",                 &True);
#else
   opengl_ns.addConstant("GL_ATI_texture_mirror_once",                 &False);
#endif
#if GL_ATI_pn_triangles == 1
   opengl_ns.addConstant("GL_ATI_pn_triangles",                        &True);
#else
   opengl_ns.addConstant("GL_ATI_pn_triangles",                        &False);
#endif
#if GL_ATI_text_fragment_shader == 1
   opengl_ns.addConstant("GL_ATI_text_fragment_shader",                &True);
#else
   opengl_ns.addConstant("GL_ATI_text_fragment_shader",                &False);
#endif
#if GL_ATI_blend_equation_separate == 1
   opengl_ns.addConstant("GL_ATI_blend_equation_separate",             &True);
#else
   opengl_ns.addConstant("GL_ATI_blend_equation_separate",             &False);
#endif
#if GL_ATI_blend_weighted_minmax == 1
   opengl_ns.addConstant("GL_ATI_blend_weighted_minmax",               &True);
#else
   opengl_ns.addConstant("GL_ATI_blend_weighted_minmax",               &False);
#endif
#if GL_ATI_texture_env_combine3 == 1
   opengl_ns.addConstant("GL_ATI_texture_env_combine3",                &True);
#else
   opengl_ns.addConstant("GL_ATI_texture_env_combine3",                &False);
#endif
#if GL_ATI_separate_stencil == 1
   opengl_ns.addConstant("GL_ATI_separate_stencil",                    &True);
#else
   opengl_ns.addConstant("GL_ATI_separate_stencil",                    &False);
#endif
#if GL_ATI_array_rev_comps_in_4_bytes == 1
   opengl_ns.addConstant("GL_ATI_array_rev_comps_in_4_bytes",          &True);
#else
   opengl_ns.addConstant("GL_ATI_array_rev_comps_in_4_bytes",          &False);
#endif
#if GL_ATI_texture_compression_3dc == 1
   opengl_ns.addConstant("GL_ATI_texture_compression_3dc",             &True);
#else
   opengl_ns.addConstant("GL_ATI_texture_compression_3dc",             &False);
#endif
#if GL_ATI_texture_float == 1
   opengl_ns.addConstant("GL_ATI_texture_float",                       &True);
#else
   opengl_ns.addConstant("GL_ATI_texture_float",                       &False);
#endif
#if GL_ATIX_pn_triangles == 1
   opengl_ns.addConstant("GL_ATIX_pn_triangles",                       &True);
#else
   opengl_ns.addConstant("GL_ATIX_pn_triangles",                       &False);
#endif
#if GL_IBM_rasterpos_clip == 1
   opengl_ns.addConstant("GL_IBM_rasterpos_clip",                      &True);
#else
   opengl_ns.addConstant("GL_IBM_rasterpos_clip",                      &False);
#endif
#if GL_NV_point_sprite == 1
   opengl_ns.addConstant("GL_NV_point_sprite",                         &True);
#else
   opengl_ns.addConstant("GL_NV_point_sprite",                         &False);
#endif
#if GL_NV_register_combiners == 1
   opengl_ns.addConstant("GL_NV_register_combiners",                   &True);
#else
   opengl_ns.addConstant("GL_NV_register_combiners",                   &False);
#endif
#if GL_NV_register_combiners2 == 1
   opengl_ns.addConstant("GL_NV_register_combiners2",                  &True);
#else
   opengl_ns.addConstant("GL_NV_register_combiners2",                  &False);
#endif
#if GL_NV_blend_square == 1
   opengl_ns.addConstant("GL_NV_blend_square",                         &True);
#else
   opengl_ns.addConstant("GL_NV_blend_square",                         &False);
#endif
#if GL_NV_fog_distance == 1
   opengl_ns.addConstant("GL_NV_fog_distance",                         &True);
#else
   opengl_ns.addConstant("GL_NV_fog_distance",                         &False);
#endif
#if GL_NV_multisample_filter_hint == 1
   opengl_ns.addConstant("GL_NV_multisample_filter_hint",              &True);
#else
   opengl_ns.addConstant("GL_NV_multisample_filter_hint",              &False);
#endif
#if GL_NV_texgen_reflection == 1
   opengl_ns.addConstant("GL_NV_texgen_reflection",                    &True);
#else
   opengl_ns.addConstant("GL_NV_texgen_reflection",                    &False);
#endif
#if GL_NV_texture_shader == 1
   opengl_ns.addConstant("GL_NV_texture_shader",                       &True);
#else
   opengl_ns.addConstant("GL_NV_texture_shader",                       &False);
#endif
#if GL_NV_texture_shader2 == 1
   opengl_ns.addConstant("GL_NV_texture_shader2",                      &True);
#else
   opengl_ns.addConstant("GL_NV_texture_shader2",                      &False);
#endif
#if GL_NV_texture_shader3 == 1
   opengl_ns.addConstant("GL_NV_texture_shader3",                      &True);
#else
   opengl_ns.addConstant("GL_NV_texture_shader3",                      &False);
#endif
#if GL_NV_depth_clamp == 1
   opengl_ns.addConstant("GL_NV_depth_clamp",                          &True);
#else
   opengl_ns.addConstant("GL_NV_depth_clamp",                          &False);
#endif
#if GL_NV_light_max_exponent == 1
   opengl_ns.addConstant("GL_NV_light_max_exponent",                   &True);
#else
   opengl_ns.addConstant("GL_NV_light_max_exponent",                   &False);
#endif
#if GL_NV_fragment_program_option == 1
   opengl_ns.addConstant("GL_NV_fragment_program_option",              &True);
#else
   opengl_ns.addConstant("GL_NV_fragment_program_option",              &False);
#endif
#if GL_NV_fragment_program2 == 1
   opengl_ns.addConstant("GL_NV_fragment_program2",                    &True);
#else
   opengl_ns.addConstant("GL_NV_fragment_program2",                    &False);
#endif
#if GL_NV_vertex_program2_option == 1
   opengl_ns.addConstant("GL_NV_vertex_program2_option",               &True);
#else
   opengl_ns.addConstant("GL_NV_vertex_program2_option",               &False);
#endif
#if GL_NV_vertex_program3 == 1
   opengl_ns.addConstant("GL_NV_vertex_program3",                      &True);
#else
   opengl_ns.addConstant("GL_NV_vertex_program3",                      &False);
#endif
#if GL_SGI_color_matrix == 1
   opengl_ns.addConstant("GL_SGI_color_matrix",                        &True);
#else
   opengl_ns.addConstant("GL_SGI_color_matrix",                        &False);
#endif
#if GL_SGIS_texture_edge_clamp == 1
   opengl_ns.addConstant("GL_SGIS_texture_edge_clamp",                 &True);
#else
   opengl_ns.addConstant("GL_SGIS_texture_edge_clamp",                 &False);
#endif
#if GL_SGIS_generate_mipmap == 1
   opengl_ns.addConstant("GL_SGIS_generate_mipmap",                    &True);
#else
   opengl_ns.addConstant("GL_SGIS_generate_mipmap",                    &False);
#endif
#if GL_SGIS_texture_lod == 1
   opengl_ns.addConstant("GL_SGIS_texture_lod",                        &True);
#else
   opengl_ns.addConstant("GL_SGIS_texture_lod",                        &False);
#endif
   opengl_ns.addConstant("GL_GLEXT_VERSION",                           new QoreBigIntNode(7));
   opengl_ns.addConstant("GL_TEXTURE0_ARB",                            new QoreBigIntNode(0x84C0));
   opengl_ns.addConstant("GL_TEXTURE1_ARB",                            new QoreBigIntNode(0x84C1));
   opengl_ns.addConstant("GL_TEXTURE2_ARB",                            new QoreBigIntNode(0x84C2));
   opengl_ns.addConstant("GL_TEXTURE3_ARB",                            new QoreBigIntNode(0x84C3));
   opengl_ns.addConstant("GL_TEXTURE4_ARB",                            new QoreBigIntNode(0x84C4));
   opengl_ns.addConstant("GL_TEXTURE5_ARB",                            new QoreBigIntNode(0x84C5));
   opengl_ns.addConstant("GL_TEXTURE6_ARB",                            new QoreBigIntNode(0x84C6));
   opengl_ns.addConstant("GL_TEXTURE7_ARB",                            new QoreBigIntNode(0x84C7));
   opengl_ns.addConstant("GL_TEXTURE8_ARB",                            new QoreBigIntNode(0x84C8));
   opengl_ns.addConstant("GL_TEXTURE9_ARB",                            new QoreBigIntNode(0x84C9));
   opengl_ns.addConstant("GL_TEXTURE10_ARB",                           new QoreBigIntNode(0x84CA));
   opengl_ns.addConstant("GL_TEXTURE11_ARB",                           new QoreBigIntNode(0x84CB));
   opengl_ns.addConstant("GL_TEXTURE12_ARB",                           new QoreBigIntNode(0x84CC));
   opengl_ns.addConstant("GL_TEXTURE13_ARB",                           new QoreBigIntNode(0x84CD));
   opengl_ns.addConstant("GL_TEXTURE14_ARB",                           new QoreBigIntNode(0x84CE));
   opengl_ns.addConstant("GL_TEXTURE15_ARB",                           new QoreBigIntNode(0x84CF));
   opengl_ns.addConstant("GL_TEXTURE16_ARB",                           new QoreBigIntNode(0x84D0));
   opengl_ns.addConstant("GL_TEXTURE17_ARB",                           new QoreBigIntNode(0x84D1));
   opengl_ns.addConstant("GL_TEXTURE18_ARB",                           new QoreBigIntNode(0x84D2));
   opengl_ns.addConstant("GL_TEXTURE19_ARB",                           new QoreBigIntNode(0x84D3));
   opengl_ns.addConstant("GL_TEXTURE20_ARB",                           new QoreBigIntNode(0x84D4));
   opengl_ns.addConstant("GL_TEXTURE21_ARB",                           new QoreBigIntNode(0x84D5));
   opengl_ns.addConstant("GL_TEXTURE22_ARB",                           new QoreBigIntNode(0x84D6));
   opengl_ns.addConstant("GL_TEXTURE23_ARB",                           new QoreBigIntNode(0x84D7));
   opengl_ns.addConstant("GL_TEXTURE24_ARB",                           new QoreBigIntNode(0x84D8));
   opengl_ns.addConstant("GL_TEXTURE25_ARB",                           new QoreBigIntNode(0x84D9));
   opengl_ns.addConstant("GL_TEXTURE26_ARB",                           new QoreBigIntNode(0x84DA));
   opengl_ns.addConstant("GL_TEXTURE27_ARB",                           new QoreBigIntNode(0x84DB));
   opengl_ns.addConstant("GL_TEXTURE28_ARB",                           new QoreBigIntNode(0x84DC));
   opengl_ns.addConstant("GL_TEXTURE29_ARB",                           new QoreBigIntNode(0x84DD));
   opengl_ns.addConstant("GL_TEXTURE30_ARB",                           new QoreBigIntNode(0x84DE));
   opengl_ns.addConstant("GL_TEXTURE31_ARB",                           new QoreBigIntNode(0x84DF));
   opengl_ns.addConstant("GL_ACTIVE_TEXTURE_ARB",                      new QoreBigIntNode(0x84E0));
   opengl_ns.addConstant("GL_CLIENT_ACTIVE_TEXTURE_ARB",               new QoreBigIntNode(0x84E1));
   opengl_ns.addConstant("GL_MAX_TEXTURE_UNITS_ARB",                   new QoreBigIntNode(0x84E2));
   opengl_ns.addConstant("GL_TRANSPOSE_MODELVIEW_MATRIX_ARB",          new QoreBigIntNode(0x84E3));
   opengl_ns.addConstant("GL_TRANSPOSE_PROJECTION_MATRIX_ARB",         new QoreBigIntNode(0x84E4));
   opengl_ns.addConstant("GL_TRANSPOSE_TEXTURE_MATRIX_ARB",            new QoreBigIntNode(0x84E5));
   opengl_ns.addConstant("GL_TRANSPOSE_COLOR_MATRIX_ARB",              new QoreBigIntNode(0x84E6));
   opengl_ns.addConstant("GL_MULTISAMPLE_ARB",                         new QoreBigIntNode(0x809D));
   opengl_ns.addConstant("GL_SAMPLE_ALPHA_TO_COVERAGE_ARB",            new QoreBigIntNode(0x809E));
   opengl_ns.addConstant("GL_SAMPLE_ALPHA_TO_ONE_ARB",                 new QoreBigIntNode(0x809F));
   opengl_ns.addConstant("GL_SAMPLE_COVERAGE_ARB",                     new QoreBigIntNode(0x80A0));
   opengl_ns.addConstant("GL_SAMPLE_BUFFERS_ARB",                      new QoreBigIntNode(0x80A8));
   opengl_ns.addConstant("GL_SAMPLES_ARB",                             new QoreBigIntNode(0x80A9));
   opengl_ns.addConstant("GL_SAMPLE_COVERAGE_VALUE_ARB",               new QoreBigIntNode(0x80AA));
   opengl_ns.addConstant("GL_SAMPLE_COVERAGE_INVERT_ARB",              new QoreBigIntNode(0x80AB));
   opengl_ns.addConstant("GL_MULTISAMPLE_BIT_ARB",                     new QoreBigIntNode(0x20000000));
   opengl_ns.addConstant("GL_NORMAL_MAP_ARB",                          new QoreBigIntNode(0x8511));
   opengl_ns.addConstant("GL_REFLECTION_MAP_ARB",                      new QoreBigIntNode(0x8512));
   opengl_ns.addConstant("GL_TEXTURE_CUBE_MAP_ARB",                    new QoreBigIntNode(0x8513));
   opengl_ns.addConstant("GL_TEXTURE_BINDING_CUBE_MAP_ARB",            new QoreBigIntNode(0x8514));
   opengl_ns.addConstant("GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB",         new QoreBigIntNode(0x8515));
   opengl_ns.addConstant("GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB",         new QoreBigIntNode(0x8516));
   opengl_ns.addConstant("GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB",         new QoreBigIntNode(0x8517));
   opengl_ns.addConstant("GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB",         new QoreBigIntNode(0x8518));
   opengl_ns.addConstant("GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB",         new QoreBigIntNode(0x8519));
   opengl_ns.addConstant("GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB",         new QoreBigIntNode(0x851A));
   opengl_ns.addConstant("GL_PROXY_TEXTURE_CUBE_MAP_ARB",              new QoreBigIntNode(0x851B));
   opengl_ns.addConstant("GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB",           new QoreBigIntNode(0x851C));
   opengl_ns.addConstant("GL_COMPRESSED_ALPHA_ARB",                    new QoreBigIntNode(0x84E9));
   opengl_ns.addConstant("GL_COMPRESSED_LUMINANCE_ARB",                new QoreBigIntNode(0x84EA));
   opengl_ns.addConstant("GL_COMPRESSED_LUMINANCE_ALPHA_ARB",          new QoreBigIntNode(0x84EB));
   opengl_ns.addConstant("GL_COMPRESSED_INTENSITY_ARB",                new QoreBigIntNode(0x84EC));
   opengl_ns.addConstant("GL_COMPRESSED_RGB_ARB",                      new QoreBigIntNode(0x84ED));
   opengl_ns.addConstant("GL_COMPRESSED_RGBA_ARB",                     new QoreBigIntNode(0x84EE));
   opengl_ns.addConstant("GL_TEXTURE_COMPRESSION_HINT_ARB",            new QoreBigIntNode(0x84EF));
   opengl_ns.addConstant("GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB",       new QoreBigIntNode(0x86A0));
   opengl_ns.addConstant("GL_TEXTURE_COMPRESSED_ARB",                  new QoreBigIntNode(0x86A1));
   opengl_ns.addConstant("GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB",      new QoreBigIntNode(0x86A2));
   opengl_ns.addConstant("GL_COMPRESSED_TEXTURE_FORMATS_ARB",          new QoreBigIntNode(0x86A3));
   opengl_ns.addConstant("GL_MAX_VERTEX_UNITS_ARB",                    new QoreBigIntNode(0x86A4));
   opengl_ns.addConstant("GL_ACTIVE_VERTEX_UNITS_ARB",                 new QoreBigIntNode(0x86A5));
   opengl_ns.addConstant("GL_WEIGHT_SUM_UNITY_ARB",                    new QoreBigIntNode(0x86A6));
   opengl_ns.addConstant("GL_VERTEX_BLEND_ARB",                        new QoreBigIntNode(0x86A7));
   opengl_ns.addConstant("GL_CURRENT_WEIGHT_ARB",                      new QoreBigIntNode(0x86A8));
   opengl_ns.addConstant("GL_WEIGHT_ARRAY_TYPE_ARB",                   new QoreBigIntNode(0x86A9));
   opengl_ns.addConstant("GL_WEIGHT_ARRAY_STRIDE_ARB",                 new QoreBigIntNode(0x86AA));
   opengl_ns.addConstant("GL_WEIGHT_ARRAY_SIZE_ARB",                   new QoreBigIntNode(0x86AB));
   opengl_ns.addConstant("GL_WEIGHT_ARRAY_POINTER_ARB",                new QoreBigIntNode(0x86AC));
   opengl_ns.addConstant("GL_WEIGHT_ARRAY_ARB",                        new QoreBigIntNode(0x86AD));
   opengl_ns.addConstant("GL_MODELVIEW0_ARB",                          new QoreBigIntNode(0x1700));
   opengl_ns.addConstant("GL_MODELVIEW1_ARB",                          new QoreBigIntNode(0x850A));
   opengl_ns.addConstant("GL_MODELVIEW2_ARB",                          new QoreBigIntNode(0x8722));
   opengl_ns.addConstant("GL_MODELVIEW3_ARB",                          new QoreBigIntNode(0x8723));
   opengl_ns.addConstant("GL_MODELVIEW4_ARB",                          new QoreBigIntNode(0x8724));
   opengl_ns.addConstant("GL_MODELVIEW5_ARB",                          new QoreBigIntNode(0x8725));
   opengl_ns.addConstant("GL_MODELVIEW6_ARB",                          new QoreBigIntNode(0x8726));
   opengl_ns.addConstant("GL_MODELVIEW7_ARB",                          new QoreBigIntNode(0x8727));
   opengl_ns.addConstant("GL_MODELVIEW8_ARB",                          new QoreBigIntNode(0x8728));
   opengl_ns.addConstant("GL_MODELVIEW9_ARB",                          new QoreBigIntNode(0x8729));
   opengl_ns.addConstant("GL_MODELVIEW10_ARB",                         new QoreBigIntNode(0x872A));
   opengl_ns.addConstant("GL_MODELVIEW11_ARB",                         new QoreBigIntNode(0x872B));
   opengl_ns.addConstant("GL_MODELVIEW12_ARB",                         new QoreBigIntNode(0x872C));
   opengl_ns.addConstant("GL_MODELVIEW13_ARB",                         new QoreBigIntNode(0x872D));
   opengl_ns.addConstant("GL_MODELVIEW14_ARB",                         new QoreBigIntNode(0x872E));
   opengl_ns.addConstant("GL_MODELVIEW15_ARB",                         new QoreBigIntNode(0x872F));
   opengl_ns.addConstant("GL_MODELVIEW16_ARB",                         new QoreBigIntNode(0x8730));
   opengl_ns.addConstant("GL_MODELVIEW17_ARB",                         new QoreBigIntNode(0x8731));
   opengl_ns.addConstant("GL_MODELVIEW18_ARB",                         new QoreBigIntNode(0x8732));
   opengl_ns.addConstant("GL_MODELVIEW19_ARB",                         new QoreBigIntNode(0x8733));
   opengl_ns.addConstant("GL_MODELVIEW20_ARB",                         new QoreBigIntNode(0x8734));
   opengl_ns.addConstant("GL_MODELVIEW21_ARB",                         new QoreBigIntNode(0x8735));
   opengl_ns.addConstant("GL_MODELVIEW22_ARB",                         new QoreBigIntNode(0x8736));
   opengl_ns.addConstant("GL_MODELVIEW23_ARB",                         new QoreBigIntNode(0x8737));
   opengl_ns.addConstant("GL_MODELVIEW24_ARB",                         new QoreBigIntNode(0x8738));
   opengl_ns.addConstant("GL_MODELVIEW25_ARB",                         new QoreBigIntNode(0x8739));
   opengl_ns.addConstant("GL_MODELVIEW26_ARB",                         new QoreBigIntNode(0x873A));
   opengl_ns.addConstant("GL_MODELVIEW27_ARB",                         new QoreBigIntNode(0x873B));
   opengl_ns.addConstant("GL_MODELVIEW28_ARB",                         new QoreBigIntNode(0x873C));
   opengl_ns.addConstant("GL_MODELVIEW29_ARB",                         new QoreBigIntNode(0x873D));
   opengl_ns.addConstant("GL_MODELVIEW30_ARB",                         new QoreBigIntNode(0x873E));
   opengl_ns.addConstant("GL_MODELVIEW31_ARB",                         new QoreBigIntNode(0x873F));
   opengl_ns.addConstant("GL_SAMPLES_PASSED_ARB",                      new QoreBigIntNode(0x8914));
   opengl_ns.addConstant("GL_QUERY_COUNTER_BITS_ARB",                  new QoreBigIntNode(0x8864));
   opengl_ns.addConstant("GL_CURRENT_QUERY_ARB",                       new QoreBigIntNode(0x8865));
   opengl_ns.addConstant("GL_QUERY_RESULT_ARB",                        new QoreBigIntNode(0x8866));
   opengl_ns.addConstant("GL_QUERY_RESULT_AVAILABLE_ARB",              new QoreBigIntNode(0x8867));
   opengl_ns.addConstant("GL_CLAMP_TO_BORDER_ARB",                     new QoreBigIntNode(0x812D));
   opengl_ns.addConstant("GL_DEPTH_COMPONENT16_ARB",                   new QoreBigIntNode(0x81A5));
   opengl_ns.addConstant("GL_DEPTH_COMPONENT24_ARB",                   new QoreBigIntNode(0x81A6));
   opengl_ns.addConstant("GL_DEPTH_COMPONENT32_ARB",                   new QoreBigIntNode(0x81A7));
   opengl_ns.addConstant("GL_TEXTURE_DEPTH_SIZE_ARB",                  new QoreBigIntNode(0x884A));
   opengl_ns.addConstant("GL_DEPTH_TEXTURE_MODE_ARB",                  new QoreBigIntNode(0x884B));
   opengl_ns.addConstant("GL_TEXTURE_COMPARE_MODE_ARB",                new QoreBigIntNode(0x884C));
   opengl_ns.addConstant("GL_TEXTURE_COMPARE_FUNC_ARB",                new QoreBigIntNode(0x884D));
   opengl_ns.addConstant("GL_COMPARE_R_TO_TEXTURE_ARB",                new QoreBigIntNode(0x884E));
   opengl_ns.addConstant("GL_TEXTURE_COMPARE_FAIL_VALUE_ARB",          new QoreBigIntNode(0x80BF));
   opengl_ns.addConstant("GL_COMBINE_ARB",                             new QoreBigIntNode(0x8570));
   opengl_ns.addConstant("GL_COMBINE_RGB_ARB",                         new QoreBigIntNode(0x8571));
   opengl_ns.addConstant("GL_COMBINE_ALPHA_ARB",                       new QoreBigIntNode(0x8572));
   opengl_ns.addConstant("GL_RGB_SCALE_ARB",                           new QoreBigIntNode(0x8573));
   opengl_ns.addConstant("GL_ADD_SIGNED_ARB",                          new QoreBigIntNode(0x8574));
   opengl_ns.addConstant("GL_INTERPOLATE_ARB",                         new QoreBigIntNode(0x8575));
   opengl_ns.addConstant("GL_CONSTANT_ARB",                            new QoreBigIntNode(0x8576));
   opengl_ns.addConstant("GL_PRIMARY_COLOR_ARB",                       new QoreBigIntNode(0x8577));
   opengl_ns.addConstant("GL_PREVIOUS_ARB",                            new QoreBigIntNode(0x8578));
   opengl_ns.addConstant("GL_SUBTRACT_ARB",                            new QoreBigIntNode(0x84E7));
   opengl_ns.addConstant("GL_SOURCE0_RGB_ARB",                         new QoreBigIntNode(0x8580));
   opengl_ns.addConstant("GL_SOURCE1_RGB_ARB",                         new QoreBigIntNode(0x8581));
   opengl_ns.addConstant("GL_SOURCE2_RGB_ARB",                         new QoreBigIntNode(0x8582));
   opengl_ns.addConstant("GL_SOURCE3_RGB_ARB",                         new QoreBigIntNode(0x8583));
   opengl_ns.addConstant("GL_SOURCE4_RGB_ARB",                         new QoreBigIntNode(0x8584));
   opengl_ns.addConstant("GL_SOURCE5_RGB_ARB",                         new QoreBigIntNode(0x8585));
   opengl_ns.addConstant("GL_SOURCE6_RGB_ARB",                         new QoreBigIntNode(0x8586));
   opengl_ns.addConstant("GL_SOURCE7_RGB_ARB",                         new QoreBigIntNode(0x8587));
   opengl_ns.addConstant("GL_SOURCE0_ALPHA_ARB",                       new QoreBigIntNode(0x8588));
   opengl_ns.addConstant("GL_SOURCE1_ALPHA_ARB",                       new QoreBigIntNode(0x8589));
   opengl_ns.addConstant("GL_SOURCE2_ALPHA_ARB",                       new QoreBigIntNode(0x858A));
   opengl_ns.addConstant("GL_SOURCE3_ALPHA_ARB",                       new QoreBigIntNode(0x858B));
   opengl_ns.addConstant("GL_SOURCE4_ALPHA_ARB",                       new QoreBigIntNode(0x858C));
   opengl_ns.addConstant("GL_SOURCE5_ALPHA_ARB",                       new QoreBigIntNode(0x858D));
   opengl_ns.addConstant("GL_SOURCE6_ALPHA_ARB",                       new QoreBigIntNode(0x858E));
   opengl_ns.addConstant("GL_SOURCE7_ALPHA_ARB",                       new QoreBigIntNode(0x858F));
   opengl_ns.addConstant("GL_OPERAND0_RGB_ARB",                        new QoreBigIntNode(0x8590));
   opengl_ns.addConstant("GL_OPERAND1_RGB_ARB",                        new QoreBigIntNode(0x8591));
   opengl_ns.addConstant("GL_OPERAND2_RGB_ARB",                        new QoreBigIntNode(0x8592));
   opengl_ns.addConstant("GL_OPERAND3_RGB_ARB",                        new QoreBigIntNode(0x8593));
   opengl_ns.addConstant("GL_OPERAND4_RGB_ARB",                        new QoreBigIntNode(0x8594));
   opengl_ns.addConstant("GL_OPERAND5_RGB_ARB",                        new QoreBigIntNode(0x8595));
   opengl_ns.addConstant("GL_OPERAND6_RGB_ARB",                        new QoreBigIntNode(0x8596));
   opengl_ns.addConstant("GL_OPERAND7_RGB_ARB",                        new QoreBigIntNode(0x8597));
   opengl_ns.addConstant("GL_OPERAND0_ALPHA_ARB",                      new QoreBigIntNode(0x8598));
   opengl_ns.addConstant("GL_OPERAND1_ALPHA_ARB",                      new QoreBigIntNode(0x8599));
   opengl_ns.addConstant("GL_OPERAND2_ALPHA_ARB",                      new QoreBigIntNode(0x859A));
   opengl_ns.addConstant("GL_OPERAND3_ALPHA_ARB",                      new QoreBigIntNode(0x859B));
   opengl_ns.addConstant("GL_OPERAND4_ALPHA_ARB",                      new QoreBigIntNode(0x859C));
   opengl_ns.addConstant("GL_OPERAND5_ALPHA_ARB",                      new QoreBigIntNode(0x859D));
   opengl_ns.addConstant("GL_OPERAND6_ALPHA_ARB",                      new QoreBigIntNode(0x859E));
   opengl_ns.addConstant("GL_OPERAND7_ALPHA_ARB",                      new QoreBigIntNode(0x859F));
   opengl_ns.addConstant("GL_MIRRORED_REPEAT_ARB",                     new QoreBigIntNode(0x8370));
   opengl_ns.addConstant("GL_DOT3_RGB_ARB",                            new QoreBigIntNode(0x86AE));
   opengl_ns.addConstant("GL_DOT3_RGBA_ARB",                           new QoreBigIntNode(0x86AF));
   opengl_ns.addConstant("GL_POINT_SIZE_MIN_ARB",                      new QoreBigIntNode(0x8126));
   opengl_ns.addConstant("GL_POINT_SIZE_MAX_ARB",                      new QoreBigIntNode(0x8127));
   opengl_ns.addConstant("GL_POINT_FADE_THRESHOLD_SIZE_ARB",           new QoreBigIntNode(0x8128));
   opengl_ns.addConstant("GL_POINT_DISTANCE_ATTENUATION_ARB",          new QoreBigIntNode(0x8129));
   opengl_ns.addConstant("GL_FRAGMENT_PROGRAM_ARB",                    new QoreBigIntNode(0x8804));
   opengl_ns.addConstant("GL_PROGRAM_ALU_INSTRUCTIONS_ARB",            new QoreBigIntNode(0x8805));
   opengl_ns.addConstant("GL_PROGRAM_TEX_INSTRUCTIONS_ARB",            new QoreBigIntNode(0x8806));
   opengl_ns.addConstant("GL_PROGRAM_TEX_INDIRECTIONS_ARB",            new QoreBigIntNode(0x8807));
   opengl_ns.addConstant("GL_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB",     new QoreBigIntNode(0x8808));
   opengl_ns.addConstant("GL_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB",     new QoreBigIntNode(0x8809));
   opengl_ns.addConstant("GL_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB",     new QoreBigIntNode(0x880A));
   opengl_ns.addConstant("GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB",        new QoreBigIntNode(0x880B));
   opengl_ns.addConstant("GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB",        new QoreBigIntNode(0x880C));
   opengl_ns.addConstant("GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB",        new QoreBigIntNode(0x880D));
   opengl_ns.addConstant("GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB", new QoreBigIntNode(0x880E));
   opengl_ns.addConstant("GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB", new QoreBigIntNode(0x880F));
   opengl_ns.addConstant("GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB", new QoreBigIntNode(0x8810));
   opengl_ns.addConstant("GL_MAX_TEXTURE_COORDS_ARB",                  new QoreBigIntNode(0x8871));
   opengl_ns.addConstant("GL_MAX_TEXTURE_IMAGE_UNITS_ARB",             new QoreBigIntNode(0x8872));
   opengl_ns.addConstant("GL_VERTEX_PROGRAM_ARB",                      new QoreBigIntNode(0x8620));
   opengl_ns.addConstant("GL_VERTEX_PROGRAM_POINT_SIZE_ARB",           new QoreBigIntNode(0x8642));
   opengl_ns.addConstant("GL_VERTEX_PROGRAM_TWO_SIDE_ARB",             new QoreBigIntNode(0x8643));
   opengl_ns.addConstant("GL_PROGRAM_FORMAT_ASCII_ARB",                new QoreBigIntNode(0x8875));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB",         new QoreBigIntNode(0x8622));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY_SIZE_ARB",            new QoreBigIntNode(0x8623));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY_STRIDE_ARB",          new QoreBigIntNode(0x8624));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY_TYPE_ARB",            new QoreBigIntNode(0x8625));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY_NORMALIZED_ARB",      new QoreBigIntNode(0x886A));
   opengl_ns.addConstant("GL_CURRENT_VERTEX_ATTRIB_ARB",               new QoreBigIntNode(0x8626));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY_POINTER_ARB",         new QoreBigIntNode(0x8645));
   opengl_ns.addConstant("GL_PROGRAM_LENGTH_ARB",                      new QoreBigIntNode(0x8627));
   opengl_ns.addConstant("GL_PROGRAM_FORMAT_ARB",                      new QoreBigIntNode(0x8876));
   opengl_ns.addConstant("GL_PROGRAM_NAME_ARB",                        new QoreBigIntNode(0x8677));
   opengl_ns.addConstant("GL_PROGRAM_BINDING_ARB",                     new QoreBigIntNode(0x8677));
   opengl_ns.addConstant("GL_PROGRAM_INSTRUCTIONS_ARB",                new QoreBigIntNode(0x88A0));
   opengl_ns.addConstant("GL_MAX_PROGRAM_INSTRUCTIONS_ARB",            new QoreBigIntNode(0x88A1));
   opengl_ns.addConstant("GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB",         new QoreBigIntNode(0x88A2));
   opengl_ns.addConstant("GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB",     new QoreBigIntNode(0x88A3));
   opengl_ns.addConstant("GL_PROGRAM_TEMPORARIES_ARB",                 new QoreBigIntNode(0x88A4));
   opengl_ns.addConstant("GL_MAX_PROGRAM_TEMPORARIES_ARB",             new QoreBigIntNode(0x88A5));
   opengl_ns.addConstant("GL_PROGRAM_NATIVE_TEMPORARIES_ARB",          new QoreBigIntNode(0x88A6));
   opengl_ns.addConstant("GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB",      new QoreBigIntNode(0x88A7));
   opengl_ns.addConstant("GL_PROGRAM_PARAMETERS_ARB",                  new QoreBigIntNode(0x88A8));
   opengl_ns.addConstant("GL_MAX_PROGRAM_PARAMETERS_ARB",              new QoreBigIntNode(0x88A9));
   opengl_ns.addConstant("GL_PROGRAM_NATIVE_PARAMETERS_ARB",           new QoreBigIntNode(0x88AA));
   opengl_ns.addConstant("GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB",       new QoreBigIntNode(0x88AB));
   opengl_ns.addConstant("GL_PROGRAM_ATTRIBS_ARB",                     new QoreBigIntNode(0x88AC));
   opengl_ns.addConstant("GL_MAX_PROGRAM_ATTRIBS_ARB",                 new QoreBigIntNode(0x88AD));
   opengl_ns.addConstant("GL_PROGRAM_NATIVE_ATTRIBS_ARB",              new QoreBigIntNode(0x88AE));
   opengl_ns.addConstant("GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB",          new QoreBigIntNode(0x88AF));
   opengl_ns.addConstant("GL_PROGRAM_ADDRESS_REGISTERS_ARB",           new QoreBigIntNode(0x88B0));
   opengl_ns.addConstant("GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB",       new QoreBigIntNode(0x88B1));
   opengl_ns.addConstant("GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB",    new QoreBigIntNode(0x88B2));
   opengl_ns.addConstant("GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB",new QoreBigIntNode(0x88B3));
   opengl_ns.addConstant("GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB",        new QoreBigIntNode(0x88B4));
   opengl_ns.addConstant("GL_MAX_PROGRAM_ENV_PARAMETERS_ARB",          new QoreBigIntNode(0x88B5));
   opengl_ns.addConstant("GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB",         new QoreBigIntNode(0x88B6));
   opengl_ns.addConstant("GL_PROGRAM_STRING_ARB",                      new QoreBigIntNode(0x8628));
   opengl_ns.addConstant("GL_PROGRAM_ERROR_POSITION_ARB",              new QoreBigIntNode(0x864B));
   opengl_ns.addConstant("GL_CURRENT_MATRIX_ARB",                      new QoreBigIntNode(0x8641));
   opengl_ns.addConstant("GL_TRANSPOSE_CURRENT_MATRIX_ARB",            new QoreBigIntNode(0x88B7));
   opengl_ns.addConstant("GL_CURRENT_MATRIX_STACK_DEPTH_ARB",          new QoreBigIntNode(0x8640));
   opengl_ns.addConstant("GL_MAX_VERTEX_ATTRIBS_ARB",                  new QoreBigIntNode(0x8869));
   opengl_ns.addConstant("GL_MAX_PROGRAM_MATRICES_ARB",                new QoreBigIntNode(0x862F));
   opengl_ns.addConstant("GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB",      new QoreBigIntNode(0x862E));
   opengl_ns.addConstant("GL_PROGRAM_ERROR_STRING_ARB",                new QoreBigIntNode(0x8874));
   opengl_ns.addConstant("GL_MATRIX0_ARB",                             new QoreBigIntNode(0x88C0));
   opengl_ns.addConstant("GL_MATRIX1_ARB",                             new QoreBigIntNode(0x88C1));
   opengl_ns.addConstant("GL_MATRIX2_ARB",                             new QoreBigIntNode(0x88C2));
   opengl_ns.addConstant("GL_MATRIX3_ARB",                             new QoreBigIntNode(0x88C3));
   opengl_ns.addConstant("GL_MATRIX4_ARB",                             new QoreBigIntNode(0x88C4));
   opengl_ns.addConstant("GL_MATRIX5_ARB",                             new QoreBigIntNode(0x88C5));
   opengl_ns.addConstant("GL_MATRIX6_ARB",                             new QoreBigIntNode(0x88C6));
   opengl_ns.addConstant("GL_MATRIX7_ARB",                             new QoreBigIntNode(0x88C7));
   opengl_ns.addConstant("GL_MATRIX8_ARB",                             new QoreBigIntNode(0x88C8));
   opengl_ns.addConstant("GL_MATRIX9_ARB",                             new QoreBigIntNode(0x88C9));
   opengl_ns.addConstant("GL_MATRIX10_ARB",                            new QoreBigIntNode(0x88CA));
   opengl_ns.addConstant("GL_MATRIX11_ARB",                            new QoreBigIntNode(0x88CB));
   opengl_ns.addConstant("GL_MATRIX12_ARB",                            new QoreBigIntNode(0x88CC));
   opengl_ns.addConstant("GL_MATRIX13_ARB",                            new QoreBigIntNode(0x88CD));
   opengl_ns.addConstant("GL_MATRIX14_ARB",                            new QoreBigIntNode(0x88CE));
   opengl_ns.addConstant("GL_MATRIX15_ARB",                            new QoreBigIntNode(0x88CF));
   opengl_ns.addConstant("GL_MATRIX16_ARB",                            new QoreBigIntNode(0x88D0));
   opengl_ns.addConstant("GL_MATRIX17_ARB",                            new QoreBigIntNode(0x88D1));
   opengl_ns.addConstant("GL_MATRIX18_ARB",                            new QoreBigIntNode(0x88D2));
   opengl_ns.addConstant("GL_MATRIX19_ARB",                            new QoreBigIntNode(0x88D3));
   opengl_ns.addConstant("GL_MATRIX20_ARB",                            new QoreBigIntNode(0x88D4));
   opengl_ns.addConstant("GL_MATRIX21_ARB",                            new QoreBigIntNode(0x88D5));
   opengl_ns.addConstant("GL_MATRIX22_ARB",                            new QoreBigIntNode(0x88D6));
   opengl_ns.addConstant("GL_MATRIX23_ARB",                            new QoreBigIntNode(0x88D7));
   opengl_ns.addConstant("GL_MATRIX24_ARB",                            new QoreBigIntNode(0x88D8));
   opengl_ns.addConstant("GL_MATRIX25_ARB",                            new QoreBigIntNode(0x88D9));
   opengl_ns.addConstant("GL_MATRIX26_ARB",                            new QoreBigIntNode(0x88DA));
   opengl_ns.addConstant("GL_MATRIX27_ARB",                            new QoreBigIntNode(0x88DB));
   opengl_ns.addConstant("GL_MATRIX28_ARB",                            new QoreBigIntNode(0x88DC));
   opengl_ns.addConstant("GL_MATRIX29_ARB",                            new QoreBigIntNode(0x88DD));
   opengl_ns.addConstant("GL_MATRIX30_ARB",                            new QoreBigIntNode(0x88DE));
   opengl_ns.addConstant("GL_MATRIX31_ARB",                            new QoreBigIntNode(0x88DF));
   opengl_ns.addConstant("GL_COLOR_SUM_ARB",                           new QoreBigIntNode(0x8458));
   opengl_ns.addConstant("GL_PROGRAM_OBJECT_ARB",                      new QoreBigIntNode(0x8B40));
   opengl_ns.addConstant("GL_OBJECT_TYPE_ARB",                         new QoreBigIntNode(0x8B4E));
   opengl_ns.addConstant("GL_OBJECT_SUBTYPE_ARB",                      new QoreBigIntNode(0x8B4F));
   opengl_ns.addConstant("GL_OBJECT_DELETE_STATUS_ARB",                new QoreBigIntNode(0x8B80));
   opengl_ns.addConstant("GL_OBJECT_COMPILE_STATUS_ARB",               new QoreBigIntNode(0x8B81));
   opengl_ns.addConstant("GL_OBJECT_LINK_STATUS_ARB",                  new QoreBigIntNode(0x8B82));
   opengl_ns.addConstant("GL_OBJECT_VALIDATE_STATUS_ARB",              new QoreBigIntNode(0x8B83));
   opengl_ns.addConstant("GL_OBJECT_INFO_LOG_LENGTH_ARB",              new QoreBigIntNode(0x8B84));
   opengl_ns.addConstant("GL_OBJECT_ATTACHED_OBJECTS_ARB",             new QoreBigIntNode(0x8B85));
   opengl_ns.addConstant("GL_OBJECT_ACTIVE_UNIFORMS_ARB",              new QoreBigIntNode(0x8B86));
   opengl_ns.addConstant("GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB",    new QoreBigIntNode(0x8B87));
   opengl_ns.addConstant("GL_OBJECT_SHADER_SOURCE_LENGTH_ARB",         new QoreBigIntNode(0x8B88));
   opengl_ns.addConstant("GL_SHADER_OBJECT_ARB",                       new QoreBigIntNode(0x8B48));
   opengl_ns.addConstant("GL_FLOAT_VEC2_ARB",                          new QoreBigIntNode(0x8B50));
   opengl_ns.addConstant("GL_FLOAT_VEC3_ARB",                          new QoreBigIntNode(0x8B51));
   opengl_ns.addConstant("GL_FLOAT_VEC4_ARB",                          new QoreBigIntNode(0x8B52));
   opengl_ns.addConstant("GL_INT_VEC2_ARB",                            new QoreBigIntNode(0x8B53));
   opengl_ns.addConstant("GL_INT_VEC3_ARB",                            new QoreBigIntNode(0x8B54));
   opengl_ns.addConstant("GL_INT_VEC4_ARB",                            new QoreBigIntNode(0x8B55));
   opengl_ns.addConstant("GL_BOOL_ARB",                                new QoreBigIntNode(0x8B56));
   opengl_ns.addConstant("GL_BOOL_VEC2_ARB",                           new QoreBigIntNode(0x8B57));
   opengl_ns.addConstant("GL_BOOL_VEC3_ARB",                           new QoreBigIntNode(0x8B58));
   opengl_ns.addConstant("GL_BOOL_VEC4_ARB",                           new QoreBigIntNode(0x8B59));
   opengl_ns.addConstant("GL_FLOAT_MAT2_ARB",                          new QoreBigIntNode(0x8B5A));
   opengl_ns.addConstant("GL_FLOAT_MAT3_ARB",                          new QoreBigIntNode(0x8B5B));
   opengl_ns.addConstant("GL_FLOAT_MAT4_ARB",                          new QoreBigIntNode(0x8B5C));
   opengl_ns.addConstant("GL_SAMPLER_1D_ARB",                          new QoreBigIntNode(0x8B5D));
   opengl_ns.addConstant("GL_SAMPLER_2D_ARB",                          new QoreBigIntNode(0x8B5E));
   opengl_ns.addConstant("GL_SAMPLER_3D_ARB",                          new QoreBigIntNode(0x8B5F));
   opengl_ns.addConstant("GL_SAMPLER_CUBE_ARB",                        new QoreBigIntNode(0x8B60));
   opengl_ns.addConstant("GL_SAMPLER_1D_SHADOW_ARB",                   new QoreBigIntNode(0x8B61));
   opengl_ns.addConstant("GL_SAMPLER_2D_SHADOW_ARB",                   new QoreBigIntNode(0x8B62));
   opengl_ns.addConstant("GL_SAMPLER_2D_RECT_ARB",                     new QoreBigIntNode(0x8B63));
   opengl_ns.addConstant("GL_SAMPLER_2D_RECT_SHADOW_ARB",              new QoreBigIntNode(0x8B64));
   opengl_ns.addConstant("GL_VERTEX_SHADER_ARB",                       new QoreBigIntNode(0x8B31));
   opengl_ns.addConstant("GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB",       new QoreBigIntNode(0x8B4A));
   opengl_ns.addConstant("GL_MAX_VARYING_FLOATS_ARB",                  new QoreBigIntNode(0x8B4B));
   opengl_ns.addConstant("GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB",    new QoreBigIntNode(0x8B4D));
   opengl_ns.addConstant("GL_OBJECT_ACTIVE_ATTRIBUTES_ARB",            new QoreBigIntNode(0x8B89));
   opengl_ns.addConstant("GL_OBJECT_ACTIVE_ATTRIBUTE_MAX_LENGTH_ARB",  new QoreBigIntNode(0x8B8A));
   opengl_ns.addConstant("GL_FRAGMENT_SHADER_ARB",                     new QoreBigIntNode(0x8B30));
   opengl_ns.addConstant("GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB",     new QoreBigIntNode(0x8B49));
   opengl_ns.addConstant("GL_FRAGMENT_SHADER_DERIVATIVE_HINT_ARB",     new QoreBigIntNode(0x8B8B));
   opengl_ns.addConstant("GL_SHADING_LANGUAGE_VERSION_ARB",            new QoreBigIntNode(0x8B8C));
   opengl_ns.addConstant("GL_ARRAY_BUFFER_ARB",                        new QoreBigIntNode(0x8892));
   opengl_ns.addConstant("GL_ELEMENT_ARRAY_BUFFER_ARB",                new QoreBigIntNode(0x8893));
   opengl_ns.addConstant("GL_ARRAY_BUFFER_BINDING_ARB",                new QoreBigIntNode(0x8894));
   opengl_ns.addConstant("GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB",        new QoreBigIntNode(0x8895));
   opengl_ns.addConstant("GL_VERTEX_ARRAY_BUFFER_BINDING_ARB",         new QoreBigIntNode(0x8896));
   opengl_ns.addConstant("GL_NORMAL_ARRAY_BUFFER_BINDING_ARB",         new QoreBigIntNode(0x8897));
   opengl_ns.addConstant("GL_COLOR_ARRAY_BUFFER_BINDING_ARB",          new QoreBigIntNode(0x8898));
   opengl_ns.addConstant("GL_INDEX_ARRAY_BUFFER_BINDING_ARB",          new QoreBigIntNode(0x8899));
   opengl_ns.addConstant("GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING_ARB",  new QoreBigIntNode(0x889A));
   opengl_ns.addConstant("GL_EDGE_FLAG_ARRAY_BUFFER_BINDING_ARB",      new QoreBigIntNode(0x889B));
   opengl_ns.addConstant("GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING_ARB",new QoreBigIntNode(0x889C));
   opengl_ns.addConstant("GL_FOG_COORD_ARRAY_BUFFER_BINDING_ARB",      new QoreBigIntNode(0x889D));
   opengl_ns.addConstant("GL_WEIGHT_ARRAY_BUFFER_BINDING_ARB",         new QoreBigIntNode(0x889E));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB",  new QoreBigIntNode(0x889F));
   opengl_ns.addConstant("GL_STREAM_DRAW_ARB",                         new QoreBigIntNode(0x88E0));
   opengl_ns.addConstant("GL_STREAM_READ_ARB",                         new QoreBigIntNode(0x88E1));
   opengl_ns.addConstant("GL_STREAM_COPY_ARB",                         new QoreBigIntNode(0x88E2));
   opengl_ns.addConstant("GL_STATIC_DRAW_ARB",                         new QoreBigIntNode(0x88E4));
   opengl_ns.addConstant("GL_STATIC_READ_ARB",                         new QoreBigIntNode(0x88E5));
   opengl_ns.addConstant("GL_STATIC_COPY_ARB",                         new QoreBigIntNode(0x88E6));
   opengl_ns.addConstant("GL_DYNAMIC_DRAW_ARB",                        new QoreBigIntNode(0x88E8));
   opengl_ns.addConstant("GL_DYNAMIC_READ_ARB",                        new QoreBigIntNode(0x88E9));
   opengl_ns.addConstant("GL_DYNAMIC_COPY_ARB",                        new QoreBigIntNode(0x88EA));
   opengl_ns.addConstant("GL_READ_ONLY_ARB",                           new QoreBigIntNode(0x88B8));
   opengl_ns.addConstant("GL_WRITE_ONLY_ARB",                          new QoreBigIntNode(0x88B9));
   opengl_ns.addConstant("GL_READ_WRITE_ARB",                          new QoreBigIntNode(0x88BA));
   opengl_ns.addConstant("GL_BUFFER_SIZE_ARB",                         new QoreBigIntNode(0x8764));
   opengl_ns.addConstant("GL_BUFFER_USAGE_ARB",                        new QoreBigIntNode(0x8765));
   opengl_ns.addConstant("GL_BUFFER_ACCESS_ARB",                       new QoreBigIntNode(0x88BB));
   opengl_ns.addConstant("GL_BUFFER_MAPPED_ARB",                       new QoreBigIntNode(0x88BC));
   opengl_ns.addConstant("GL_BUFFER_MAP_POINTER_ARB",                  new QoreBigIntNode(0x88BD));
   opengl_ns.addConstant("GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING_ARB", new QoreBigIntNode(0x889D));
   opengl_ns.addConstant("GL_POINT_SPRITE_ARB",                        new QoreBigIntNode(0x8861));
   opengl_ns.addConstant("GL_COORD_REPLACE_ARB",                       new QoreBigIntNode(0x8862));
   opengl_ns.addConstant("GL_TEXTURE_RECTANGLE_ARB",                   new QoreBigIntNode(0x84F5));
   opengl_ns.addConstant("GL_TEXTURE_BINDING_RECTANGLE_ARB",           new QoreBigIntNode(0x84F6));
   opengl_ns.addConstant("GL_PROXY_TEXTURE_RECTANGLE_ARB",             new QoreBigIntNode(0x84F7));
   opengl_ns.addConstant("GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB",          new QoreBigIntNode(0x84F8));
   opengl_ns.addConstant("GL_MAX_DRAW_BUFFERS_ARB",                    new QoreBigIntNode(0x8824));
   opengl_ns.addConstant("GL_DRAW_BUFFER0_ARB",                        new QoreBigIntNode(0x8825));
   opengl_ns.addConstant("GL_DRAW_BUFFER1_ARB",                        new QoreBigIntNode(0x8826));
   opengl_ns.addConstant("GL_DRAW_BUFFER2_ARB",                        new QoreBigIntNode(0x8827));
   opengl_ns.addConstant("GL_DRAW_BUFFER3_ARB",                        new QoreBigIntNode(0x8828));
   opengl_ns.addConstant("GL_DRAW_BUFFER4_ARB",                        new QoreBigIntNode(0x8829));
   opengl_ns.addConstant("GL_DRAW_BUFFER5_ARB",                        new QoreBigIntNode(0x882A));
   opengl_ns.addConstant("GL_DRAW_BUFFER6_ARB",                        new QoreBigIntNode(0x882B));
   opengl_ns.addConstant("GL_DRAW_BUFFER7_ARB",                        new QoreBigIntNode(0x882C));
   opengl_ns.addConstant("GL_DRAW_BUFFER8_ARB",                        new QoreBigIntNode(0x882D));
   opengl_ns.addConstant("GL_DRAW_BUFFER9_ARB",                        new QoreBigIntNode(0x882E));
   opengl_ns.addConstant("GL_DRAW_BUFFER10_ARB",                       new QoreBigIntNode(0x882F));
   opengl_ns.addConstant("GL_DRAW_BUFFER11_ARB",                       new QoreBigIntNode(0x8830));
   opengl_ns.addConstant("GL_DRAW_BUFFER12_ARB",                       new QoreBigIntNode(0x8831));
   opengl_ns.addConstant("GL_DRAW_BUFFER13_ARB",                       new QoreBigIntNode(0x8832));
   opengl_ns.addConstant("GL_DRAW_BUFFER14_ARB",                       new QoreBigIntNode(0x8833));
   opengl_ns.addConstant("GL_DRAW_BUFFER15_ARB",                       new QoreBigIntNode(0x8834));
   opengl_ns.addConstant("GL_PIXEL_PACK_BUFFER_ARB",                   new QoreBigIntNode(0x88EB));
   opengl_ns.addConstant("GL_PIXEL_UNPACK_BUFFER_ARB",                 new QoreBigIntNode(0x88EC));
   opengl_ns.addConstant("GL_PIXEL_PACK_BUFFER_BINDING_ARB",           new QoreBigIntNode(0x88ED));
   opengl_ns.addConstant("GL_PIXEL_UNPACK_BUFFER_BINDING_ARB",         new QoreBigIntNode(0x88EF));
   opengl_ns.addConstant("GL_TEXTURE_RED_TYPE_ARB",                    new QoreBigIntNode(0x8C10));
   opengl_ns.addConstant("GL_TEXTURE_GREEN_TYPE_ARB",                  new QoreBigIntNode(0x8C11));
   opengl_ns.addConstant("GL_TEXTURE_BLUE_TYPE_ARB",                   new QoreBigIntNode(0x8C12));
   opengl_ns.addConstant("GL_TEXTURE_ALPHA_TYPE_ARB",                  new QoreBigIntNode(0x8C13));
   opengl_ns.addConstant("GL_TEXTURE_LUMINANCE_TYPE_ARB",              new QoreBigIntNode(0x8C14));
   opengl_ns.addConstant("GL_TEXTURE_INTENSITY_TYPE_ARB",              new QoreBigIntNode(0x8C15));
   opengl_ns.addConstant("GL_TEXTURE_DEPTH_TYPE_ARB",                  new QoreBigIntNode(0x8C16));
   opengl_ns.addConstant("GL_UNSIGNED_NORMALIZED_ARB",                 new QoreBigIntNode(0x8C17));
   opengl_ns.addConstant("GL_RGBA32F_ARB",                             new QoreBigIntNode(0x8814));
   opengl_ns.addConstant("GL_RGB32F_ARB",                              new QoreBigIntNode(0x8815));
   opengl_ns.addConstant("GL_ALPHA32F_ARB",                            new QoreBigIntNode(0x8816));
   opengl_ns.addConstant("GL_INTENSITY32F_ARB",                        new QoreBigIntNode(0x8817));
   opengl_ns.addConstant("GL_LUMINANCE32F_ARB",                        new QoreBigIntNode(0x8818));
   opengl_ns.addConstant("GL_LUMINANCE_ALPHA32F_ARB",                  new QoreBigIntNode(0x8819));
   opengl_ns.addConstant("GL_RGBA16F_ARB",                             new QoreBigIntNode(0x881A));
   opengl_ns.addConstant("GL_RGB16F_ARB",                              new QoreBigIntNode(0x881B));
   opengl_ns.addConstant("GL_ALPHA16F_ARB",                            new QoreBigIntNode(0x881C));
   opengl_ns.addConstant("GL_INTENSITY16F_ARB",                        new QoreBigIntNode(0x881D));
   opengl_ns.addConstant("GL_LUMINANCE16F_ARB",                        new QoreBigIntNode(0x881E));
   opengl_ns.addConstant("GL_LUMINANCE_ALPHA16F_ARB",                  new QoreBigIntNode(0x881F));
   opengl_ns.addConstant("GL_HALF_FLOAT_ARB",                          new QoreBigIntNode(0x140B));
   opengl_ns.addConstant("GL_ABGR_EXT",                                new QoreBigIntNode(0x8000));
   opengl_ns.addConstant("GL_CONSTANT_COLOR_EXT",                      new QoreBigIntNode(0x8001));
   opengl_ns.addConstant("GL_ONE_MINUS_CONSTANT_COLOR_EXT",            new QoreBigIntNode(0x8002));
   opengl_ns.addConstant("GL_CONSTANT_ALPHA_EXT",                      new QoreBigIntNode(0x8003));
   opengl_ns.addConstant("GL_ONE_MINUS_CONSTANT_ALPHA_EXT",            new QoreBigIntNode(0x8004));
   opengl_ns.addConstant("GL_BLEND_COLOR_EXT",                         new QoreBigIntNode(0x8005));
   opengl_ns.addConstant("GL_POLYGON_OFFSET_EXT",                      new QoreBigIntNode(0x8037));
   opengl_ns.addConstant("GL_POLYGON_OFFSET_FACTOR_EXT",               new QoreBigIntNode(0x8038));
   opengl_ns.addConstant("GL_POLYGON_OFFSET_BIAS_EXT",                 new QoreBigIntNode(0x8039));
   opengl_ns.addConstant("GL_ALPHA4_EXT",                              new QoreBigIntNode(0x803B));
   opengl_ns.addConstant("GL_ALPHA8_EXT",                              new QoreBigIntNode(0x803C));
   opengl_ns.addConstant("GL_ALPHA12_EXT",                             new QoreBigIntNode(0x803D));
   opengl_ns.addConstant("GL_ALPHA16_EXT",                             new QoreBigIntNode(0x803E));
   opengl_ns.addConstant("GL_LUMINANCE4_EXT",                          new QoreBigIntNode(0x803F));
   opengl_ns.addConstant("GL_LUMINANCE8_EXT",                          new QoreBigIntNode(0x8040));
   opengl_ns.addConstant("GL_LUMINANCE12_EXT",                         new QoreBigIntNode(0x8041));
   opengl_ns.addConstant("GL_LUMINANCE16_EXT",                         new QoreBigIntNode(0x8042));
   opengl_ns.addConstant("GL_LUMINANCE4_ALPHA4_EXT",                   new QoreBigIntNode(0x8043));
   opengl_ns.addConstant("GL_LUMINANCE6_ALPHA2_EXT",                   new QoreBigIntNode(0x8044));
   opengl_ns.addConstant("GL_LUMINANCE8_ALPHA8_EXT",                   new QoreBigIntNode(0x8045));
   opengl_ns.addConstant("GL_LUMINANCE12_ALPHA4_EXT",                  new QoreBigIntNode(0x8046));
   opengl_ns.addConstant("GL_LUMINANCE12_ALPHA12_EXT",                 new QoreBigIntNode(0x8047));
   opengl_ns.addConstant("GL_LUMINANCE16_ALPHA16_EXT",                 new QoreBigIntNode(0x8048));
   opengl_ns.addConstant("GL_INTENSITY_EXT",                           new QoreBigIntNode(0x8049));
   opengl_ns.addConstant("GL_INTENSITY4_EXT",                          new QoreBigIntNode(0x804A));
   opengl_ns.addConstant("GL_INTENSITY8_EXT",                          new QoreBigIntNode(0x804B));
   opengl_ns.addConstant("GL_INTENSITY12_EXT",                         new QoreBigIntNode(0x804C));
   opengl_ns.addConstant("GL_INTENSITY16_EXT",                         new QoreBigIntNode(0x804D));
   opengl_ns.addConstant("GL_RGB2_EXT",                                new QoreBigIntNode(0x804E));
   opengl_ns.addConstant("GL_RGB4_EXT",                                new QoreBigIntNode(0x804F));
   opengl_ns.addConstant("GL_RGB5_EXT",                                new QoreBigIntNode(0x8050));
   opengl_ns.addConstant("GL_RGB8_EXT",                                new QoreBigIntNode(0x8051));
   opengl_ns.addConstant("GL_RGB10_EXT",                               new QoreBigIntNode(0x8052));
   opengl_ns.addConstant("GL_RGB12_EXT",                               new QoreBigIntNode(0x8053));
   opengl_ns.addConstant("GL_RGB16_EXT",                               new QoreBigIntNode(0x8054));
   opengl_ns.addConstant("GL_RGBA2_EXT",                               new QoreBigIntNode(0x8055));
   opengl_ns.addConstant("GL_RGBA4_EXT",                               new QoreBigIntNode(0x8056));
   opengl_ns.addConstant("GL_RGB5_A1_EXT",                             new QoreBigIntNode(0x8057));
   opengl_ns.addConstant("GL_RGBA8_EXT",                               new QoreBigIntNode(0x8058));
   opengl_ns.addConstant("GL_RGB10_A2_EXT",                            new QoreBigIntNode(0x8059));
   opengl_ns.addConstant("GL_RGBA12_EXT",                              new QoreBigIntNode(0x805A));
   opengl_ns.addConstant("GL_RGBA16_EXT",                              new QoreBigIntNode(0x805B));
   opengl_ns.addConstant("GL_TEXTURE_RED_SIZE_EXT",                    new QoreBigIntNode(0x805C));
   opengl_ns.addConstant("GL_TEXTURE_GREEN_SIZE_EXT",                  new QoreBigIntNode(0x805D));
   opengl_ns.addConstant("GL_TEXTURE_BLUE_SIZE_EXT",                   new QoreBigIntNode(0x805E));
   opengl_ns.addConstant("GL_TEXTURE_ALPHA_SIZE_EXT",                  new QoreBigIntNode(0x805F));
   opengl_ns.addConstant("GL_TEXTURE_LUMINANCE_SIZE_EXT",              new QoreBigIntNode(0x8060));
   opengl_ns.addConstant("GL_TEXTURE_INTENSITY_SIZE_EXT",              new QoreBigIntNode(0x8061));
   opengl_ns.addConstant("GL_REPLACE_EXT",                             new QoreBigIntNode(0x8062));
   opengl_ns.addConstant("GL_PROXY_TEXTURE_1D_EXT",                    new QoreBigIntNode(0x8063));
   opengl_ns.addConstant("GL_PROXY_TEXTURE_2D_EXT",                    new QoreBigIntNode(0x8064));
   opengl_ns.addConstant("GL_TEXTURE_TOO_LARGE_EXT",                   new QoreBigIntNode(0x8065));
   opengl_ns.addConstant("GL_PACK_SKIP_IMAGES_EXT",                    new QoreBigIntNode(0x806B));
   opengl_ns.addConstant("GL_PACK_IMAGE_HEIGHT_EXT",                   new QoreBigIntNode(0x806C));
   opengl_ns.addConstant("GL_UNPACK_SKIP_IMAGES_EXT",                  new QoreBigIntNode(0x806D));
   opengl_ns.addConstant("GL_UNPACK_IMAGE_HEIGHT_EXT",                 new QoreBigIntNode(0x806E));
   opengl_ns.addConstant("GL_TEXTURE_3D_EXT",                          new QoreBigIntNode(0x806F));
   opengl_ns.addConstant("GL_PROXY_TEXTURE_3D_EXT",                    new QoreBigIntNode(0x8070));
   opengl_ns.addConstant("GL_TEXTURE_DEPTH_EXT",                       new QoreBigIntNode(0x8071));
   opengl_ns.addConstant("GL_TEXTURE_WRAP_R_EXT",                      new QoreBigIntNode(0x8072));
   opengl_ns.addConstant("GL_MAX_3D_TEXTURE_SIZE_EXT",                 new QoreBigIntNode(0x8073));
   opengl_ns.addConstant("GL_HISTOGRAM_EXT",                           new QoreBigIntNode(0x8024));
   opengl_ns.addConstant("GL_PROXY_HISTOGRAM_EXT",                     new QoreBigIntNode(0x8025));
   opengl_ns.addConstant("GL_HISTOGRAM_WIDTH_EXT",                     new QoreBigIntNode(0x8026));
   opengl_ns.addConstant("GL_HISTOGRAM_FORMAT_EXT",                    new QoreBigIntNode(0x8027));
   opengl_ns.addConstant("GL_HISTOGRAM_RED_SIZE_EXT",                  new QoreBigIntNode(0x8028));
   opengl_ns.addConstant("GL_HISTOGRAM_GREEN_SIZE_EXT",                new QoreBigIntNode(0x8029));
   opengl_ns.addConstant("GL_HISTOGRAM_BLUE_SIZE_EXT",                 new QoreBigIntNode(0x802A));
   opengl_ns.addConstant("GL_HISTOGRAM_ALPHA_SIZE_EXT",                new QoreBigIntNode(0x802B));
   opengl_ns.addConstant("GL_HISTOGRAM_LUMINANCE_SIZE_EXT",            new QoreBigIntNode(0x802C));
   opengl_ns.addConstant("GL_HISTOGRAM_SINK_EXT",                      new QoreBigIntNode(0x802D));
   opengl_ns.addConstant("GL_MINMAX_EXT",                              new QoreBigIntNode(0x802E));
   opengl_ns.addConstant("GL_MINMAX_FORMAT_EXT",                       new QoreBigIntNode(0x802F));
   opengl_ns.addConstant("GL_MINMAX_SINK_EXT",                         new QoreBigIntNode(0x8030));
   opengl_ns.addConstant("GL_TABLE_TOO_LARGE_EXT",                     new QoreBigIntNode(0x8031));
   opengl_ns.addConstant("GL_CONVOLUTION_1D_EXT",                      new QoreBigIntNode(0x8010));
   opengl_ns.addConstant("GL_CONVOLUTION_2D_EXT",                      new QoreBigIntNode(0x8011));
   opengl_ns.addConstant("GL_SEPARABLE_2D_EXT",                        new QoreBigIntNode(0x8012));
   opengl_ns.addConstant("GL_CONVOLUTION_BORDER_MODE_EXT",             new QoreBigIntNode(0x8013));
   opengl_ns.addConstant("GL_CONVOLUTION_FILTER_SCALE_EXT",            new QoreBigIntNode(0x8014));
   opengl_ns.addConstant("GL_CONVOLUTION_FILTER_BIAS_EXT",             new QoreBigIntNode(0x8015));
   opengl_ns.addConstant("GL_REDUCE_EXT",                              new QoreBigIntNode(0x8016));
   opengl_ns.addConstant("GL_CONVOLUTION_FORMAT_EXT",                  new QoreBigIntNode(0x8017));
   opengl_ns.addConstant("GL_CONVOLUTION_WIDTH_EXT",                   new QoreBigIntNode(0x8018));
   opengl_ns.addConstant("GL_CONVOLUTION_HEIGHT_EXT",                  new QoreBigIntNode(0x8019));
   opengl_ns.addConstant("GL_MAX_CONVOLUTION_WIDTH_EXT",               new QoreBigIntNode(0x801A));
   opengl_ns.addConstant("GL_MAX_CONVOLUTION_HEIGHT_EXT",              new QoreBigIntNode(0x801B));
   opengl_ns.addConstant("GL_POST_CONVOLUTION_RED_SCALE_EXT",          new QoreBigIntNode(0x801C));
   opengl_ns.addConstant("GL_POST_CONVOLUTION_GREEN_SCALE_EXT",        new QoreBigIntNode(0x801D));
   opengl_ns.addConstant("GL_POST_CONVOLUTION_BLUE_SCALE_EXT",         new QoreBigIntNode(0x801E));
   opengl_ns.addConstant("GL_POST_CONVOLUTION_ALPHA_SCALE_EXT",        new QoreBigIntNode(0x801F));
   opengl_ns.addConstant("GL_POST_CONVOLUTION_RED_BIAS_EXT",           new QoreBigIntNode(0x8020));
   opengl_ns.addConstant("GL_POST_CONVOLUTION_GREEN_BIAS_EXT",         new QoreBigIntNode(0x8021));
   opengl_ns.addConstant("GL_POST_CONVOLUTION_BLUE_BIAS_EXT",          new QoreBigIntNode(0x8022));
   opengl_ns.addConstant("GL_POST_CONVOLUTION_ALPHA_BIAS_EXT",         new QoreBigIntNode(0x8023));
   opengl_ns.addConstant("GL_CMYK_EXT",                                new QoreBigIntNode(0x800C));
   opengl_ns.addConstant("GL_CMYKA_EXT",                               new QoreBigIntNode(0x800D));
   opengl_ns.addConstant("GL_PACK_CMYK_HINT_EXT",                      new QoreBigIntNode(0x800E));
   opengl_ns.addConstant("GL_UNPACK_CMYK_HINT_EXT",                    new QoreBigIntNode(0x800F));
   opengl_ns.addConstant("GL_TEXTURE_PRIORITY_EXT",                    new QoreBigIntNode(0x8066));
   opengl_ns.addConstant("GL_TEXTURE_RESIDENT_EXT",                    new QoreBigIntNode(0x8067));
   opengl_ns.addConstant("GL_TEXTURE_1D_BINDING_EXT",                  new QoreBigIntNode(0x8068));
   opengl_ns.addConstant("GL_TEXTURE_2D_BINDING_EXT",                  new QoreBigIntNode(0x8069));
   opengl_ns.addConstant("GL_TEXTURE_3D_BINDING_EXT",                  new QoreBigIntNode(0x806A));
   opengl_ns.addConstant("GL_UNSIGNED_BYTE_3_3_2_EXT",                 new QoreBigIntNode(0x8032));
   opengl_ns.addConstant("GL_UNSIGNED_SHORT_4_4_4_4_EXT",              new QoreBigIntNode(0x8033));
   opengl_ns.addConstant("GL_UNSIGNED_SHORT_5_5_5_1_EXT",              new QoreBigIntNode(0x8034));
   opengl_ns.addConstant("GL_UNSIGNED_INT_8_8_8_8_EXT",                new QoreBigIntNode(0x8035));
   opengl_ns.addConstant("GL_UNSIGNED_INT_10_10_10_2_EXT",             new QoreBigIntNode(0x8036));
   opengl_ns.addConstant("GL_RESCALE_NORMAL_EXT",                      new QoreBigIntNode(0x803A));
   opengl_ns.addConstant("GL_VERTEX_ARRAY_EXT",                        new QoreBigIntNode(0x8074));
   opengl_ns.addConstant("GL_NORMAL_ARRAY_EXT",                        new QoreBigIntNode(0x8075));
   opengl_ns.addConstant("GL_COLOR_ARRAY_EXT",                         new QoreBigIntNode(0x8076));
   opengl_ns.addConstant("GL_INDEX_ARRAY_EXT",                         new QoreBigIntNode(0x8077));
   opengl_ns.addConstant("GL_TEXTURE_COORD_ARRAY_EXT",                 new QoreBigIntNode(0x8078));
   opengl_ns.addConstant("GL_EDGE_FLAG_ARRAY_EXT",                     new QoreBigIntNode(0x8079));
   opengl_ns.addConstant("GL_VERTEX_ARRAY_SIZE_EXT",                   new QoreBigIntNode(0x807A));
   opengl_ns.addConstant("GL_VERTEX_ARRAY_TYPE_EXT",                   new QoreBigIntNode(0x807B));
   opengl_ns.addConstant("GL_VERTEX_ARRAY_STRIDE_EXT",                 new QoreBigIntNode(0x807C));
   opengl_ns.addConstant("GL_VERTEX_ARRAY_COUNT_EXT",                  new QoreBigIntNode(0x807D));
   opengl_ns.addConstant("GL_NORMAL_ARRAY_TYPE_EXT",                   new QoreBigIntNode(0x807E));
   opengl_ns.addConstant("GL_NORMAL_ARRAY_STRIDE_EXT",                 new QoreBigIntNode(0x807F));
   opengl_ns.addConstant("GL_NORMAL_ARRAY_COUNT_EXT",                  new QoreBigIntNode(0x8080));
   opengl_ns.addConstant("GL_COLOR_ARRAY_SIZE_EXT",                    new QoreBigIntNode(0x8081));
   opengl_ns.addConstant("GL_COLOR_ARRAY_TYPE_EXT",                    new QoreBigIntNode(0x8082));
   opengl_ns.addConstant("GL_COLOR_ARRAY_STRIDE_EXT",                  new QoreBigIntNode(0x8083));
   opengl_ns.addConstant("GL_COLOR_ARRAY_COUNT_EXT",                   new QoreBigIntNode(0x8084));
   opengl_ns.addConstant("GL_INDEX_ARRAY_TYPE_EXT",                    new QoreBigIntNode(0x8085));
   opengl_ns.addConstant("GL_INDEX_ARRAY_STRIDE_EXT",                  new QoreBigIntNode(0x8086));
   opengl_ns.addConstant("GL_INDEX_ARRAY_COUNT_EXT",                   new QoreBigIntNode(0x8087));
   opengl_ns.addConstant("GL_TEXTURE_COORD_ARRAY_SIZE_EXT",            new QoreBigIntNode(0x8088));
   opengl_ns.addConstant("GL_TEXTURE_COORD_ARRAY_TYPE_EXT",            new QoreBigIntNode(0x8089));
   opengl_ns.addConstant("GL_TEXTURE_COORD_ARRAY_STRIDE_EXT",          new QoreBigIntNode(0x808A));
   opengl_ns.addConstant("GL_TEXTURE_COORD_ARRAY_COUNT_EXT",           new QoreBigIntNode(0x808B));
   opengl_ns.addConstant("GL_EDGE_FLAG_ARRAY_STRIDE_EXT",              new QoreBigIntNode(0x808C));
   opengl_ns.addConstant("GL_EDGE_FLAG_ARRAY_COUNT_EXT",               new QoreBigIntNode(0x808D));
   opengl_ns.addConstant("GL_VERTEX_ARRAY_POINTER_EXT",                new QoreBigIntNode(0x808E));
   opengl_ns.addConstant("GL_NORMAL_ARRAY_POINTER_EXT",                new QoreBigIntNode(0x808F));
   opengl_ns.addConstant("GL_COLOR_ARRAY_POINTER_EXT",                 new QoreBigIntNode(0x8090));
   opengl_ns.addConstant("GL_INDEX_ARRAY_POINTER_EXT",                 new QoreBigIntNode(0x8091));
   opengl_ns.addConstant("GL_TEXTURE_COORD_ARRAY_POINTER_EXT",         new QoreBigIntNode(0x8092));
   opengl_ns.addConstant("GL_EDGE_FLAG_ARRAY_POINTER_EXT",             new QoreBigIntNode(0x8093));
   opengl_ns.addConstant("GL_FUNC_ADD_EXT",                            new QoreBigIntNode(0x8006));
   opengl_ns.addConstant("GL_MIN_EXT",                                 new QoreBigIntNode(0x8007));
   opengl_ns.addConstant("GL_MAX_EXT",                                 new QoreBigIntNode(0x8008));
   opengl_ns.addConstant("GL_BLEND_EQUATION_EXT",                      new QoreBigIntNode(0x8009));
   opengl_ns.addConstant("GL_FUNC_SUBTRACT_EXT",                       new QoreBigIntNode(0x800A));
   opengl_ns.addConstant("GL_FUNC_REVERSE_SUBTRACT_EXT",               new QoreBigIntNode(0x800B));
   opengl_ns.addConstant("GL_COLOR_INDEX1_EXT",                        new QoreBigIntNode(0x80E2));
   opengl_ns.addConstant("GL_COLOR_INDEX2_EXT",                        new QoreBigIntNode(0x80E3));
   opengl_ns.addConstant("GL_COLOR_INDEX4_EXT",                        new QoreBigIntNode(0x80E4));
   opengl_ns.addConstant("GL_COLOR_INDEX8_EXT",                        new QoreBigIntNode(0x80E5));
   opengl_ns.addConstant("GL_COLOR_INDEX12_EXT",                       new QoreBigIntNode(0x80E6));
   opengl_ns.addConstant("GL_COLOR_INDEX16_EXT",                       new QoreBigIntNode(0x80E7));
   opengl_ns.addConstant("GL_TEXTURE_INDEX_SIZE_EXT",                  new QoreBigIntNode(0x80ED));
   opengl_ns.addConstant("GL_CLIP_VOLUME_CLIPPING_HINT_EXT",           new QoreBigIntNode(0x80F0));
   opengl_ns.addConstant("GL_INDEX_MATERIAL_EXT",                      new QoreBigIntNode(0x81B8));
   opengl_ns.addConstant("GL_INDEX_MATERIAL_PARAMETER_EXT",            new QoreBigIntNode(0x81B9));
   opengl_ns.addConstant("GL_INDEX_MATERIAL_FACE_EXT",                 new QoreBigIntNode(0x81BA));
   opengl_ns.addConstant("GL_INDEX_TEST_EXT",                          new QoreBigIntNode(0x81B5));
   opengl_ns.addConstant("GL_INDEX_TEST_FUNC_EXT",                     new QoreBigIntNode(0x81B6));
   opengl_ns.addConstant("GL_INDEX_TEST_REF_EXT",                      new QoreBigIntNode(0x81B7));
   opengl_ns.addConstant("GL_IUI_V2F_EXT",                             new QoreBigIntNode(0x81AD));
   opengl_ns.addConstant("GL_IUI_V3F_EXT",                             new QoreBigIntNode(0x81AE));
   opengl_ns.addConstant("GL_IUI_N3F_V2F_EXT",                         new QoreBigIntNode(0x81AF));
   opengl_ns.addConstant("GL_IUI_N3F_V3F_EXT",                         new QoreBigIntNode(0x81B0));
   opengl_ns.addConstant("GL_T2F_IUI_V2F_EXT",                         new QoreBigIntNode(0x81B1));
   opengl_ns.addConstant("GL_T2F_IUI_V3F_EXT",                         new QoreBigIntNode(0x81B2));
   opengl_ns.addConstant("GL_T2F_IUI_N3F_V2F_EXT",                     new QoreBigIntNode(0x81B3));
   opengl_ns.addConstant("GL_T2F_IUI_N3F_V3F_EXT",                     new QoreBigIntNode(0x81B4));
   opengl_ns.addConstant("GL_ARRAY_ELEMENT_LOCK_FIRST_EXT",            new QoreBigIntNode(0x81A8));
   opengl_ns.addConstant("GL_ARRAY_ELEMENT_LOCK_COUNT_EXT",            new QoreBigIntNode(0x81A9));
   opengl_ns.addConstant("GL_CULL_VERTEX_EXT",                         new QoreBigIntNode(0x81AA));
   opengl_ns.addConstant("GL_CULL_VERTEX_EYE_POSITION_EXT",            new QoreBigIntNode(0x81AB));
   opengl_ns.addConstant("GL_CULL_VERTEX_OBJECT_POSITION_EXT",         new QoreBigIntNode(0x81AC));
   opengl_ns.addConstant("GL_MAX_ELEMENTS_VERTICES_EXT",               new QoreBigIntNode(0x80E8));
   opengl_ns.addConstant("GL_MAX_ELEMENTS_INDICES_EXT",                new QoreBigIntNode(0x80E9));
   opengl_ns.addConstant("GL_FRAGMENT_MATERIAL_EXT",                   new QoreBigIntNode(0x8349));
   opengl_ns.addConstant("GL_FRAGMENT_NORMAL_EXT",                     new QoreBigIntNode(0x834A));
   opengl_ns.addConstant("GL_FRAGMENT_COLOR_EXT",                      new QoreBigIntNode(0x834C));
   opengl_ns.addConstant("GL_ATTENUATION_EXT",                         new QoreBigIntNode(0x834D));
   opengl_ns.addConstant("GL_SHADOW_ATTENUATION_EXT",                  new QoreBigIntNode(0x834E));
   opengl_ns.addConstant("GL_TEXTURE_APPLICATION_MODE_EXT",            new QoreBigIntNode(0x834F));
   opengl_ns.addConstant("GL_TEXTURE_LIGHT_EXT",                       new QoreBigIntNode(0x8350));
   opengl_ns.addConstant("GL_TEXTURE_MATERIAL_FACE_EXT",               new QoreBigIntNode(0x8351));
   opengl_ns.addConstant("GL_TEXTURE_MATERIAL_PARAMETER_EXT",          new QoreBigIntNode(0x8352));
   opengl_ns.addConstant("GL_BGR_EXT",                                 new QoreBigIntNode(0x80E0));
   opengl_ns.addConstant("GL_BGRA_EXT",                                new QoreBigIntNode(0x80E1));
   opengl_ns.addConstant("GL_PIXEL_TRANSFORM_2D_EXT",                  new QoreBigIntNode(0x8330));
   opengl_ns.addConstant("GL_PIXEL_MAG_FILTER_EXT",                    new QoreBigIntNode(0x8331));
   opengl_ns.addConstant("GL_PIXEL_MIN_FILTER_EXT",                    new QoreBigIntNode(0x8332));
   opengl_ns.addConstant("GL_PIXEL_CUBIC_WEIGHT_EXT",                  new QoreBigIntNode(0x8333));
   opengl_ns.addConstant("GL_CUBIC_EXT",                               new QoreBigIntNode(0x8334));
   opengl_ns.addConstant("GL_AVERAGE_EXT",                             new QoreBigIntNode(0x8335));
   opengl_ns.addConstant("GL_PIXEL_TRANSFORM_2D_STACK_DEPTH_EXT",      new QoreBigIntNode(0x8336));
   opengl_ns.addConstant("GL_MAX_PIXEL_TRANSFORM_2D_STACK_DEPTH_EXT",  new QoreBigIntNode(0x8337));
   opengl_ns.addConstant("GL_PIXEL_TRANSFORM_2D_MATRIX_EXT",           new QoreBigIntNode(0x8338));
   opengl_ns.addConstant("GL_SHARED_TEXTURE_PALETTE_EXT",              new QoreBigIntNode(0x81FB));
   opengl_ns.addConstant("GL_LIGHT_MODEL_COLOR_CONTROL_EXT",           new QoreBigIntNode(0x81F8));
   opengl_ns.addConstant("GL_SINGLE_COLOR_EXT",                        new QoreBigIntNode(0x81F9));
   opengl_ns.addConstant("GL_SEPARATE_SPECULAR_COLOR_EXT",             new QoreBigIntNode(0x81FA));
   opengl_ns.addConstant("GL_COLOR_SUM_EXT",                           new QoreBigIntNode(0x8458));
   opengl_ns.addConstant("GL_CURRENT_SECONDARY_COLOR_EXT",             new QoreBigIntNode(0x8459));
   opengl_ns.addConstant("GL_SECONDARY_COLOR_ARRAY_SIZE_EXT",          new QoreBigIntNode(0x845A));
   opengl_ns.addConstant("GL_SECONDARY_COLOR_ARRAY_TYPE_EXT",          new QoreBigIntNode(0x845B));
   opengl_ns.addConstant("GL_SECONDARY_COLOR_ARRAY_STRIDE_EXT",        new QoreBigIntNode(0x845C));
   opengl_ns.addConstant("GL_SECONDARY_COLOR_ARRAY_POINTER_EXT",       new QoreBigIntNode(0x845D));
   opengl_ns.addConstant("GL_SECONDARY_COLOR_ARRAY_EXT",               new QoreBigIntNode(0x845E));
   opengl_ns.addConstant("GL_PERTURB_EXT",                             new QoreBigIntNode(0x85AE));
   opengl_ns.addConstant("GL_TEXTURE_NORMAL_EXT",                      new QoreBigIntNode(0x85AF));
   opengl_ns.addConstant("GL_FOG_COORDINATE_SOURCE_EXT",               new QoreBigIntNode(0x8450));
   opengl_ns.addConstant("GL_FOG_COORDINATE_EXT",                      new QoreBigIntNode(0x8451));
   opengl_ns.addConstant("GL_FRAGMENT_DEPTH_EXT",                      new QoreBigIntNode(0x8452));
   opengl_ns.addConstant("GL_CURRENT_FOG_COORDINATE_EXT",              new QoreBigIntNode(0x8453));
   opengl_ns.addConstant("GL_FOG_COORDINATE_ARRAY_TYPE_EXT",           new QoreBigIntNode(0x8454));
   opengl_ns.addConstant("GL_FOG_COORDINATE_ARRAY_STRIDE_EXT",         new QoreBigIntNode(0x8455));
   opengl_ns.addConstant("GL_FOG_COORDINATE_ARRAY_POINTER_EXT",        new QoreBigIntNode(0x8456));
   opengl_ns.addConstant("GL_FOG_COORDINATE_ARRAY_EXT",                new QoreBigIntNode(0x8457));
   opengl_ns.addConstant("GL_TANGENT_ARRAY_EXT",                       new QoreBigIntNode(0x8439));
   opengl_ns.addConstant("GL_BINORMAL_ARRAY_EXT",                      new QoreBigIntNode(0x843A));
   opengl_ns.addConstant("GL_CURRENT_TANGENT_EXT",                     new QoreBigIntNode(0x843B));
   opengl_ns.addConstant("GL_CURRENT_BINORMAL_EXT",                    new QoreBigIntNode(0x843C));
   opengl_ns.addConstant("GL_TANGENT_ARRAY_TYPE_EXT",                  new QoreBigIntNode(0x843E));
   opengl_ns.addConstant("GL_TANGENT_ARRAY_STRIDE_EXT",                new QoreBigIntNode(0x843F));
   opengl_ns.addConstant("GL_BINORMAL_ARRAY_TYPE_EXT",                 new QoreBigIntNode(0x8440));
   opengl_ns.addConstant("GL_BINORMAL_ARRAY_STRIDE_EXT",               new QoreBigIntNode(0x8441));
   opengl_ns.addConstant("GL_TANGENT_ARRAY_POINTER_EXT",               new QoreBigIntNode(0x8442));
   opengl_ns.addConstant("GL_BINORMAL_ARRAY_POINTER_EXT",              new QoreBigIntNode(0x8443));
   opengl_ns.addConstant("GL_MAP1_TANGENT_EXT",                        new QoreBigIntNode(0x8444));
   opengl_ns.addConstant("GL_MAP2_TANGENT_EXT",                        new QoreBigIntNode(0x8445));
   opengl_ns.addConstant("GL_MAP1_BINORMAL_EXT",                       new QoreBigIntNode(0x8446));
   opengl_ns.addConstant("GL_MAP2_BINORMAL_EXT",                       new QoreBigIntNode(0x8447));
   opengl_ns.addConstant("GL_COMBINE_EXT",                             new QoreBigIntNode(0x8570));
   opengl_ns.addConstant("GL_COMBINE_RGB_EXT",                         new QoreBigIntNode(0x8571));
   opengl_ns.addConstant("GL_COMBINE_ALPHA_EXT",                       new QoreBigIntNode(0x8572));
   opengl_ns.addConstant("GL_RGB_SCALE_EXT",                           new QoreBigIntNode(0x8573));
   opengl_ns.addConstant("GL_ADD_SIGNED_EXT",                          new QoreBigIntNode(0x8574));
   opengl_ns.addConstant("GL_INTERPOLATE_EXT",                         new QoreBigIntNode(0x8575));
   opengl_ns.addConstant("GL_CONSTANT_EXT",                            new QoreBigIntNode(0x8576));
   opengl_ns.addConstant("GL_PRIMARY_COLOR_EXT",                       new QoreBigIntNode(0x8577));
   opengl_ns.addConstant("GL_PREVIOUS_EXT",                            new QoreBigIntNode(0x8578));
   opengl_ns.addConstant("GL_SOURCE0_RGB_EXT",                         new QoreBigIntNode(0x8580));
   opengl_ns.addConstant("GL_SOURCE1_RGB_EXT",                         new QoreBigIntNode(0x8581));
   opengl_ns.addConstant("GL_SOURCE2_RGB_EXT",                         new QoreBigIntNode(0x8582));
   opengl_ns.addConstant("GL_SOURCE3_RGB_EXT",                         new QoreBigIntNode(0x8583));
   opengl_ns.addConstant("GL_SOURCE4_RGB_EXT",                         new QoreBigIntNode(0x8584));
   opengl_ns.addConstant("GL_SOURCE5_RGB_EXT",                         new QoreBigIntNode(0x8585));
   opengl_ns.addConstant("GL_SOURCE6_RGB_EXT",                         new QoreBigIntNode(0x8586));
   opengl_ns.addConstant("GL_SOURCE7_RGB_EXT",                         new QoreBigIntNode(0x8587));
   opengl_ns.addConstant("GL_SOURCE0_ALPHA_EXT",                       new QoreBigIntNode(0x8588));
   opengl_ns.addConstant("GL_SOURCE1_ALPHA_EXT",                       new QoreBigIntNode(0x8589));
   opengl_ns.addConstant("GL_SOURCE2_ALPHA_EXT",                       new QoreBigIntNode(0x858A));
   opengl_ns.addConstant("GL_SOURCE3_ALPHA_EXT",                       new QoreBigIntNode(0x858B));
   opengl_ns.addConstant("GL_SOURCE4_ALPHA_EXT",                       new QoreBigIntNode(0x858C));
   opengl_ns.addConstant("GL_SOURCE5_ALPHA_EXT",                       new QoreBigIntNode(0x858D));
   opengl_ns.addConstant("GL_SOURCE6_ALPHA_EXT",                       new QoreBigIntNode(0x858E));
   opengl_ns.addConstant("GL_SOURCE7_ALPHA_EXT",                       new QoreBigIntNode(0x858F));
   opengl_ns.addConstant("GL_OPERAND0_RGB_EXT",                        new QoreBigIntNode(0x8590));
   opengl_ns.addConstant("GL_OPERAND1_RGB_EXT",                        new QoreBigIntNode(0x8591));
   opengl_ns.addConstant("GL_OPERAND2_RGB_EXT",                        new QoreBigIntNode(0x8592));
   opengl_ns.addConstant("GL_OPERAND3_RGB_EXT",                        new QoreBigIntNode(0x8593));
   opengl_ns.addConstant("GL_OPERAND4_RGB_EXT",                        new QoreBigIntNode(0x8594));
   opengl_ns.addConstant("GL_OPERAND5_RGB_EXT",                        new QoreBigIntNode(0x8595));
   opengl_ns.addConstant("GL_OPERAND6_RGB_EXT",                        new QoreBigIntNode(0x8596));
   opengl_ns.addConstant("GL_OPERAND7_RGB_EXT",                        new QoreBigIntNode(0x8597));
   opengl_ns.addConstant("GL_OPERAND0_ALPHA_EXT",                      new QoreBigIntNode(0x8598));
   opengl_ns.addConstant("GL_OPERAND1_ALPHA_EXT",                      new QoreBigIntNode(0x8599));
   opengl_ns.addConstant("GL_OPERAND2_ALPHA_EXT",                      new QoreBigIntNode(0x859A));
   opengl_ns.addConstant("GL_OPERAND3_ALPHA_EXT",                      new QoreBigIntNode(0x859B));
   opengl_ns.addConstant("GL_OPERAND4_ALPHA_EXT",                      new QoreBigIntNode(0x859C));
   opengl_ns.addConstant("GL_OPERAND5_ALPHA_EXT",                      new QoreBigIntNode(0x859D));
   opengl_ns.addConstant("GL_OPERAND6_ALPHA_EXT",                      new QoreBigIntNode(0x859E));
   opengl_ns.addConstant("GL_OPERAND7_ALPHA_EXT",                      new QoreBigIntNode(0x859F));
   opengl_ns.addConstant("GL_BLEND_DST_RGB_EXT",                       new QoreBigIntNode(0x80C8));
   opengl_ns.addConstant("GL_BLEND_SRC_RGB_EXT",                       new QoreBigIntNode(0x80C9));
   opengl_ns.addConstant("GL_BLEND_DST_ALPHA_EXT",                     new QoreBigIntNode(0x80CA));
   opengl_ns.addConstant("GL_BLEND_SRC_ALPHA_EXT",                     new QoreBigIntNode(0x80CB));
   opengl_ns.addConstant("GL_INCR_WRAP_EXT",                           new QoreBigIntNode(0x8507));
   opengl_ns.addConstant("GL_DECR_WRAP_EXT",                           new QoreBigIntNode(0x8508));
   opengl_ns.addConstant("GL_422_EXT",                                 new QoreBigIntNode(0x80CC));
   opengl_ns.addConstant("GL_422_REV_EXT",                             new QoreBigIntNode(0x80CD));
   opengl_ns.addConstant("GL_422_AVERAGE_EXT",                         new QoreBigIntNode(0x80CE));
   opengl_ns.addConstant("GL_422_REV_AVERAGE_EXT",                     new QoreBigIntNode(0x80CF));
   opengl_ns.addConstant("GL_NORMAL_MAP_EXT",                          new QoreBigIntNode(0x8511));
   opengl_ns.addConstant("GL_REFLECTION_MAP_EXT",                      new QoreBigIntNode(0x8512));
   opengl_ns.addConstant("GL_TEXTURE_CUBE_MAP_EXT",                    new QoreBigIntNode(0x8513));
   opengl_ns.addConstant("GL_TEXTURE_BINDING_CUBE_MAP_EXT",            new QoreBigIntNode(0x8514));
   opengl_ns.addConstant("GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT",         new QoreBigIntNode(0x8515));
   opengl_ns.addConstant("GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT",         new QoreBigIntNode(0x8516));
   opengl_ns.addConstant("GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT",         new QoreBigIntNode(0x8517));
   opengl_ns.addConstant("GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT",         new QoreBigIntNode(0x8518));
   opengl_ns.addConstant("GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT",         new QoreBigIntNode(0x8519));
   opengl_ns.addConstant("GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT",         new QoreBigIntNode(0x851A));
   opengl_ns.addConstant("GL_PROXY_TEXTURE_CUBE_MAP_EXT",              new QoreBigIntNode(0x851B));
   opengl_ns.addConstant("GL_MAX_CUBE_MAP_TEXTURE_SIZE_EXT",           new QoreBigIntNode(0x851C));
   opengl_ns.addConstant("GL_MAX_TEXTURE_LOD_BIAS_EXT",                new QoreBigIntNode(0x84FD));
   opengl_ns.addConstant("GL_TEXTURE_FILTER_CONTROL_EXT",              new QoreBigIntNode(0x8500));
   opengl_ns.addConstant("GL_TEXTURE_LOD_BIAS_EXT",                    new QoreBigIntNode(0x8501));
   opengl_ns.addConstant("GL_TEXTURE_MAX_ANISOTROPY_EXT",              new QoreBigIntNode(0x84FE));
   opengl_ns.addConstant("GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT",          new QoreBigIntNode(0x84FF));
   opengl_ns.addConstant("GL_MODELVIEW0_STACK_DEPTH_EXT",              new QoreBigIntNode(GL_MODELVIEW_STACK_DEPTH));
   opengl_ns.addConstant("GL_MODELVIEW1_STACK_DEPTH_EXT",              new QoreBigIntNode(0x8502));
   opengl_ns.addConstant("GL_MODELVIEW0_MATRIX_EXT",                   new QoreBigIntNode(GL_MODELVIEW_MATRIX));
   opengl_ns.addConstant("GL_MODELVIEW_MATRIX1_EXT",                   new QoreBigIntNode(0x8506));
   opengl_ns.addConstant("GL_VERTEX_WEIGHTING_EXT",                    new QoreBigIntNode(0x8509));
   opengl_ns.addConstant("GL_MODELVIEW0_EXT",                          new QoreBigIntNode(GL_MODELVIEW));
   opengl_ns.addConstant("GL_MODELVIEW1_EXT",                          new QoreBigIntNode(0x850A));
   opengl_ns.addConstant("GL_CURRENT_VERTEX_WEIGHT_EXT",               new QoreBigIntNode(0x850B));
   opengl_ns.addConstant("GL_VERTEX_WEIGHT_ARRAY_EXT",                 new QoreBigIntNode(0x850C));
   opengl_ns.addConstant("GL_VERTEX_WEIGHT_ARRAY_SIZE_EXT",            new QoreBigIntNode(0x850D));
   opengl_ns.addConstant("GL_VERTEX_WEIGHT_ARRAY_TYPE_EXT",            new QoreBigIntNode(0x850E));
   opengl_ns.addConstant("GL_VERTEX_WEIGHT_ARRAY_STRIDE_EXT",          new QoreBigIntNode(0x850F));
   opengl_ns.addConstant("GL_VERTEX_WEIGHT_ARRAY_POINTER_EXT",         new QoreBigIntNode(0x8510));
   opengl_ns.addConstant("GL_COMPRESSED_RGB_S3TC_DXT1_EXT",            new QoreBigIntNode(0x83F0));
   opengl_ns.addConstant("GL_COMPRESSED_RGBA_S3TC_DXT1_EXT",           new QoreBigIntNode(0x83F1));
   opengl_ns.addConstant("GL_COMPRESSED_RGBA_S3TC_DXT3_EXT",           new QoreBigIntNode(0x83F2));
   opengl_ns.addConstant("GL_COMPRESSED_RGBA_S3TC_DXT5_EXT",           new QoreBigIntNode(0x83F3));
   opengl_ns.addConstant("GL_TEXTURE_RECTANGLE_EXT",                   new QoreBigIntNode(0x84F5));
   opengl_ns.addConstant("GL_TEXTURE_BINDING_RECTANGLE_EXT",           new QoreBigIntNode(0x84F6));
   opengl_ns.addConstant("GL_PROXY_TEXTURE_RECTANGLE_EXT",             new QoreBigIntNode(0x84F7));
   opengl_ns.addConstant("GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT",          new QoreBigIntNode(0x84F8));
   opengl_ns.addConstant("GL_SRGB_EXT",                                new QoreBigIntNode(0x8C40));
   opengl_ns.addConstant("GL_SRGB8_EXT",                               new QoreBigIntNode(0x8C41));
   opengl_ns.addConstant("GL_SRGB_ALPHA_EXT",                          new QoreBigIntNode(0x8C42));
   opengl_ns.addConstant("GL_SRGB8_ALPHA8_EXT",                        new QoreBigIntNode(0x8C43));
   opengl_ns.addConstant("GL_SLUMINANCE_ALPHA_EXT",                    new QoreBigIntNode(0x8C44));
   opengl_ns.addConstant("GL_SLUMINANCE8_ALPHA8_EXT",                  new QoreBigIntNode(0x8C45));
   opengl_ns.addConstant("GL_SLUMINANCE_EXT",                          new QoreBigIntNode(0x8C46));
   opengl_ns.addConstant("GL_SLUMINANCE8_EXT",                         new QoreBigIntNode(0x8C47));
   opengl_ns.addConstant("GL_COMPRESSED_SRGB_EXT",                     new QoreBigIntNode(0x8C48));
   opengl_ns.addConstant("GL_COMPRESSED_SRGB_ALPHA_EXT",               new QoreBigIntNode(0x8C49));
   opengl_ns.addConstant("GL_COMPRESSED_SLUMINANCE_EXT",               new QoreBigIntNode(0x8C4A));
   opengl_ns.addConstant("GL_COMPRESSED_SLUMINANCE_ALPHA_EXT",         new QoreBigIntNode(0x8C4B));
   opengl_ns.addConstant("GL_COMPRESSED_SRGB_S3TC_DXT1_EXT",           new QoreBigIntNode(0x8C4C));
   opengl_ns.addConstant("GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT",     new QoreBigIntNode(0x8C4D));
   opengl_ns.addConstant("GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT",     new QoreBigIntNode(0x8C4E));
   opengl_ns.addConstant("GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT",     new QoreBigIntNode(0x8C4F));
   opengl_ns.addConstant("GL_VERTEX_SHADER_EXT",                       new QoreBigIntNode(0x8780));
   opengl_ns.addConstant("GL_VARIANT_VALUE_EXT",                       new QoreBigIntNode(0x87E4));
   opengl_ns.addConstant("GL_VARIANT_DATATYPE_EXT",                    new QoreBigIntNode(0x87E5));
   opengl_ns.addConstant("GL_VARIANT_ARRAY_STRIDE_EXT",                new QoreBigIntNode(0x87E6));
   opengl_ns.addConstant("GL_VARIANT_ARRAY_TYPE_EXT",                  new QoreBigIntNode(0x87E7));
   opengl_ns.addConstant("GL_VARIANT_ARRAY_EXT",                       new QoreBigIntNode(0x87E8));
   opengl_ns.addConstant("GL_VARIANT_ARRAY_POINTER_EXT",               new QoreBigIntNode(0x87E9));
   opengl_ns.addConstant("GL_INVARIANT_VALUE_EXT",                     new QoreBigIntNode(0x87EA));
   opengl_ns.addConstant("GL_INVARIANT_DATATYPE_EXT",                  new QoreBigIntNode(0x87EB));
   opengl_ns.addConstant("GL_LOCAL_CONSTANT_VALUE_EXT",                new QoreBigIntNode(0x87EC));
   opengl_ns.addConstant("GL_LOCAL_CONSTANT_DATATYPE_EXT",             new QoreBigIntNode(0x87Ed));
   opengl_ns.addConstant("GL_OP_INDEX_EXT",                            new QoreBigIntNode(0x8782));
   opengl_ns.addConstant("GL_OP_NEGATE_EXT",                           new QoreBigIntNode(0x8783));
   opengl_ns.addConstant("GL_OP_DOT3_EXT",                             new QoreBigIntNode(0x8784));
   opengl_ns.addConstant("GL_OP_DOT4_EXT",                             new QoreBigIntNode(0x8785));
   opengl_ns.addConstant("GL_OP_MUL_EXT",                              new QoreBigIntNode(0x8786));
   opengl_ns.addConstant("GL_OP_ADD_EXT",                              new QoreBigIntNode(0x8787));
   opengl_ns.addConstant("GL_OP_MADD_EXT",                             new QoreBigIntNode(0x8788));
   opengl_ns.addConstant("GL_OP_FRAC_EXT",                             new QoreBigIntNode(0x8789));
   opengl_ns.addConstant("GL_OP_MAX_EXT",                              new QoreBigIntNode(0x878A));
   opengl_ns.addConstant("GL_OP_MIN_EXT",                              new QoreBigIntNode(0x878B));
   opengl_ns.addConstant("GL_OP_SET_GE_EXT",                           new QoreBigIntNode(0x878C));
   opengl_ns.addConstant("GL_OP_SET_LT_EXT",                           new QoreBigIntNode(0x878D));
   opengl_ns.addConstant("GL_OP_CLAMP_EXT",                            new QoreBigIntNode(0x878E));
   opengl_ns.addConstant("GL_OP_FLOOR_EXT",                            new QoreBigIntNode(0x878F));
   opengl_ns.addConstant("GL_OP_ROUND_EXT",                            new QoreBigIntNode(0x8790));
   opengl_ns.addConstant("GL_OP_EXP_BASE_2_EXT",                       new QoreBigIntNode(0x8791));
   opengl_ns.addConstant("GL_OP_LOG_BASE_2_EXT",                       new QoreBigIntNode(0x8792));
   opengl_ns.addConstant("GL_OP_POWER_EXT",                            new QoreBigIntNode(0x8793));
   opengl_ns.addConstant("GL_OP_RECIP_EXT",                            new QoreBigIntNode(0x8794));
   opengl_ns.addConstant("GL_OP_RECIP_SQRT_EXT",                       new QoreBigIntNode(0x8795));
   opengl_ns.addConstant("GL_OP_SUB_EXT",                              new QoreBigIntNode(0x8796));
   opengl_ns.addConstant("GL_OP_CROSS_PRODUCT_EXT",                    new QoreBigIntNode(0x8797));
   opengl_ns.addConstant("GL_OP_MULTIPLY_MATRIX_EXT",                  new QoreBigIntNode(0x8798));
   opengl_ns.addConstant("GL_OP_MOV_EXT",                              new QoreBigIntNode(0x8799));
   opengl_ns.addConstant("GL_OUTPUT_VERTEX_EXT",                       new QoreBigIntNode(0x879A));
   opengl_ns.addConstant("GL_OUTPUT_COLOR0_EXT",                       new QoreBigIntNode(0x879B));
   opengl_ns.addConstant("GL_OUTPUT_COLOR1_EXT",                       new QoreBigIntNode(0x879C));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD0_EXT",               new QoreBigIntNode(0x879D));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD1_EXT",               new QoreBigIntNode(0x879E));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD2_EXT",               new QoreBigIntNode(0x879F));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD3_EXT",               new QoreBigIntNode(0x87A0));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD4_EXT",               new QoreBigIntNode(0x87A1));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD5_EXT",               new QoreBigIntNode(0x87A2));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD6_EXT",               new QoreBigIntNode(0x87A3));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD7_EXT",               new QoreBigIntNode(0x87A4));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD8_EXT",               new QoreBigIntNode(0x87A5));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD9_EXT",               new QoreBigIntNode(0x87A6));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD10_EXT",              new QoreBigIntNode(0x87A7));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD11_EXT",              new QoreBigIntNode(0x87A8));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD12_EXT",              new QoreBigIntNode(0x87A9));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD13_EXT",              new QoreBigIntNode(0x87AA));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD14_EXT",              new QoreBigIntNode(0x87AB));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD15_EXT",              new QoreBigIntNode(0x87AC));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD16_EXT",              new QoreBigIntNode(0x87AD));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD17_EXT",              new QoreBigIntNode(0x87AE));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD18_EXT",              new QoreBigIntNode(0x87AF));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD19_EXT",              new QoreBigIntNode(0x87B0));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD20_EXT",              new QoreBigIntNode(0x87B1));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD21_EXT",              new QoreBigIntNode(0x87B2));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD22_EXT",              new QoreBigIntNode(0x87B3));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD23_EXT",              new QoreBigIntNode(0x87B4));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD24_EXT",              new QoreBigIntNode(0x87B5));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD25_EXT",              new QoreBigIntNode(0x87B6));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD26_EXT",              new QoreBigIntNode(0x87B7));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD27_EXT",              new QoreBigIntNode(0x87B8));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD28_EXT",              new QoreBigIntNode(0x87B9));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD29_EXT",              new QoreBigIntNode(0x87BA));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD30_EXT",              new QoreBigIntNode(0x87BB));
   opengl_ns.addConstant("GL_OUTPUT_TEXTURE_COORD31_EXT",              new QoreBigIntNode(0x87BC));
   opengl_ns.addConstant("GL_OUTPUT_FOG_EXT",                          new QoreBigIntNode(0x87BD));
   opengl_ns.addConstant("GL_SCALAR_EXT",                              new QoreBigIntNode(0x87BE));
   opengl_ns.addConstant("GL_VECTOR_EXT",                              new QoreBigIntNode(0x87BF));
   opengl_ns.addConstant("GL_MATRIX_EXT",                              new QoreBigIntNode(0x87C0));
   opengl_ns.addConstant("GL_VARIANT_EXT",                             new QoreBigIntNode(0x87C1));
   opengl_ns.addConstant("GL_INVARIANT_EXT",                           new QoreBigIntNode(0x87C2));
   opengl_ns.addConstant("GL_LOCAL_CONSTANT_EXT",                      new QoreBigIntNode(0x87C3));
   opengl_ns.addConstant("GL_LOCAL_EXT",                               new QoreBigIntNode(0x87C4));
   opengl_ns.addConstant("GL_MAX_VERTEX_SHADER_INSTRUCTIONS_EXT",      new QoreBigIntNode(0x87C5));
   opengl_ns.addConstant("GL_MAX_VERTEX_SHADER_VARIANTS_EXT",          new QoreBigIntNode(0x87C6));
   opengl_ns.addConstant("GL_MAX_VERTEX_SHADER_INVARIANTS_EXT",        new QoreBigIntNode(0x87C7));
   opengl_ns.addConstant("GL_MAX_VERTEX_SHADER_LOCAL_CONSTANTS_EXT",   new QoreBigIntNode(0x87C8));
   opengl_ns.addConstant("GL_MAX_VERTEX_SHADER_LOCALS_EXT",            new QoreBigIntNode(0x87C9));
   opengl_ns.addConstant("GL_MAX_OPTIMIZED_VERTEX_SHADER_INSTRUCTIONS_EXT",new QoreBigIntNode(0x87CA));
   opengl_ns.addConstant("GL_MAX_OPTIMIZED_VERTEX_SHADER_VARIANTS_EXT",new QoreBigIntNode(0x87CB));
   opengl_ns.addConstant("GL_MAX_OPTIMIZED_VERTEX_SHADER_LOCAL_CONSTANTS_EXT",new QoreBigIntNode(0x87CC));
   opengl_ns.addConstant("GL_MAX_OPTIMIZED_VERTEX_SHADER_INVARIANTS_EXT",new QoreBigIntNode(0x87CD));
   opengl_ns.addConstant("GL_MAX_OPTIMIZED_VERTEX_SHADER_LOCALS_EXT",  new QoreBigIntNode(0x87CE));
   opengl_ns.addConstant("GL_VERTEX_SHADER_INSTRUCTIONS_EXT",          new QoreBigIntNode(0x87CF));
   opengl_ns.addConstant("GL_VERTEX_SHADER_VARIANTS_EXT",              new QoreBigIntNode(0x87D0));
   opengl_ns.addConstant("GL_VERTEX_SHADER_INVARIANTS_EXT",            new QoreBigIntNode(0x87D1));
   opengl_ns.addConstant("GL_VERTEX_SHADER_LOCAL_CONSTANTS_EXT",       new QoreBigIntNode(0x87D2));
   opengl_ns.addConstant("GL_VERTEX_SHADER_LOCALS_EXT",                new QoreBigIntNode(0x87D3));
   opengl_ns.addConstant("GL_VERTEX_SHADER_BINDING_EXT",               new QoreBigIntNode(0x8781));
   opengl_ns.addConstant("GL_VERTEX_SHADER_OPTIMIZED_EXT",             new QoreBigIntNode(0x87D4));
   opengl_ns.addConstant("GL_X_EXT",                                   new QoreBigIntNode(0x87D5));
   opengl_ns.addConstant("GL_Y_EXT",                                   new QoreBigIntNode(0x87D6));
   opengl_ns.addConstant("GL_Z_EXT",                                   new QoreBigIntNode(0x87D7));
   opengl_ns.addConstant("GL_W_EXT",                                   new QoreBigIntNode(0x87D8));
   opengl_ns.addConstant("GL_NEGATIVE_X_EXT",                          new QoreBigIntNode(0x87D9));
   opengl_ns.addConstant("GL_NEGATIVE_Y_EXT",                          new QoreBigIntNode(0x87DA));
   opengl_ns.addConstant("GL_NEGATIVE_Z_EXT",                          new QoreBigIntNode(0x87DB));
   opengl_ns.addConstant("GL_NEGATIVE_W_EXT",                          new QoreBigIntNode(0x87DC));
   opengl_ns.addConstant("GL_NEGATIVE_ONE_EXT",                        new QoreBigIntNode(0x87DF));
   opengl_ns.addConstant("GL_NORMALIZED_RANGE_EXT",                    new QoreBigIntNode(0x87E0));
   opengl_ns.addConstant("GL_FULL_RANGE_EXT",                          new QoreBigIntNode(0x87E1));
   opengl_ns.addConstant("GL_CURRENT_VERTEX_EXT",                      new QoreBigIntNode(0x87E2));
   opengl_ns.addConstant("GL_MVP_MATRIX_EXT",                          new QoreBigIntNode(0x87E3));
   opengl_ns.addConstant("GL_FRAGMENT_SHADER_EXT",                     new QoreBigIntNode(0x8920));
   opengl_ns.addConstant("GL_REG_0_EXT",                               new QoreBigIntNode(0x8921));
   opengl_ns.addConstant("GL_REG_1_EXT",                               new QoreBigIntNode(0x8922));
   opengl_ns.addConstant("GL_REG_2_EXT",                               new QoreBigIntNode(0x8923));
   opengl_ns.addConstant("GL_REG_3_EXT",                               new QoreBigIntNode(0x8924));
   opengl_ns.addConstant("GL_REG_4_EXT",                               new QoreBigIntNode(0x8925));
   opengl_ns.addConstant("GL_REG_5_EXT",                               new QoreBigIntNode(0x8926));
   opengl_ns.addConstant("GL_REG_6_EXT",                               new QoreBigIntNode(0x8927));
   opengl_ns.addConstant("GL_REG_7_EXT",                               new QoreBigIntNode(0x8928));
   opengl_ns.addConstant("GL_REG_8_EXT",                               new QoreBigIntNode(0x8929));
   opengl_ns.addConstant("GL_REG_9_EXT",                               new QoreBigIntNode(0x892A));
   opengl_ns.addConstant("GL_REG_10_EXT",                              new QoreBigIntNode(0x892B));
   opengl_ns.addConstant("GL_REG_11_EXT",                              new QoreBigIntNode(0x892C));
   opengl_ns.addConstant("GL_REG_12_EXT",                              new QoreBigIntNode(0x892D));
   opengl_ns.addConstant("GL_REG_13_EXT",                              new QoreBigIntNode(0x892E));
   opengl_ns.addConstant("GL_REG_14_EXT",                              new QoreBigIntNode(0x892F));
   opengl_ns.addConstant("GL_REG_15_EXT",                              new QoreBigIntNode(0x8930));
   opengl_ns.addConstant("GL_REG_16_EXT",                              new QoreBigIntNode(0x8931));
   opengl_ns.addConstant("GL_REG_17_EXT",                              new QoreBigIntNode(0x8932));
   opengl_ns.addConstant("GL_REG_18_EXT",                              new QoreBigIntNode(0x8933));
   opengl_ns.addConstant("GL_REG_19_EXT",                              new QoreBigIntNode(0x8934));
   opengl_ns.addConstant("GL_REG_20_EXT",                              new QoreBigIntNode(0x8935));
   opengl_ns.addConstant("GL_REG_21_EXT",                              new QoreBigIntNode(0x8936));
   opengl_ns.addConstant("GL_REG_22_EXT",                              new QoreBigIntNode(0x8937));
   opengl_ns.addConstant("GL_REG_23_EXT",                              new QoreBigIntNode(0x8938));
   opengl_ns.addConstant("GL_REG_24_EXT",                              new QoreBigIntNode(0x8939));
   opengl_ns.addConstant("GL_REG_25_EXT",                              new QoreBigIntNode(0x893A));
   opengl_ns.addConstant("GL_REG_26_EXT",                              new QoreBigIntNode(0x893B));
   opengl_ns.addConstant("GL_REG_27_EXT",                              new QoreBigIntNode(0x893C));
   opengl_ns.addConstant("GL_REG_28_EXT",                              new QoreBigIntNode(0x893D));
   opengl_ns.addConstant("GL_REG_29_EXT",                              new QoreBigIntNode(0x893E));
   opengl_ns.addConstant("GL_REG_30_EXT",                              new QoreBigIntNode(0x893F));
   opengl_ns.addConstant("GL_REG_31_EXT",                              new QoreBigIntNode(0x8940));
   opengl_ns.addConstant("GL_CON_0_EXT",                               new QoreBigIntNode(0x8941));
   opengl_ns.addConstant("GL_CON_1_EXT",                               new QoreBigIntNode(0x8942));
   opengl_ns.addConstant("GL_CON_2_EXT",                               new QoreBigIntNode(0x8943));
   opengl_ns.addConstant("GL_CON_3_EXT",                               new QoreBigIntNode(0x8944));
   opengl_ns.addConstant("GL_CON_4_EXT",                               new QoreBigIntNode(0x8945));
   opengl_ns.addConstant("GL_CON_5_EXT",                               new QoreBigIntNode(0x8946));
   opengl_ns.addConstant("GL_CON_6_EXT",                               new QoreBigIntNode(0x8947));
   opengl_ns.addConstant("GL_CON_7_EXT",                               new QoreBigIntNode(0x8948));
   opengl_ns.addConstant("GL_CON_8_EXT",                               new QoreBigIntNode(0x8949));
   opengl_ns.addConstant("GL_CON_9_EXT",                               new QoreBigIntNode(0x894A));
   opengl_ns.addConstant("GL_CON_10_EXT",                              new QoreBigIntNode(0x894B));
   opengl_ns.addConstant("GL_CON_11_EXT",                              new QoreBigIntNode(0x894C));
   opengl_ns.addConstant("GL_CON_12_EXT",                              new QoreBigIntNode(0x894D));
   opengl_ns.addConstant("GL_CON_13_EXT",                              new QoreBigIntNode(0x894E));
   opengl_ns.addConstant("GL_CON_14_EXT",                              new QoreBigIntNode(0x894F));
   opengl_ns.addConstant("GL_CON_15_EXT",                              new QoreBigIntNode(0x8950));
   opengl_ns.addConstant("GL_CON_16_EXT",                              new QoreBigIntNode(0x8951));
   opengl_ns.addConstant("GL_CON_17_EXT",                              new QoreBigIntNode(0x8952));
   opengl_ns.addConstant("GL_CON_18_EXT",                              new QoreBigIntNode(0x8953));
   opengl_ns.addConstant("GL_CON_19_EXT",                              new QoreBigIntNode(0x8954));
   opengl_ns.addConstant("GL_CON_20_EXT",                              new QoreBigIntNode(0x8955));
   opengl_ns.addConstant("GL_CON_21_EXT",                              new QoreBigIntNode(0x8956));
   opengl_ns.addConstant("GL_CON_22_EXT",                              new QoreBigIntNode(0x8957));
   opengl_ns.addConstant("GL_CON_23_EXT",                              new QoreBigIntNode(0x8958));
   opengl_ns.addConstant("GL_CON_24_EXT",                              new QoreBigIntNode(0x8959));
   opengl_ns.addConstant("GL_CON_25_EXT",                              new QoreBigIntNode(0x895A));
   opengl_ns.addConstant("GL_CON_26_EXT",                              new QoreBigIntNode(0x895B));
   opengl_ns.addConstant("GL_CON_27_EXT",                              new QoreBigIntNode(0x895C));
   opengl_ns.addConstant("GL_CON_28_EXT",                              new QoreBigIntNode(0x895D));
   opengl_ns.addConstant("GL_CON_29_EXT",                              new QoreBigIntNode(0x895E));
   opengl_ns.addConstant("GL_CON_30_EXT",                              new QoreBigIntNode(0x895F));
   opengl_ns.addConstant("GL_CON_31_EXT",                              new QoreBigIntNode(0x8960));
   opengl_ns.addConstant("GL_MOV_EXT",                                 new QoreBigIntNode(0x8961));
   opengl_ns.addConstant("GL_ADD_EXT",                                 new QoreBigIntNode(0x8963));
   opengl_ns.addConstant("GL_MUL_EXT",                                 new QoreBigIntNode(0x8964));
   opengl_ns.addConstant("GL_SUB_EXT",                                 new QoreBigIntNode(0x8965));
   opengl_ns.addConstant("GL_DOT3_EXT",                                new QoreBigIntNode(0x8966));
   opengl_ns.addConstant("GL_DOT4_EXT",                                new QoreBigIntNode(0x8967));
   opengl_ns.addConstant("GL_MAD_EXT",                                 new QoreBigIntNode(0x8968));
   opengl_ns.addConstant("GL_LERP_EXT",                                new QoreBigIntNode(0x8969));
   opengl_ns.addConstant("GL_CND_EXT",                                 new QoreBigIntNode(0x896A));
   opengl_ns.addConstant("GL_CND0_EXT",                                new QoreBigIntNode(0x896B));
   opengl_ns.addConstant("GL_DOT2_ADD_EXT",                            new QoreBigIntNode(0x896C));
   opengl_ns.addConstant("GL_SECONDARY_INTERPOLATOR_EXT",              new QoreBigIntNode(0x896D));
   opengl_ns.addConstant("GL_NUM_FRAGMENT_REGISTERS_EXT",              new QoreBigIntNode(0x896E));
   opengl_ns.addConstant("GL_NUM_FRAGMENT_CONSTANTS_EXT",              new QoreBigIntNode(0x896F));
   opengl_ns.addConstant("GL_NUM_PASSES_EXT",                          new QoreBigIntNode(0x8970));
   opengl_ns.addConstant("GL_NUM_INSTRUCTIONS_PER_PASS_EXT",           new QoreBigIntNode(0x8971));
   opengl_ns.addConstant("GL_NUM_INSTRUCTIONS_TOTAL_EXT",              new QoreBigIntNode(0x8972));
   opengl_ns.addConstant("GL_NUM_INPUT_INTERPOLATOR_COMPONENTS_EXT",   new QoreBigIntNode(0x8973));
   opengl_ns.addConstant("GL_NUM_LOOPBACK_COMPONENTS_EXT",             new QoreBigIntNode(0x8974));
   opengl_ns.addConstant("GL_COLOR_ALPHA_PAIRING_EXT",                 new QoreBigIntNode(0x8975));
   opengl_ns.addConstant("GL_SWIZZLE_STR_EXT",                         new QoreBigIntNode(0x8976));
   opengl_ns.addConstant("GL_SWIZZLE_STQ_EXT",                         new QoreBigIntNode(0x8977));
   opengl_ns.addConstant("GL_SWIZZLE_STR_DR_EXT",                      new QoreBigIntNode(0x8978));
   opengl_ns.addConstant("GL_SWIZZLE_STQ_DQ_EXT",                      new QoreBigIntNode(0x8979));
   opengl_ns.addConstant("GL_SWIZZLE_STRQ_EXT",                        new QoreBigIntNode(0x897A));
   opengl_ns.addConstant("GL_SWIZZLE_STRQ_DQ_EXT",                     new QoreBigIntNode(0x897B));
   opengl_ns.addConstant("GL_RED_BIT_EXT",                             new QoreBigIntNode(0x00000001));
   opengl_ns.addConstant("GL_GREEN_BIT_EXT",                           new QoreBigIntNode(0x00000002));
   opengl_ns.addConstant("GL_BLUE_BIT_EXT",                            new QoreBigIntNode(0x00000004));
   opengl_ns.addConstant("GL_2X_BIT_EXT",                              new QoreBigIntNode(0x00000001));
   opengl_ns.addConstant("GL_4X_BIT_EXT",                              new QoreBigIntNode(0x00000002));
   opengl_ns.addConstant("GL_8X_BIT_EXT",                              new QoreBigIntNode(0x00000004));
   opengl_ns.addConstant("GL_HALF_BIT_EXT",                            new QoreBigIntNode(0x00000008));
   opengl_ns.addConstant("GL_QUARTER_BIT_EXT",                         new QoreBigIntNode(0x00000010));
   opengl_ns.addConstant("GL_EIGHTH_BIT_EXT",                          new QoreBigIntNode(0x00000020));
   opengl_ns.addConstant("GL_SATURATE_BIT_EXT",                        new QoreBigIntNode(0x00000040));
   opengl_ns.addConstant("GL_COMP_BIT_EXT",                            new QoreBigIntNode(0x00000002));
   opengl_ns.addConstant("GL_NEGATE_BIT_EXT",                          new QoreBigIntNode(0x00000004));
   opengl_ns.addConstant("GL_BIAS_BIT_EXT",                            new QoreBigIntNode(0x00000008));
   opengl_ns.addConstant("GL_MULTISAMPLE_EXT",                         new QoreBigIntNode(0x809D));
   opengl_ns.addConstant("GL_SAMPLE_ALPHA_TO_MASK_EXT",                new QoreBigIntNode(0x809E));
   opengl_ns.addConstant("GL_SAMPLE_ALPHA_TO_ONE_EXT",                 new QoreBigIntNode(0x809F));
   opengl_ns.addConstant("GL_SAMPLE_MASK_EXT",                         new QoreBigIntNode(0x80A0));
   opengl_ns.addConstant("GL_1PASS_EXT",                               new QoreBigIntNode(0x80A1));
   opengl_ns.addConstant("GL_2PASS_0_EXT",                             new QoreBigIntNode(0x80A2));
   opengl_ns.addConstant("GL_2PASS_1_EXT",                             new QoreBigIntNode(0x80A3));
   opengl_ns.addConstant("GL_4PASS_0_EXT",                             new QoreBigIntNode(0x80A4));
   opengl_ns.addConstant("GL_4PASS_1_EXT",                             new QoreBigIntNode(0x80A5));
   opengl_ns.addConstant("GL_4PASS_2_EXT",                             new QoreBigIntNode(0x80A6));
   opengl_ns.addConstant("GL_4PASS_3_EXT",                             new QoreBigIntNode(0x80A7));
   opengl_ns.addConstant("GL_SAMPLE_BUFFERS_EXT",                      new QoreBigIntNode(0x80A8));
   opengl_ns.addConstant("GL_SAMPLES_EXT",                             new QoreBigIntNode(0x80A9));
   opengl_ns.addConstant("GL_SAMPLE_MASK_VALUE_EXT",                   new QoreBigIntNode(0x80AA));
   opengl_ns.addConstant("GL_SAMPLE_MASK_INVERT_EXT",                  new QoreBigIntNode(0x80AB));
   opengl_ns.addConstant("GL_SAMPLE_PATTERN_EXT",                      new QoreBigIntNode(0x80AC));
   opengl_ns.addConstant("GL_STENCIL_TEST_TWO_SIDE_EXT",               new QoreBigIntNode(0x8910));
   opengl_ns.addConstant("GL_ACTIVE_STENCIL_FACE_EXT",                 new QoreBigIntNode(0x8911));
   opengl_ns.addConstant("GL_DEPTH_BOUNDS_TEST_EXT",                   new QoreBigIntNode(0x8890));
   opengl_ns.addConstant("GL_DEPTH_BOUNDS_EXT",                        new QoreBigIntNode(0x8891));
   opengl_ns.addConstant("GL_BLEND_EQUATION_RGB_EXT",                  new QoreBigIntNode(0x8009));
   opengl_ns.addConstant("GL_BLEND_EQUATION_ALPHA_EXT",                new QoreBigIntNode(0x883D));
   opengl_ns.addConstant("GL_MIRROR_CLAMP_EXT",                        new QoreBigIntNode(0x8742));
   opengl_ns.addConstant("GL_MIRROR_CLAMP_TO_EDGE_EXT",                new QoreBigIntNode(0x8743));
   opengl_ns.addConstant("GL_MIRROR_CLAMP_TO_BORDER_EXT",              new QoreBigIntNode(0x8912));
   opengl_ns.addConstant("GL_FRAMEBUFFER_EXT",                         new QoreBigIntNode(0x8D40));
   opengl_ns.addConstant("GL_RENDERBUFFER_EXT",                        new QoreBigIntNode(0x8D41));
   opengl_ns.addConstant("GL_STENCIL_INDEX1_EXT",                      new QoreBigIntNode(0x8D46));
   opengl_ns.addConstant("GL_STENCIL_INDEX4_EXT",                      new QoreBigIntNode(0x8D47));
   opengl_ns.addConstant("GL_STENCIL_INDEX8_EXT",                      new QoreBigIntNode(0x8D48));
   opengl_ns.addConstant("GL_STENCIL_INDEX16_EXT",                     new QoreBigIntNode(0x8D49));
   opengl_ns.addConstant("GL_RENDERBUFFER_WIDTH_EXT",                  new QoreBigIntNode(0x8D42));
   opengl_ns.addConstant("GL_RENDERBUFFER_HEIGHT_EXT",                 new QoreBigIntNode(0x8D43));
   opengl_ns.addConstant("GL_RENDERBUFFER_INTERNAL_FORMAT_EXT",        new QoreBigIntNode(0x8D44));
   opengl_ns.addConstant("GL_RENDERBUFFER_RED_SIZE_EXT",               new QoreBigIntNode(0x8D50));
   opengl_ns.addConstant("GL_RENDERBUFFER_GREEN_SIZE_EXT",             new QoreBigIntNode(0x8D51));
   opengl_ns.addConstant("GL_RENDERBUFFER_BLUE_SIZE_EXT",              new QoreBigIntNode(0x8D52));
   opengl_ns.addConstant("GL_RENDERBUFFER_ALPHA_SIZE_EXT",             new QoreBigIntNode(0x8D53));
   opengl_ns.addConstant("GL_RENDERBUFFER_DEPTH_SIZE_EXT",             new QoreBigIntNode(0x8D54));
   opengl_ns.addConstant("GL_RENDERBUFFER_STENCIL_SIZE_EXT",           new QoreBigIntNode(0x8D55));
   opengl_ns.addConstant("GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT",  new QoreBigIntNode(0x8CD0));
   opengl_ns.addConstant("GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT",  new QoreBigIntNode(0x8CD1));
   opengl_ns.addConstant("GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT",new QoreBigIntNode(0x8CD2));
   opengl_ns.addConstant("GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT",new QoreBigIntNode(0x8CD3));
   opengl_ns.addConstant("GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT",new QoreBigIntNode(0x8CD4));
   opengl_ns.addConstant("GL_COLOR_ATTACHMENT0_EXT",                   new QoreBigIntNode(0x8CE0));
   opengl_ns.addConstant("GL_COLOR_ATTACHMENT1_EXT",                   new QoreBigIntNode(0x8CE1));
   opengl_ns.addConstant("GL_COLOR_ATTACHMENT2_EXT",                   new QoreBigIntNode(0x8CE2));
   opengl_ns.addConstant("GL_COLOR_ATTACHMENT3_EXT",                   new QoreBigIntNode(0x8CE3));
   opengl_ns.addConstant("GL_COLOR_ATTACHMENT4_EXT",                   new QoreBigIntNode(0x8CE4));
   opengl_ns.addConstant("GL_COLOR_ATTACHMENT5_EXT",                   new QoreBigIntNode(0x8CE5));
   opengl_ns.addConstant("GL_COLOR_ATTACHMENT6_EXT",                   new QoreBigIntNode(0x8CE6));
   opengl_ns.addConstant("GL_COLOR_ATTACHMENT7_EXT",                   new QoreBigIntNode(0x8CE7));
   opengl_ns.addConstant("GL_COLOR_ATTACHMENT8_EXT",                   new QoreBigIntNode(0x8CE8));
   opengl_ns.addConstant("GL_COLOR_ATTACHMENT9_EXT",                   new QoreBigIntNode(0x8CE9));
   opengl_ns.addConstant("GL_COLOR_ATTACHMENT10_EXT",                  new QoreBigIntNode(0x8CEA));
   opengl_ns.addConstant("GL_COLOR_ATTACHMENT11_EXT",                  new QoreBigIntNode(0x8CEB));
   opengl_ns.addConstant("GL_COLOR_ATTACHMENT12_EXT",                  new QoreBigIntNode(0x8CEC));
   opengl_ns.addConstant("GL_COLOR_ATTACHMENT13_EXT",                  new QoreBigIntNode(0x8CED));
   opengl_ns.addConstant("GL_COLOR_ATTACHMENT14_EXT",                  new QoreBigIntNode(0x8CEE));
   opengl_ns.addConstant("GL_COLOR_ATTACHMENT15_EXT",                  new QoreBigIntNode(0x8CEF));
   opengl_ns.addConstant("GL_DEPTH_ATTACHMENT_EXT",                    new QoreBigIntNode(0x8D00));
   opengl_ns.addConstant("GL_STENCIL_ATTACHMENT_EXT",                  new QoreBigIntNode(0x8D20));
   opengl_ns.addConstant("GL_FRAMEBUFFER_COMPLETE_EXT",                new QoreBigIntNode(0x8CD5));
   opengl_ns.addConstant("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT",   new QoreBigIntNode(0x8CD6));
   opengl_ns.addConstant("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT",new QoreBigIntNode(0x8CD7));
   opengl_ns.addConstant("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT",   new QoreBigIntNode(0x8CD9));
   opengl_ns.addConstant("GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT",      new QoreBigIntNode(0x8CDA));
   opengl_ns.addConstant("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT",  new QoreBigIntNode(0x8CDB));
   opengl_ns.addConstant("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT",  new QoreBigIntNode(0x8CDC));
   opengl_ns.addConstant("GL_FRAMEBUFFER_UNSUPPORTED_EXT",             new QoreBigIntNode(0x8CDD));
   opengl_ns.addConstant("GL_FRAMEBUFFER_BINDING_EXT",                 new QoreBigIntNode(0x8CA6));
   opengl_ns.addConstant("GL_RENDERBUFFER_BINDING_EXT",                new QoreBigIntNode(0x8CA7));
   opengl_ns.addConstant("GL_MAX_COLOR_ATTACHMENTS_EXT",               new QoreBigIntNode(0x8CDF));
   opengl_ns.addConstant("GL_MAX_RENDERBUFFER_SIZE_EXT",               new QoreBigIntNode(0x84E8));
   opengl_ns.addConstant("GL_INVALID_FRAMEBUFFER_OPERATION_EXT",       new QoreBigIntNode(0x0506));
   opengl_ns.addConstant("GL_READ_FRAMEBUFFER_EXT",                    new QoreBigIntNode(0x8CA8));
   opengl_ns.addConstant("GL_DRAW_FRAMEBUFFER_EXT",                    new QoreBigIntNode(0x8CA9));
   opengl_ns.addConstant("GL_DRAW_FRAMEBUFFER_BINDING_EXT",            new QoreBigIntNode(0x8CA6));
   opengl_ns.addConstant("GL_READ_FRAMEBUFFER_BINDING_EXT",            new QoreBigIntNode(0x8CAA));
   opengl_ns.addConstant("GL_RENDERBUFFER_SAMPLES_EXT",                new QoreBigIntNode(0x8CAB));
   opengl_ns.addConstant("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT",  new QoreBigIntNode(0x8D56));
   opengl_ns.addConstant("GL_MAX_SAMPLES_EXT",                         new QoreBigIntNode(0x8D57));
   opengl_ns.addConstant("GL_DEPTH_STENCIL_EXT",                       new QoreBigIntNode(0x84F9));
   opengl_ns.addConstant("GL_UNSIGNED_INT_24_8_EXT",                   new QoreBigIntNode(0x84FA));
   opengl_ns.addConstant("GL_DEPTH24_STENCIL8_EXT",                    new QoreBigIntNode(0x88F0));
   opengl_ns.addConstant("GL_TEXTURE_STENCIL_SIZE_EXT",                new QoreBigIntNode(0x88F1));
   opengl_ns.addConstant("GL_GEOMETRY_SHADER_EXT",                     new QoreBigIntNode(0x8DD9));
   opengl_ns.addConstant("GL_GEOMETRY_VERTICES_OUT_EXT",               new QoreBigIntNode(0x8DDA));
   opengl_ns.addConstant("GL_GEOMETRY_INPUT_TYPE_EXT",                 new QoreBigIntNode(0x8DDB));
   opengl_ns.addConstant("GL_GEOMETRY_OUTPUT_TYPE_EXT",                new QoreBigIntNode(0x8DDC));
   opengl_ns.addConstant("GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS_EXT",    new QoreBigIntNode(0x8C29));
   opengl_ns.addConstant("GL_MAX_GEOMETRY_VARYING_COMPONENTS_EXT",     new QoreBigIntNode(0x8DDD));
   opengl_ns.addConstant("GL_MAX_VERTEX_VARYING_COMPONENTS_EXT",       new QoreBigIntNode(0x8DDE));
   opengl_ns.addConstant("GL_MAX_VARYING_COMPONENTS_EXT",              new QoreBigIntNode(0x8B4B));
   opengl_ns.addConstant("GL_MAX_GEOMETRY_UNIFORM_COMPONENTS_EXT",     new QoreBigIntNode(0x8DDF));
   opengl_ns.addConstant("GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT",        new QoreBigIntNode(0x8DE0));
   opengl_ns.addConstant("GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS_EXT",new QoreBigIntNode(0x8DE1));
   opengl_ns.addConstant("GL_LINES_ADJACENCY_EXT",                     new QoreBigIntNode(0xA));
   opengl_ns.addConstant("GL_LINE_STRIP_ADJACENCY_EXT",                new QoreBigIntNode(0xB));
   opengl_ns.addConstant("GL_TRIANGLES_ADJACENCY_EXT",                 new QoreBigIntNode(0xC));
   opengl_ns.addConstant("GL_TRIANGLE_STRIP_ADJACENCY_EXT",            new QoreBigIntNode(0xD));
   opengl_ns.addConstant("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT",new QoreBigIntNode(0x8DA8));
   opengl_ns.addConstant("GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_EXT",  new QoreBigIntNode(0x8DA9));
   opengl_ns.addConstant("GL_FRAMEBUFFER_ATTACHMENT_LAYERED_EXT",      new QoreBigIntNode(0x8DA7));
   opengl_ns.addConstant("GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER_EXT",new QoreBigIntNode(0x8CD4));
   opengl_ns.addConstant("GL_PROGRAM_POINT_SIZE_EXT",                  new QoreBigIntNode(0x8642));
   opengl_ns.addConstant("GL_TRANSFORM_FEEDBACK_BUFFER_EXT",           new QoreBigIntNode(0x8C8E));
   opengl_ns.addConstant("GL_TRANSFORM_FEEDBACK_BUFFER_START_EXT",     new QoreBigIntNode(0x8C84));
   opengl_ns.addConstant("GL_TRANSFORM_FEEDBACK_BUFFER_SIZE_EXT",      new QoreBigIntNode(0x8C85));
   opengl_ns.addConstant("GL_TRANSFORM_FEEDBACK_BUFFER_BINDING_EXT",   new QoreBigIntNode(0x8C8F));
   opengl_ns.addConstant("GL_INTERLEAVED_ATTRIBS_EXT",                 new QoreBigIntNode(0x8C8C));
   opengl_ns.addConstant("GL_SEPARATE_ATTRIBS_EXT",                    new QoreBigIntNode(0x8C8D));
   opengl_ns.addConstant("GL_PRIMITIVES_GENERATED_EXT",                new QoreBigIntNode(0x8C87));
   opengl_ns.addConstant("GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN_EXT",new QoreBigIntNode(0x8C88));
   opengl_ns.addConstant("GL_RASTERIZER_DISCARD_EXT",                  new QoreBigIntNode(0x8C89));
   opengl_ns.addConstant("GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS_EXT",new QoreBigIntNode(0x8C8A));
   opengl_ns.addConstant("GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS_EXT",new QoreBigIntNode(0x8C8B));
   opengl_ns.addConstant("GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS_EXT",new QoreBigIntNode(0x8C80));
   opengl_ns.addConstant("GL_TRANSFORM_FEEDBACK_VARYINGS_EXT",         new QoreBigIntNode(0x8C83));
   opengl_ns.addConstant("GL_TRANSFORM_FEEDBACK_BUFFER_MODE_EXT",      new QoreBigIntNode(0x8C7F));
   opengl_ns.addConstant("GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH_EXT",new QoreBigIntNode(0x8C76));
   opengl_ns.addConstant("GL_MAX_VERTEX_BINDABLE_UNIFORMS_EXT",        new QoreBigIntNode(0x8DE2));
   opengl_ns.addConstant("GL_MAX_FRAGMENT_BINDABLE_UNIFORMS_EXT",      new QoreBigIntNode(0x8DE3));
   opengl_ns.addConstant("GL_MAX_GEOMETRY_BINDABLE_UNIFORMS_EXT",      new QoreBigIntNode(0x8DE4));
   opengl_ns.addConstant("GL_MAX_BINDABLE_UNIFORM_SIZE_EXT",           new QoreBigIntNode(0x8DED));
   opengl_ns.addConstant("GL_UNIFORM_BUFFER_BINDING_EXT",              new QoreBigIntNode(0x8DEF));
   opengl_ns.addConstant("GL_UNIFORM_BUFFER_EXT",                      new QoreBigIntNode(0x8DEE));
   opengl_ns.addConstant("GL_RGBA_INTEGER_MODE_EXT",                   new QoreBigIntNode(0x8D9E));
   opengl_ns.addConstant("GL_RGBA32UI_EXT",                            new QoreBigIntNode(0x8D70));
   opengl_ns.addConstant("GL_RGB32UI_EXT",                             new QoreBigIntNode(0x8D71));
   opengl_ns.addConstant("GL_ALPHA32UI_EXT",                           new QoreBigIntNode(0x8D72));
   opengl_ns.addConstant("GL_INTENSITY32UI_EXT",                       new QoreBigIntNode(0x8D73));
   opengl_ns.addConstant("GL_LUMINANCE32UI_EXT",                       new QoreBigIntNode(0x8D74));
   opengl_ns.addConstant("GL_LUMINANCE_ALPHA32UI_EXT",                 new QoreBigIntNode(0x8D75));
   opengl_ns.addConstant("GL_RGBA16UI_EXT",                            new QoreBigIntNode(0x8D76));
   opengl_ns.addConstant("GL_RGB16UI_EXT",                             new QoreBigIntNode(0x8D77));
   opengl_ns.addConstant("GL_ALPHA16UI_EXT",                           new QoreBigIntNode(0x8D78));
   opengl_ns.addConstant("GL_INTENSITY16UI_EXT",                       new QoreBigIntNode(0x8D79));
   opengl_ns.addConstant("GL_LUMINANCE16UI_EXT",                       new QoreBigIntNode(0x8D7A));
   opengl_ns.addConstant("GL_LUMINANCE_ALPHA16UI_EXT",                 new QoreBigIntNode(0x8D7B));
   opengl_ns.addConstant("GL_RGBA8UI_EXT",                             new QoreBigIntNode(0x8D7C));
   opengl_ns.addConstant("GL_RGB8UI_EXT",                              new QoreBigIntNode(0x8D7D));
   opengl_ns.addConstant("GL_ALPHA8UI_EXT",                            new QoreBigIntNode(0x8D7E));
   opengl_ns.addConstant("GL_INTENSITY8UI_EXT",                        new QoreBigIntNode(0x8D7F));
   opengl_ns.addConstant("GL_LUMINANCE8UI_EXT",                        new QoreBigIntNode(0x8D80));
   opengl_ns.addConstant("GL_LUMINANCE_ALPHA8UI_EXT",                  new QoreBigIntNode(0x8D81));
   opengl_ns.addConstant("GL_RGBA32I_EXT",                             new QoreBigIntNode(0x8D82));
   opengl_ns.addConstant("GL_RGB32I_EXT",                              new QoreBigIntNode(0x8D83));
   opengl_ns.addConstant("GL_ALPHA32I_EXT",                            new QoreBigIntNode(0x8D84));
   opengl_ns.addConstant("GL_INTENSITY32I_EXT",                        new QoreBigIntNode(0x8D85));
   opengl_ns.addConstant("GL_LUMINANCE32I_EXT",                        new QoreBigIntNode(0x8D86));
   opengl_ns.addConstant("GL_LUMINANCE_ALPHA32I_EXT",                  new QoreBigIntNode(0x8D87));
   opengl_ns.addConstant("GL_RGBA16I_EXT",                             new QoreBigIntNode(0x8D88));
   opengl_ns.addConstant("GL_RGB16I_EXT",                              new QoreBigIntNode(0x8D89));
   opengl_ns.addConstant("GL_ALPHA16I_EXT",                            new QoreBigIntNode(0x8D8A));
   opengl_ns.addConstant("GL_INTENSITY16I_EXT",                        new QoreBigIntNode(0x8D8B));
   opengl_ns.addConstant("GL_LUMINANCE16I_EXT",                        new QoreBigIntNode(0x8D8C));
   opengl_ns.addConstant("GL_LUMINANCE_ALPHA16I_EXT",                  new QoreBigIntNode(0x8D8D));
   opengl_ns.addConstant("GL_RGBA8I_EXT",                              new QoreBigIntNode(0x8D8E));
   opengl_ns.addConstant("GL_RGB8I_EXT",                               new QoreBigIntNode(0x8D8F));
   opengl_ns.addConstant("GL_ALPHA8I_EXT",                             new QoreBigIntNode(0x8D90));
   opengl_ns.addConstant("GL_INTENSITY8I_EXT",                         new QoreBigIntNode(0x8D91));
   opengl_ns.addConstant("GL_LUMINANCE8I_EXT",                         new QoreBigIntNode(0x8D92));
   opengl_ns.addConstant("GL_LUMINANCE_ALPHA8I_EXT",                   new QoreBigIntNode(0x8D93));
   opengl_ns.addConstant("GL_RED_INTEGER_EXT",                         new QoreBigIntNode(0x8D94));
   opengl_ns.addConstant("GL_GREEN_INTEGER_EXT",                       new QoreBigIntNode(0x8D95));
   opengl_ns.addConstant("GL_BLUE_INTEGER_EXT",                        new QoreBigIntNode(0x8D96));
   opengl_ns.addConstant("GL_ALPHA_INTEGER_EXT",                       new QoreBigIntNode(0x8D97));
   opengl_ns.addConstant("GL_RGB_INTEGER_EXT",                         new QoreBigIntNode(0x8D98));
   opengl_ns.addConstant("GL_RGBA_INTEGER_EXT",                        new QoreBigIntNode(0x8D99));
   opengl_ns.addConstant("GL_BGR_INTEGER_EXT",                         new QoreBigIntNode(0x8D9A));
   opengl_ns.addConstant("GL_BGRA_INTEGER_EXT",                        new QoreBigIntNode(0x8D9B));
   opengl_ns.addConstant("GL_LUMINANCE_INTEGER_EXT",                   new QoreBigIntNode(0x8D9C));
   opengl_ns.addConstant("GL_LUMINANCE_ALPHA_INTEGER_EXT",             new QoreBigIntNode(0x8D9D));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY_INTEGER_EXT",         new QoreBigIntNode(0x88FD));
   opengl_ns.addConstant("GL_SAMPLER_1D_ARRAY_EXT",                    new QoreBigIntNode(0x8DC0));
   opengl_ns.addConstant("GL_SAMPLER_2D_ARRAY_EXT",                    new QoreBigIntNode(0x8DC1));
   opengl_ns.addConstant("GL_SAMPLER_BUFFER_EXT",                      new QoreBigIntNode(0x8DC2));
   opengl_ns.addConstant("GL_SAMPLER_1D_ARRAY_SHADOW_EXT",             new QoreBigIntNode(0x8DC3));
   opengl_ns.addConstant("GL_SAMPLER_2D_ARRAY_SHADOW_EXT",             new QoreBigIntNode(0x8DC4));
   opengl_ns.addConstant("GL_SAMPLER_CUBE_SHADOW_EXT",                 new QoreBigIntNode(0x8DC5));
   opengl_ns.addConstant("GL_UNSIGNED_INT_VEC2_EXT",                   new QoreBigIntNode(0x8DC6));
   opengl_ns.addConstant("GL_UNSIGNED_INT_VEC3_EXT",                   new QoreBigIntNode(0x8DC7));
   opengl_ns.addConstant("GL_UNSIGNED_INT_VEC4_EXT",                   new QoreBigIntNode(0x8DC8));
   opengl_ns.addConstant("GL_INT_SAMPLER_1D_EXT",                      new QoreBigIntNode(0x8DC9));
   opengl_ns.addConstant("GL_INT_SAMPLER_2D_EXT",                      new QoreBigIntNode(0x8DCA));
   opengl_ns.addConstant("GL_INT_SAMPLER_3D_EXT",                      new QoreBigIntNode(0x8DCB));
   opengl_ns.addConstant("GL_INT_SAMPLER_CUBE_EXT",                    new QoreBigIntNode(0x8DCC));
   opengl_ns.addConstant("GL_INT_SAMPLER_2D_RECT_EXT",                 new QoreBigIntNode(0x8DCD));
   opengl_ns.addConstant("GL_INT_SAMPLER_1D_ARRAY_EXT",                new QoreBigIntNode(0x8DCE));
   opengl_ns.addConstant("GL_INT_SAMPLER_2D_ARRAY_EXT",                new QoreBigIntNode(0x8DCF));
   opengl_ns.addConstant("GL_INT_SAMPLER_BUFFER_EXT",                  new QoreBigIntNode(0x8DD0));
   opengl_ns.addConstant("GL_UNSIGNED_INT_SAMPLER_1D_EXT",             new QoreBigIntNode(0x8DD1));
   opengl_ns.addConstant("GL_UNSIGNED_INT_SAMPLER_2D_EXT",             new QoreBigIntNode(0x8DD2));
   opengl_ns.addConstant("GL_UNSIGNED_INT_SAMPLER_3D_EXT",             new QoreBigIntNode(0x8DD3));
   opengl_ns.addConstant("GL_UNSIGNED_INT_SAMPLER_CUBE_EXT",           new QoreBigIntNode(0x8DD4));
   opengl_ns.addConstant("GL_UNSIGNED_INT_SAMPLER_2D_RECT_EXT",        new QoreBigIntNode(0x8DD5));
   opengl_ns.addConstant("GL_UNSIGNED_INT_SAMPLER_1D_ARRAY_EXT",       new QoreBigIntNode(0x8DD6));
   opengl_ns.addConstant("GL_UNSIGNED_INT_SAMPLER_2D_ARRAY_EXT",       new QoreBigIntNode(0x8DD7));
   opengl_ns.addConstant("GL_UNSIGNED_INT_SAMPLER_BUFFER_EXT",         new QoreBigIntNode(0x8DD8));
   opengl_ns.addConstant("GL_MIN_PROGRAM_TEXEL_OFFSET_EXT",            new QoreBigIntNode(0x8904));
   opengl_ns.addConstant("GL_MAX_PROGRAM_TEXEL_OFFSET_EXT",            new QoreBigIntNode(0x8905));
   opengl_ns.addConstant("GL_VERTEX_ARRAY_RANGE_APPLE",                new QoreBigIntNode(0x851D));
   opengl_ns.addConstant("GL_VERTEX_ARRAY_RANGE_LENGTH_APPLE",         new QoreBigIntNode(0x851E));
   opengl_ns.addConstant("GL_MAX_VERTEX_ARRAY_RANGE_ELEMENT_APPLE",    new QoreBigIntNode(0x8520));
   opengl_ns.addConstant("GL_VERTEX_ARRAY_RANGE_POINTER_APPLE",        new QoreBigIntNode(0x8521));
   opengl_ns.addConstant("GL_VERTEX_ARRAY_STORAGE_HINT_APPLE",         new QoreBigIntNode(0x851F));
   opengl_ns.addConstant("GL_STORAGE_CLIENT_APPLE",                    new QoreBigIntNode(0x85B4));
   opengl_ns.addConstant("GL_STORAGE_PRIVATE_APPLE",                   new QoreBigIntNode(0x85BD));
   opengl_ns.addConstant("GL_STORAGE_CACHED_APPLE",                    new QoreBigIntNode(0x85BE));
   opengl_ns.addConstant("GL_STORAGE_SHARED_APPLE",                    new QoreBigIntNode(0x85BF));
   opengl_ns.addConstant("GL_VERTEX_ARRAY_BINDING_APPLE",              new QoreBigIntNode(0x85B5));
   opengl_ns.addConstant("GL_ELEMENT_ARRAY_APPLE",                     new QoreBigIntNode(0x8A0C));
   opengl_ns.addConstant("GL_ELEMENT_ARRAY_TYPE_APPLE",                new QoreBigIntNode(0x8A0D));
   opengl_ns.addConstant("GL_ELEMENT_ARRAY_POINTER_APPLE",             new QoreBigIntNode(0x8A0E));
   opengl_ns.addConstant("GL_ELEMENT_BUFFER_BINDING_APPLE",            new QoreBigIntNode(0x8A11));
   opengl_ns.addConstant("GL_DRAW_PIXELS_APPLE",                       new QoreBigIntNode(0x8A0A));
   opengl_ns.addConstant("GL_FENCE_APPLE",                             new QoreBigIntNode(0x8A0B));
   opengl_ns.addConstant("GL_BUFFER_OBJECT_APPLE",                     new QoreBigIntNode(0x85B3));
   opengl_ns.addConstant("GL_LIGHT_MODEL_SPECULAR_VECTOR_APPLE",       new QoreBigIntNode(0x85B0));
   opengl_ns.addConstant("GL_TRANSFORM_HINT_APPLE",                    new QoreBigIntNode(0x85B1));
   opengl_ns.addConstant("GL_UNPACK_CLIENT_STORAGE_APPLE",             new QoreBigIntNode(0x85B2));
   opengl_ns.addConstant("GL_YCBCR_422_APPLE",                         new QoreBigIntNode(0x85B9));
   opengl_ns.addConstant("GL_UNSIGNED_SHORT_8_8_APPLE",                new QoreBigIntNode(0x85BA));
   opengl_ns.addConstant("GL_UNSIGNED_SHORT_8_8_REV_APPLE",            new QoreBigIntNode(0x85BB));
   opengl_ns.addConstant("GL_TEXTURE_RANGE_LENGTH_APPLE",              new QoreBigIntNode(0x85B7));
   opengl_ns.addConstant("GL_TEXTURE_RANGE_POINTER_APPLE",             new QoreBigIntNode(0x85B8));
   opengl_ns.addConstant("GL_TEXTURE_STORAGE_HINT_APPLE",              new QoreBigIntNode(0x85BC));
   opengl_ns.addConstant("GL_TEXTURE_MINIMIZE_STORAGE_APPLE",          new QoreBigIntNode(0x85B6));
   opengl_ns.addConstant("GL_HALF_APPLE",                              new QoreBigIntNode(0x140B));
   opengl_ns.addConstant("GL_COLOR_FLOAT_APPLE",                       new QoreBigIntNode(0x8A0F));
   opengl_ns.addConstant("GL_RGBA_FLOAT32_APPLE",                      new QoreBigIntNode(0x8814));
   opengl_ns.addConstant("GL_RGB_FLOAT32_APPLE",                       new QoreBigIntNode(0x8815));
   opengl_ns.addConstant("GL_ALPHA_FLOAT32_APPLE",                     new QoreBigIntNode(0x8816));
   opengl_ns.addConstant("GL_INTENSITY_FLOAT32_APPLE",                 new QoreBigIntNode(0x8817));
   opengl_ns.addConstant("GL_LUMINANCE_FLOAT32_APPLE",                 new QoreBigIntNode(0x8818));
   opengl_ns.addConstant("GL_LUMINANCE_ALPHA_FLOAT32_APPLE",           new QoreBigIntNode(0x8819));
   opengl_ns.addConstant("GL_RGBA_FLOAT16_APPLE",                      new QoreBigIntNode(0x881A));
   opengl_ns.addConstant("GL_RGB_FLOAT16_APPLE",                       new QoreBigIntNode(0x881B));
   opengl_ns.addConstant("GL_ALPHA_FLOAT16_APPLE",                     new QoreBigIntNode(0x881C));
   opengl_ns.addConstant("GL_INTENSITY_FLOAT16_APPLE",                 new QoreBigIntNode(0x881D));
   opengl_ns.addConstant("GL_LUMINANCE_FLOAT16_APPLE",                 new QoreBigIntNode(0x881E));
   opengl_ns.addConstant("GL_LUMINANCE_ALPHA_FLOAT16_APPLE",           new QoreBigIntNode(0x881F));
   opengl_ns.addConstant("GL_MIN_PBUFFER_VIEWPORT_DIMS_APPLE",         new QoreBigIntNode(0x8A10));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_MAP1_APPLE",                new QoreBigIntNode(0x8A00));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_MAP2_APPLE",                new QoreBigIntNode(0x8A01));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_MAP1_SIZE_APPLE",           new QoreBigIntNode(0x8A02));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_MAP1_COEFF_APPLE",          new QoreBigIntNode(0x8A03));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_MAP1_ORDER_APPLE",          new QoreBigIntNode(0x8A04));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_MAP1_DOMAIN_APPLE",         new QoreBigIntNode(0x8A05));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_MAP2_SIZE_APPLE",           new QoreBigIntNode(0x8A06));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_MAP2_COEFF_APPLE",          new QoreBigIntNode(0x8A07));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_MAP2_ORDER_APPLE",          new QoreBigIntNode(0x8A08));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_MAP2_DOMAIN_APPLE",         new QoreBigIntNode(0x8A09));
   opengl_ns.addConstant("GL_BUFFER_SERIALIZED_MODIFY_APPLE",          new QoreBigIntNode(0x8A12));
   opengl_ns.addConstant("GL_BUFFER_FLUSHING_UNMAP_APPLE",             new QoreBigIntNode(0x8A13));
   opengl_ns.addConstant("GL_AUX_DEPTH_STENCIL_APPLE",                 new QoreBigIntNode(0x8A14));
   opengl_ns.addConstant("GL_PACK_ROW_BYTES_APPLE",                    new QoreBigIntNode(0x8A15));
   opengl_ns.addConstant("GL_UNPACK_ROW_BYTES_APPLE",                  new QoreBigIntNode(0x8A16));
   opengl_ns.addConstant("GL_PACK_IMAGE_BYTES_APPLE",                  new QoreBigIntNode(0x8A17));
   opengl_ns.addConstant("GL_UNPACK_IMAGE_BYTES_APPLE",                new QoreBigIntNode(0x8A18));
   opengl_ns.addConstant("GL_RELEASED_APPLE",                          new QoreBigIntNode(0x8A19));
   opengl_ns.addConstant("GL_VOLATILE_APPLE",                          new QoreBigIntNode(0x8A1A));
   opengl_ns.addConstant("GL_RETAINED_APPLE",                          new QoreBigIntNode(0x8A1B));
   opengl_ns.addConstant("GL_UNDEFINED_APPLE",                         new QoreBigIntNode(0x8A1C));
   opengl_ns.addConstant("GL_PURGEABLE_APPLE",                         new QoreBigIntNode(0x8A1D));
   opengl_ns.addConstant("GL_MIN_WEIGHTED_ATI",                        new QoreBigIntNode(0x877D));
   opengl_ns.addConstant("GL_MAX_WEIGHTED_ATI",                        new QoreBigIntNode(0x877E));
   opengl_ns.addConstant("GL_MODULATE_ADD_ATI",                        new QoreBigIntNode(0x8744));
   opengl_ns.addConstant("GL_MODULATE_SIGNED_ADD_ATI",                 new QoreBigIntNode(0x8745));
   opengl_ns.addConstant("GL_MODULATE_SUBTRACT_ATI",                   new QoreBigIntNode(0x8746));
   opengl_ns.addConstant("GL_STENCIL_BACK_FUNC_ATI",                   new QoreBigIntNode(0x8800));
   opengl_ns.addConstant("GL_STENCIL_BACK_FAIL_ATI",                   new QoreBigIntNode(0x8801));
   opengl_ns.addConstant("GL_STENCIL_BACK_PASS_DEPTH_FAIL_ATI",        new QoreBigIntNode(0x8802));
   opengl_ns.addConstant("GL_STENCIL_BACK_PASS_DEPTH_PASS_ATI",        new QoreBigIntNode(0x8803));
   opengl_ns.addConstant("GL_ARRAY_REV_COMPS_IN_4_BYTES_ATI",          new QoreBigIntNode(0x897C));
   opengl_ns.addConstant("GL_MIRROR_CLAMP_ATI",                        new QoreBigIntNode(0x8742));
   opengl_ns.addConstant("GL_MIRROR_CLAMP_TO_EDGE_ATI",                new QoreBigIntNode(0x8743));
   opengl_ns.addConstant("GL_PN_TRIANGLES_ATI",                        new QoreBigIntNode(0x6090));
   opengl_ns.addConstant("GL_MAX_PN_TRIANGLES_TESSELATION_LEVEL_ATI",  new QoreBigIntNode(0x6091));
   opengl_ns.addConstant("GL_PN_TRIANGLES_POINT_MODE_ATI",             new QoreBigIntNode(0x6092));
   opengl_ns.addConstant("GL_PN_TRIANGLES_NORMAL_MODE_ATI",            new QoreBigIntNode(0x6093));
   opengl_ns.addConstant("GL_PN_TRIANGLES_TESSELATION_LEVEL_ATI",      new QoreBigIntNode(0x6094));
   opengl_ns.addConstant("GL_PN_TRIANGLES_POINT_MODE_LINEAR_ATI",      new QoreBigIntNode(0x6095));
   opengl_ns.addConstant("GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATI",       new QoreBigIntNode(0x6096));
   opengl_ns.addConstant("GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATI",     new QoreBigIntNode(0x6097));
   opengl_ns.addConstant("GL_PN_TRIANGLES_NORMAL_MODE_QUADRATIC_ATI",  new QoreBigIntNode(0x6098));
   opengl_ns.addConstant("GL_TEXT_FRAGMENT_SHADER_ATI",                new QoreBigIntNode(0x8200));
   opengl_ns.addConstant("GL_ALPHA_BLEND_EQUATION_ATI",                new QoreBigIntNode(0x883D));
   opengl_ns.addConstant("GL_POINT_CULL_MODE_ATI",                     new QoreBigIntNode(0x60B3));
   opengl_ns.addConstant("GL_POINT_CULL_CENTER_ATI",                   new QoreBigIntNode(0x60B4));
   opengl_ns.addConstant("GL_POINT_CULL_CLIP_ATI",                     new QoreBigIntNode(0x60B5));
   opengl_ns.addConstant("GL_PN_TRIANGLES_ATIX",                       new QoreBigIntNode(0x6090));
   opengl_ns.addConstant("GL_MAX_PN_TRIANGLES_TESSELATION_LEVEL_ATIX", new QoreBigIntNode(0x6091));
   opengl_ns.addConstant("GL_PN_TRIANGLES_POINT_MODE_ATIX",            new QoreBigIntNode(0x6092));
   opengl_ns.addConstant("GL_PN_TRIANGLES_NORMAL_MODE_ATIX",           new QoreBigIntNode(0x6093));
   opengl_ns.addConstant("GL_PN_TRIANGLES_TESSELATION_LEVEL_ATIX",     new QoreBigIntNode(0x6094));
   opengl_ns.addConstant("GL_PN_TRIANGLES_POINT_MODE_LINEAR_ATIX",     new QoreBigIntNode(0x6095));
   opengl_ns.addConstant("GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATIX",      new QoreBigIntNode(0x6096));
   opengl_ns.addConstant("GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATIX",    new QoreBigIntNode(0x6097));
   opengl_ns.addConstant("GL_PN_TRIANGLES_NORMAL_MODE_QUADRATIC_ATIX", new QoreBigIntNode(0x6098));
   opengl_ns.addConstant("GL_COMPRESSED_LUMINANCE_ALPHA_3DC_ATI",      new QoreBigIntNode(0x8837));
   opengl_ns.addConstant("GL_RGBA_FLOAT32_ATI",                        new QoreBigIntNode(0x8814));
   opengl_ns.addConstant("GL_RGB_FLOAT32_ATI",                         new QoreBigIntNode(0x8815));
   opengl_ns.addConstant("GL_ALPHA_FLOAT32_ATI",                       new QoreBigIntNode(0x8816));
   opengl_ns.addConstant("GL_INTENSITY_FLOAT32_ATI",                   new QoreBigIntNode(0x8817));
   opengl_ns.addConstant("GL_LUMINANCE_FLOAT32_ATI",                   new QoreBigIntNode(0x8818));
   opengl_ns.addConstant("GL_LUMINANCE_ALPHA_FLOAT32_ATI",             new QoreBigIntNode(0x8819));
   opengl_ns.addConstant("GL_RGBA_FLOAT16_ATI",                        new QoreBigIntNode(0x881A));
   opengl_ns.addConstant("GL_RGB_FLOAT16_ATI",                         new QoreBigIntNode(0x881B));
   opengl_ns.addConstant("GL_ALPHA_FLOAT16_ATI",                       new QoreBigIntNode(0x881C));
   opengl_ns.addConstant("GL_INTENSITY_FLOAT16_ATI",                   new QoreBigIntNode(0x881D));
   opengl_ns.addConstant("GL_LUMINANCE_FLOAT16_ATI",                   new QoreBigIntNode(0x881E));
   opengl_ns.addConstant("GL_LUMINANCE_ALPHA_FLOAT16_ATI",             new QoreBigIntNode(0x881F));
   opengl_ns.addConstant("GL_IMAGE_SCALE_X_HP",                        new QoreBigIntNode(0x8155));
   opengl_ns.addConstant("GL_IMAGE_SCALE_Y_HP",                        new QoreBigIntNode(0x8156));
   opengl_ns.addConstant("GL_IMAGE_TRANSLATE_X_HP",                    new QoreBigIntNode(0x8157));
   opengl_ns.addConstant("GL_IMAGE_TRANSLATE_Y_HP",                    new QoreBigIntNode(0x8158));
   opengl_ns.addConstant("GL_IMAGE_ROTATE_ANGLE_HP",                   new QoreBigIntNode(0x8159));
   opengl_ns.addConstant("GL_IMAGE_ROTATE_ORIGIN_X_HP",                new QoreBigIntNode(0x815A));
   opengl_ns.addConstant("GL_IMAGE_ROTATE_ORIGIN_Y_HP",                new QoreBigIntNode(0x815B));
   opengl_ns.addConstant("GL_IMAGE_MAG_FILTER_HP",                     new QoreBigIntNode(0x815C));
   opengl_ns.addConstant("GL_IMAGE_MIN_FILTER_HP",                     new QoreBigIntNode(0x815D));
   opengl_ns.addConstant("GL_IMAGE_CUBIC_WEIGHT_HP",                   new QoreBigIntNode(0x815E));
   opengl_ns.addConstant("GL_CUBIC_HP",                                new QoreBigIntNode(0x815F));
   opengl_ns.addConstant("GL_AVERAGE_HP",                              new QoreBigIntNode(0x8160));
   opengl_ns.addConstant("GL_IMAGE_TRANSFORM_2D_HP",                   new QoreBigIntNode(0x8161));
   opengl_ns.addConstant("GL_POST_IMAGE_TRANSFORM_COLOR_TABLE_HP",     new QoreBigIntNode(0x8162));
   opengl_ns.addConstant("GL_PROXY_POST_IMAGE_TRANSFORM_COLOR_TABLE_HP",new QoreBigIntNode(0x8163));
   opengl_ns.addConstant("GL_IGNORE_BORDER_HP",                        new QoreBigIntNode(0x8150));
   opengl_ns.addConstant("GL_CONSTANT_BORDER_HP",                      new QoreBigIntNode(0x8151));
   opengl_ns.addConstant("GL_REPLICATE_BORDER_HP",                     new QoreBigIntNode(0x8153));
   opengl_ns.addConstant("GL_CONVOLUTION_BORDER_COLOR_HP",             new QoreBigIntNode(0x8154));
   opengl_ns.addConstant("GL_TEXTURE_LIGHTING_MODE_HP",                new QoreBigIntNode(0x8167));
   opengl_ns.addConstant("GL_TEXTURE_POST_SPECULAR_HP",                new QoreBigIntNode(0x8168));
   opengl_ns.addConstant("GL_TEXTURE_PRE_SPECULAR_HP",                 new QoreBigIntNode(0x8169));
   opengl_ns.addConstant("GL_OCCLUSION_TEST_HP",                       new QoreBigIntNode(0x8165));
   opengl_ns.addConstant("GL_OCCLUSION_TEST_RESULT_HP",                new QoreBigIntNode(0x8166));
   opengl_ns.addConstant("GL_RASTER_POSITION_UNCLIPPED_IBM",           new QoreBigIntNode(0x19262));
   opengl_ns.addConstant("GL_CULL_VERTEX_IBM",                         new QoreBigIntNode(103050));
   opengl_ns.addConstant("GL_VERTEX_ARRAY_LIST_IBM",                   new QoreBigIntNode(103070));
   opengl_ns.addConstant("GL_NORMAL_ARRAY_LIST_IBM",                   new QoreBigIntNode(103071));
   opengl_ns.addConstant("GL_COLOR_ARRAY_LIST_IBM",                    new QoreBigIntNode(103072));
   opengl_ns.addConstant("GL_INDEX_ARRAY_LIST_IBM",                    new QoreBigIntNode(103073));
   opengl_ns.addConstant("GL_TEXTURE_COORD_ARRAY_LIST_IBM",            new QoreBigIntNode(103074));
   opengl_ns.addConstant("GL_EDGE_FLAG_ARRAY_LIST_IBM",                new QoreBigIntNode(103075));
   opengl_ns.addConstant("GL_FOG_COORDINATE_ARRAY_LIST_IBM",           new QoreBigIntNode(103076));
   opengl_ns.addConstant("GL_SECONDARY_COLOR_ARRAY_LIST_IBM",          new QoreBigIntNode(103077));
   opengl_ns.addConstant("GL_VERTEX_ARRAY_LIST_STRIDE_IBM",            new QoreBigIntNode(103080));
   opengl_ns.addConstant("GL_NORMAL_ARRAY_LIST_STRIDE_IBM",            new QoreBigIntNode(103081));
   opengl_ns.addConstant("GL_COLOR_ARRAY_LIST_STRIDE_IBM",             new QoreBigIntNode(103082));
   opengl_ns.addConstant("GL_INDEX_ARRAY_LIST_STRIDE_IBM",             new QoreBigIntNode(103083));
   opengl_ns.addConstant("GL_TEXTURE_COORD_ARRAY_LIST_STRIDE_IBM",     new QoreBigIntNode(103084));
   opengl_ns.addConstant("GL_EDGE_FLAG_ARRAY_LIST_STRIDE_IBM",         new QoreBigIntNode(103085));
   opengl_ns.addConstant("GL_FOG_COORDINATE_ARRAY_LIST_STRIDE_IBM",    new QoreBigIntNode(103086));
   opengl_ns.addConstant("GL_SECONDARY_COLOR_ARRAY_LIST_STRIDE_IBM",   new QoreBigIntNode(103087));
   opengl_ns.addConstant("GL_RED_MIN_CLAMP_INGR",                      new QoreBigIntNode(0x8560));
   opengl_ns.addConstant("GL_GREEN_MIN_CLAMP_INGR",                    new QoreBigIntNode(0x8561));
   opengl_ns.addConstant("GL_BLUE_MIN_CLAMP_INGR",                     new QoreBigIntNode(0x8562));
   opengl_ns.addConstant("GL_ALPHA_MIN_CLAMP_INGR",                    new QoreBigIntNode(0x8563));
   opengl_ns.addConstant("GL_RED_MAX_CLAMP_INGR",                      new QoreBigIntNode(0x8564));
   opengl_ns.addConstant("GL_GREEN_MAX_CLAMP_INGR",                    new QoreBigIntNode(0x8565));
   opengl_ns.addConstant("GL_BLUE_MAX_CLAMP_INGR",                     new QoreBigIntNode(0x8566));
   opengl_ns.addConstant("GL_ALPHA_MAX_CLAMP_INGR",                    new QoreBigIntNode(0x8567));
   opengl_ns.addConstant("GL_INTERLACE_READ_INGR",                     new QoreBigIntNode(0x8568));
   opengl_ns.addConstant("GL_PARALLEL_ARRAYS_INTEL",                   new QoreBigIntNode(0x83F4));
   opengl_ns.addConstant("GL_VERTEX_ARRAY_PARALLEL_POINTERS_INTEL",    new QoreBigIntNode(0x83F5));
   opengl_ns.addConstant("GL_NORMAL_ARRAY_PARALLEL_POINTERS_INTEL",    new QoreBigIntNode(0x83F6));
   opengl_ns.addConstant("GL_COLOR_ARRAY_PARALLEL_POINTERS_INTEL",     new QoreBigIntNode(0x83F7));
   opengl_ns.addConstant("GL_TEXTURE_COORD_ARRAY_PARALLEL_POINTERS_INTEL",new QoreBigIntNode(0x83F8));
   opengl_ns.addConstant("GL_NORMAL_MAP_NV",                           new QoreBigIntNode(0x8511));
   opengl_ns.addConstant("GL_REFLECTION_MAP_NV",                       new QoreBigIntNode(0x8512));
   opengl_ns.addConstant("GL_VERTEX_ARRAY_RANGE_NV",                   new QoreBigIntNode(0x851D));
   opengl_ns.addConstant("GL_VERTEX_ARRAY_RANGE_LENGTH_NV",            new QoreBigIntNode(0x851E));
   opengl_ns.addConstant("GL_VERTEX_ARRAY_RANGE_VALID_NV",             new QoreBigIntNode(0x851F));
   opengl_ns.addConstant("GL_MAX_VERTEX_ARRAY_RANGE_ELEMENT_NV",       new QoreBigIntNode(0x8520));
   opengl_ns.addConstant("GL_VERTEX_ARRAY_RANGE_POINTER_NV",           new QoreBigIntNode(0x8521));
   opengl_ns.addConstant("GL_VERTEX_ARRAY_RANGE_WITHOUT_FLUSH_NV",     new QoreBigIntNode(0x8533));
   opengl_ns.addConstant("GL_REGISTER_COMBINERS_NV",                   new QoreBigIntNode(0x8522));
   opengl_ns.addConstant("GL_VARIABLE_A_NV",                           new QoreBigIntNode(0x8523));
   opengl_ns.addConstant("GL_VARIABLE_B_NV",                           new QoreBigIntNode(0x8524));
   opengl_ns.addConstant("GL_VARIABLE_C_NV",                           new QoreBigIntNode(0x8525));
   opengl_ns.addConstant("GL_VARIABLE_D_NV",                           new QoreBigIntNode(0x8526));
   opengl_ns.addConstant("GL_VARIABLE_E_NV",                           new QoreBigIntNode(0x8527));
   opengl_ns.addConstant("GL_VARIABLE_F_NV",                           new QoreBigIntNode(0x8528));
   opengl_ns.addConstant("GL_VARIABLE_G_NV",                           new QoreBigIntNode(0x8529));
   opengl_ns.addConstant("GL_CONSTANT_COLOR0_NV",                      new QoreBigIntNode(0x852A));
   opengl_ns.addConstant("GL_CONSTANT_COLOR1_NV",                      new QoreBigIntNode(0x852B));
   opengl_ns.addConstant("GL_PRIMARY_COLOR_NV",                        new QoreBigIntNode(0x852C));
   opengl_ns.addConstant("GL_SECONDARY_COLOR_NV",                      new QoreBigIntNode(0x852D));
   opengl_ns.addConstant("GL_SPARE0_NV",                               new QoreBigIntNode(0x852E));
   opengl_ns.addConstant("GL_SPARE1_NV",                               new QoreBigIntNode(0x852F));
   opengl_ns.addConstant("GL_DISCARD_NV",                              new QoreBigIntNode(0x8530));
   opengl_ns.addConstant("GL_E_TIMES_F_NV",                            new QoreBigIntNode(0x8531));
   opengl_ns.addConstant("GL_SPARE0_PLUS_SECONDARY_COLOR_NV",          new QoreBigIntNode(0x8532));
   opengl_ns.addConstant("GL_UNSIGNED_IDENTITY_NV",                    new QoreBigIntNode(0x8536));
   opengl_ns.addConstant("GL_UNSIGNED_INVERT_NV",                      new QoreBigIntNode(0x8537));
   opengl_ns.addConstant("GL_EXPAND_NORMAL_NV",                        new QoreBigIntNode(0x8538));
   opengl_ns.addConstant("GL_EXPAND_NEGATE_NV",                        new QoreBigIntNode(0x8539));
   opengl_ns.addConstant("GL_HALF_BIAS_NORMAL_NV",                     new QoreBigIntNode(0x853A));
   opengl_ns.addConstant("GL_HALF_BIAS_NEGATE_NV",                     new QoreBigIntNode(0x853B));
   opengl_ns.addConstant("GL_SIGNED_IDENTITY_NV",                      new QoreBigIntNode(0x853C));
   opengl_ns.addConstant("GL_SIGNED_NEGATE_NV",                        new QoreBigIntNode(0x853D));
   opengl_ns.addConstant("GL_SCALE_BY_TWO_NV",                         new QoreBigIntNode(0x853E));
   opengl_ns.addConstant("GL_SCALE_BY_FOUR_NV",                        new QoreBigIntNode(0x853F));
   opengl_ns.addConstant("GL_SCALE_BY_ONE_HALF_NV",                    new QoreBigIntNode(0x8540));
   opengl_ns.addConstant("GL_BIAS_BY_NEGATIVE_ONE_HALF_NV",            new QoreBigIntNode(0x8541));
   opengl_ns.addConstant("GL_COMBINER_INPUT_NV",                       new QoreBigIntNode(0x8542));
   opengl_ns.addConstant("GL_COMBINER_MAPPING_NV",                     new QoreBigIntNode(0x8543));
   opengl_ns.addConstant("GL_COMBINER_COMPONENT_USAGE_NV",             new QoreBigIntNode(0x8544));
   opengl_ns.addConstant("GL_COMBINER_AB_DOT_PRODUCT_NV",              new QoreBigIntNode(0x8545));
   opengl_ns.addConstant("GL_COMBINER_CD_DOT_PRODUCT_NV",              new QoreBigIntNode(0x8546));
   opengl_ns.addConstant("GL_COMBINER_MUX_SUM_NV",                     new QoreBigIntNode(0x8547));
   opengl_ns.addConstant("GL_COMBINER_SCALE_NV",                       new QoreBigIntNode(0x8548));
   opengl_ns.addConstant("GL_COMBINER_BIAS_NV",                        new QoreBigIntNode(0x8549));
   opengl_ns.addConstant("GL_COMBINER_AB_OUTPUT_NV",                   new QoreBigIntNode(0x854A));
   opengl_ns.addConstant("GL_COMBINER_CD_OUTPUT_NV",                   new QoreBigIntNode(0x854B));
   opengl_ns.addConstant("GL_COMBINER_SUM_OUTPUT_NV",                  new QoreBigIntNode(0x854C));
   opengl_ns.addConstant("GL_MAX_GENERAL_COMBINERS_NV",                new QoreBigIntNode(0x854D));
   opengl_ns.addConstant("GL_NUM_GENERAL_COMBINERS_NV",                new QoreBigIntNode(0x854E));
   opengl_ns.addConstant("GL_COLOR_SUM_CLAMP_NV",                      new QoreBigIntNode(0x854F));
   opengl_ns.addConstant("GL_COMBINER0_NV",                            new QoreBigIntNode(0x8550));
   opengl_ns.addConstant("GL_COMBINER1_NV",                            new QoreBigIntNode(0x8551));
   opengl_ns.addConstant("GL_COMBINER2_NV",                            new QoreBigIntNode(0x8552));
   opengl_ns.addConstant("GL_COMBINER3_NV",                            new QoreBigIntNode(0x8553));
   opengl_ns.addConstant("GL_COMBINER4_NV",                            new QoreBigIntNode(0x8554));
   opengl_ns.addConstant("GL_COMBINER5_NV",                            new QoreBigIntNode(0x8555));
   opengl_ns.addConstant("GL_COMBINER6_NV",                            new QoreBigIntNode(0x8556));
   opengl_ns.addConstant("GL_COMBINER7_NV",                            new QoreBigIntNode(0x8557));
   opengl_ns.addConstant("GL_PER_STAGE_CONSTANTS_NV",                  new QoreBigIntNode(0x8535));
   opengl_ns.addConstant("GL_FOG_DISTANCE_MODE_NV",                    new QoreBigIntNode(0x855A));
   opengl_ns.addConstant("GL_EYE_RADIAL_NV",                           new QoreBigIntNode(0x855B));
   opengl_ns.addConstant("GL_EYE_PLANE_ABSOLUTE_NV",                   new QoreBigIntNode(0x855C));
   opengl_ns.addConstant("GL_EMBOSS_LIGHT_NV",                         new QoreBigIntNode(0x855D));
   opengl_ns.addConstant("GL_EMBOSS_CONSTANT_NV",                      new QoreBigIntNode(0x855E));
   opengl_ns.addConstant("GL_EMBOSS_MAP_NV",                           new QoreBigIntNode(0x855F));
   opengl_ns.addConstant("GL_VERTEX_PROGRAM_NV",                       new QoreBigIntNode(0x8620));
   opengl_ns.addConstant("GL_VERTEX_STATE_PROGRAM_NV",                 new QoreBigIntNode(0x8621));
   opengl_ns.addConstant("GL_ATTRIB_ARRAY_SIZE_NV",                    new QoreBigIntNode(0x8623));
   opengl_ns.addConstant("GL_ATTRIB_ARRAY_STRIDE_NV",                  new QoreBigIntNode(0x8624));
   opengl_ns.addConstant("GL_ATTRIB_ARRAY_TYPE_NV",                    new QoreBigIntNode(0x8625));
   opengl_ns.addConstant("GL_CURRENT_ATTRIB_NV",                       new QoreBigIntNode(0x8626));
   opengl_ns.addConstant("GL_PROGRAM_LENGTH_NV",                       new QoreBigIntNode(0x8627));
   opengl_ns.addConstant("GL_PROGRAM_STRING_NV",                       new QoreBigIntNode(0x8628));
   opengl_ns.addConstant("GL_MODELVIEW_PROJECTION_NV",                 new QoreBigIntNode(0x8629));
   opengl_ns.addConstant("GL_IDENTITY_NV",                             new QoreBigIntNode(0x862A));
   opengl_ns.addConstant("GL_INVERSE_NV",                              new QoreBigIntNode(0x862B));
   opengl_ns.addConstant("GL_TRANSPOSE_NV",                            new QoreBigIntNode(0x862C));
   opengl_ns.addConstant("GL_INVERSE_TRANSPOSE_NV",                    new QoreBigIntNode(0x862D));
   opengl_ns.addConstant("GL_MAX_TRACK_MATRIX_STACK_DEPTH_NV",         new QoreBigIntNode(0x862E));
   opengl_ns.addConstant("GL_MAX_TRACK_MATRICES_NV",                   new QoreBigIntNode(0x862F));
   opengl_ns.addConstant("GL_MATRIX0_NV",                              new QoreBigIntNode(0x8630));
   opengl_ns.addConstant("GL_MATRIX1_NV",                              new QoreBigIntNode(0x8631));
   opengl_ns.addConstant("GL_MATRIX2_NV",                              new QoreBigIntNode(0x8632));
   opengl_ns.addConstant("GL_MATRIX3_NV",                              new QoreBigIntNode(0x8633));
   opengl_ns.addConstant("GL_MATRIX4_NV",                              new QoreBigIntNode(0x8634));
   opengl_ns.addConstant("GL_MATRIX5_NV",                              new QoreBigIntNode(0x8635));
   opengl_ns.addConstant("GL_MATRIX6_NV",                              new QoreBigIntNode(0x8636));
   opengl_ns.addConstant("GL_MATRIX7_NV",                              new QoreBigIntNode(0x8637));
   opengl_ns.addConstant("GL_CURRENT_MATRIX_STACK_DEPTH_NV",           new QoreBigIntNode(0x8640));
   opengl_ns.addConstant("GL_CURRENT_MATRIX_NV",                       new QoreBigIntNode(0x8641));
   opengl_ns.addConstant("GL_VERTEX_PROGRAM_POINT_SIZE_NV",            new QoreBigIntNode(0x8642));
   opengl_ns.addConstant("GL_VERTEX_PROGRAM_TWO_SIDE_NV",              new QoreBigIntNode(0x8643));
   opengl_ns.addConstant("GL_PROGRAM_PARAMETER_NV",                    new QoreBigIntNode(0x8644));
   opengl_ns.addConstant("GL_ATTRIB_ARRAY_POINTER_NV",                 new QoreBigIntNode(0x8645));
   opengl_ns.addConstant("GL_PROGRAM_TARGET_NV",                       new QoreBigIntNode(0x8646));
   opengl_ns.addConstant("GL_PROGRAM_RESIDENT_NV",                     new QoreBigIntNode(0x8647));
   opengl_ns.addConstant("GL_TRACK_MATRIX_NV",                         new QoreBigIntNode(0x8648));
   opengl_ns.addConstant("GL_TRACK_MATRIX_TRANSFORM_NV",               new QoreBigIntNode(0x8649));
   opengl_ns.addConstant("GL_VERTEX_PROGRAM_BINDING_NV",               new QoreBigIntNode(0x864A));
   opengl_ns.addConstant("GL_PROGRAM_ERROR_POSITION_NV",               new QoreBigIntNode(0x864B));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY0_NV",                 new QoreBigIntNode(0x8650));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY1_NV",                 new QoreBigIntNode(0x8651));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY2_NV",                 new QoreBigIntNode(0x8652));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY3_NV",                 new QoreBigIntNode(0x8653));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY4_NV",                 new QoreBigIntNode(0x8654));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY5_NV",                 new QoreBigIntNode(0x8655));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY6_NV",                 new QoreBigIntNode(0x8656));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY7_NV",                 new QoreBigIntNode(0x8657));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY8_NV",                 new QoreBigIntNode(0x8658));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY9_NV",                 new QoreBigIntNode(0x8659));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY10_NV",                new QoreBigIntNode(0x865A));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY11_NV",                new QoreBigIntNode(0x865B));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY12_NV",                new QoreBigIntNode(0x865C));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY13_NV",                new QoreBigIntNode(0x865D));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY14_NV",                new QoreBigIntNode(0x865E));
   opengl_ns.addConstant("GL_VERTEX_ATTRIB_ARRAY15_NV",                new QoreBigIntNode(0x865F));
   opengl_ns.addConstant("GL_MAP1_VERTEX_ATTRIB0_4_NV",                new QoreBigIntNode(0x8660));
   opengl_ns.addConstant("GL_MAP1_VERTEX_ATTRIB1_4_NV",                new QoreBigIntNode(0x8661));
   opengl_ns.addConstant("GL_MAP1_VERTEX_ATTRIB2_4_NV",                new QoreBigIntNode(0x8662));
   opengl_ns.addConstant("GL_MAP1_VERTEX_ATTRIB3_4_NV",                new QoreBigIntNode(0x8663));
   opengl_ns.addConstant("GL_MAP1_VERTEX_ATTRIB4_4_NV",                new QoreBigIntNode(0x8664));
   opengl_ns.addConstant("GL_MAP1_VERTEX_ATTRIB5_4_NV",                new QoreBigIntNode(0x8665));
   opengl_ns.addConstant("GL_MAP1_VERTEX_ATTRIB6_4_NV",                new QoreBigIntNode(0x8666));
   opengl_ns.addConstant("GL_MAP1_VERTEX_ATTRIB7_4_NV",                new QoreBigIntNode(0x8667));
   opengl_ns.addConstant("GL_MAP1_VERTEX_ATTRIB8_4_NV",                new QoreBigIntNode(0x8668));
   opengl_ns.addConstant("GL_MAP1_VERTEX_ATTRIB9_4_NV",                new QoreBigIntNode(0x8669));
   opengl_ns.addConstant("GL_MAP1_VERTEX_ATTRIB10_4_NV",               new QoreBigIntNode(0x866A));
   opengl_ns.addConstant("GL_MAP1_VERTEX_ATTRIB11_4_NV",               new QoreBigIntNode(0x866B));
   opengl_ns.addConstant("GL_MAP1_VERTEX_ATTRIB12_4_NV",               new QoreBigIntNode(0x866C));
   opengl_ns.addConstant("GL_MAP1_VERTEX_ATTRIB13_4_NV",               new QoreBigIntNode(0x866D));
   opengl_ns.addConstant("GL_MAP1_VERTEX_ATTRIB14_4_NV",               new QoreBigIntNode(0x866E));
   opengl_ns.addConstant("GL_MAP1_VERTEX_ATTRIB15_4_NV",               new QoreBigIntNode(0x866F));
   opengl_ns.addConstant("GL_MAP2_VERTEX_ATTRIB0_4_NV",                new QoreBigIntNode(0x8670));
   opengl_ns.addConstant("GL_MAP2_VERTEX_ATTRIB1_4_NV",                new QoreBigIntNode(0x8671));
   opengl_ns.addConstant("GL_MAP2_VERTEX_ATTRIB2_4_NV",                new QoreBigIntNode(0x8672));
   opengl_ns.addConstant("GL_MAP2_VERTEX_ATTRIB3_4_NV",                new QoreBigIntNode(0x8673));
   opengl_ns.addConstant("GL_MAP2_VERTEX_ATTRIB4_4_NV",                new QoreBigIntNode(0x8674));
   opengl_ns.addConstant("GL_MAP2_VERTEX_ATTRIB5_4_NV",                new QoreBigIntNode(0x8675));
   opengl_ns.addConstant("GL_MAP2_VERTEX_ATTRIB6_4_NV",                new QoreBigIntNode(0x8676));
   opengl_ns.addConstant("GL_MAP2_VERTEX_ATTRIB7_4_NV",                new QoreBigIntNode(0x8677));
   opengl_ns.addConstant("GL_MAP2_VERTEX_ATTRIB8_4_NV",                new QoreBigIntNode(0x8678));
   opengl_ns.addConstant("GL_MAP2_VERTEX_ATTRIB9_4_NV",                new QoreBigIntNode(0x8679));
   opengl_ns.addConstant("GL_MAP2_VERTEX_ATTRIB10_4_NV",               new QoreBigIntNode(0x867A));
   opengl_ns.addConstant("GL_MAP2_VERTEX_ATTRIB11_4_NV",               new QoreBigIntNode(0x867B));
   opengl_ns.addConstant("GL_MAP2_VERTEX_ATTRIB12_4_NV",               new QoreBigIntNode(0x867C));
   opengl_ns.addConstant("GL_MAP2_VERTEX_ATTRIB13_4_NV",               new QoreBigIntNode(0x867D));
   opengl_ns.addConstant("GL_MAP2_VERTEX_ATTRIB14_4_NV",               new QoreBigIntNode(0x867E));
   opengl_ns.addConstant("GL_MAP2_VERTEX_ATTRIB15_4_NV",               new QoreBigIntNode(0x867F));
   opengl_ns.addConstant("GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV",    new QoreBigIntNode(0x86D9));
   opengl_ns.addConstant("GL_UNSIGNED_INT_S8_S8_8_8_NV",               new QoreBigIntNode(0x86DA));
   opengl_ns.addConstant("GL_UNSIGNED_INT_8_8_S8_S8_REV_NV",           new QoreBigIntNode(0x86DB));
   opengl_ns.addConstant("GL_DSDT_MAG_INTENSITY_NV",                   new QoreBigIntNode(0x86DC));
   opengl_ns.addConstant("GL_TEXTURE_SHADER_NV",                       new QoreBigIntNode(0x86DE));
   opengl_ns.addConstant("GL_SHADER_OPERATION_NV",                     new QoreBigIntNode(0x86DF));
   opengl_ns.addConstant("GL_CULL_MODES_NV",                           new QoreBigIntNode(0x86E0));
   opengl_ns.addConstant("GL_OFFSET_TEXTURE_MATRIX_NV",                new QoreBigIntNode(0x86E1));
   opengl_ns.addConstant("GL_OFFSET_TEXTURE_SCALE_NV",                 new QoreBigIntNode(0x86E2));
   opengl_ns.addConstant("GL_OFFSET_TEXTURE_BIAS_NV",                  new QoreBigIntNode(0x86E3));
   opengl_ns.addConstant("GL_OFFSET_TEXTURE_2D_MATRIX_NV",             new QoreBigIntNode(GL_OFFSET_TEXTURE_MATRIX_NV));
   opengl_ns.addConstant("GL_OFFSET_TEXTURE_2D_SCALE_NV",              new QoreBigIntNode(GL_OFFSET_TEXTURE_SCALE_NV));
   opengl_ns.addConstant("GL_OFFSET_TEXTURE_2D_BIAS_NV",               new QoreBigIntNode(GL_OFFSET_TEXTURE_BIAS_NV));
   opengl_ns.addConstant("GL_PREVIOUS_TEXTURE_INPUT_NV",               new QoreBigIntNode(0x86E4));
   opengl_ns.addConstant("GL_CONST_EYE_NV",                            new QoreBigIntNode(0x86E5));
   opengl_ns.addConstant("GL_SHADER_CONSISTENT_NV",                    new QoreBigIntNode(0x86DD));
   opengl_ns.addConstant("GL_PASS_THROUGH_NV",                         new QoreBigIntNode(0x86E6));
   opengl_ns.addConstant("GL_CULL_FRAGMENT_NV",                        new QoreBigIntNode(0x86E7));
   opengl_ns.addConstant("GL_OFFSET_TEXTURE_2D_NV",                    new QoreBigIntNode(0x86E8));
   opengl_ns.addConstant("GL_OFFSET_TEXTURE_RECTANGLE_NV",             new QoreBigIntNode(0x864C));
   opengl_ns.addConstant("GL_OFFSET_TEXTURE_RECTANGLE_SCALE_NV",       new QoreBigIntNode(0x864D));
   opengl_ns.addConstant("GL_DEPENDENT_AR_TEXTURE_2D_NV",              new QoreBigIntNode(0x86E9));
   opengl_ns.addConstant("GL_DEPENDENT_GB_TEXTURE_2D_NV",              new QoreBigIntNode(0x86EA));
   opengl_ns.addConstant("GL_DOT_PRODUCT_NV",                          new QoreBigIntNode(0x86EC));
   opengl_ns.addConstant("GL_DOT_PRODUCT_DEPTH_REPLACE_NV",            new QoreBigIntNode(0x86ED));
   opengl_ns.addConstant("GL_DOT_PRODUCT_TEXTURE_2D_NV",               new QoreBigIntNode(0x86EE));
   opengl_ns.addConstant("GL_DOT_PRODUCT_TEXTURE_RECTANGLE_NV",        new QoreBigIntNode(0x864E));
   opengl_ns.addConstant("GL_DOT_PRODUCT_TEXTURE_CUBE_MAP_NV",         new QoreBigIntNode(0x86F0));
   opengl_ns.addConstant("GL_DOT_PRODUCT_DIFFUSE_CUBE_MAP_NV",         new QoreBigIntNode(0x86F1));
   opengl_ns.addConstant("GL_DOT_PRODUCT_REFLECT_CUBE_MAP_NV",         new QoreBigIntNode(0x86F2));
   opengl_ns.addConstant("GL_DOT_PRODUCT_CONST_EYE_REFLECT_CUBE_MAP_NV",new QoreBigIntNode(0x86F3));
   opengl_ns.addConstant("GL_HILO_NV",                                 new QoreBigIntNode(0x86F4));
   opengl_ns.addConstant("GL_DSDT_NV",                                 new QoreBigIntNode(0x86F5));
   opengl_ns.addConstant("GL_DSDT_MAG_NV",                             new QoreBigIntNode(0x86F6));
   opengl_ns.addConstant("GL_DSDT_MAG_VIB_NV",                         new QoreBigIntNode(0x86F7));
   opengl_ns.addConstant("GL_HILO16_NV",                               new QoreBigIntNode(0x86F8));
   opengl_ns.addConstant("GL_SIGNED_HILO_NV",                          new QoreBigIntNode(0x86F9));
   opengl_ns.addConstant("GL_SIGNED_HILO16_NV",                        new QoreBigIntNode(0x86FA));
   opengl_ns.addConstant("GL_SIGNED_RGBA_NV",                          new QoreBigIntNode(0x86FB));
   opengl_ns.addConstant("GL_SIGNED_RGBA8_NV",                         new QoreBigIntNode(0x86FC));
   opengl_ns.addConstant("GL_SIGNED_RGB_NV",                           new QoreBigIntNode(0x86FE));
   opengl_ns.addConstant("GL_SIGNED_RGB8_NV",                          new QoreBigIntNode(0x86FF));
   opengl_ns.addConstant("GL_SIGNED_LUMINANCE_NV",                     new QoreBigIntNode(0x8701));
   opengl_ns.addConstant("GL_SIGNED_LUMINANCE8_NV",                    new QoreBigIntNode(0x8702));
   opengl_ns.addConstant("GL_SIGNED_LUMINANCE_ALPHA_NV",               new QoreBigIntNode(0x8703));
   opengl_ns.addConstant("GL_SIGNED_LUMINANCE8_ALPHA8_NV",             new QoreBigIntNode(0x8704));
   opengl_ns.addConstant("GL_SIGNED_ALPHA_NV",                         new QoreBigIntNode(0x8705));
   opengl_ns.addConstant("GL_SIGNED_ALPHA8_NV",                        new QoreBigIntNode(0x8706));
   opengl_ns.addConstant("GL_SIGNED_INTENSITY_NV",                     new QoreBigIntNode(0x8707));
   opengl_ns.addConstant("GL_SIGNED_INTENSITY8_NV",                    new QoreBigIntNode(0x8708));
   opengl_ns.addConstant("GL_DSDT8_NV",                                new QoreBigIntNode(0x8709));
   opengl_ns.addConstant("GL_DSDT8_MAG8_NV",                           new QoreBigIntNode(0x870A));
   opengl_ns.addConstant("GL_DSDT8_MAG8_INTENSITY8_NV",                new QoreBigIntNode(0x870B));
   opengl_ns.addConstant("GL_SIGNED_RGB_UNSIGNED_ALPHA_NV",            new QoreBigIntNode(0x870C));
   opengl_ns.addConstant("GL_SIGNED_RGB8_UNSIGNED_ALPHA8_NV",          new QoreBigIntNode(0x870D));
   opengl_ns.addConstant("GL_HI_SCALE_NV",                             new QoreBigIntNode(0x870E));
   opengl_ns.addConstant("GL_LO_SCALE_NV",                             new QoreBigIntNode(0x870F));
   opengl_ns.addConstant("GL_DS_SCALE_NV",                             new QoreBigIntNode(0x8710));
   opengl_ns.addConstant("GL_DT_SCALE_NV",                             new QoreBigIntNode(0x8711));
   opengl_ns.addConstant("GL_MAGNITUDE_SCALE_NV",                      new QoreBigIntNode(0x8712));
   opengl_ns.addConstant("GL_VIBRANCE_SCALE_NV",                       new QoreBigIntNode(0x8713));
   opengl_ns.addConstant("GL_HI_BIAS_NV",                              new QoreBigIntNode(0x8714));
   opengl_ns.addConstant("GL_LO_BIAS_NV",                              new QoreBigIntNode(0x8715));
   opengl_ns.addConstant("GL_DS_BIAS_NV",                              new QoreBigIntNode(0x8716));
   opengl_ns.addConstant("GL_DT_BIAS_NV",                              new QoreBigIntNode(0x8717));
   opengl_ns.addConstant("GL_MAGNITUDE_BIAS_NV",                       new QoreBigIntNode(0x8718));
   opengl_ns.addConstant("GL_VIBRANCE_BIAS_NV",                        new QoreBigIntNode(0x8719));
   opengl_ns.addConstant("GL_TEXTURE_BORDER_VALUES_NV",                new QoreBigIntNode(0x871A));
   opengl_ns.addConstant("GL_TEXTURE_HI_SIZE_NV",                      new QoreBigIntNode(0x871B));
   opengl_ns.addConstant("GL_TEXTURE_LO_SIZE_NV",                      new QoreBigIntNode(0x871C));
   opengl_ns.addConstant("GL_TEXTURE_DS_SIZE_NV",                      new QoreBigIntNode(0x871D));
   opengl_ns.addConstant("GL_TEXTURE_DT_SIZE_NV",                      new QoreBigIntNode(0x871E));
   opengl_ns.addConstant("GL_TEXTURE_MAG_SIZE_NV",                     new QoreBigIntNode(0x871F));
   opengl_ns.addConstant("GL_DOT_PRODUCT_TEXTURE_3D_NV",               new QoreBigIntNode(0x86EF));
   opengl_ns.addConstant("GL_OFFSET_PROJECTIVE_TEXTURE_2D_NV",         new QoreBigIntNode(0x8850));
   opengl_ns.addConstant("GL_OFFSET_PROJECTIVE_TEXTURE_2D_SCALE_NV",   new QoreBigIntNode(0x8851));
   opengl_ns.addConstant("GL_OFFSET_PROJECTIVE_TEXTURE_RECTANGLE_NV",  new QoreBigIntNode(0x8852));
   opengl_ns.addConstant("GL_OFFSET_PROJECTIVE_TEXTURE_RECTANGLE_SCALE_NV",new QoreBigIntNode(0x8853));
   opengl_ns.addConstant("GL_OFFSET_HILO_TEXTURE_2D_NV",               new QoreBigIntNode(0x8854));
   opengl_ns.addConstant("GL_OFFSET_HILO_TEXTURE_RECTANGLE_NV",        new QoreBigIntNode(0x8855));
   opengl_ns.addConstant("GL_OFFSET_HILO_PROJECTIVE_TEXTURE_2D_NV",    new QoreBigIntNode(0x8856));
   opengl_ns.addConstant("GL_OFFSET_HILO_PROJECTIVE_TEXTURE_RECTANGLE_NV",new QoreBigIntNode(0x8857));
   opengl_ns.addConstant("GL_DEPENDENT_HILO_TEXTURE_2D_NV",            new QoreBigIntNode(0x8858));
   opengl_ns.addConstant("GL_DEPENDENT_RGB_TEXTURE_3D_NV",             new QoreBigIntNode(0x8859));
   opengl_ns.addConstant("GL_DEPENDENT_RGB_TEXTURE_CUBE_MAP_NV",       new QoreBigIntNode(0x885A));
   opengl_ns.addConstant("GL_DOT_PRODUCT_PASS_THROUGH_NV",             new QoreBigIntNode(0x885B));
   opengl_ns.addConstant("GL_DOT_PRODUCT_TEXTURE_1D_NV",               new QoreBigIntNode(0x885C));
   opengl_ns.addConstant("GL_DOT_PRODUCT_AFFINE_DEPTH_REPLACE_NV",     new QoreBigIntNode(0x885D));
   opengl_ns.addConstant("GL_HILO8_NV",                                new QoreBigIntNode(0x885E));
   opengl_ns.addConstant("GL_SIGNED_HILO8_NV",                         new QoreBigIntNode(0x885F));
   opengl_ns.addConstant("GL_FORCE_BLUE_TO_ONE_NV",                    new QoreBigIntNode(0x8860));
   opengl_ns.addConstant("GL_POINT_SPRITE_NV",                         new QoreBigIntNode(0x8861));
   opengl_ns.addConstant("GL_COORD_REPLACE_NV",                        new QoreBigIntNode(0x8862));
   opengl_ns.addConstant("GL_POINT_SPRITE_R_MODE_NV",                  new QoreBigIntNode(0x8863));
   opengl_ns.addConstant("GL_DEPTH_CLAMP_NV",                          new QoreBigIntNode(0x864F));
   opengl_ns.addConstant("GL_MULTISAMPLE_FILTER_HINT_NV",              new QoreBigIntNode(0x8534));
   opengl_ns.addConstant("GL_MAX_SHININESS_NV",                        new QoreBigIntNode(0x8504));
   opengl_ns.addConstant("GL_MAX_SPOT_EXPONENT_NV",                    new QoreBigIntNode(0x8505));
   opengl_ns.addConstant("GL_MAX_PROGRAM_EXEC_INSTRUCTIONS_NV",        new QoreBigIntNode(0x88F4));
   opengl_ns.addConstant("GL_MAX_PROGRAM_CALL_DEPTH_NV",               new QoreBigIntNode(0x88F5));
   opengl_ns.addConstant("GL_MAX_PROGRAM_IF_DEPTH_NV",                 new QoreBigIntNode(0x88F6));
   opengl_ns.addConstant("GL_MAX_PROGRAM_LOOP_DEPTH_NV",               new QoreBigIntNode(0x88F7));
   opengl_ns.addConstant("GL_MAX_PROGRAM_LOOP_COUNT_NV",               new QoreBigIntNode(0x88F8));
   opengl_ns.addConstant("GL_VERTEX_DATA_HINT_PGI",                    new QoreBigIntNode(0x1A22A));
   opengl_ns.addConstant("GL_VERTEX_CONSISTENT_HINT_PGI",              new QoreBigIntNode(0x1A22B));
   opengl_ns.addConstant("GL_MATERIAL_SIDE_HINT_PGI",                  new QoreBigIntNode(0x1A22C));
   opengl_ns.addConstant("GL_MAX_VERTEX_HINT_PGI",                     new QoreBigIntNode(0x1A22D));
   opengl_ns.addConstant("GL_COLOR3_BIT_PGI",                          new QoreBigIntNode(0x00010000));
   opengl_ns.addConstant("GL_COLOR4_BIT_PGI",                          new QoreBigIntNode(0x00020000));
   opengl_ns.addConstant("GL_EDGEFLAG_BIT_PGI",                        new QoreBigIntNode(0x00040000));
   opengl_ns.addConstant("GL_INDEX_BIT_PGI",                           new QoreBigIntNode(0x00080000));
   opengl_ns.addConstant("GL_MAT_AMBIENT_BIT_PGI",                     new QoreBigIntNode(0x00100000));
   opengl_ns.addConstant("GL_MAT_AMBIENT_AND_DIFFUSE_BIT_PGI",         new QoreBigIntNode(0x00200000));
   opengl_ns.addConstant("GL_MAT_DIFFUSE_BIT_PGI",                     new QoreBigIntNode(0x00400000));
   opengl_ns.addConstant("GL_MAT_EMISSION_BIT_PGI",                    new QoreBigIntNode(0x00800000));
   opengl_ns.addConstant("GL_MAT_COLOR_INDEXES_BIT_PGI",               new QoreBigIntNode(0x01000000));
   opengl_ns.addConstant("GL_MAT_SHININESS_BIT_PGI",                   new QoreBigIntNode(0x02000000));
   opengl_ns.addConstant("GL_MAT_SPECULAR_BIT_PGI",                    new QoreBigIntNode(0x04000000));
   opengl_ns.addConstant("GL_NORMAL_BIT_PGI",                          new QoreBigIntNode(0x08000000));
   opengl_ns.addConstant("GL_TEXCOORD1_BIT_PGI",                       new QoreBigIntNode(0x10000000));
   opengl_ns.addConstant("GL_TEXCOORD2_BIT_PGI",                       new QoreBigIntNode(0x20000000));
   opengl_ns.addConstant("GL_TEXCOORD3_BIT_PGI",                       new QoreBigIntNode(0x40000000));
   opengl_ns.addConstant("GL_TEXCOORD4_BIT_PGI",                       new QoreBigIntNode(0x80000000));
   opengl_ns.addConstant("GL_VERTEX23_BIT_PGI",                        new QoreBigIntNode(0x00000004));
   opengl_ns.addConstant("GL_VERTEX4_BIT_PGI",                         new QoreBigIntNode(0x00000008));
   opengl_ns.addConstant("GL_PREFER_DOUBLEBUFFER_HINT_PGI",            new QoreBigIntNode(0x1A1F8));
   opengl_ns.addConstant("GL_CONSERVE_MEMORY_HINT_PGI",                new QoreBigIntNode(0x1A1FD));
   opengl_ns.addConstant("GL_RECLAIM_MEMORY_HINT_PGI",                 new QoreBigIntNode(0x1A1FE));
   opengl_ns.addConstant("GL_NATIVE_GRAPHICS_HANDLE_PGI",              new QoreBigIntNode(0x1A202));
   opengl_ns.addConstant("GL_NATIVE_GRAPHICS_BEGIN_HINT_PGI",          new QoreBigIntNode(0x1A203));
   opengl_ns.addConstant("GL_NATIVE_GRAPHICS_END_HINT_PGI",            new QoreBigIntNode(0x1A204));
   opengl_ns.addConstant("GL_ALWAYS_FAST_HINT_PGI",                    new QoreBigIntNode(0x1A20C));
   opengl_ns.addConstant("GL_ALWAYS_SOFT_HINT_PGI",                    new QoreBigIntNode(0x1A20D));
   opengl_ns.addConstant("GL_ALLOW_DRAW_OBJ_HINT_PGI",                 new QoreBigIntNode(0x1A20E));
   opengl_ns.addConstant("GL_ALLOW_DRAW_WIN_HINT_PGI",                 new QoreBigIntNode(0x1A20F));
   opengl_ns.addConstant("GL_ALLOW_DRAW_FRG_HINT_PGI",                 new QoreBigIntNode(0x1A210));
   opengl_ns.addConstant("GL_ALLOW_DRAW_MEM_HINT_PGI",                 new QoreBigIntNode(0x1A211));
   opengl_ns.addConstant("GL_STRICT_DEPTHFUNC_HINT_PGI",               new QoreBigIntNode(0x1A216));
   opengl_ns.addConstant("GL_STRICT_LIGHTING_HINT_PGI",                new QoreBigIntNode(0x1A217));
   opengl_ns.addConstant("GL_STRICT_SCISSOR_HINT_PGI",                 new QoreBigIntNode(0x1A218));
   opengl_ns.addConstant("GL_FULL_STIPPLE_HINT_PGI",                   new QoreBigIntNode(0x1A219));
   opengl_ns.addConstant("GL_CLIP_NEAR_HINT_PGI",                      new QoreBigIntNode(0x1A220));
   opengl_ns.addConstant("GL_CLIP_FAR_HINT_PGI",                       new QoreBigIntNode(0x1A221));
   opengl_ns.addConstant("GL_WIDE_LINE_HINT_PGI",                      new QoreBigIntNode(0x1A222));
   opengl_ns.addConstant("GL_BACK_NORMALS_HINT_PGI",                   new QoreBigIntNode(0x1A223));
   opengl_ns.addConstant("GL_SCREEN_COORDINATES_REND",                 new QoreBigIntNode(0x8490));
   opengl_ns.addConstant("GL_INVERTED_SCREEN_W_REND",                  new QoreBigIntNode(0x8491));
   opengl_ns.addConstant("GL_COLOR_MATRIX_SGI",                        new QoreBigIntNode(0x80B1));
   opengl_ns.addConstant("GL_COLOR_MATRIX_STACK_DEPTH_SGI",            new QoreBigIntNode(0x80B2));
   opengl_ns.addConstant("GL_MAX_COLOR_MATRIX_STACK_DEPTH_SGI",        new QoreBigIntNode(0x80B3));
   opengl_ns.addConstant("GL_POST_COLOR_MATRIX_RED_SCALE_SGI",         new QoreBigIntNode(0x80B4));
   opengl_ns.addConstant("GL_POST_COLOR_MATRIX_GREEN_SCALE_SGI",       new QoreBigIntNode(0x80B5));
   opengl_ns.addConstant("GL_POST_COLOR_MATRIX_BLUE_SCALE_SGI",        new QoreBigIntNode(0x80B6));
   opengl_ns.addConstant("GL_POST_COLOR_MATRIX_ALPHA_SCALE_SGI",       new QoreBigIntNode(0x80B7));
   opengl_ns.addConstant("GL_POST_COLOR_MATRIX_RED_BIAS_SGI",          new QoreBigIntNode(0x80B8));
   opengl_ns.addConstant("GL_POST_COLOR_MATRIX_GREEN_BIAS_SGI",        new QoreBigIntNode(0x80B9));
   opengl_ns.addConstant("GL_POST_COLOR_MATRIX_BLUE_BIAS_SGI",         new QoreBigIntNode(0x80BA));
   opengl_ns.addConstant("GL_POST_COLOR_MATRIX_ALPHA_BIAS_SGI",        new QoreBigIntNode(0x80BB));
   opengl_ns.addConstant("GL_COLOR_TABLE_SGI",                         new QoreBigIntNode(0x80D0));
   opengl_ns.addConstant("GL_POST_CONVOLUTION_COLOR_TABLE_SGI",        new QoreBigIntNode(0x80D1));
   opengl_ns.addConstant("GL_POST_COLOR_MATRIX_COLOR_TABLE_SGI",       new QoreBigIntNode(0x80D2));
   opengl_ns.addConstant("GL_PROXY_COLOR_TABLE_SGI",                   new QoreBigIntNode(0x80D3));
   opengl_ns.addConstant("GL_PROXY_POST_CONVOLUTION_COLOR_TABLE_SGI",  new QoreBigIntNode(0x80D4));
   opengl_ns.addConstant("GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE_SGI", new QoreBigIntNode(0x80D5));
   opengl_ns.addConstant("GL_COLOR_TABLE_SCALE_SGI",                   new QoreBigIntNode(0x80D6));
   opengl_ns.addConstant("GL_COLOR_TABLE_BIAS_SGI",                    new QoreBigIntNode(0x80D7));
   opengl_ns.addConstant("GL_COLOR_TABLE_FORMAT_SGI",                  new QoreBigIntNode(0x80D8));
   opengl_ns.addConstant("GL_COLOR_TABLE_WIDTH_SGI",                   new QoreBigIntNode(0x80D9));
   opengl_ns.addConstant("GL_COLOR_TABLE_RED_SIZE_SGI",                new QoreBigIntNode(0x80DA));
   opengl_ns.addConstant("GL_COLOR_TABLE_GREEN_SIZE_SGI",              new QoreBigIntNode(0x80DB));
   opengl_ns.addConstant("GL_COLOR_TABLE_BLUE_SIZE_SGI",               new QoreBigIntNode(0x80DC));
   opengl_ns.addConstant("GL_COLOR_TABLE_ALPHA_SIZE_SGI",              new QoreBigIntNode(0x80DD));
   opengl_ns.addConstant("GL_COLOR_TABLE_LUMINANCE_SIZE_SGI",          new QoreBigIntNode(0x80DE));
   opengl_ns.addConstant("GL_COLOR_TABLE_INTENSITY_SIZE_SGI",          new QoreBigIntNode(0x80DF));
   opengl_ns.addConstant("GL_TEXTURE_COLOR_TABLE_SGI",                 new QoreBigIntNode(0x80BC));
   opengl_ns.addConstant("GL_PROXY_TEXTURE_COLOR_TABLE_SGI",           new QoreBigIntNode(0x80BD));
   opengl_ns.addConstant("GL_DEPTH_PASS_INSTRUMENT_SGIX",              new QoreBigIntNode(0x8310));
   opengl_ns.addConstant("GL_DEPTH_PASS_INSTRUMENT_COUNTERS_SGIX",     new QoreBigIntNode(0x8311));
   opengl_ns.addConstant("GL_DEPTH_PASS_INSTRUMENT_MAX_SGIX",          new QoreBigIntNode(0x8312));
   opengl_ns.addConstant("GL_FILTER4_SGIS",                            new QoreBigIntNode(0x8146));
   opengl_ns.addConstant("GL_TEXTURE_FILTER4_SIZE_SGIS",               new QoreBigIntNode(0x8147));
   opengl_ns.addConstant("GL_PIXEL_TEXTURE_SGIS",                      new QoreBigIntNode(0x8353));
   opengl_ns.addConstant("GL_PIXEL_FRAGMENT_RGB_SOURCE_SGIS",          new QoreBigIntNode(0x8354));
   opengl_ns.addConstant("GL_PIXEL_FRAGMENT_ALPHA_SOURCE_SGIS",        new QoreBigIntNode(0x8355));
   opengl_ns.addConstant("GL_PIXEL_GROUP_COLOR_SGIS",                  new QoreBigIntNode(0x8356));
   opengl_ns.addConstant("GL_PACK_SKIP_VOLUMES_SGIS",                  new QoreBigIntNode(0x8130));
   opengl_ns.addConstant("GL_PACK_IMAGE_DEPTH_SGIS",                   new QoreBigIntNode(0x8131));
   opengl_ns.addConstant("GL_UNPACK_SKIP_VOLUMES_SGIS",                new QoreBigIntNode(0x8132));
   opengl_ns.addConstant("GL_UNPACK_IMAGE_DEPTH_SGIS",                 new QoreBigIntNode(0x8133));
   opengl_ns.addConstant("GL_TEXTURE_4D_SGIS",                         new QoreBigIntNode(0x8134));
   opengl_ns.addConstant("GL_PROXY_TEXTURE_4D_SGIS",                   new QoreBigIntNode(0x8135));
   opengl_ns.addConstant("GL_TEXTURE_4DSIZE_SGIS",                     new QoreBigIntNode(0x8136));
   opengl_ns.addConstant("GL_TEXTURE_WRAP_Q_SGIS",                     new QoreBigIntNode(0x8137));
   opengl_ns.addConstant("GL_MAX_4D_TEXTURE_SIZE_SGIS",                new QoreBigIntNode(0x8138));
   opengl_ns.addConstant("GL_TEXTURE_4D_BINDING_SGIS",                 new QoreBigIntNode(0x814F));
   opengl_ns.addConstant("GL_DETAIL_TEXTURE_2D_SGIS",                  new QoreBigIntNode(0x8095));
   opengl_ns.addConstant("GL_DETAIL_TEXTURE_2D_BINDING_SGIS",          new QoreBigIntNode(0x8096));
   opengl_ns.addConstant("GL_LINEAR_DETAIL_SGIS",                      new QoreBigIntNode(0x8097));
   opengl_ns.addConstant("GL_LINEAR_DETAIL_ALPHA_SGIS",                new QoreBigIntNode(0x8098));
   opengl_ns.addConstant("GL_LINEAR_DETAIL_COLOR_SGIS",                new QoreBigIntNode(0x8099));
   opengl_ns.addConstant("GL_DETAIL_TEXTURE_LEVEL_SGIS",               new QoreBigIntNode(0x809A));
   opengl_ns.addConstant("GL_DETAIL_TEXTURE_MODE_SGIS",                new QoreBigIntNode(0x809B));
   opengl_ns.addConstant("GL_DETAIL_TEXTURE_FUNC_POINTS_SGIS",         new QoreBigIntNode(0x809C));
   opengl_ns.addConstant("GL_LINEAR_SHARPEN_SGIS",                     new QoreBigIntNode(0x80AD));
   opengl_ns.addConstant("GL_LINEAR_SHARPEN_ALPHA_SGIS",               new QoreBigIntNode(0x80AE));
   opengl_ns.addConstant("GL_LINEAR_SHARPEN_COLOR_SGIS",               new QoreBigIntNode(0x80AF));
   opengl_ns.addConstant("GL_SHARPEN_TEXTURE_FUNC_POINTS_SGIS",        new QoreBigIntNode(0x80B0));
   opengl_ns.addConstant("GL_TEXTURE_MIN_LOD_SGIS",                    new QoreBigIntNode(0x813A));
   opengl_ns.addConstant("GL_TEXTURE_MAX_LOD_SGIS",                    new QoreBigIntNode(0x813B));
   opengl_ns.addConstant("GL_TEXTURE_BASE_LEVEL_SGIS",                 new QoreBigIntNode(0x813C));
   opengl_ns.addConstant("GL_TEXTURE_MAX_LEVEL_SGIS",                  new QoreBigIntNode(0x813D));
   opengl_ns.addConstant("GL_MULTISAMPLE_SGIS",                        new QoreBigIntNode(0x809D));
   opengl_ns.addConstant("GL_SAMPLE_ALPHA_TO_MASK_SGIS",               new QoreBigIntNode(0x809E));
   opengl_ns.addConstant("GL_SAMPLE_ALPHA_TO_ONE_SGIS",                new QoreBigIntNode(0x809F));
   opengl_ns.addConstant("GL_SAMPLE_MASK_SGIS",                        new QoreBigIntNode(0x80A0));
   opengl_ns.addConstant("GL_1PASS_SGIS",                              new QoreBigIntNode(0x80A1));
   opengl_ns.addConstant("GL_2PASS_0_SGIS",                            new QoreBigIntNode(0x80A2));
   opengl_ns.addConstant("GL_2PASS_1_SGIS",                            new QoreBigIntNode(0x80A3));
   opengl_ns.addConstant("GL_4PASS_0_SGIS",                            new QoreBigIntNode(0x80A4));
   opengl_ns.addConstant("GL_4PASS_1_SGIS",                            new QoreBigIntNode(0x80A5));
   opengl_ns.addConstant("GL_4PASS_2_SGIS",                            new QoreBigIntNode(0x80A6));
   opengl_ns.addConstant("GL_4PASS_3_SGIS",                            new QoreBigIntNode(0x80A7));
   opengl_ns.addConstant("GL_SAMPLE_BUFFERS_SGIS",                     new QoreBigIntNode(0x80A8));
   opengl_ns.addConstant("GL_SAMPLES_SGIS",                            new QoreBigIntNode(0x80A9));
   opengl_ns.addConstant("GL_SAMPLE_MASK_VALUE_SGIS",                  new QoreBigIntNode(0x80AA));
   opengl_ns.addConstant("GL_SAMPLE_MASK_INVERT_SGIS",                 new QoreBigIntNode(0x80AB));
   opengl_ns.addConstant("GL_SAMPLE_PATTERN_SGIS",                     new QoreBigIntNode(0x80AC));
   opengl_ns.addConstant("GL_GENERATE_MIPMAP_SGIS",                    new QoreBigIntNode(0x8191));
   opengl_ns.addConstant("GL_GENERATE_MIPMAP_HINT_SGIS",               new QoreBigIntNode(0x8192));
   opengl_ns.addConstant("GL_CLAMP_TO_EDGE_SGIS",                      new QoreBigIntNode(0x812F));
   opengl_ns.addConstant("GL_CLAMP_TO_BORDER_SGIS",                    new QoreBigIntNode(0x812D));
   opengl_ns.addConstant("GL_DUAL_ALPHA4_SGIS",                        new QoreBigIntNode(0x8110));
   opengl_ns.addConstant("GL_DUAL_ALPHA8_SGIS",                        new QoreBigIntNode(0x8111));
   opengl_ns.addConstant("GL_DUAL_ALPHA12_SGIS",                       new QoreBigIntNode(0x8112));
   opengl_ns.addConstant("GL_DUAL_ALPHA16_SGIS",                       new QoreBigIntNode(0x8113));
   opengl_ns.addConstant("GL_DUAL_LUMINANCE4_SGIS",                    new QoreBigIntNode(0x8114));
   opengl_ns.addConstant("GL_DUAL_LUMINANCE8_SGIS",                    new QoreBigIntNode(0x8115));
   opengl_ns.addConstant("GL_DUAL_LUMINANCE12_SGIS",                   new QoreBigIntNode(0x8116));
   opengl_ns.addConstant("GL_DUAL_LUMINANCE16_SGIS",                   new QoreBigIntNode(0x8117));
   opengl_ns.addConstant("GL_DUAL_INTENSITY4_SGIS",                    new QoreBigIntNode(0x8118));
   opengl_ns.addConstant("GL_DUAL_INTENSITY8_SGIS",                    new QoreBigIntNode(0x8119));
   opengl_ns.addConstant("GL_DUAL_INTENSITY12_SGIS",                   new QoreBigIntNode(0x811A));
   opengl_ns.addConstant("GL_DUAL_INTENSITY16_SGIS",                   new QoreBigIntNode(0x811B));
   opengl_ns.addConstant("GL_DUAL_LUMINANCE_ALPHA4_SGIS",              new QoreBigIntNode(0x811C));
   opengl_ns.addConstant("GL_DUAL_LUMINANCE_ALPHA8_SGIS",              new QoreBigIntNode(0x811D));
   opengl_ns.addConstant("GL_QUAD_ALPHA4_SGIS",                        new QoreBigIntNode(0x811E));
   opengl_ns.addConstant("GL_QUAD_ALPHA8_SGIS",                        new QoreBigIntNode(0x811F));
   opengl_ns.addConstant("GL_QUAD_LUMINANCE4_SGIS",                    new QoreBigIntNode(0x8120));
   opengl_ns.addConstant("GL_QUAD_LUMINANCE8_SGIS",                    new QoreBigIntNode(0x8121));
   opengl_ns.addConstant("GL_QUAD_INTENSITY4_SGIS",                    new QoreBigIntNode(0x8122));
   opengl_ns.addConstant("GL_QUAD_INTENSITY8_SGIS",                    new QoreBigIntNode(0x8123));
   opengl_ns.addConstant("GL_DUAL_TEXTURE_SELECT_SGIS",                new QoreBigIntNode(0x8124));
   opengl_ns.addConstant("GL_QUAD_TEXTURE_SELECT_SGIS",                new QoreBigIntNode(0x8125));
   opengl_ns.addConstant("GL_POINT_SIZE_MIN_EXT",                      new QoreBigIntNode(0x8126));
   opengl_ns.addConstant("GL_POINT_SIZE_MIN_SGIS",                     new QoreBigIntNode(0x8126));
   opengl_ns.addConstant("GL_POINT_SIZE_MAX_EXT",                      new QoreBigIntNode(0x8127));
   opengl_ns.addConstant("GL_POINT_SIZE_MAX_SGIS",                     new QoreBigIntNode(0x8127));
   opengl_ns.addConstant("GL_POINT_FADE_THRESHOLD_SIZE_EXT",           new QoreBigIntNode(0x8128));
   opengl_ns.addConstant("GL_POINT_FADE_THRESHOLD_SIZE_SGIS",          new QoreBigIntNode(0x8128));
   opengl_ns.addConstant("GL_DISTANCE_ATTENUATION_EXT",                new QoreBigIntNode(0x8129));
   opengl_ns.addConstant("GL_DISTANCE_ATTENUATION_SGIS",               new QoreBigIntNode(0x8129));
   opengl_ns.addConstant("GL_FOG_FUNC_SGIS",                           new QoreBigIntNode(0x812A));
   opengl_ns.addConstant("GL_FOG_FUNC_POINTS_SGIS",                    new QoreBigIntNode(0x812B));
   opengl_ns.addConstant("GL_MAX_FOG_FUNC_POINTS_SGIS",                new QoreBigIntNode(0x812C));
   opengl_ns.addConstant("GL_EYE_DISTANCE_TO_POINT_SGIS",              new QoreBigIntNode(0x81F0));
   opengl_ns.addConstant("GL_OBJECT_DISTANCE_TO_POINT_SGIS",           new QoreBigIntNode(0x81F1));
   opengl_ns.addConstant("GL_EYE_DISTANCE_TO_LINE_SGIS",               new QoreBigIntNode(0x81F2));
   opengl_ns.addConstant("GL_OBJECT_DISTANCE_TO_LINE_SGIS",            new QoreBigIntNode(0x81F3));
   opengl_ns.addConstant("GL_EYE_POINT_SGIS",                          new QoreBigIntNode(0x81F4));
   opengl_ns.addConstant("GL_OBJECT_POINT_SGIS",                       new QoreBigIntNode(0x81F5));
   opengl_ns.addConstant("GL_EYE_LINE_SGIS",                           new QoreBigIntNode(0x81F6));
   opengl_ns.addConstant("GL_OBJECT_LINE_SGIS",                        new QoreBigIntNode(0x81F7));
   opengl_ns.addConstant("GL_TEXTURE_COLOR_WRITEMASK_SGIS",            new QoreBigIntNode(0x81EF));
   opengl_ns.addConstant("GL_PIXEL_TEX_GEN_SGIX",                      new QoreBigIntNode(0x8139));
   opengl_ns.addConstant("GL_PIXEL_TEX_GEN_MODE_SGIX",                 new QoreBigIntNode(0x832B));
   opengl_ns.addConstant("GL_LINEAR_CLIPMAP_LINEAR_SGIX",              new QoreBigIntNode(0x8170));
   opengl_ns.addConstant("GL_TEXTURE_CLIPMAP_CENTER_SGIX",             new QoreBigIntNode(0x8171));
   opengl_ns.addConstant("GL_TEXTURE_CLIPMAP_FRAME_SGIX",              new QoreBigIntNode(0x8172));
   opengl_ns.addConstant("GL_TEXTURE_CLIPMAP_OFFSET_SGIX",             new QoreBigIntNode(0x8173));
   opengl_ns.addConstant("GL_TEXTURE_CLIPMAP_VIRTUAL_DEPTH_SGIX",      new QoreBigIntNode(0x8174));
   opengl_ns.addConstant("GL_TEXTURE_CLIPMAP_LOD_OFFSET_SGIX",         new QoreBigIntNode(0x8175));
   opengl_ns.addConstant("GL_TEXTURE_CLIPMAP_DEPTH_SGIX",              new QoreBigIntNode(0x8176));
   opengl_ns.addConstant("GL_MAX_CLIPMAP_DEPTH_SGIX",                  new QoreBigIntNode(0x8177));
   opengl_ns.addConstant("GL_MAX_CLIPMAP_VIRTUAL_DEPTH_SGIX",          new QoreBigIntNode(0x8178));
   opengl_ns.addConstant("GL_NEAREST_CLIPMAP_NEAREST_SGIX",            new QoreBigIntNode(0x844D));
   opengl_ns.addConstant("GL_NEAREST_CLIPMAP_LINEAR_SGIX",             new QoreBigIntNode(0x844E));
   opengl_ns.addConstant("GL_LINEAR_CLIPMAP_NEAREST_SGIX",             new QoreBigIntNode(0x844F));
   opengl_ns.addConstant("GL_TEXTURE_COMPARE_SGIX",                    new QoreBigIntNode(0x819A));
   opengl_ns.addConstant("GL_TEXTURE_COMPARE_OPERATOR_SGIX",           new QoreBigIntNode(0x819B));
   opengl_ns.addConstant("GL_TEXTURE_LEQUAL_R_SGIX",                   new QoreBigIntNode(0x819C));
   opengl_ns.addConstant("GL_TEXTURE_GEQUAL_R_SGIX",                   new QoreBigIntNode(0x819D));
   opengl_ns.addConstant("GL_INTERLACE_SGIX",                          new QoreBigIntNode(0x8094));
   opengl_ns.addConstant("GL_PIXEL_TILE_BEST_ALIGNMENT_SGIX",          new QoreBigIntNode(0x813E));
   opengl_ns.addConstant("GL_PIXEL_TILE_CACHE_INCREMENT_SGIX",         new QoreBigIntNode(0x813F));
   opengl_ns.addConstant("GL_PIXEL_TILE_WIDTH_SGIX",                   new QoreBigIntNode(0x8140));
   opengl_ns.addConstant("GL_PIXEL_TILE_HEIGHT_SGIX",                  new QoreBigIntNode(0x8141));
   opengl_ns.addConstant("GL_PIXEL_TILE_GRID_WIDTH_SGIX",              new QoreBigIntNode(0x8142));
   opengl_ns.addConstant("GL_PIXEL_TILE_GRID_HEIGHT_SGIX",             new QoreBigIntNode(0x8143));
   opengl_ns.addConstant("GL_PIXEL_TILE_GRID_DEPTH_SGIX",              new QoreBigIntNode(0x8144));
   opengl_ns.addConstant("GL_PIXEL_TILE_CACHE_SIZE_SGIX",              new QoreBigIntNode(0x8145));
   opengl_ns.addConstant("GL_SPRITE_SGIX",                             new QoreBigIntNode(0x8148));
   opengl_ns.addConstant("GL_SPRITE_MODE_SGIX",                        new QoreBigIntNode(0x8149));
   opengl_ns.addConstant("GL_SPRITE_AXIS_SGIX",                        new QoreBigIntNode(0x814A));
   opengl_ns.addConstant("GL_SPRITE_TRANSLATION_SGIX",                 new QoreBigIntNode(0x814B));
   opengl_ns.addConstant("GL_SPRITE_AXIAL_SGIX",                       new QoreBigIntNode(0x814C));
   opengl_ns.addConstant("GL_SPRITE_OBJECT_ALIGNED_SGIX",              new QoreBigIntNode(0x814D));
   opengl_ns.addConstant("GL_SPRITE_EYE_ALIGNED_SGIX",                 new QoreBigIntNode(0x814E));
   opengl_ns.addConstant("GL_TEXTURE_MULTI_BUFFER_HINT_SGIX",          new QoreBigIntNode(0x812E));
   opengl_ns.addConstant("GL_INSTRUMENT_BUFFER_POINTER_SGIX",          new QoreBigIntNode(0x8180));
   opengl_ns.addConstant("GL_INSTRUMENT_MEASUREMENTS_SGIX",            new QoreBigIntNode(0x8181));
   opengl_ns.addConstant("GL_POST_TEXTURE_FILTER_BIAS_SGIX",           new QoreBigIntNode(0x8179));
   opengl_ns.addConstant("GL_POST_TEXTURE_FILTER_SCALE_SGIX",          new QoreBigIntNode(0x817A));
   opengl_ns.addConstant("GL_POST_TEXTURE_FILTER_BIAS_RANGE_SGIX",     new QoreBigIntNode(0x817B));
   opengl_ns.addConstant("GL_POST_TEXTURE_FILTER_SCALE_RANGE_SGIX",    new QoreBigIntNode(0x817C));
   opengl_ns.addConstant("GL_FRAMEZOOM_SGIX",                          new QoreBigIntNode(0x818B));
   opengl_ns.addConstant("GL_FRAMEZOOM_FACTOR_SGIX",                   new QoreBigIntNode(0x818C));
   opengl_ns.addConstant("GL_MAX_FRAMEZOOM_FACTOR_SGIX",               new QoreBigIntNode(0x818D));
   opengl_ns.addConstant("GL_GEOMETRY_DEFORMATION_SGIX",               new QoreBigIntNode(0x8194));
   opengl_ns.addConstant("GL_TEXTURE_DEFORMATION_SGIX",                new QoreBigIntNode(0x8195));
   opengl_ns.addConstant("GL_DEFORMATIONS_MASK_SGIX",                  new QoreBigIntNode(0x8196));
   opengl_ns.addConstant("GL_MAX_DEFORMATION_ORDER_SGIX",              new QoreBigIntNode(0x8197));
   opengl_ns.addConstant("GL_REFERENCE_PLANE_SGIX",                    new QoreBigIntNode(0x817D));
   opengl_ns.addConstant("GL_REFERENCE_PLANE_EQUATION_SGIX",           new QoreBigIntNode(0x817E));
   opengl_ns.addConstant("GL_DEPTH_COMPONENT16_SGIX",                  new QoreBigIntNode(0x81A5));
   opengl_ns.addConstant("GL_DEPTH_COMPONENT24_SGIX",                  new QoreBigIntNode(0x81A6));
   opengl_ns.addConstant("GL_DEPTH_COMPONENT32_SGIX",                  new QoreBigIntNode(0x81A7));
   opengl_ns.addConstant("GL_FOG_OFFSET_SGIX",                         new QoreBigIntNode(0x8198));
   opengl_ns.addConstant("GL_FOG_OFFSET_VALUE_SGIX",                   new QoreBigIntNode(0x8199));
   opengl_ns.addConstant("GL_TEXTURE_ENV_BIAS_SGIX",                   new QoreBigIntNode(0x80BE));
   opengl_ns.addConstant("GL_LIST_PRIORITY_SGIX",                      new QoreBigIntNode(0x8182));
   opengl_ns.addConstant("GL_IR_INSTRUMENT1_SGIX",                     new QoreBigIntNode(0x817F));
   opengl_ns.addConstant("GL_CALLIGRAPHIC_FRAGMENT_SGIX",              new QoreBigIntNode(0x8183));
   opengl_ns.addConstant("GL_TEXTURE_LOD_BIAS_S_SGIX",                 new QoreBigIntNode(0x818E));
   opengl_ns.addConstant("GL_TEXTURE_LOD_BIAS_T_SGIX",                 new QoreBigIntNode(0x818F));
   opengl_ns.addConstant("GL_TEXTURE_LOD_BIAS_R_SGIX",                 new QoreBigIntNode(0x8190));
   opengl_ns.addConstant("GL_YCRCB_422_SGIX",                          new QoreBigIntNode(0x81BB));
   opengl_ns.addConstant("GL_YCRCB_444_SGIX",                          new QoreBigIntNode(0x81BC));
   opengl_ns.addConstant("GL_FRAGMENT_LIGHTING_SGIX",                  new QoreBigIntNode(0x8400));
   opengl_ns.addConstant("GL_FRAGMENT_COLOR_MATERIAL_SGIX",            new QoreBigIntNode(0x8401));
   opengl_ns.addConstant("GL_FRAGMENT_COLOR_MATERIAL_FACE_SGIX",       new QoreBigIntNode(0x8402));
   opengl_ns.addConstant("GL_FRAGMENT_COLOR_MATERIAL_PARAMETER_SGIX",  new QoreBigIntNode(0x8403));
   opengl_ns.addConstant("GL_MAX_FRAGMENT_LIGHTS_SGIX",                new QoreBigIntNode(0x8404));
   opengl_ns.addConstant("GL_MAX_ACTIVE_LIGHTS_SGIX",                  new QoreBigIntNode(0x8405));
   opengl_ns.addConstant("GL_CURRENT_RASTER_NORMAL_SGIX",              new QoreBigIntNode(0x8406));
   opengl_ns.addConstant("GL_LIGHT_ENV_MODE_SGIX",                     new QoreBigIntNode(0x8407));
   opengl_ns.addConstant("GL_FRAGMENT_LIGHT_MODEL_LOCAL_VIEWER_SGIX",  new QoreBigIntNode(0x8408));
   opengl_ns.addConstant("GL_FRAGMENT_LIGHT_MODEL_TWO_SIDE_SGIX",      new QoreBigIntNode(0x8409));
   opengl_ns.addConstant("GL_FRAGMENT_LIGHT_MODEL_AMBIENT_SGIX",       new QoreBigIntNode(0x840A));
   opengl_ns.addConstant("GL_FRAGMENT_LIGHT_MODEL_NORMAL_INTERPOLATION_SGIX",new QoreBigIntNode(0x840B));
   opengl_ns.addConstant("GL_FRAGMENT_LIGHT0_SGIX",                    new QoreBigIntNode(0x840C));
   opengl_ns.addConstant("GL_FRAGMENT_LIGHT1_SGIX",                    new QoreBigIntNode(0x840D));
   opengl_ns.addConstant("GL_FRAGMENT_LIGHT2_SGIX",                    new QoreBigIntNode(0x840E));
   opengl_ns.addConstant("GL_FRAGMENT_LIGHT3_SGIX",                    new QoreBigIntNode(0x840F));
   opengl_ns.addConstant("GL_FRAGMENT_LIGHT4_SGIX",                    new QoreBigIntNode(0x8410));
   opengl_ns.addConstant("GL_FRAGMENT_LIGHT5_SGIX",                    new QoreBigIntNode(0x8411));
   opengl_ns.addConstant("GL_FRAGMENT_LIGHT6_SGIX",                    new QoreBigIntNode(0x8412));
   opengl_ns.addConstant("GL_FRAGMENT_LIGHT7_SGIX",                    new QoreBigIntNode(0x8413));
   opengl_ns.addConstant("GL_ALPHA_MIN_SGIX",                          new QoreBigIntNode(0x8320));
   opengl_ns.addConstant("GL_ALPHA_MAX_SGIX",                          new QoreBigIntNode(0x8321));
   opengl_ns.addConstant("GL_ASYNC_MARKER_SGIX",                       new QoreBigIntNode(0x8329));
   opengl_ns.addConstant("GL_ASYNC_TEX_IMAGE_SGIX",                    new QoreBigIntNode(0x835C));
   opengl_ns.addConstant("GL_ASYNC_DRAW_PIXELS_SGIX",                  new QoreBigIntNode(0x835D));
   opengl_ns.addConstant("GL_ASYNC_READ_PIXELS_SGIX",                  new QoreBigIntNode(0x835E));
   opengl_ns.addConstant("GL_MAX_ASYNC_TEX_IMAGE_SGIX",                new QoreBigIntNode(0x835F));
   opengl_ns.addConstant("GL_MAX_ASYNC_DRAW_PIXELS_SGIX",              new QoreBigIntNode(0x8360));
   opengl_ns.addConstant("GL_MAX_ASYNC_READ_PIXELS_SGIX",              new QoreBigIntNode(0x8361));
   opengl_ns.addConstant("GL_ASYNC_HISTOGRAM_SGIX",                    new QoreBigIntNode(0x832C));
   opengl_ns.addConstant("GL_MAX_ASYNC_HISTOGRAM_SGIX",                new QoreBigIntNode(0x832D));
   opengl_ns.addConstant("GL_FOG_SCALE_SGIX",                          new QoreBigIntNode(0x81FC));
   opengl_ns.addConstant("GL_FOG_SCALE_VALUE_SGIX",                    new QoreBigIntNode(0x81FD));
   opengl_ns.addConstant("GL_PACK_SUBSAMPLE_RATE_SGIX",                new QoreBigIntNode(0x85A0));
   opengl_ns.addConstant("GL_UNPACK_SUBSAMPLE_RATE_SGIX",              new QoreBigIntNode(0x85A1));
   opengl_ns.addConstant("GL_PIXEL_SUBSAMPLE_4444_SGIX",               new QoreBigIntNode(0x85A2));
   opengl_ns.addConstant("GL_PIXEL_SUBSAMPLE_2424_SGIX",               new QoreBigIntNode(0x85A3));
   opengl_ns.addConstant("GL_PIXEL_SUBSAMPLE_4242_SGIX",               new QoreBigIntNode(0x85A4));
   opengl_ns.addConstant("GL_YCRCB_SGIX",                              new QoreBigIntNode(0x8318));
   opengl_ns.addConstant("GL_YCRCBA_SGIX",                             new QoreBigIntNode(0x8319));
   opengl_ns.addConstant("GL_VERTEX_PRECLIP_SGIX",                     new QoreBigIntNode(0x83EE));
   opengl_ns.addConstant("GL_VERTEX_PRECLIP_HINT_SGIX",                new QoreBigIntNode(0x83EF));
   opengl_ns.addConstant("GL_CONVOLUTION_HINT_SGIX",                   new QoreBigIntNode(0x8316));
   opengl_ns.addConstant("GL_PACK_RESAMPLE_SGIX",                      new QoreBigIntNode(0x842C));
   opengl_ns.addConstant("GL_UNPACK_RESAMPLE_SGIX",                    new QoreBigIntNode(0x842D));
   opengl_ns.addConstant("GL_RESAMPLE_REPLICATE_SGIX",                 new QoreBigIntNode(0x842E));
   opengl_ns.addConstant("GL_RESAMPLE_ZERO_FILL_SGIX",                 new QoreBigIntNode(0x842F));
   opengl_ns.addConstant("GL_RESAMPLE_DECIMATE_SGIX",                  new QoreBigIntNode(0x8430));
   opengl_ns.addConstant("GL_GLOBAL_ALPHA_SUN",                        new QoreBigIntNode(0x81D9));
   opengl_ns.addConstant("GL_GLOBAL_ALPHA_FACTOR_SUN",                 new QoreBigIntNode(0x81DA));
   opengl_ns.addConstant("GL_RESTART_SUN",                             new QoreBigIntNode(0x01));
   opengl_ns.addConstant("GL_REPLACE_MIDDLE_SUN",                      new QoreBigIntNode(0x02));
   opengl_ns.addConstant("GL_REPLACE_OLDEST_SUN",                      new QoreBigIntNode(0x03));
   opengl_ns.addConstant("GL_TRIANGLE_LIST_SUN",                       new QoreBigIntNode(0x81D7));
   opengl_ns.addConstant("GL_REPLACEMENT_CODE_SUN",                    new QoreBigIntNode(0x81D8));
   opengl_ns.addConstant("GL_REPLACEMENT_CODE_ARRAY_SUN",              new QoreBigIntNode(0x85C0));
   opengl_ns.addConstant("GL_REPLACEMENT_CODE_ARRAY_TYPE_SUN",         new QoreBigIntNode(0x85C1));
   opengl_ns.addConstant("GL_REPLACEMENT_CODE_ARRAY_STRIDE_SUN",       new QoreBigIntNode(0x85C2));
   opengl_ns.addConstant("GL_REPLACEMENT_CODE_ARRAY_POINTER_SUN",      new QoreBigIntNode(0x85C3));
   opengl_ns.addConstant("GL_R1UI_V3F_SUN",                            new QoreBigIntNode(0x85C4));
   opengl_ns.addConstant("GL_R1UI_C4UB_V3F_SUN",                       new QoreBigIntNode(0x85C5));
   opengl_ns.addConstant("GL_R1UI_C3F_V3F_SUN",                        new QoreBigIntNode(0x85C6));
   opengl_ns.addConstant("GL_R1UI_N3F_V3F_SUN",                        new QoreBigIntNode(0x85C7));
   opengl_ns.addConstant("GL_R1UI_C4F_N3F_V3F_SUN",                    new QoreBigIntNode(0x85C8));
   opengl_ns.addConstant("GL_R1UI_T2F_V3F_SUN",                        new QoreBigIntNode(0x85C9));
   opengl_ns.addConstant("GL_R1UI_T2F_N3F_V3F_SUN",                    new QoreBigIntNode(0x85CA));
   opengl_ns.addConstant("GL_R1UI_T2F_C4F_N3F_V3F_SUN",                new QoreBigIntNode(0x85CB));
   opengl_ns.addConstant("GL_WRAP_BORDER_SUN",                         new QoreBigIntNode(0x81D4));
   opengl_ns.addConstant("GL_UNPACK_CONSTANT_DATA_SUNX",               new QoreBigIntNode(0x81D5));
   opengl_ns.addConstant("GL_TEXTURE_CONSTANT_DATA_SUNX",              new QoreBigIntNode(0x81D6));
   opengl_ns.addConstant("GL_PHONG_WIN",                               new QoreBigIntNode(0x80EA));
   opengl_ns.addConstant("GL_PHONG_HINT_WIN",                          new QoreBigIntNode(0x80EB));
   opengl_ns.addConstant("GL_FOG_SPECULAR_TEXTURE_WIN",                new QoreBigIntNode(0x80EC));
   opengl_ns.addConstant("GL_COMPRESSED_RGB_FXT1_3DFX",                new QoreBigIntNode(0x86B0));
   opengl_ns.addConstant("GL_COMPRESSED_RGBA_FXT1_3DFX",               new QoreBigIntNode(0x86B1));
   opengl_ns.addConstant("GL_MULTISAMPLE_3DFX",                        new QoreBigIntNode(0x86B2));
   opengl_ns.addConstant("GL_SAMPLE_BUFFERS_3DFX",                     new QoreBigIntNode(0x86B3));
   opengl_ns.addConstant("GL_SAMPLES_3DFX",                            new QoreBigIntNode(0x86B4));
   opengl_ns.addConstant("GL_MULTISAMPLE_BIT_3DFX",                    new QoreBigIntNode(0x20000000));
}
