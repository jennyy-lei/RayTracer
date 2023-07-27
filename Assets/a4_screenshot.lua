-- Materials
mat_rock = gr.material({150/255, 150/255, 150/255}, {0, 0, 0}, 150, 0)
mat_grass = gr.material({116/255.0, 173/255.0, 58/255.0}, {0.0, 0.0, 0.0}, 150, 0)
mat_water = gr.material({116/255.0, 173/255.0, 255/255.0}, {0.0, 0.0, 0.0}, 1, .4)
mat_leaves = gr.material({234/255, 131/255, 131/255}, {0, 0, 0}, 150, 0)
mat_trunk = gr.material({89/255, 73/255, 49/255}, {0, 0, 0}, 150, 0)
mat_piller_top = gr.material({89/255, 73/255, 49/255}, {0, 0, 0}, 100, 0)
mat_pillar = gr.material({255/255, 53/255, 43/255}, {0, 0, 0}, 25, 0)
mat_pillar_cover = gr.material({50/255, 50/255, 50/255}, {0, 0, 0}, 50, 0)
mat_beam_cover = gr.material({255/255, 226/255, 66/255}, {0, 0, 0}, 150, 0)
mat_light = gr.material({255/255, 213/255, 63/255}, {0255/255, 213/255, 63/255}, 1, 0)
mat_light_structure = gr.material({150/255, 150/255, 150/255}, {0, 0, 0}, 50, 0)
mat_shiny_sphere = gr.material({255/255, 192/255, 184/255}, {0, 0, 0}, 1, 0)
mat_shiny_sphere2 = gr.material({150/255.0, 213/255.0, 255/255.0}, {0.0, 0.0, 0.0}, 1, 0)

-- Meshs
mesh_grass = gr.cube('plane')
mesh_grass:scale(2, 1, 2)
mesh_grass:translate(-1, -1, -1)
mesh_grass:set_material(mat_grass)

light_structure = gr.mesh('lightstructure', 'lightstructure.obj')
light_structure:set_material(mat_light_structure)
light_ball = gr.mesh('lightball', 'lightball.obj')
light_ball:set_material(mat_light)
mesh_light = gr.node('light')
mesh_light:add_child(light_ball)
mesh_light:add_child(light_structure)

rock_poly = gr.mesh('rock', 'rock.obj')
rock_poly:set_material(mat_rock)

leaves = gr.mesh('leaves', 'leaves.obj')
leaves:set_material(mat_leaves)
trunk = gr.mesh('trunk', 'trunk.obj')
trunk:set_material(mat_trunk)
tree = gr.node('tree')
tree:add_child(leaves)
tree:add_child(trunk)

function pillar(name)
  pillar_bottom = gr.mesh(name..'bottom', 'gate/pillarbottom.obj')
  pillar_bottom:set_material(mat_pillar_cover)
  pillar_middle = gr.mesh(name, 'gate/pillar.obj')
  pillar_middle:set_material(mat_pillar)
  pillar_middle:translate(0, 0.505793, 0)
  pillar_top = gr.mesh(name..'top', 'gate/pillartop.obj')
  pillar_top:set_material(mat_pillar_cover)
  pillar_top:translate(0, 4.70197, 0)

  pillar_node = gr.node(name)
  pillar_node:add_child(pillar_bottom)
  pillar_node:add_child(pillar_middle)
  pillar_node:add_child(pillar_top)
  return pillar_node
end
left_pillar = pillar('left pillar')
left_pillar:translate(-1.66375, 0, 0)
right_pillar = pillar('left pillar')
right_pillar:translate(1.66375, 0, 0)
lower_beam = gr.mesh('lower beam', 'gate/lowerbeam.obj')
lower_beam:set_material(mat_pillar)
lower_beam:translate(0, 3.97251, 0)
lower_beam_left = gr.mesh('lower beam left', 'gate/lowerbeamend.obj')
lower_beam_left:set_material(mat_beam_cover)
lower_beam_left:translate(-2.3434, 3.94962, 0)
lower_beam_right = gr.mesh('lower beam right', 'gate/lowerbeamend.obj')
lower_beam_right:set_material(mat_beam_cover)
lower_beam_right:translate(2.3434, 3.94962, 0)
upper_beam = gr.mesh('upper beam', 'gate/topbeam.obj')
upper_beam:set_material(mat_pillar)
upper_beam:translate(0, 4.95146, 0)
upper_beam_left = gr.mesh('upper beam left', 'gate/topbeamend.obj')
upper_beam_left:set_material(mat_beam_cover)
upper_beam_left:translate(-2.61863, 4.92857, 0)
upper_beam_right = gr.mesh('upper beam left', 'gate/topbeamend.obj')
upper_beam_right:set_material(mat_beam_cover)
upper_beam_right:translate(2.61863, 4.92857, 0)
brown_beam = gr.mesh('brown beam', 'gate/brownbeam.obj')
brown_beam:set_material(mat_piller_top)
brown_beam:translate(0, 5.26981, 0)
tori = gr.node('tori')
tori:add_child(left_pillar)
tori:add_child(right_pillar)
tori:add_child(lower_beam)
tori:add_child(lower_beam_left)
tori:add_child(lower_beam_right)
tori:add_child(upper_beam)
tori:add_child(brown_beam)
tori:add_child(upper_beam_left)
tori:add_child(upper_beam_right)


