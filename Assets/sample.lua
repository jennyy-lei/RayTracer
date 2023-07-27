-- A simple scene with five spheres

shiny = gr.material({0.6, 0.7, 1}, {0.5, 0.7, 0.5}, 5)
less_shiny = gr.material({0.6, 0.7, 1}, {0.5, 0.7, 0.5}, 20)
not_shiny = gr.material({0.6, 0.7, 1}, {0.5, 0.7, 0.5}, 100)
shiny2 = gr.material({1, 0.7, 0.6}, {0.5, 0.7, 0.5}, 5)
less_shiny2 = gr.material({0.8, 0.7, 0.8}, {0.5, 0.7, 0.5}, 20)
not_shiny2 = gr.material({0.6, 0.7, 1}, {0.5, 0.7, 0.5}, 100)
ground = gr.material({0.3, 0.3, 0.3}, {0.0, 0.0, 0.0}, 1)

scene_root = gr.node('root')

s1 = gr.nh_sphere('s1', {-100, 120, -70}, 40)
scene_root:add_child(s1)
s1:set_material(shiny)

s2 = gr.nh_sphere('s2', {0, 120, -70}, 40)
scene_root:add_child(s2)
s2:set_material(less_shiny)

s3 = gr.nh_sphere('s3', {100, 120, -70}, 40)
scene_root:add_child(s3)
s3:set_material(not_shiny)

b1 = gr.nh_box('b1', {-140, 20, 60}, 80)
scene_root:add_child(b1)
b1:set_material(shiny2)

b2 = gr.nh_box('b2', {-40, 20, 60}, 80)
scene_root:add_child(b2)
b2:set_material(less_shiny2)

b3 = gr.nh_box('b3', {60, 20, 60}, 80)
scene_root:add_child(b3)
b3:set_material(not_shiny2)

plane = gr.mesh('plane', 'plane.obj' )
scene_root:add_child(plane)
plane:set_material(ground)
plane:scale(200, 150, 200)
plane:translate(0, -20, 0)

white_light = gr.light({00.0, 500.0, 00.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
-- magenta_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'sample-aa.png', 512, 512,
	  {0, 400, 500}, {0, -400, -500}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, magenta_light})