function create(mesh, name)
  instance = gr.node(name)
  instance:add_child(mesh)
  return instance
end

--Scene
scene = gr.node('root')

grass_instance = create(mesh_grass, 'grass1')
grass_instance:scale(2.5, 1, 6)
grass_instance:translate(-3.5, 0, 0)
scene:add_child(grass_instance)

grass_instance = create(mesh_grass, 'grass2')
grass_instance:scale(2.5, 1, 6)
grass_instance:translate(3.5, 0, 0)
scene:add_child(grass_instance)

water = gr.cube('water')
water:set_material(mat_water)
water:scale(2, 0.8, 12)
water:translate(-1, -1, -6)
scene:add_child(water)

bridge = gr.cube('bridge')
bridge:set_material(mat_trunk)
bridge:scale(4, 0.1, 1.6)
bridge:translate(-2, 0, 2.2)
scene:add_child(bridge)

tree_instance = create(tree, 'tree')
tree_instance:scale(1.2, 1.2, 1.2)
tree_instance:rotate('y', -25)
tree_instance:translate(-3.5, 0, -3)
scene:add_child(tree_instance)

light_number = 0
for _, pt in pairs({
  {{-4.5, 0, 4.5}},
  {{-4.5, 0, 1.5}},
  {{-1.5, 0, 4.5}},
  {{-1.5, 0, 1.5}},
  {{1.5, 0, 4.5}},
  {{1.5, 0, 1.5}},
}) do
  light_instance = create(mesh_light, 'light '..tostring(light_number))
  light_instance:translate(table.unpack(pt[1]))
  scene:add_child(light_instance)

  light_number = light_number + 1
end


tori_instance = create(tori, 'tori')
tori_instance:scale(1, 0.8, 1)
tori_instance:rotate('y', 90)
tori_instance:translate(4, 0, 3)
scene:add_child(tori_instance)

rock_number = 0
for _, pt in pairs({
  {{-2.5, 0, -2.2}, {0.3, 0.3, 0.3}, 180},
  {{-3.4, 0, -1.5}, {0.2, 0.2, 0.2}, 30},
  {{-4.6, 0, -3.5}, {0.18, 0.18, 0.18}, 270},
}) do
rock_instance = gr.node('rock'..tostring(rock_number))
rock_instance:add_child(rock_poly)
rock_instance:rotate('Y', pt[3])
rock_instance:scale(table.unpack(pt[2]))
rock_instance:translate(table.unpack(pt[1]))
scene:add_child(rock_instance)

rock_number = rock_number + 1
end


sphere = gr.sphere('sphere')
sphere:set_material(mat_shiny_sphere)
sphere:scale(0.4, 0.4, 0.4)
sphere:translate(-4.1, 0.3, -1.74)
scene:add_child(sphere)


sphere2 = gr.sphere('sphere')
sphere2:set_material(mat_shiny_sphere2)
sphere2:scale(0.3, 0.3, 0.3)
sphere2:translate(3.7, 0.2, 1.8)
scene:add_child(sphere2)


-- Lights
light_white = gr.light({0, 500.0, 0}, {0.9, 0.9, 0.9}, {1, 0, 0})
light_blue = gr.light({-15, 5, 0}, {92/255, 171/255, 255/255}, {1, 0, 0})
light_warm = gr.light({15, 15, 15}, {255/255, 177/255, 133/255}, {1, 0, 0})

-- Render
gr.render(scene, 'model_512_depth3.png', 512, 512,
	  -- {0, 3, 15}, {0, -0, -15}, {0, 1, 0}, 50,
	  {-12, 8, 12}, {12, -8.2, -12}, {0, 1, 0}, 50,
	  {0.35, 0.25, 0.4}, {light_white, light_blue, light_warm})
